// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include <thread>
#include "user_context.h"
#include "xbox_system_factory.h"
#include "web_socket_connection.h"
#include "utils.h"

using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

web_socket_connection::web_socket_connection(
    _In_ std::shared_ptr<user_context> userContext,
    _In_ const xsapi_internal_string& uri,
    _In_ const xsapi_internal_string& subProtocol,
    _In_ std::shared_ptr<xbox_live_context_settings> httpSetting
) :
    m_userContext(std::move(userContext)),
    m_uri(std::move(uri)),
    m_subProtocol(std::move(subProtocol)),
    m_httpSetting(httpSetting),
    m_state(web_socket_connection_state::disconnected),
    m_client(system::xbox_system_factory::get_factory()->create_web_socket_client()),
    m_closeRequested(false),
    m_attemptingConnection(false)
{
    XSAPI_ASSERT(m_httpSetting != nullptr);
}

void web_socket_connection::attempt_connect(
    _In_ std::chrono::milliseconds retryInterval,
    _In_ std::chrono::time_point<std::chrono::steady_clock> startTime
    )
{
    LOG_DEBUG("Start websocket connection attempt");

    auto pThis = shared_from_this();
    m_client->connect(m_userContext, m_uri, m_subProtocol,
        [pThis, retryInterval, startTime](HC_RESULT errorCode, uint32_t platformErrorCode)
    {
        if (errorCode == HC_OK)
        {
            pThis->m_attemptingConnection = false;
            pThis->set_state_helper(web_socket_connection_state::connected);

            pThis->m_client->set_closed_handler([pThis](HC_WEBSOCKET_CLOSE_STATUS closeStatus)
            {
                pThis->on_close(closeStatus);
            });
        }
        else
        {
            LOGS_INFO << "Websocket connect attempt failed with platform error code..." << platformErrorCode;

            // check if we need to retry
            auto timeCurrent = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeCurrent - startTime);

            if (duration >= pThis->m_httpSetting->websocket_timeout_window())
            {
                //retry didn't help, notify caller, we're in stable disconnected state
                pThis->set_state_helper(web_socket_connection_state::disconnected);
            }

            std::this_thread::sleep_for(retryInterval);

            if (!pThis->m_closeRequested)
            {
                // increase retry interval each time
                auto newRetryInterval = min((3 * retryInterval), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes(1)));
                pThis->attempt_connect(newRetryInterval, startTime);
            }
            else
            {
                pThis->m_attemptingConnection = false;
            }
        }
    });
}

void web_socket_connection::ensure_connected()
{
    // As soon as this API gets called, move away from disconnected state
    set_state_helper(web_socket_connection_state::activated);

    {
        std::lock_guard<std::mutex> lock(m_stateLocker);

        // If it's still connecting or connected return.
        if (m_attemptingConnection || m_state == web_socket_connection_state::connected)
        {
            return;
        }
    }

    m_closeRequested = false;
    m_attemptingConnection = true;
    set_state_helper(web_socket_connection_state::connecting);

    auto context = utils::store_shared_ptr(shared_from_this());
    HCTaskCreate(HC_SUBSYSTEM_ID_XSAPI, XSAPI_DEFAULT_TASKGROUP,
        [](void* context, HC_TASK_HANDLE taskHandle)
        {
            auto pThis = utils::remove_shared_ptr<web_socket_connection>(context);
            pThis->attempt_connect(std::chrono::milliseconds(100), std::chrono::high_resolution_clock::now());
            return HC_OK;
        },
        context,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
        );
}

web_socket_connection_state
web_socket_connection::state()
{
    std::lock_guard<std::mutex> lock(m_stateLocker);
    return m_state;
}

void 
web_socket_connection::send(
    _In_ const xsapi_internal_string& message,
    _In_ xbox::services::xbox_live_callback<HC_RESULT, uint32_t> callback
    )
{
    m_client->send(message, callback);
}

void
web_socket_connection::close()
{
    m_closeRequested = true;
    m_client->close();
}

void 
web_socket_connection::set_received_handler(
    _In_ xbox::services::xbox_live_callback<xsapi_internal_string> handler
    )
{
    if (m_client != nullptr)
    {
        m_client->set_received_handler(handler);
    }
}

void
web_socket_connection::on_close(HC_WEBSOCKET_CLOSE_STATUS closeStatus)
{
    // websocket_close_status::normal means the socket completed its purpose. Normally it means close
    // was triggered by client. Don't reconnect.
    LOGS_INFO << "web_socket_connection on_close code:" << closeStatus;
    if (closeStatus != HC_WEBSOCKET_CLOSE_NORMAL && !m_closeRequested)
    {
        LOG_INFO("web_socket_connection on close, not requested");
        // try to reconnect
        set_state_helper(web_socket_connection_state::connecting);
        ensure_connected();
    }
    else
    {
        LOG_INFO("web_socket_connection on close, requested");
        set_state_helper(web_socket_connection_state::disconnected);
    }
}

void
web_socket_connection::set_connection_state_change_handler(
    _In_ xbox::services::xbox_live_callback<web_socket_connection_state, web_socket_connection_state> handler
    )
{
    std::lock_guard<std::mutex> lock(m_stateLocker);
    m_externalStateChangeHandler = handler;
}

void web_socket_connection::set_state_helper(_In_ web_socket_connection_state newState)
{
    web_socket_connection_state oldState;
    xbox_live_callback<web_socket_connection_state, web_socket_connection_state> externalStateChangeHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_stateLocker);

        // Can only set state to activated if current state is disconnected
        if ((newState == web_socket_connection_state::activated && m_state != web_socket_connection_state::disconnected) ||
            newState == m_state)
        {
            return;
        }

        oldState = m_state;
        m_state = newState;
        externalStateChangeHandlerCopy = m_externalStateChangeHandler;
    }

    LOGS_DEBUG << "websocket state change: " << convert_web_socket_connection_state_to_string(oldState) << " -> " << convert_web_socket_connection_state_to_string(newState);

    if (externalStateChangeHandlerCopy != nullptr)
    {
        try
        {
            externalStateChangeHandlerCopy(oldState, newState);
        }
        catch (...)
        {
            LOG_ERROR("web_socket_connection::external state handler had an exception");
        }
    }
}

xsapi_internal_string
web_socket_connection::convert_web_socket_connection_state_to_string(_In_ web_socket_connection_state state)
{
    switch (state)
    {
    case web_socket_connection_state::disconnected: return "disconnected";
    case web_socket_connection_state::activated: return "activated";
    case web_socket_connection_state::connecting: return "connecting";
    case web_socket_connection_state::connected: return "connected";
    default: return "unknownState";
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

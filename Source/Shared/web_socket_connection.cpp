// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include <cpprest/ws_client.h>
#include <thread>
#include "user_context.h"
#include "xbox_system_factory.h"
#include "web_socket_connection.h"
#include "utils.h"

using namespace web::websockets::client;
using namespace XBOX_LIVE_NAMESPACE::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

web_socket_connection::web_socket_connection(
    _In_ std::shared_ptr<user_context> userContext,
    _In_ web::uri uri,
    _In_ string_t subProtocol,
    _In_ std::shared_ptr<xbox_live_context_settings> httpSetting
    ):
    m_userContext(std::move(userContext)),
    m_uri(std::move(uri)),
    m_subProtocol(std::move(subProtocol)),
    m_httpSetting(httpSetting),
    m_state(web_socket_connection_state::disconnected),
    m_client(system::xbox_system_factory::get_factory()->create_web_socket_client()),
    m_closeCallbackSet(false),
    m_closeRequested(false)
{
    XSAPI_ASSERT(m_httpSetting != nullptr);

    m_connectingTask = pplx::task_from_result();
}

void
web_socket_connection::ensure_connected()
{
    // As soon as this API gets called, move away from disconnected state
    set_state_helper(web_socket_connection_state::activated);

    std::lock_guard<std::mutex> lock(m_stateLocker);

    // If it's still connecting or connected return.
    if (!m_connectingTask.is_done() || m_state == web_socket_connection_state::connected) return;

    m_closeRequested = false;

    // kick off connection 
    std::weak_ptr<web_socket_connection> thisWeakPtr = shared_from_this();
    m_connectingTask = pplx::create_task([thisWeakPtr]
    {
        std::shared_ptr<web_socket_connection> pThis;
        try
        {
            pThis = thisWeakPtr.lock();
            if (pThis == nullptr)
            {
                throw std::runtime_error("xbox_web_socket_client_impl shutting down");
            }

            LOG_DEBUG("Start websocket connection task");

            pThis->set_state_helper(web_socket_connection_state::connecting);
            bool connected = false;
            bool isStableDisconnected = false;

            std::chrono::milliseconds retryInterval(100); // start retry internal with 100ms
            auto timeStart = std::chrono::high_resolution_clock::now();

            int connectAttempt = 0;
            while (!connected && !pThis->m_closeRequested)
            {
                try
                {
                    LOGS_INFO <<"Websocket trying to connnect... attempt " << ++connectAttempt;
                    // real web socket connect call 
                    pThis->m_client->connect(
                        pThis->m_userContext,
                        pThis->m_uri,
                        pThis->m_subProtocol
                        ).get();

                    LOG_INFO("Websocket connnection established.");

                    // This needs to execute after connected 
                    // Can't get 'this' shared pointer in constructor, so place socket client calling setting to here.
                    if (!pThis->m_closeCallbackSet)
                    {
                        std::weak_ptr<web_socket_connection> thisWeakPtr2 = pThis;
                        pThis->m_client->set_closed_handler([thisWeakPtr2](uint16_t code, string_t reason)
                        {
                            auto pThis2 = thisWeakPtr2.lock();
                            if (pThis2 != nullptr)
                            {
                                pThis2->on_close(code, reason);
                            }
                        });
                        pThis->m_closeCallbackSet = true;
                    }

                    //connected, set state
                    connected = true;
                    pThis->set_state_helper(web_socket_connection_state::connected);
                }
                catch (...)
                {
                    LOG_INFO("Websocket connnection failed.");

                    // check if we need to retry
                    auto timeCurrent = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeCurrent - timeStart);

                    if (!isStableDisconnected)
                    {
                        isStableDisconnected = (duration >= pThis->m_httpSetting->websocket_timeout_window());
                        if (isStableDisconnected)
                        {
                            //retry didn't help, notify caller, we're in stable disconnected state
                            pThis->set_state_helper(web_socket_connection_state::disconnected);
                        }
                    }
                    std::this_thread::sleep_for(retryInterval);

                    // increase retry interval each time
                    retryInterval = min((3 * retryInterval), std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::minutes(1)));
                }
            }
        }
        catch (...)
        {
            if (pThis != nullptr)
            {
                pThis->set_state_helper(web_socket_connection_state::disconnected);
            }
        }

        LOG_DEBUG("Finish websocket connection task");
    });
}

web_socket_connection_state
web_socket_connection::state()
{
    std::lock_guard<std::mutex> lock(m_stateLocker);
    return m_state;
}

pplx::task<void>
web_socket_connection::send(
    _In_ const string_t& message
    )
{
    if (m_client == nullptr)
        return pplx::task_from_exception<void>(std::runtime_error("web socket is not created yet."));

    return m_client->send(message);
}

pplx::task<void>
web_socket_connection::close()
{
    if (m_client == nullptr)
        return pplx::task_from_exception<void>(std::runtime_error("web socket is not created yet."));

    m_closeRequested = true;
    return m_client->close();
}

void 
web_socket_connection::set_received_handler(
    _In_ std::function<void(string_t)> handler
    )
{
    if (m_client != nullptr)
    {
        m_client->set_received_handler(handler);
    }
}

void
web_socket_connection::on_close(uint16_t code, string_t reason)
{
    // websocket_close_status::normal means the socket completed its purpose. Normally it means close
    // was triggered by client. Don't reconnect.
    LOGS_INFO << "web_socket_connection on_close code:" << code << " ,reason:" << reason;
    if (static_cast<websocket_close_status>(code) != websocket_close_status::normal && !m_closeRequested)
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
    _In_ std::function<void(web_socket_connection_state oldState, web_socket_connection_state newState)> handler
    )
{
    std::lock_guard<std::mutex> lock(m_stateLocker);
    m_externalStateChangeHandler = handler;
}

void web_socket_connection::set_state_helper(_In_ web_socket_connection_state newState)
{
    web_socket_connection_state oldState;
    std::function<void(web_socket_connection_state oldState, web_socket_connection_state newState)> externalStateChangeHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_stateLocker);
        // Can only set state to activated if current state is disconnected
        if (newState == web_socket_connection_state::activated && m_state != web_socket_connection_state::disconnected)
        {
            return;
        }

        oldState = m_state;
        m_state = newState;
        externalStateChangeHandlerCopy = m_externalStateChangeHandler;
    }

    LOGS_DEBUG << "websocket state change: " << convert_web_socket_connection_state_to_string(oldState) << " -> " << convert_web_socket_connection_state_to_string(newState);

    if (oldState != newState && externalStateChangeHandlerCopy)
    {
        try
        {
            if (externalStateChangeHandlerCopy != nullptr)
            {
                externalStateChangeHandlerCopy(oldState, newState);
            }
        }
        catch (...)
        {
            LOG_ERROR("web_socket_connection::external state handler had an exception");
        }
    }
}

const string_t
web_socket_connection::convert_web_socket_connection_state_to_string(_In_ web_socket_connection_state state)
{
    switch (state)
    {
    case web_socket_connection_state::disconnected: return _T("disconnected");
    case web_socket_connection_state::activated: return _T("activated");
    case web_socket_connection_state::connecting: return _T("connecting");
    case web_socket_connection_state::connected: return _T("connected");
    default: return _T("unknownState");
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

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

HRESULT web_socket_connection::attempt_connect(retry_context* retryContext, AsyncBlock* asyncBlock)
{
    HRESULT hr = BeginAsync(asyncBlock, retryContext, nullptr, __FUNCTION__,
        [](AsyncOp opCode, const AsyncProviderData* data)
    {
        switch (opCode)
        {
        case AsyncOp_DoWork:
            auto retryContext = static_cast<retry_context*>(data->context);

            retryContext->pThis->m_client->connect(retryContext->pThis->m_userContext, 
                retryContext->pThis->m_uri, 
                retryContext->pThis->m_subProtocol,
                [data, retryContext](WebSocketCompletionResult result)
            {
                retryContext->result = result;
                CompleteAsync(data->async, S_OK, 0);
            });
            return E_PENDING;
        }
        return S_OK;
    });

    if (SUCCEEDED(hr))
    {
        hr = ScheduleAsync(asyncBlock, retryContext->delay);
    }
    return hr;
}

void web_socket_connection::retry_until_connected(retry_context* context)
{
    AsyncBlock* nestedAsyncBlock = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    nestedAsyncBlock->context = context;
    CreateNestedAsyncQueue(context->asyncProviderData->queue, &nestedAsyncBlock->queue);
    
    nestedAsyncBlock->callback = [](AsyncBlock* async)
    {
        retry_context* retryContext = static_cast<retry_context*>(async->context);
        auto pThis = retryContext->pThis;
        if (SUCCEEDED(retryContext->result.errorCode))
        {
            pThis->m_attemptingConnection = false;
            pThis->set_state_helper(web_socket_connection_state::connected);

            pThis->m_client->set_closed_handler([pThis](HCWebSocketCloseStatus closeStatus)
            {
                pThis->on_close(closeStatus);
            });

            CompleteAsync(retryContext->outerAsyncBlock, S_OK, 0);
        }
        else
        {
            LOGS_INFO << "Websocket connect attempt failed with platform error code..." << retryContext->result.platformErrorCode;

            // check if we need to retry
            auto timeCurrent = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timeCurrent - retryContext->startTime);

            if (duration >= pThis->m_httpSetting->websocket_timeout_window())
            {
                //retry didn't help, notify caller, we're in stable disconnected state
                pThis->set_state_helper(web_socket_connection_state::disconnected);
            }

            if (!pThis->m_closeRequested)
            {
                // increase retry interval each time
                retryContext->delay = min((3 * retryContext->delay), 1000 * 60 /* 1 minute in ms */);
                pThis->retry_until_connected(retryContext);
            }
            else
            {
                pThis->m_attemptingConnection = false;
                CompleteAsync(retryContext->outerAsyncBlock, E_FAIL, 0);
            }
        }

        CloseAsyncQueue(async->queue);
        xsapi_memory::mem_free(async);
    };

    HRESULT hr = attempt_connect(context, nestedAsyncBlock);
    if (FAILED(hr))
    {
        CompleteAsync(context->outerAsyncBlock, hr, 0);
        return;
    }
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

    AsyncBlock* outerAsync = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    outerAsync->queue = get_xsapi_singleton()->m_asyncQueue;
    outerAsync->callback = [](AsyncBlock* async)
    {
        LOG_DEBUG("Web socket connection completed.");
        xsapi_memory::mem_free(async);
    };

    auto retryContext = xsapi_allocate_shared<retry_context>();
    retryContext->delay = 0;
    retryContext->outerAsyncBlock = outerAsync;
    retryContext->pThis = shared_from_this();
    retryContext->startTime = std::chrono::high_resolution_clock::now();

    BeginAsync(outerAsync, utils::store_shared_ptr(retryContext), nullptr, __FUNCTION__,
        [](_In_ AsyncOp op, _In_ const AsyncProviderData* data)
    {
        auto context = utils::get_shared_ptr<retry_context>(data->context, op == AsyncOp_Cleanup);
        context->asyncProviderData = data;

        switch (op)
        {
        case AsyncOp_DoWork:
            context->pThis->retry_until_connected(context.get());
            return E_PENDING;
        }
        return S_OK;
    });
    ScheduleAsync(outerAsync, 0);
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
    _In_ xbox::services::xbox_live_callback<WebSocketCompletionResult> callback
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
web_socket_connection::on_close(HCWebSocketCloseStatus closeStatus)
{
    // websocket_close_status::normal means the socket completed its purpose. Normally it means close
    // was triggered by client. Don't reconnect.
    LOGS_INFO << "web_socket_connection on_close code:" << closeStatus;
    if (closeStatus != HCWebSocketCloseStatus_Normal && !m_closeRequested)
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

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "build_version.h"
#include "user_context.h"
#include "web_socket_client.h"
#include "utils.h"
#include "xbox_live_app_config_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

using namespace xbox::services::system;

xbox_web_socket_client::xbox_web_socket_client()
    : m_websocket(nullptr)
{
}

xbox_web_socket_client::~xbox_web_socket_client()
{
    auto singleton = get_xsapi_singleton(false);
    if (singleton != nullptr)
    {
        singleton->m_websocketHandles.erase(m_websocket);
    }
    if (m_websocket != nullptr)
    {
        HCWebSocketCloseHandle(m_websocket);
    }
}

void xbox_web_socket_client::connect(
    _In_ const std::shared_ptr<user_context>& userContext,
    _In_ const xsapi_internal_string& uri,
    _In_ const xsapi_internal_string& subProtocol,
    _In_ xbox_live_callback<WebSocketCompletionResult> callback
    )
{
    THROW_CPP_INVALIDARGUMENT_IF_NULL(userContext);

    std::weak_ptr<xbox_web_socket_client> thisWeakPtr = shared_from_this();

    HCWebSocketCreate(&m_websocket);
    get_xsapi_singleton()->m_websocketHandles[m_websocket] = thisWeakPtr;

    xsapi_internal_string callerContext = userContext->caller_context();
    userContext->get_auth_result("GET", uri, xsapi_internal_string(), xsapi_internal_string(), false, get_xsapi_singleton()->m_asyncQueue, 
        [uri, subProtocol, callerContext, thisWeakPtr, callback](xbox::services::xbox_live_result<user_context_auth_result> xblResult)
    {
        std::shared_ptr<xbox_web_socket_client> pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            throw std::runtime_error("xbox_web_socket_client shutting down");
        }

        if (xblResult.err())
        {
            throw std::runtime_error("Failed to get xtoken during connect");
        }

        const auto& result = xblResult.payload();

        auto proxyUri = utils::internal_string_from_string_t(xbox_live_app_config_internal::get_app_config_singleton()->proxy().to_string());
        if (!proxyUri.empty())
        {
            HCWebSocketSetProxyUri(pThis->m_websocket, proxyUri.data());
        }

        HCWebSocketSetHeader(pThis->m_websocket, "Authorization", result.token().data());
        HCWebSocketSetHeader(pThis->m_websocket, "Signature", result.signature().data());
        HCWebSocketSetHeader(pThis->m_websocket, "Accept-Language", utils::get_locales().data());

        xsapi_internal_string userAgent = DEFAULT_USER_AGENT;
        if (!callerContext.empty())
        {
            userAgent += " " + callerContext;
        }
        HCWebSocketSetHeader(pThis->m_websocket, "User-Agent", userAgent.data());

        HCWebSocketSetFunctions([](hc_websocket_handle_t websocket, _In_z_ const char* incomingBodyString)
        {
            try
            {
                auto singleton = get_xsapi_singleton();
                auto iter = singleton->m_websocketHandles.find(websocket);
                if (iter != singleton->m_websocketHandles.end())
                {
                    auto pThis = iter->second.lock();
                    if (pThis != nullptr)
                    {
                        pThis->m_receiveHandler(incomingBodyString);
                    }
                }
                else
                {
#ifndef UNIT_TEST_SERVICES
                    XSAPI_ASSERT(false && "Could not find web_socket_client associated with HC_WEBSOCKET_HANDLER");
#endif
                }
            }
            catch (...)
            {
                LOG_ERROR("Exception happened in web socket receiving handler.");
            }
        },
        [](hc_websocket_handle_t websocket, HCWebSocketCloseStatus closeStatus)
        {
            try
            {
                auto singleton = get_xsapi_singleton();
                auto iter = singleton->m_websocketHandles.find(websocket);
                if (iter != singleton->m_websocketHandles.end())
                {
                    auto pThis = iter->second.lock();
                    if (pThis != nullptr)
                    {
                        pThis->m_closeHandler(closeStatus);
                    }
                }
                else
                {
#ifndef UNIT_TEST_SERVICES
                    XSAPI_ASSERT(false && "Could not find web_socket_client associated with HC_WEBSOCKET_HANDLER");
#endif
                }
            }
            catch (...)
            {
                LOG_ERROR("Exception happened in web socket close handler.");
            }
        });

        AsyncBlock *asyncBlock = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
        asyncBlock->queue = get_xsapi_singleton()->m_asyncQueue;
        asyncBlock->context = utils::store_shared_ptr(xsapi_allocate_shared<xbox_live_callback<WebSocketCompletionResult>>(callback));
        asyncBlock->callback = [](_Inout_ AsyncBlock* asyncBlock)
        {
            WebSocketCompletionResult result = {};
            HCGetWebSocketConnectResult(asyncBlock, &result);
            auto callback = utils::get_shared_ptr<xbox_live_callback<WebSocketCompletionResult>>(asyncBlock->context);
            (*callback)(result);
            xsapi_memory::mem_free(asyncBlock);
        };

        HCWebSocketConnectAsync(uri.data(), subProtocol.data(), pThis->m_websocket, asyncBlock);
    });
}

void xbox_web_socket_client::send(
    _In_ const xsapi_internal_string& message,
    _In_ xbox::services::xbox_live_callback<WebSocketCompletionResult> callback
    )
{
    AsyncBlock* asyncBlock = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    asyncBlock->context = utils::store_shared_ptr(xsapi_allocate_shared<xbox_live_callback<WebSocketCompletionResult>>(callback));
    asyncBlock->callback = [](_Inout_ AsyncBlock* asyncBlock)
    {
        WebSocketCompletionResult result = {};
        HCGetWebSocketSendMessageResult(asyncBlock, &result);
        auto callback = utils::get_shared_ptr<xbox_live_callback<WebSocketCompletionResult>>(asyncBlock->context);
        (*callback)(result);
        xsapi_memory::mem_free(asyncBlock);
    };
    HCWebSocketSendMessageAsync(m_websocket, message.data(), asyncBlock);
}

void xbox_web_socket_client::close()
{
    HCWebSocketDisconnect(m_websocket);
}

void xbox_web_socket_client::set_received_handler(
    _In_ xbox_live_callback<xsapi_internal_string> handler
    )
{
    m_receiveHandler = handler;
}

void xbox_web_socket_client::set_closed_handler(
    _In_ xbox_live_callback<HCWebSocketCloseStatus> handler
    )
{
    m_closeHandler = handler;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

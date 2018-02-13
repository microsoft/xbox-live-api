// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "build_version.h"
#include "user_context.h"
#include "web_socket_client.h"
#include "utils.h"
#include "xbox_live_app_config_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xsapi_internal_unordered_map<HC_WEBSOCKET_HANDLE, xbox_web_socket_client*> xbox_web_socket_client::m_handleMap;

xbox_web_socket_client::xbox_web_socket_client()
{
    HCWebSocketCreate(&m_websocket);
    m_handleMap[m_websocket] = this;
}

xbox_web_socket_client::~xbox_web_socket_client()
{
    m_handleMap.erase(m_websocket);
    HCWebSocketCloseHandle(m_websocket);
}

void xbox_web_socket_client::connect(
    _In_ const std::shared_ptr<user_context>& userContext,
    _In_ const xsapi_internal_string& uri,
    _In_ const xsapi_internal_string& subProtocol,
    _In_ xbox_live_callback<HC_RESULT, uint32_t> callback
    )
{
    THROW_CPP_INVALIDARGUMENT_IF_NULL(userContext);

    std::weak_ptr<xbox_web_socket_client> thisWeakPtr = shared_from_this();

    string_t callerContext = userContext->caller_context();
    userContext->get_auth_result(_T("GET"), utils::string_t_from_internal_string(uri), string_t(), string_t(), false, XSAPI_DEFAULT_TASKGROUP, 
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

        HCWebSocketSetHeader(pThis->m_websocket, "Authorization", utils::internal_string_from_string_t(result.token()).data());
        HCWebSocketSetHeader(pThis->m_websocket, "Signature", utils::internal_string_from_string_t(result.signature()).data());
        HCWebSocketSetHeader(pThis->m_websocket, "Accept-Language", utils::get_locales().data());

        xsapi_internal_string userAgent = DEFAULT_USER_AGENT;
        if (!callerContext.empty())
        {
            userAgent += " " + utils::internal_string_from_string_t(callerContext);
        }
        HCWebSocketSetHeader(pThis->m_websocket, "User-Agent", userAgent.data());

        HCWebSocketSetFunctions([](HC_WEBSOCKET_HANDLE websocket, PCSTR incomingBodyString)
        {
            try
            {
                auto iter = xbox_web_socket_client::m_handleMap.find(websocket);
                if (iter != xbox_web_socket_client::m_handleMap.end())
                {
                    iter->second->m_receiveHandler(incomingBodyString);
                }
                else
                {
                    XSAPI_ASSERT(false && "Could not find web_socket_client associated with HC_WEBSOCKET_HANDLER");
                }
            }
            catch (...)
            {
                LOG_ERROR("Exception happened in web socket receiving handler.");
            }
        },
        [](HC_WEBSOCKET_HANDLE websocket, HC_WEBSOCKET_CLOSE_STATUS closeStatus)
        {
            try
            {
                auto iter = xbox_web_socket_client::m_handleMap.find(websocket);
                if (iter != xbox_web_socket_client::m_handleMap.end())
                {
                    iter->second->m_closeHandler(closeStatus);
                }
                else
                {
                    XSAPI_ASSERT(false && "Could not find web_socket_client associated with HC_WEBSOCKET_HANDLER");
                }
            }
            catch (...)
            {
                LOG_ERROR("Exception happened in web socket receiving handler.");
            }
        });

        auto connectContext = utils::store_shared_ptr(xsapi_allocate_shared<xbox_live_callback<HC_RESULT, uint32_t>>(callback));

        HCWebSocketConnect(uri.data(), subProtocol.data(), pThis->m_websocket, HC_SUBSYSTEM_ID_XSAPI, XSAPI_DEFAULT_TASKGROUP, connectContext,
            [](void* context, HC_WEBSOCKET_HANDLE websocket, HC_RESULT errorCode, uint32_t platformErrorCode)
        {
            auto callback = utils::remove_shared_ptr<xbox_live_callback<HC_RESULT, uint32_t>>(context);
            (*callback)(errorCode, platformErrorCode);
        });
    });
}

void xbox_web_socket_client::send(
    _In_ const xsapi_internal_string& message,
    _In_ xbox::services::xbox_live_callback<HC_RESULT, uint32_t> callback
    )
{
    auto sendContext = utils::store_shared_ptr(xsapi_allocate_shared<xbox_live_callback<HC_RESULT, uint32_t>>(callback));

    HCWebSocketSendMessage(m_websocket, message.data(), HC_SUBSYSTEM_ID_XSAPI, XSAPI_DEFAULT_TASKGROUP, sendContext,
        [](void* context, HC_WEBSOCKET_HANDLE websocket, HC_RESULT errorCode, uint32_t platformErrorCode)
    {
        auto callback = utils::remove_shared_ptr<xbox_live_callback<HC_RESULT, uint32_t>>(context);
        (*callback)(errorCode, platformErrorCode);
    });
}

void xbox_web_socket_client::close()
{
    HCWebSocketCloseHandle(m_websocket);
}

void xbox_web_socket_client::set_received_handler(
    _In_ xbox_live_callback<xsapi_internal_string> handler
    )
{
    m_receiveHandler = handler;
}

void xbox_web_socket_client::set_closed_handler(
    _In_ xbox_live_callback<HC_WEBSOCKET_CLOSE_STATUS> handler
    )
{
    m_closeHandler = handler;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

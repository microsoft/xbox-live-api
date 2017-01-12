//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "build_version.h"
#include "user_context.h"
#include "web_socket_client.h"
#include "utils.h"

using namespace web::websockets::client;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
 
xbox_web_socket_client::xbox_web_socket_client()
{
}

pplx::task<void>
xbox_web_socket_client::connect(
    _In_ const std::shared_ptr<user_context>& userContext,
    _In_ const web::uri& uri,
    _In_ const string_t& subProtocol
    )
{
    THROW_CPP_INVALIDARGUMENT_IF_NULL(userContext);

    std::weak_ptr<xbox_web_socket_client> thisWeakPtr = shared_from_this();

    string_t callerContext = userContext->caller_context();
    return userContext->get_auth_result(_T("GET"), uri.to_string(), string_t(), string_t())
    .then([uri, subProtocol, callerContext, thisWeakPtr](XBOX_LIVE_NAMESPACE::xbox_live_result<user_context_auth_result> xblResult)
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
        websocket_client_config config;
        config.add_subprotocol(subProtocol);
        config.headers().add(_T("Authorization"), result.token());
        config.headers().add(_T("Signature"), result.signature());
        config.headers().add(_T("Accept-Language"), utils::get_locales());
        auto proxyUri = xbox_live_app_config::get_app_config_singleton()->_Proxy();
        if (!proxyUri.is_empty())
        {
            web::web_proxy proxy(proxyUri);
            config.set_proxy(proxy);
        }
        string_t userAgent = DEFAULT_USER_AGENT;
        if (!callerContext.empty())
        {
            userAgent += _T(" ") + callerContext;
        }
        config.headers().add(_T("User-Agent"), userAgent);

        pThis->m_client = std::make_shared<websocket_callback_client>(config);

        pThis->m_client->set_message_handler([thisWeakPtr](websocket_incoming_message msg)
        {
            try
            {
                std::shared_ptr<xbox_web_socket_client> pThis2(thisWeakPtr.lock());
                if (pThis2 == nullptr)
                {
                    throw std::runtime_error("xbox_web_socket_client shutting down");
                }

                if (msg.message_type() == websocket_message_type::text_message)
                {
                    auto msg_body = msg.extract_string().get();
                    if (pThis2->m_receiveHandler)
                    {
                        pThis2->m_receiveHandler(utility::conversions::to_string_t(msg_body));
                    }
                }
            }
            catch(...)
            {
                LOG_ERROR("Exception happened in web socket receiving handler.");
            }
            
        });

        pThis->m_client->set_close_handler([thisWeakPtr](websocket_close_status closeStatus, utility::string_t closeReason, const std::error_code errc)
        {
            UNREFERENCED_PARAMETER(errc);

            try
            {
                std::shared_ptr<xbox_web_socket_client> pThis2(thisWeakPtr.lock());
                if (pThis2 == nullptr)
                {
                    LOG_DEBUG("xbox_web_socket_client shutting down");
                    throw std::runtime_error("xbox_web_socket_client shutting down");
                }

                if (pThis2->m_closeHandler)
                {
                    pThis2->m_closeHandler(static_cast<uint16_t>(closeStatus), closeReason);
                };
            }
            catch (...)
            {
                LOG_ERROR("Exception happened in web socket receiving handler.");
            }
        });

        return pThis->m_client->connect(uri);
    });
}

pplx::task<void>
xbox_web_socket_client::send(
    _In_ const string_t& message
    )
{
    if (m_client == nullptr)
        return pplx::task_from_exception<void>(std::runtime_error("web socket is not created yet."));

    websocket_outgoing_message msg;
    msg.set_utf8_message(utility::conversions::to_utf8string(message));
    return m_client->send(msg);
}

pplx::task<void>
xbox_web_socket_client::close()
{
    if (m_client == nullptr)
        return pplx::task_from_exception<void>(std::runtime_error("web socket is not created yet."));

    return m_client->close();
}

void 
xbox_web_socket_client::set_received_handler(
    _In_ std::function<void(string_t)> handler
    )
{
    m_receiveHandler = handler;
}

void 
xbox_web_socket_client::set_closed_handler(
    _In_ std::function<void(uint16_t closeStatus, string_t closeReason)> handler
    )
{
    m_closeHandler = handler;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

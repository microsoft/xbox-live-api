//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include <cpprest/ws_client.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xbox_web_socket_client : public std::enable_shared_from_this<xbox_web_socket_client>
{
public:
    xbox_web_socket_client();

    virtual pplx::task<void> connect(
        _In_ const std::shared_ptr<user_context>& userContext,
        _In_ const web::uri& uri,
        _In_ const string_t& subProtocol
        );

    virtual pplx::task<void> send(
        _In_ const string_t& message
        );

    virtual pplx::task<void> close();

    virtual void set_received_handler(
        _In_ std::function<void(string_t)> handler
        );

    virtual void set_closed_handler(
        _In_ std::function<void(uint16_t closeStatus, string_t closeReason)> handler
        );

private:
    std::shared_ptr<web::websockets::client::websocket_callback_client> m_client;
    std::function<void(string_t)> m_receiveHandler;
    std::function<void(uint16_t closeStatus, string_t closeReason)> m_closeHandler;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

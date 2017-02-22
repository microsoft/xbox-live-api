// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "web_socket_client.h"
#include "web_socket_connection_state.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class web_socket_connection : public std::enable_shared_from_this<web_socket_connection>
{
public:
    web_socket_connection(
        _In_ std::shared_ptr<user_context> userContext,
        _In_ web::uri uri,
        _In_ string_t subProtocol,
        _In_ std::shared_ptr<xbox_live_context_settings> httpSetting
        );

    // ensure_connected() will try to connect if you didn't start one. 
    void ensure_connected();

    pplx::task<void> send(
        _In_ const string_t& message
        );

    pplx::task<void> close();

    // current connection state of the web_socket_connection
    web_socket_connection_state state();

    void set_received_handler(
        _In_ std::function<void(string_t)> handler
        );

    void set_connection_state_change_handler(
        _In_ std::function<void(web_socket_connection_state oldState, web_socket_connection_state newState)> handler
        );

    pplx::task<void>& connection_task() { return m_connectingTask; }

private:

    void set_state_helper(_In_ web_socket_connection_state newState);

    // Close callback. It could because of client call, network issue or service termination
    void on_close(uint16_t code, string_t reason);

    const string_t convert_web_socket_connection_state_to_string(_In_ web_socket_connection_state state);

    std::shared_ptr<xbox_web_socket_client> m_client;
    std::shared_ptr<user_context> m_userContext;
    std::shared_ptr<xbox_live_context_settings> m_httpSetting;
    web::uri m_uri;
    string_t m_subProtocol;

    std::mutex m_stateLocker;
    web_socket_connection_state m_state;

    pplx::task<void> m_connectingTask;
    pplx::task_completion_event<void> m_connectedEvent;

    std::function<void(web_socket_connection_state oldState, web_socket_connection_state newState)> m_externalStateChangeHandler;

    bool m_closeCallbackSet;
    bool m_connectionActive;
    bool m_closeRequested;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

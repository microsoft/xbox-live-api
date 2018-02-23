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
        _In_ const xsapi_internal_string& uri,
        _In_ const xsapi_internal_string& subProtocol,
        _In_ std::shared_ptr<xbox_live_context_settings> httpSetting
        );

    // ensure_connected() will try to connect if you didn't start one. 
    void ensure_connected();

    void send(
        _In_ const xsapi_internal_string& message,
        _In_ xbox::services::xbox_live_callback<HC_RESULT, uint32_t> callback = nullptr
        );

    void close();

    // current connection state of the web_socket_connection
    web_socket_connection_state state();

    void set_received_handler(
        _In_ xbox::services::xbox_live_callback<xsapi_internal_string> handler
        );

    void set_connection_state_change_handler(
        _In_ xbox::services::xbox_live_callback<web_socket_connection_state, web_socket_connection_state> handler
        );

private:

    void attempt_connect(_In_ std::chrono::milliseconds retryInterval, _In_ std::chrono::time_point<std::chrono::steady_clock> startTime);

    void set_state_helper(_In_ web_socket_connection_state newState);

    // Close callback. It could because of client call, network issue or service termination
    void on_close(HC_WEBSOCKET_CLOSE_STATUS closeStatus);

    xsapi_internal_string convert_web_socket_connection_state_to_string(_In_ web_socket_connection_state state);

    std::shared_ptr<xbox_web_socket_client> m_client;
    std::shared_ptr<user_context> m_userContext;
    std::shared_ptr<xbox_live_context_settings> m_httpSetting;
    xsapi_internal_string m_uri;
    xsapi_internal_string m_subProtocol;

    std::mutex m_stateLocker;
    web_socket_connection_state m_state;

    xbox::services::xbox_live_callback<web_socket_connection_state, web_socket_connection_state> m_externalStateChangeHandler;

    bool m_connectionActive;
    bool m_closeRequested;
    bool m_attemptingConnection;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"
#include "xbox_system_factory.h"
#include "MockUser.h"
#include "MockHttpCall.h"
#include "MockWebSocketClient.h"
#include "MockLocalConfig.h"
#include "MockMultiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

using namespace multiplayer;

struct HttpResponseStruct
{
    uint32_t counter = 0;

    std::vector<std::shared_ptr<http_call_response>> responseList; // TODO get rid of this eventually
    std::vector<std::shared_ptr<http_call_response_internal>> responseListInternal;

    std::function<void(std::shared_ptr<http_call_response>&, const string_t& requestPost)> fRequestPostFunc;
    xbox_live_callback<std::shared_ptr<http_call_response_internal>, const xsapi_internal_string&> fRequestPostFuncInternal;
};

struct WebsocketMockResponse
{
    string_t uri;
    string_t data;
    xbox::services::real_time_activity::real_time_activity_message_type eventType;
    bool isError;
};


class MockXboxSystemFactory : public xbox_system_factory
{
public:
    MockXboxSystemFactory();

    std::shared_ptr<http_call> create_http_call(
        _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
        _In_ const std::wstring& httpMethod,
        _In_ const std::wstring& serverName,
        _In_ const web::uri& pathQueryFragment,
        _In_ xbox_live_api xboxLiveApi
        ) override;

    std::shared_ptr<http_call_internal> create_http_call(
        _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& serverName,
        _In_ const web::uri& pathQueryFragment,
        _In_ xbox_live_api xboxLiveApi
        ) override;

    std::shared_ptr<xbox_web_socket_client> create_web_socket_client() override
    {
        if (m_mockWebSocketClients.size() == 0)
        {
            // Force to create a new one.
            GetMockWebSocketClient();
        }

        auto webSocketClient = m_mockWebSocketClients[m_webSocketClientCounter];
        if (m_webSocketClientCounter + 1 < m_mockWebSocketClients.size())
        {
            ++m_webSocketClientCounter;
        }
        return webSocketClient;
    }

    std::shared_ptr<local_config> create_local_config() override;

    std::shared_ptr<user_impl> create_user_impl(user_creation_context userCreationContext) override;

    std::shared_ptr<multiplayer_subscription> create_multiplayer_subscription(
        _In_ const std::function<void(const multiplayer_session_change_event_args&)>& multiplayerSessionChangeHandler,
        _In_ const std::function<void()>& multiplayerSubscriptionLostHandler,
        _In_ const std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)>& subscriptionErrorHandler
    ) override;

    std::shared_ptr<MockUser> GetMockUser() { return m_mockUser; }
    std::shared_ptr<MockHttpCall> GetMockHttpCall() { return m_mockHttpCall; }
    std::shared_ptr<MockWebSocketClient> GetMockWebSocketClient();
    std::vector<std::shared_ptr<MockWebSocketClient>> GetMockWebSocketClients();
    std::vector<std::shared_ptr<MockWebSocketClient>> AddMultipleMockWebSocketClients(uint32_t numberOfClients);
    std::shared_ptr<MockLocalConfig> GetMockLocalConfig() { return m_mockLocalConfig; }
    std::shared_ptr<MockMultiplayerSubscription> GetMocckMultiplayerSubscription() { return m_multiplayerSubscription; }

    void add_http_state_response(_In_ const std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>>& stateResponses, _In_ bool overrideStateValue = true); // uri -> vector of http responses
    void add_http_api_state_response(_In_ const std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>>& stateResponses, _In_ bool overrideStateValue = true); // uri -> vector of http api responses
    void add_websocket_state_responses_to_all_clients(_In_ const std::queue<WebsocketMockResponse>& stateResponses, _In_ const pplx::task_completion_event<void>& websocketCompletionEvent); // uri -> vector of requests 
    void set_websocket_delay_send_time(_In_ const std::chrono::milliseconds& delayTime);
    void clear_states();
    void reinit();
    void setup_mock_for_http_client() { m_setupMockForHttpClient = true; }

private:
    void SetupNextWebsocketResponseForDefaultClient();
    void SetupNextWebsocketResponsesForAllClients();

    bool m_setupMockForHttpClient;
    uint32_t m_webSocketClientCounter;
    std::shared_ptr<MockUser> m_mockUser;
    std::shared_ptr<MockHttpCall> m_mockHttpCall;
    std::vector<std::shared_ptr<MockWebSocketClient>> m_mockWebSocketClients;
    std::shared_ptr<MockLocalConfig> m_mockLocalConfig;
    std::shared_ptr<local_config> m_localConfig;
    std::shared_ptr<MockMultiplayerSubscription> m_multiplayerSubscription;

    xbox::services::system::xbox_live_mutex m_httpLock;
    xbox::services::system::xbox_live_mutex m_websocketLock;
    std::chrono::milliseconds m_delayTime;
    std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>> m_httpStateResponses;
    std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>> m_httpApiStateResponses;
    std::queue<WebsocketMockResponse> m_websocketResponses;
    pplx::task_completion_event<void> m_websocketCompletionEvent;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

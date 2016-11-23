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
#include "pch.h"
#include "xbox_system_factory.h"
#include "MockUserTokenService.h"
#include "MockXstsTokenService.h"
#include "MockXTitleService.h"
#include "MockUser.h"
#include "MockHttpCall.h"
#include "MockHttpClient.h"
#include "MockWebSocketClient.h"
#include "MockLocalConfig.h"
#include "MockMultiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

using namespace multiplayer;

struct HttpResponseStruct
{
    std::vector<std::shared_ptr<http_call_response>> responseList;
    uint32_t counter = 0;
    std::function<void(std::shared_ptr<http_call_response>&, const string_t& requestPost)> fRequestPostFunc;
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

#if UNIT_TEST_SYSTEM
    std::shared_ptr<xsts_token_service> create_xsts_token() override { return m_mockXstsToken; }
    std::shared_ptr<user_token_service> create_user_token() override { return m_mockUserToken; }
    std::shared_ptr<title_token_service> create_title_token() override { return m_mockTitleToken; }
    std::shared_ptr<device_token_service> create_device_token() override { return nullptr; }
    std::shared_ptr<service_token_service> create_service_token() override { return m_mockServiceToken; }
    std::shared_ptr<xtitle_service> create_xtitle_service() override { return m_mockXTitle; }
#endif

    std::shared_ptr<xbox_http_client> create_http_client(
        _In_ const web::http::uri& base_uri,
        _In_ const web::http::client::http_client_config& client_config
        ) override
    { 
        return m_mockHttpClient; 
    }
    
    std::shared_ptr<http_call> create_http_call(
        _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
        _In_ const std::wstring& httpMethod,
        _In_ const std::wstring& serverName,
        _In_ const web::uri& pathQueryFragment,
        _In_ xbox_live_api xboxLiveApi
        ) override;

    std::shared_ptr<http_call_internal> create_http_call_internal(
        _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
        _In_ const string_t& httpMethod,
        _In_ const string_t& serverName,
        _In_ const web::uri& pathQueryFragment
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

    std::shared_ptr<MockXstsTokenService> GetMockXstsTokenService() { return m_mockXstsToken; }
    std::shared_ptr<MockUserTokenService> GetMockUserTokenService() { return m_mockUserToken; }
    std::shared_ptr<MockTitleTokenService> GetMockTitleTokenService() { return m_mockTitleToken; }
    std::shared_ptr<MockServiceTokenService> GetMockServiceTokenService() { return m_mockServiceToken; }
    std::shared_ptr<MockUser> GetMockUser() { return m_mockUser; }
    std::shared_ptr<MockXTitleService> GetMockXTitleService() { return m_mockXTitle; }
    std::shared_ptr<MockHttpCall> GetMockHttpCall() { return m_mockHttpCall; }
    std::shared_ptr<MockHttpClient> GetMockHttpClient() { return m_mockHttpClient; }
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
    std::shared_ptr<MockXstsTokenService> m_mockXstsToken;
    std::shared_ptr<MockUserTokenService> m_mockUserToken;
    std::shared_ptr<MockTitleTokenService> m_mockTitleToken;
    std::shared_ptr<MockServiceTokenService> m_mockServiceToken;
    std::shared_ptr<MockUser> m_mockUser;
    std::shared_ptr<MockXTitleService> m_mockXTitle;
    std::shared_ptr<MockHttpCall> m_mockHttpCall;
    std::shared_ptr<MockHttpClient> m_mockHttpClient;
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

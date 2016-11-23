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
#include "MockXboxSystemFactory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

MockXboxSystemFactory::MockXboxSystemFactory() : 
    m_setupMockForHttpClient(false),
    m_webSocketClientCounter(0)
{
    m_mockHttpCall = std::make_shared<MockHttpCall>();
    m_mockHttpClient = std::make_shared<MockHttpClient>();
    m_mockLocalConfig = std::make_shared<MockLocalConfig>();
    m_mockWebSocketClients = std::vector<std::shared_ptr<MockWebSocketClient>>();
}

std::shared_ptr<MockWebSocketClient>
MockXboxSystemFactory::GetMockWebSocketClient()
{
    std::lock_guard<std::mutex> lock(m_websocketLock.get());
    if (m_mockWebSocketClients.size() == 0)
    {
        auto webSocketCient = std::make_shared<MockWebSocketClient>();
        m_mockWebSocketClients.push_back(webSocketCient);
    }

    return m_mockWebSocketClients.at(0);
}

std::vector<std::shared_ptr<MockWebSocketClient>>
MockXboxSystemFactory::GetMockWebSocketClients()
{
    std::lock_guard<std::mutex> lock(m_websocketLock.get());
    return m_mockWebSocketClients;
}

std::vector<std::shared_ptr<MockWebSocketClient>>
MockXboxSystemFactory::AddMultipleMockWebSocketClients(uint32_t numberOfClients)
{
    std::lock_guard<std::mutex> lock(m_websocketLock.get());
    for (uint32_t i = 0; i < numberOfClients; ++i)
    {
        auto webSocketCient = std::make_shared<MockWebSocketClient>();
        m_mockWebSocketClients.push_back(webSocketCient);
    }
    return m_mockWebSocketClients;
}

void MockXboxSystemFactory::reinit()
{
    std::lock_guard<std::mutex> lock(m_httpLock.get());
    std::lock_guard<std::mutex> wsLock(m_websocketLock.get());

    m_websocketResponses = std::queue<WebsocketMockResponse>();
    m_httpStateResponses.clear();
    m_httpApiStateResponses.clear();
    m_mockHttpCall->reinit();
    m_mockHttpClient->reinit();
    m_mockLocalConfig->reinit();
    m_mockWebSocketClients.clear();
    m_webSocketClientCounter = 0;
    m_setupMockForHttpClient = false;
}

std::shared_ptr<http_call> MockXboxSystemFactory::create_http_call(
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const std::wstring& httpMethod,
    _In_ const std::wstring& serverName,
    _In_ const web::uri& pathQueryFragment,
    _In_ xbox_live_api xboxLiveApi
    )
{
    std::lock_guard<std::mutex> lock(m_httpLock.get());
    if (m_setupMockForHttpClient)
    {
        return xbox_system_factory::create_http_call(xboxLiveContextSettings, httpMethod, serverName, pathQueryFragment, xboxLiveApi);
    }

    if (m_httpStateResponses.size() > 0 || m_httpApiStateResponses.size() > 0)
    {
        m_mockHttpCall = std::make_shared<MockHttpCall>();

        auto pathQuery = pathQueryFragment.to_string();

        auto httpStateResponses = m_httpApiStateResponses[xboxLiveApi];
        if (httpStateResponses == nullptr)
        {
            httpStateResponses = m_httpStateResponses[pathQuery];
        }

        if (httpStateResponses == nullptr)
        {
            httpStateResponses = m_httpStateResponses[serverName];
        }

        if (httpStateResponses != nullptr && httpStateResponses->responseList.size() > 0)
        {
            m_mockHttpCall->ServerName = serverName;
            m_mockHttpCall->ResultValue = std::make_shared<http_call_response>(*httpStateResponses->responseList[httpStateResponses->counter]);
            m_mockHttpCall->fRequestPostFunc = httpStateResponses->fRequestPostFunc;
            if (httpStateResponses->counter + 1 < httpStateResponses->responseList.size())
            {
                ++httpStateResponses->counter;
            }

            return m_mockHttpCall;
        }
    }

    m_mockHttpCall->fRequestPostFunc = nullptr;
    m_mockHttpCall->HttpMethod = httpMethod;
    m_mockHttpCall->ServerName = serverName;
    m_mockHttpCall->PathQueryFragment = pathQueryFragment;
    m_mockHttpCall->XboxLiveApi = xboxLiveApi;

    return m_mockHttpCall;
}

std::shared_ptr<http_call_internal> MockXboxSystemFactory::create_http_call_internal(
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const string_t& httpMethod,
    _In_ const string_t& serverName,
    _In_ const web::uri& pathQueryFragment
    ) 
{
    std::lock_guard<std::mutex> lock(m_httpLock.get());
    m_mockHttpCall->HttpMethod = httpMethod;
    m_mockHttpCall->ServerName = serverName;
    m_mockHttpCall->PathQueryFragment = pathQueryFragment;
    return m_mockHttpCall;
}

void MockXboxSystemFactory::add_http_state_response(
    _In_ const std::unordered_map<string_t, std::shared_ptr<HttpResponseStruct>>& stateResponses,
    _In_ bool overrideStateValue
    )
{
    std::lock_guard<std::mutex> lock(m_httpLock.get());
    if (overrideStateValue)
    {
        m_httpStateResponses = stateResponses;
    }
    else
    {
        for (auto constResponse : stateResponses)
        {
            m_httpStateResponses[constResponse.first] = constResponse.second;
        }
    }
}

void MockXboxSystemFactory::add_http_api_state_response(
    _In_ const std::unordered_map<xbox_live_api, std::shared_ptr<HttpResponseStruct>>& stateResponses,
    _In_ bool overrideStateValue
    )
{
    std::lock_guard<std::mutex> lock(m_httpLock.get());
    if (overrideStateValue)
    {
        m_httpApiStateResponses = stateResponses;
    }
    else
    {
        for (auto constResponse : stateResponses)
        {
            m_httpApiStateResponses[constResponse.first] = constResponse.second;
        }
    }
}

void MockXboxSystemFactory::add_websocket_state_responses_to_all_clients(
    _In_ const std::queue<WebsocketMockResponse>& stateResponses,
    _In_ const pplx::task_completion_event<void>& websocketCompletionEvent
    )
{
    std::lock_guard<std::mutex> lock(m_websocketLock.get());
    m_websocketCompletionEvent = websocketCompletionEvent;
    m_websocketResponses = stateResponses;
    SetupNextWebsocketResponsesForAllClients();
}

void MockXboxSystemFactory::set_websocket_delay_send_time(
    _In_ const std::chrono::milliseconds& delayTime
    )
{
    m_delayTime = delayTime;
}

void MockXboxSystemFactory::SetupNextWebsocketResponseForDefaultClient()
{
    while (m_websocketResponses.size() > 0)
    {
        auto requestToSend = m_websocketResponses.front();
        m_websocketResponses.pop();
        if (m_mockWebSocketClients.size() > 0)
        {
            m_mockWebSocketClients.at(0)->receive_rta_event_from_uri(requestToSend.uri, requestToSend.data, requestToSend.eventType, requestToSend.isError);
        }
    }

    m_websocketCompletionEvent.set();
}

void MockXboxSystemFactory::SetupNextWebsocketResponsesForAllClients()
{
    while (m_websocketResponses.size() > 0)
    {
        size_t size = m_websocketResponses.size();
        auto requestToSend = m_websocketResponses.front();
        m_websocketResponses.pop();
        for (auto webSocketClient : m_mockWebSocketClients)
        {
            webSocketClient->receive_rta_event_from_uri(requestToSend.uri, requestToSend.data, requestToSend.eventType, requestToSend.isError);
        }
    }

    m_websocketCompletionEvent.set();
}

void MockXboxSystemFactory::clear_states()
{
    std::lock_guard<std::mutex> lock(m_httpLock.get());
    std::lock_guard<std::mutex> wsLock(m_websocketLock.get());
    m_httpStateResponses.clear();
    m_httpApiStateResponses.clear();
    m_websocketResponses = std::queue<WebsocketMockResponse>();
}

std::shared_ptr<local_config> 
MockXboxSystemFactory::create_local_config()
{
    return m_mockLocalConfig;
}

static std::mutex s_mockCreateLock;

std::shared_ptr<user_impl>
MockXboxSystemFactory::create_user_impl(user_creation_context userCreationContext)
{
    std::lock_guard<std::mutex> guard(s_mockCreateLock);
    m_mockUser = std::make_shared<MockUser>();
    return m_mockUser;
}

std::shared_ptr<multiplayer_subscription>
MockXboxSystemFactory::create_multiplayer_subscription(
    _In_ const std::function<void(const multiplayer_session_change_event_args&)>& multiplayerSessionChangeHandler,
    _In_ const std::function<void()>& multiplayerSubscriptionLostHandler,
    _In_ const std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)>& subscriptionErrorHandler
)
{
    std::lock_guard<std::mutex> guard(s_mockCreateLock);

    m_multiplayerSubscription = std::make_shared<MockMultiplayerSubscription>(
        multiplayerSessionChangeHandler,
        multiplayerSubscriptionLostHandler,
        subscriptionErrorHandler
        );
    return m_multiplayerSubscription;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

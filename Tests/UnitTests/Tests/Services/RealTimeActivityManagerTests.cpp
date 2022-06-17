// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "real_time_activity_manager.h"
#include "real_time_activity_connection.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(RealTimeActivityManagerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(RealTimeActivityManagerTests)

    class TestSubscription : public real_time_activity::Subscription
    {
    public:
        TestSubscription(
            String uri = "https://testsubscription"
        ) noexcept
            : Uri{ m_resourceUri }
        {
            m_resourceUri = std::move(uri);
        }

        Event SubscribeComplete;
        Event EventReceived;

        uint32_t SubscribeCompleteCount{ 0 };
        uint32_t EventReceivedCount{ 0 };

        const String& Uri;

    private:
        void OnSubscribe(const JsonValue& data) noexcept override
        {
            UNREFERENCED_PARAMETER(data);
            SubscribeCompleteCount++;
            SubscribeComplete.Set();
        }

        void OnEvent(const JsonValue& data) noexcept override
        {
            UNREFERENCED_PARAMETER(data);
            EventReceivedCount++;
            EventReceived.Set();
        }

        void OnResync() noexcept {};
    };

    struct RtaConnectionMonitor
    {
        RtaConnectionMonitor(XblContextHandle xboxLiveContext) noexcept
        {
            VERIFY_SUCCEEDED(XblContextDuplicateHandle(xboxLiveContext, &m_xboxLiveContext));
            m_token = XblRealTimeActivityAddConnectionStateChangeHandler(m_xboxLiveContext, ConnectionStateChanged, this);
        }

        ~RtaConnectionMonitor() noexcept
        {
            XblRealTimeActivityRemoveConnectionStateChangeHandler(m_xboxLiveContext, m_token);
            XblContextCloseHandle(m_xboxLiveContext);
        }

        Event Connecting;
        Event Connected;
        Event Disconnected;

    private:
        static void CALLBACK ConnectionStateChanged(void* context, XblRealTimeActivityConnectionState state)
        {
            auto pThis{ static_cast<RtaConnectionMonitor*>(context) };
            switch (state)
            {
            case XblRealTimeActivityConnectionState::Connecting:
            {
                pThis->Connecting.Set();
                break;
            }
            case XblRealTimeActivityConnectionState::Connected:
            {
                pThis->Connected.Set();
                break;
            }
            case XblRealTimeActivityConnectionState::Disconnected:
            {
                pThis->Disconnected.Set();
                break;
            }
            }
        }

        XblContextHandle m_xboxLiveContext{ nullptr };
        XblFunctionContext m_token{ 0 };
    };

    DEFINE_TEST_CASE(SubscriptionTest)
    {
        TEST_LOG(L"Test starting: SubscriptionTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto& mockRta{ MockRealTimeActivityService::Instance() };

        Event completeSubscribeHandshake;
        mockRta.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            completeSubscribeHandshake.Wait();
            mockRta.CompleteSubscribeHandshake(n);
        });

        // Testing adding and removing subscriptions
        auto subscription = std::make_shared<TestSubscription>();
        auto rtaManager{ GlobalState::Get()->RTAManager() };

        VERIFY_SUCCEEDED(rtaManager->AddSubscription(xboxLiveContext->User(), subscription));
        completeSubscribeHandshake.Set();
        subscription->SubscribeComplete.Wait();

        mockRta.RaiseEvent(subscription->Uri.data(), rapidjson::Document{ rapidjson::kObjectType });
        subscription->EventReceived.Wait(); // data was successfully received and propagated

        VERIFY_SUCCEEDED(rtaManager->RemoveSubscription(xboxLiveContext->User(), subscription));
    }

    DEFINE_TEST_CASE(TestUnexpectedDataReceived)
    {
        TEST_LOG(L"Test starting: TestUnexpectedDataReceived");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto& mockRta{ MockRealTimeActivityService::Instance() };
        auto rtaManager{ GlobalState::Get()->RTAManager() };

        auto subscription = std::make_shared<TestSubscription>();
        VERIFY_SUCCEEDED(rtaManager->AddSubscription(xboxLiveContext->User(), subscription));

        VERIFY_NO_THROW(mockRta.RaiseEvent(subscription->Uri.data(), rapidjson::Document{ rapidjson::kObjectType }));
    }

    DEFINE_TEST_CASE(SubscriptionStressTest)
    {
        TEST_LOG(L"Test starting: SubscriptionStressTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto& mockRta{ MockRealTimeActivityService::Instance() };

        mockRta.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            mockRta.CompleteSubscribeHandshake(n);
        });

        // Testing adding and removing subscriptions
        auto rtaManager{ GlobalState::Get()->RTAManager() };
        RtaConnectionMonitor connectionMonitor{ xboxLiveContext.get() };

        constexpr size_t subscriptionCount{ 100 };
        std::array<std::shared_ptr<TestSubscription>, 100> subscriptions;
        for (size_t i = 0; i < subscriptions.size(); ++i)
        {
            Stringstream uri;
            uri << "https://uri" << i;
            subscriptions[i] = std::make_shared<TestSubscription>(uri.str());
            VERIFY_SUCCEEDED(rtaManager->AddSubscription(xboxLiveContext->User(), subscriptions[i]));
        }

        // Adding connections should force the websocket to be connected
        connectionMonitor.Connecting.Wait();
        connectionMonitor.Connected.Wait();

        for (auto& subscription : subscriptions)
        {
            subscription->SubscribeComplete.Wait();
        }

        for (size_t i = 0; i < subscriptions.size(); i += subscriptions.size() / 20)
        {
            mockRta.RaiseEvent(subscriptions[i]->Uri.data(), rapidjson::Document{ rapidjson::kObjectType });
            subscriptions[i]->EventReceived.Wait();
        }

        // make it reconnect and re-subscribe
        mockRta.DisconnectClient(xboxLiveContext->Xuid());
        connectionMonitor.Connecting.Wait();
        connectionMonitor.Connected.Wait();

        for (auto& subscription : subscriptions)
        {
            subscription->SubscribeComplete.Wait();
        }

        // stress test adding and removing subscriptions quickly
        for (auto& subscription : subscriptions)
        {
            VERIFY_SUCCEEDED(rtaManager->RemoveSubscription(xboxLiveContext->User(), subscription));
			subscription = std::make_shared<TestSubscription>(subscription->Uri);
            VERIFY_SUCCEEDED(rtaManager->AddSubscription(xboxLiveContext->User(), subscription));
            VERIFY_SUCCEEDED(rtaManager->RemoveSubscription(xboxLiveContext->User(), subscription));
        }

        // Socket should automatically disconnect after all subs are removed
        connectionMonitor.Disconnected.Wait();
    }

    DEFINE_TEST_CASE(TestUnsubscribeOnPendingSubscribeState)
    {
        TEST_LOG(L"Test starting: TestUnsubscribeOnPendingSubscribeState");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto rtaManager{ GlobalState::Get()->RTAManager() };

        Event websocketConnectCompletion;
        MockWebsocket::SetConnectHandler([&]
        {
            // Don't complete connection immediately
            websocketConnectCompletion.Wait();
            return WebsocketResult{ S_OK };
        });

        RtaConnectionMonitor monitor{ xboxLiveContext.get() };

        auto subscription = std::make_shared<TestSubscription>();
        VERIFY_SUCCEEDED(rtaManager->AddSubscription(xboxLiveContext->User(), subscription));

        // Wait for state change to connecting 
        monitor.Connecting.Wait();

        VERIFY_SUCCEEDED(rtaManager->RemoveSubscription(xboxLiveContext->User(), subscription));
		subscription = std::make_shared<TestSubscription>();

        websocketConnectCompletion.Set();

        // 2. Make websocket connected and try it again with holding subscribe handshake response.
        MockWebsocket::SetConnectHandler(nullptr);

        VERIFY_SUCCEEDED(rtaManager->AddSubscription(xboxLiveContext->User(), subscription));
        VERIFY_SUCCEEDED(rtaManager->RemoveSubscription(xboxLiveContext->User(), subscription));

        // Verify the subscription was never completed
        VERIFY_ARE_EQUAL_UINT(0u, subscription->SubscribeCompleteCount);
    }

    DEFINE_TEST_CASE(NoConnectionTest)
    {
        TEST_LOG(L"Test starting: NoConnectionTest");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto rtaManager{ GlobalState::Get()->RTAManager() };

        RtaConnectionMonitor connectionMonitor{ xboxLiveContext.get() };

        MockWebsocket::SetConnectHandler([] { return WebsocketResult{ E_FAIL }; });

        auto subscription = std::make_shared<TestSubscription>();
        VERIFY_SUCCEEDED(rtaManager->AddSubscription(xboxLiveContext->User(), subscription));

        connectionMonitor.Connecting.Wait();
        connectionMonitor.Disconnected.Wait();

        MockWebsocket::SetConnectHandler(nullptr);

        connectionMonitor.Connected.Wait();
    }

    DEFINE_TEST_CASE(TestConnectionInterrupt)
    {
        TEST_LOG(L"Test starting: TestConnectionInterrupt");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto rtaManager{ GlobalState::Get()->RTAManager() };

        auto& mockRta{ MockRealTimeActivityService::Instance() };
        mockRta.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            mockRta.CompleteSubscribeHandshake(n);
        });

        RtaConnectionMonitor connectionMonitor{ xboxLiveContext.get() };

        auto subscription = std::make_shared<TestSubscription>();
        VERIFY_SUCCEEDED(rtaManager->AddSubscription(xboxLiveContext->User(), subscription));

        connectionMonitor.Connecting.Wait();
        connectionMonitor.Connected.Wait();
        subscription->SubscribeComplete.Wait();

        // force to close and not able to reconnect
        MockWebsocket::SetConnectHandler([] { return WebsocketResult{ E_FAIL }; });
        mockRta.DisconnectClient(xboxLiveContext->Xuid());

        connectionMonitor.Disconnected.Wait();

        MockWebsocket::SetConnectHandler(nullptr);
        connectionMonitor.Connected.Wait();
        subscription->SubscribeComplete.Wait();

        // Make sure we can receive an event on re-established connection
        mockRta.RaiseEvent(subscription->Uri.data(), rapidjson::Document{ rapidjson::kObjectType });
        subscription->EventReceived.Wait();

        VERIFY_SUCCEEDED(rtaManager->RemoveSubscription(xboxLiveContext->User(), subscription));
    }

    DEFINE_TEST_CASE(RTAResync)
    {
        TEST_LOG(L"Test starting: RTAResync");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto rtaManager{ GlobalState::Get()->RTAManager() };

        auto& mockRta{ MockRealTimeActivityService::Instance() };
        mockRta.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            mockRta.CompleteSubscribeHandshake(n);
        });

        Event resyncCallbackReceived;
        rtaManager->AddResyncHandler(xboxLiveContext->User(), [&]
        {
            resyncCallbackReceived.Set();
        });

        auto subscription = std::make_shared<TestSubscription>();
        VERIFY_SUCCEEDED(rtaManager->AddSubscription(xboxLiveContext->User(), subscription));
        subscription->SubscribeComplete.Wait();

        MockRealTimeActivityService::Instance().RaiseResync();
        resyncCallbackReceived.Wait();
    }

    DEFINE_TEST_CASE(TestServiceThrottleError)
    {
        TEST_LOG(L"Test starting: TestServiceThrottleError");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto rtaManager{ GlobalState::Get()->RTAManager() };

        // Have MockRTA service respond with a throttle error the first time we try to subscribe.
        // RTAManager should auto retry the subscription after a small backoff. Subsequent subscribe attempts will
        // be responded to with success code
        auto& mockRta{ MockRealTimeActivityService::Instance() };
        uint8_t subscribeAttempts{ 0 };

        mockRta.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
        {
            if (subscribeAttempts++ == 0)
            {
                mockRta.CompleteSubscribeHandshake(n, rapidjson::Document{ rapidjson::kNullType }, MockRealTimeActivityService::ErrorCode::Throttled);
            }
            else
            {
                mockRta.CompleteSubscribeHandshake(n);
            }
        });

        auto subscription = std::make_shared<TestSubscription>();
        VERIFY_SUCCEEDED(rtaManager->AddSubscription(xboxLiveContext->User(), subscription));
        subscription->SubscribeComplete.Wait();

        VERIFY_ARE_EQUAL_UINT(2, subscribeAttempts);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

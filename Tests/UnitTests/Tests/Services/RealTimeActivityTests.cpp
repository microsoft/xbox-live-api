// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"RealTimeActivity"
#include "UnitTestIncludes.h"

#include "web_socket_connection.h"
#include "RealTimeActivityService_WinRT.h"
#include "xsapi/real_time_activity.h"
#include "RtaTestHelper.h"
#include "SocialManager_WinRT.h"
#include "MultiplayerManager_WinRT.h"

using namespace Microsoft::Xbox::Services::RealTimeActivity;
using namespace xbox::services::real_time_activity;
using namespace Microsoft::Xbox::Services::Social::Manager;
using namespace Microsoft::Xbox::Services::Multiplayer::Manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#define LOBBY_TEMPLATE_NAME                 L"MockLobbySessionTemplateName"

const string_t rtaSubscriptionsResponseJson = LR"(
    [1,1,0,0,{}]
)";

const string_t rtaUpdateJson = LR"(
    [3,0,{}]
)";

const string_t rtaUnsubscribeJson = LR"(
    [2,2,0]
)";

const string_t rtaTooManySubsErrorMessage = LR"(
    [1,1,1,"error message"]
)";

const string_t rtaAccessDeniedErrorMessage = LR"(
    [1,2,2,"error message"]
)";

const string_t rtaGenericErrorMessage = LR"(
    [1,3,3,"error message"]
)";

const string_t rtaResyncMessage = LR"(
    [4]
)";

class TestSubscription : public real_time_activity_subscription
{
public:
    TestSubscription(std::function<void(xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)> errFunc) : real_time_activity_subscription(errFunc) {}

    void on_event_received(_In_ const web::json::value& data) override
    {
        UNREFERENCED_PARAMETER(data);
        recieved_data = true;
    }

    void on_state_changed(_In_ real_time_activity_subscription_state state) override
    {
        if (state == real_time_activity_subscription_state::pending_subscribe)
        {
            pendingSubEvent.set();
        }
        else if (state == real_time_activity_subscription_state::subscribed)
        {
            subscribedEvent.set();
        }
        else if (state == real_time_activity_subscription_state::closed)
        {
            closedEvent.set();
        }
        else if (state == real_time_activity_subscription_state::pending_unsubscribe)
        {
            pendingUnsubEvent.set();
        }
    }

    void reset()
    {
        pendingSubEvent.reset();
        subscribedEvent.reset();
        closedEvent.reset();
        pendingUnsubEvent.reset();
        recieved_data = false;
    }

    bool recieved_data = false;

    concurrency::event pendingSubEvent;
    concurrency::event subscribedEvent;
    concurrency::event closedEvent;
    concurrency::event pendingUnsubEvent;

};

DEFINE_TEST_CLASS(RealTimeActivityTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(RealTimeActivityTests)

    std::shared_ptr<xbox_live_context_settings> GetDefaultHttpSetting()
    {
        auto httpSetting = std::make_shared<xbox_live_context_settings>();
        httpSetting->set_websocket_timeout_window(std::chrono::seconds(0));
        return httpSetting;
    }

    void SendEvent(std::shared_ptr<MockWebSocketClient> ws, int id)
    {
        stringstream_t subscriptionResponse;
        subscriptionResponse << "[3,";    //[3,(subscriptionNum),{}]
        subscriptionResponse << id;
        subscriptionResponse << ",{}]";
        ws->recieve_message(subscriptionResponse.str());
    }


    DEFINE_TEST_CASE(SubscriptionTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(SubscriptionTest);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, L"{}", 0, false);

        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        xboxLiveContext->RealTimeActivityService->Activate();

        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();
        
        // Testing adding and removing subscriptions
        auto subscription = std::make_shared<TestSubscription>(
        ([this](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)
        {
            args.err_message();
        }));
        
        auto result = nativeRTA->_Add_subscription(subscription);
        LOG_DEBUG(result.err().message());
        LOG_DEBUG(result.err_message());
        VERIFY_IS_TRUE(!result.err());

        subscription->pendingSubEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::pending_subscribe);

        // wait rta to be connected before sending anything into the wire
        helper->connectedEvent.wait();
        mockSocket->recieve_message(rtaSubscriptionsResponseJson);

        subscription->subscribedEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::subscribed);
        VERIFY_ARE_EQUAL_INT(subscription->subscription_id(), 0);   // subscription was successfully created

        mockSocket->recieve_message(rtaUpdateJson);

        VERIFY_IS_TRUE(subscription->recieved_data);      // data was successfully received and propagated

        nativeRTA->_Remove_subscription(subscription);

        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::pending_unsubscribe);

        mockSocket->recieve_message(rtaUnsubscribeJson);

        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::closed);

        VERIFY_ARE_EQUAL_INT(nativeRTA->_Subscription_Count(), 0);
    }

    DEFINE_TEST_CASE(TestUnexpectedDataReceived)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestUnexpectedDataReceived);
        auto user = SignInUserWithMocks_WinRT();
        auto userContext = std::make_shared<user_context>(user);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);

        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();

        xboxLiveContext->RealTimeActivityService->Activate();

        VERIFY_NO_THROW(
            mockSocket->recieve_message(rtaSubscriptionsResponseJson);
        );

        VERIFY_NO_THROW(
            mockSocket->recieve_message(rtaUnsubscribeJson);
        );

        VERIFY_NO_THROW(
            mockSocket->recieve_message(rtaUpdateJson);
        );
    }

    DEFINE_TEST_CASE(TestSubscriptionBeforeActivate)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestSubscriptionBeforeActivate);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();

        auto subscription = std::make_shared<TestSubscription>(
        ([this](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)
        {
            args.err_message();
        }));

        auto addResult = nativeRTA->_Add_subscription(subscription);
        VERIFY_IS_TRUE(addResult.err().operator bool());

        nativeRTA->activate();

        VERIFY_ARE_EQUAL_INT(nativeRTA->_Subscription_Count(), 0);
   }

    DEFINE_TEST_CASE(MessageReceivedBeforeActivation)
    {
        DEFINE_TEST_CASE_PROPERTIES(MessageReceivedBeforeActivation);
        // Test works if no exception was thrown
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();

        std::shared_ptr<MockWebSocketClient> mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();

        mockSocket->recieve_message(rtaSubscriptionsResponseJson);
    }

    DEFINE_TEST_CASE(SubscriptionStressTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(SubscriptionStressTest);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, L"{}");

        xboxLiveContext->RealTimeActivityService->Activate();

        const uint32 subscriptionTestAmount = 100;
        std::vector<std::shared_ptr<TestSubscription>> subscriptionList(subscriptionTestAmount);
        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();
        for (uint32 i = 0; i < subscriptionTestAmount; ++i)
        {
            auto subscription = std::make_shared<TestSubscription>(
            ([this](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)
            {
                args.err_message();
            }));
            subscriptionList[i] = subscription;
            auto result = nativeRTA->_Add_subscription(
                subscription
                );

            VERIFY_IS_TRUE(!result.err());
        }

        for (auto& subscription : subscriptionList)
        {
            subscription->subscribedEvent.wait();
            VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::subscribed);
        }

        const uint32 skipAmount = subscriptionTestAmount / 20;

        for (uint32 i = 0; i < subscriptionTestAmount; i += skipAmount)
        {
            int id = subscriptionList[i]->subscription_id();
            SendEvent(mockSocket, id);
            VERIFY_IS_TRUE(subscriptionList[i]->recieved_data);
        }

        for (auto& subscription : subscriptionList)
        {
            subscription->reset();
        }
        // make it reconnect and re-subscribe
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        for (auto& subscription : subscriptionList)
        {
            subscription->subscribedEvent.wait();
            VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::subscribed);
        }

        for (auto& subscription : subscriptionList)
        {
            auto result = nativeRTA->_Remove_subscription(subscription);
            VERIFY_IS_TRUE(!result.err());
        }

        for (auto& subscription : subscriptionList)
        {
            subscription->closedEvent.wait();
            VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::closed);
        }

        VERIFY_ARE_EQUAL_INT(nativeRTA->_Subscription_Count(), 0);
    }

    DEFINE_TEST_CASE(TestUnsubscribeOnPendingSubscribeState)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestUnsubscribeOnPendingSubscribeState);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, L"{}", 0, false);

        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);

        //1. Test remove subscription on pending_subscribe state
        TEST_LOG(L"1. Test remove subscription on pending_subscribe state");
        //Make rta stay on connecting state
        mockSocket->m_waitForSignal = true;
        xboxLiveContext->RealTimeActivityService->Activate();

        //Wait for state change to connecting 
        helper->connectingEvent.wait();

        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();

        // Testing adding and removing subscriptions
        auto subscription = std::make_shared<TestSubscription>(
            [this](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)
        {
            args.err_message();
        });

        nativeRTA->_Add_subscription(subscription);

        subscription->pendingSubEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::pending_subscribe);

        nativeRTA->_Remove_subscription(subscription);
        subscription->closedEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::closed);

        VERIFY_ARE_EQUAL_INT(nativeRTA->_Subscription_Count(), 0);

        //2. Make websocket connected and try it again with holding response.
        subscription->reset();
        mockSocket->m_connectEvent.set();

        nativeRTA->_Add_subscription(subscription);
        subscription->pendingSubEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::pending_subscribe);

        nativeRTA->_Remove_subscription(subscription);
        subscription->closedEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::closed);

        VERIFY_ARE_EQUAL_INT(nativeRTA->_Subscription_Count(), 0);
    }

    DEFINE_TEST_CASE(TestUnsubscribeOnPendingUnsubscribeState)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestUnsubscribeOnPendingUnsubscribeState);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();

        pplx::task_completion_event<void> tce;
        pplx::task<void> taskXbl = pplx::create_task(tce);
        auto token = xboxLiveContext->RealTimeActivityService->RealTimeActivityConnectionStateChange += ref new Windows::Foundation::EventHandler<Microsoft::Xbox::Services::RealTimeActivity::RealTimeActivityConnectionState>([tce](Platform::Object^ sender, RealTimeActivityConnectionState state)
        {
            if (state == RealTimeActivityConnectionState::Connected)
            {
                tce.set();
            }
        });
        SetWebSocketRTAAutoResponser(mockSocket, L"{}", 0, false);
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        xboxLiveContext->RealTimeActivityService->Activate();

        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();
        // Testing adding and removing subscriptions
        auto subscription = std::make_shared<TestSubscription>(
            ([this](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)
        {
            args.err_message();
        }));

        // Wait for subscribed.
        auto result = nativeRTA->_Add_subscription(subscription);
        if (result.err())
        {
            LOG_DEBUG(result.err().message());
            LOG_DEBUG(result.err_message());
        }
        VERIFY_IS_TRUE(!result.err());

        subscription->pendingSubEvent.wait();
        taskXbl.wait();
        mockSocket->recieve_message(rtaSubscriptionsResponseJson);
        subscription->subscribedEvent.wait();

        nativeRTA->_Remove_subscription(subscription);
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::pending_unsubscribe);

        // calling again, should be alright.
        nativeRTA->_Remove_subscription(subscription);
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::pending_unsubscribe);

        mockSocket->recieve_message(rtaUnsubscribeJson);
        subscription->closedEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::closed);

        VERIFY_ARE_EQUAL_INT(nativeRTA->_Subscription_Count(), 0);

        xboxLiveContext->RealTimeActivityService->RealTimeActivityConnectionStateChange -= token;
    }

    DEFINE_TEST_CASE(TestRTAInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestRTAInvalidArgs);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        xboxLiveContext->RealTimeActivityService->Activate();
        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();

        xbox_live_result<void> result;

        result = nativeRTA->_Add_subscription(nullptr);

        VERIFY_IS_TRUE(result.err() == xbox_live_error_code::invalid_argument);

        result = nativeRTA->_Remove_subscription(nullptr);

        VERIFY_IS_TRUE(result.err() == xbox_live_error_code::invalid_argument);

        VERIFY_ARE_EQUAL_INT(nativeRTA->_Subscription_Count(), 0);
    }

    DEFINE_TEST_CASE(NoConnectionTest)
    {
        DEFINE_TEST_CASE_PROPERTIES(NoConnectionTest);
        // Make connection time out
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        TimeSpan ts;
        ts.Duration = 1000;
        xboxLiveContext->Settings->WebsocketTimeoutWindow = ts;

        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);

        mockSocket->m_connectToFail = true;
        xboxLiveContext->RealTimeActivityService->Activate();

        helper->disconnectedEvent.wait();

        // Will be connected somehow
        helper->reset_events();
        mockSocket->m_connectToFail = false;
        helper->connectedEvent.wait();
    }

    DEFINE_TEST_CASE(TestConnectionInterrupt)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestConnectionInterrupt);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, L"{}");
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);

        xboxLiveContext->RealTimeActivityService->Activate();
        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();

        auto subscription = std::make_shared<TestSubscription>(
        ([this](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)
        {
            args.err_message();
        }));
        nativeRTA->_Add_subscription(subscription);

        subscription->subscribedEvent.wait();
        auto oldSubId = subscription->subscription_id();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::subscribed);

        // force to close and not reconnectedable
        mockSocket->m_connectToFail = true;
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);

        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::pending_subscribe);

        // reconnect 
        helper->reset_events();
        mockSocket->m_connectToFail = false;

        // Althrough we fire close event multiply times, make sure there is only one reconnect event.
        helper->connectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(helper->connected, 1);

        // Verify subscription gets re-subscribed, 
        // our test generate subscription id based on a static counter, should never be the same once re-subscribe
        auto newSubId = subscription->subscription_id();
        VERIFY_ARE_NOT_EQUAL(oldSubId, newSubId);

        // TODO: flakey test
        SendEvent(mockSocket, newSubId);
        //VERIFY_IS_TRUE(subscription->recieved_data);

        nativeRTA->_Remove_subscription(subscription);
        subscription->closedEvent.wait();

        subscription->recieved_data = false;
        SendEvent(mockSocket, newSubId);
        //VERIFY_IS_FALSE(subscription->recieved_data);

        VERIFY_ARE_EQUAL_INT(nativeRTA->_Subscription_Count(), 0);
    }

    DEFINE_TEST_CASE(TestConnectionLost)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestConnectionLost);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, L"{}");
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        TimeSpan ts;
        ts.Duration = 1000;
        xboxLiveContext->Settings->WebsocketTimeoutWindow = ts;

        xboxLiveContext->RealTimeActivityService->Activate();
        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();

        auto subscription = std::make_shared<TestSubscription>(
        ([this](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)
        {
        }));
        nativeRTA->_Add_subscription(subscription);

        subscription->subscribedEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::subscribed);

        // force to close and not reconnectedable
        mockSocket->m_connectToFail = true;
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        subscription->pendingSubEvent.wait();

        // wait the connection to be lost after timeout
        helper->disconnectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::closed);

        // reconnect 
        helper->reset_events();
        mockSocket->m_connectToFail = false;
        helper->connectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::closed);

        // send event, make sure not received
        SendEvent(mockSocket, subscription->subscription_id());
        VERIFY_IS_FALSE(subscription->recieved_data);

        VERIFY_ARE_EQUAL_INT(nativeRTA->_Subscription_Count(), 0);
    }

    DEFINE_TEST_CASE(RTAResync)
    {
        DEFINE_TEST_CASE_PROPERTIES(RTAResync);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        nativeRTA->activate();

        concurrency::event fireEvent;
        nativeRTA->add_resync_handler(
            [this, &fireEvent]()
            {
                fireEvent.set();
            }
            );

        mockSocket->recieve_message(rtaResyncMessage);
        fireEvent.wait();
        fireEvent.reset();
    }

    DEFINE_TEST_CASE(SubscriptionError)
    {
        DEFINE_TEST_CASE_PROPERTIES(SubscriptionError);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        SetWebSocketRTAAutoResponser(mockSocket, L"{}", -1, false);
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();

        bool gotErr = false;
        auto subscription = std::make_shared<TestSubscription>(
        ([this, nativeRTA](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)
        {
            nativeRTA->_Trigger_subscription_error(
                args
                );
        }));
        nativeRTA->activate();
        TEST_LOG(L"wait for RTA connected");

        uint32_t hitNum = 0;
        concurrency::event fireEvent;

        nativeRTA->add_subscription_error_handler(
        ([this, &gotErr, &hitNum, &fireEvent, nativeRTA](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)
        {
            if (args.err() == xbox_live_error_condition::generic_error)
            {
                if (hitNum == 0)
                {
                    gotErr = args.err() == xbox_live_error_code::rta_subscription_limit_reached;
                }
                else if (hitNum == 1)
                {
                    gotErr = args.err() == xbox_live_error_code::rta_access_denied;
                }
                else if (hitNum == 2)
                {
                    gotErr = args.err() == xbox_live_error_code::rta_generic_error;
                }
            }

            ++hitNum;
            fireEvent.set();
        }));

        nativeRTA->_Add_subscription(subscription);
        subscription->pendingSubEvent.wait();

        // wait rta to be connected before sending anything into the wire
        helper->connectedEvent.wait();
        mockSocket->recieve_message(rtaTooManySubsErrorMessage);
        TEST_LOG(L"wait for rtaTooManySubsErrorMessage result");
        fireEvent.wait();
        fireEvent.reset();
        VERIFY_IS_TRUE(gotErr);


        gotErr = false;
        nativeRTA->_Add_subscription(subscription);
        subscription->pendingSubEvent.wait();
        mockSocket->recieve_message(rtaAccessDeniedErrorMessage);
        TEST_LOG(L"wait for rtaAccessDeniedErrorMessage result");
        fireEvent.wait();
        fireEvent.reset();
        VERIFY_IS_TRUE(gotErr);


        gotErr = false;
        nativeRTA->_Add_subscription(subscription);
        subscription->pendingSubEvent.wait();
        mockSocket->recieve_message(rtaGenericErrorMessage);
        TEST_LOG(L"wait for rtaGenericErrorMessage result");
        fireEvent.wait();
        fireEvent.reset();
        VERIFY_IS_TRUE(gotErr);
        nativeRTA->deactivate();
    }

    DEFINE_TEST_CASE(RTADeactivate)
    {
        DEFINE_TEST_CASE_PROPERTIES(RTADeactivate);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();

        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);

        TEST_LOG(L"Test Activate and Deactivate");
        xboxLiveContext->RealTimeActivityService->Activate();
        helper->connectedEvent.wait();

        xboxLiveContext->RealTimeActivityService->Deactivate();

        TEST_LOG(L"Test re-Activate and re-Deactivate");
        // Test can be re-activated
        helper->reset_events();
        xboxLiveContext->RealTimeActivityService->Activate();
        helper->connectedEvent.wait();
        SetWebSocketRTAAutoResponser(mockSocket, L"{}");

        // Add some subscription then deactivate
        auto nativeRTA = xboxLiveContext->RealTimeActivityService->GetCppObj();
        auto subscription = std::make_shared<TestSubscription>(
            ([this](xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args)
        {
        }));
        nativeRTA->_Add_subscription(subscription);
        subscription->subscribedEvent.wait();
        VERIFY_ARE_EQUAL_INT(subscription->state(), real_time_activity_subscription_state::subscribed);

        xboxLiveContext->RealTimeActivityService->Deactivate();

        TEST_LOG(L"Make sure subscription close properly");
        //Event should change state to pending unsubscribe then close
        subscription->closedEvent.wait();

        VERIFY_ARE_EQUAL_INT(nativeRTA->_Subscription_Count(), 0);
    }

    DEFINE_TEST_CASE(RTAThreadSafe)
    {
        DEFINE_TEST_CASE_PROPERTIES(RTAThreadSafe);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);

        const int threadCount = MAXIMUM_WEBSOCKETS_ACTIVATIONS_ALLOWED_PER_USER;
        std::vector<task<void>> tasks;
        for (int i = 0; i < threadCount; i++)
        {
            tasks.push_back(create_task([xboxLiveContext]() {
                xboxLiveContext->RealTimeActivityService->Activate();
            }));
        }

        when_all(tasks.begin(), tasks.end()).wait();
        tasks.clear();

        for (int i = 0; i < threadCount; i++)
        {
            tasks.push_back(create_task([xboxLiveContext]() {
                xboxLiveContext->RealTimeActivityService->Deactivate();
            }));
        }

        when_all(tasks.begin(), tasks.end()).wait();
        tasks.clear();
    }

    DEFINE_TEST_CASE(RTAMaxWebsocketTest_1)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(RTAMaxWebsocketTest_1);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        std::unordered_map<xsapi_internal_string, uint32_t> rtaActivationMap;

        xsapi_internal_string xuid = utils::internal_string_from_utf16(xboxLiveContext->User->XboxUserId->Data());
        {
            // Ensure we decrement the counter when the context is set to null.
            auto xboxLiveContextTest = GetMockXboxLiveContext_WinRT();
            xboxLiveContextTest->RealTimeActivityService->Activate();
            xboxLiveContextTest = nullptr;
            do
            {
                rtaActivationMap = real_time_activity_service::_Rta_activation_map();
            } while (rtaActivationMap.size() != 0);
            VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 0);
        }
        {
            // Ensure we decrement the counter when the context dtor is called.
            auto xboxLiveContextTest = GetMockXboxLiveContext_WinRT();
            xboxLiveContextTest->RealTimeActivityService->Activate();
        }

        do 
        {
            rtaActivationMap = real_time_activity_service::_Rta_activation_map();
        } while (rtaActivationMap.size() != 0);

        VERIFY_IS_TRUE(rtaActivationMap.find(xuid) == rtaActivationMap.end());

        {
            auto xboxLiveContextTest = GetMockXboxLiveContext_WinRT();
            xboxLiveContextTest->RealTimeActivityService->Activate();
            xboxLiveContextTest->RealTimeActivityService->Deactivate();
        }
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 0);
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map().size() == 0);
        

        for (int i = 0; i < MAXIMUM_WEBSOCKETS_ACTIVATIONS_ALLOWED_PER_USER + 1; i++)
        {
            VERIFY_NO_THROW(xboxLiveContext->RealTimeActivityService->Activate());
        }
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 1);

        for (int i = 0; i < MAXIMUM_WEBSOCKETS_ACTIVATIONS_ALLOWED_PER_USER + 1; i++)
        {
            VERIFY_NO_THROW(xboxLiveContext->RealTimeActivityService->Deactivate());
        }
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 0);
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map().size() == 0);

        auto xboxLiveContext1 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext2 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext3 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext4 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext5 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext6 = GetMockXboxLiveContext_WinRT();

        VERIFY_NO_THROW(xboxLiveContext1->RealTimeActivityService->Activate());
        VERIFY_NO_THROW(xboxLiveContext2->RealTimeActivityService->Activate());
        VERIFY_NO_THROW(xboxLiveContext3->RealTimeActivityService->Activate());
        VERIFY_NO_THROW(xboxLiveContext4->RealTimeActivityService->Activate());
        VERIFY_NO_THROW(xboxLiveContext5->RealTimeActivityService->Activate());
        VERIFY_THROWS_HR_CX(xboxLiveContext6->RealTimeActivityService->Activate(), E_INVALIDARG);       // This will auto decrement the counter before asserting.
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 5);

        VERIFY_NO_THROW(xboxLiveContext1->RealTimeActivityService->Deactivate());
        VERIFY_NO_THROW(xboxLiveContext2->RealTimeActivityService->Deactivate());
        VERIFY_NO_THROW(xboxLiveContext3->RealTimeActivityService->Deactivate());
        VERIFY_NO_THROW(xboxLiveContext4->RealTimeActivityService->Deactivate());
        VERIFY_NO_THROW(xboxLiveContext5->RealTimeActivityService->Deactivate());
        VERIFY_NO_THROW(xboxLiveContext6->RealTimeActivityService->Deactivate());
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 0);
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map().size() == 0);
    }

    DEFINE_TEST_CASE(RTAMaxWebsocketTest_2)
    {
        DEFINE_TEST_CASE_PROPERTIES_IGNORE(RTAMaxWebsocketTest_2);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);
        xsapi_internal_string xuid = utils::internal_string_from_utf16(xboxLiveContext->User->XboxUserId->Data());

        auto xboxLiveContext1 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext2 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext3 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext4 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext5 = GetMockXboxLiveContext_WinRT();
        auto xboxLiveContext6 = GetMockXboxLiveContext_WinRT();

        // Both managers use 1 websocket connection
        auto socialManager = SocialManager::SingletonInstance;
        auto mpManager = MultiplayerManager::SingletonInstance;
        mpManager->Initialize(LOBBY_TEMPLATE_NAME);
        VERIFY_NO_THROW(socialManager->AddLocalUser(xboxLiveContext->User, SocialManagerExtraDetailLevel::NoExtraDetail));
        bool notFound = true;
        while (notFound)
        {
            auto events = socialManager->DoWork();
            for (auto evt : events)
            {
                if (evt->EventType == SocialEventType::LocalUserAdded)
                {
                    notFound = false;
                }
            }
        }
        VERIFY_NO_THROW(mpManager->LobbySession->AddLocalUser(xboxLiveContext->User));
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 1);
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map().size() == 1);
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_manager_activation_map()[xuid] == 2);

        VERIFY_NO_THROW(xboxLiveContext1->RealTimeActivityService->Activate());
        VERIFY_NO_THROW(xboxLiveContext2->RealTimeActivityService->Activate());
        VERIFY_NO_THROW(xboxLiveContext3->RealTimeActivityService->Activate());
        VERIFY_NO_THROW(xboxLiveContext4->RealTimeActivityService->Activate());
        VERIFY_THROWS_HR_CX(xboxLiveContext5->RealTimeActivityService->Activate(), E_INVALIDARG);       // This will auto decrement the counter before asserting.

                                                                                                        // xboxLiveContext was not activated, so it should not reduce the websocket count.
        xboxLiveContext->RealTimeActivityService->Deactivate();
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 5);

        xboxLiveContext1->RealTimeActivityService->Deactivate();
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 4);

        // Since MPM is still active, it should not reduce the webscoket count.
        socialManager->RemoveLocalUser(xboxLiveContext->User);
        notFound = true;
        while (notFound)
        {
            auto events = socialManager->DoWork();
            for (auto evt : events)
            {
                if (evt->EventType == SocialEventType::LocalUserRemoved)
                {
                    notFound = false;
                }
            }
        }
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_manager_activation_map()[xuid] == 1);
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 4);

        VERIFY_NO_THROW(xboxLiveContext1->RealTimeActivityService->Activate());
        VERIFY_THROWS_HR_CX(xboxLiveContext->RealTimeActivityService->Activate(), E_INVALIDARG);        // This will auto decrement the counter before asserting.
        xboxLiveContext->RealTimeActivityService->Deactivate();
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 5);


        VERIFY_NO_THROW(socialManager->AddLocalUser(xboxLiveContext->User, SocialManagerExtraDetailLevel::NoExtraDetail));
        notFound = true;
        while (notFound)
        {
            auto events = socialManager->DoWork();
            for (auto evt : events)
            {
                if (evt->EventType == SocialEventType::LocalUserAdded)
                {
                    notFound = false;
                }
            }
        }
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_manager_activation_map()[xuid] == 2);
        socialManager->RemoveLocalUser(xboxLiveContext->User);
        notFound = true;
        while (notFound)
        {
            auto events = socialManager->DoWork();
            for (auto evt : events)
            {
                if (evt->EventType == SocialEventType::LocalUserRemoved)
                {
                    notFound = false;
                }
            }
        }
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_manager_activation_map()[xuid] == 1);
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 5);               // Since MPM hasn't been deactivated yet, this should not count towards the limit.

        mpManager->_Shutdown();
        Sleep(100);
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_manager_activation_map()[xuid] == 0);
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_manager_activation_map().size() == 0);

        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 4);
        VERIFY_NO_THROW(xboxLiveContext->RealTimeActivityService->Activate());
        VERIFY_THROWS_HR_CX(xboxLiveContext6->RealTimeActivityService->Activate(), E_INVALIDARG);       // This will auto decrement the counter before asserting.
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 5);

        xboxLiveContext->RealTimeActivityService->Deactivate();
        xboxLiveContext1->RealTimeActivityService->Deactivate();
        xboxLiveContext2->RealTimeActivityService->Deactivate();
        xboxLiveContext3->RealTimeActivityService->Deactivate();
        xboxLiveContext4->RealTimeActivityService->Deactivate();
        xboxLiveContext5->RealTimeActivityService->Deactivate();
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map()[xuid] == 0);
        VERIFY_IS_TRUE(real_time_activity_service::_Rta_activation_map().size() == 0);
    }

    DEFINE_TEST_CASE(RTATestDestruction)
    {
        DEFINE_TEST_CASE_PROPERTIES(RTAMaxWebsocketTest_1);
        auto xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        auto helper = SetupStateChangeHelper(xboxLiveContext->RealTimeActivityService);

        {
            auto xboxLiveContextTest = GetMockXboxLiveContext_WinRT();
            xboxLiveContextTest->RealTimeActivityService->Activate();
            xboxLiveContextTest = nullptr;
        }

        {
            auto xboxLiveContextTest = GetMockXboxLiveContext_WinRT();
            xboxLiveContextTest->RealTimeActivityService->Activate();
        }

        {
            auto xboxLiveContextTest = GetMockXboxLiveContext_WinRT();
            xboxLiveContextTest->RealTimeActivityService->Activate();
            xboxLiveContextTest->RealTimeActivityService->Deactivate();
        }
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

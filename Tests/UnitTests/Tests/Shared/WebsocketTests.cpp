// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jicailiu"
#define TEST_CLASS_AREA L"WebSocketTests"
#include "UnitTestIncludes.h"
#include "web_socket_connection.h"
#include "MockWebSocketClient.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
using namespace xbox::services;

DEFINE_TEST_CLASS(WebSocketTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(WebSocketTests)

    struct StateChangeHelper
    {
        int disconnected = 0;
        int connecting = 0;
        int connected = 0;
        concurrency::event disconnectedEvent;
        concurrency::event connectedEvent;
        concurrency::event connectingEvent;

        void reset_events()
        {
            disconnectedEvent.reset();
            connectedEvent.reset();
            connectingEvent.reset();
        }
    };

    std::shared_ptr<xbox_live_context_settings> GetDefaultHttpSetting()
    {
        auto httpSetting = std::make_shared<xbox_live_context_settings>();
        httpSetting->set_websocket_timeout_window(std::chrono::seconds(0));
        return httpSetting;
    }

    std::shared_ptr<StateChangeHelper> SetupStateChangeHelper(std::shared_ptr<web_socket_connection>& connection)
    {
        auto helper = std::make_shared<StateChangeHelper>();

        connection->set_connection_state_change_handler([helper](web_socket_connection_state oldState, web_socket_connection_state newState)
        {
            UNREFERENCED_PARAMETER(oldState);
            if (newState == web_socket_connection_state::disconnected)
            {
                ++helper->disconnected;
                helper->disconnectedEvent.set();
            }

            if (newState == web_socket_connection_state::connecting)
            {
                ++helper->connecting;
                helper->connectingEvent.set();
            }

            if (newState == web_socket_connection_state::connected)
            {
                ++helper->connected;
                helper->connectedEvent.set();
            }
        });

        return helper;
    }

    DEFINE_TEST_CASE(Connect)
    {
        DEFINE_TEST_CASE_PROPERTIES(Connect);

        auto user = SignInUserWithMocks_WinRT();
        auto userContext = std::make_shared<user_context>(user);

        std::shared_ptr<web_socket_connection> connection = std::make_shared<web_socket_connection>(
            userContext,
            "wss://rta.xboxlive.com/connect",
            "rta.xboxlive.com",
            GetDefaultHttpSetting()
            );

        // setup state change callback
        auto stateChangeHelper = SetupStateChangeHelper(connection);

        std::shared_ptr<MockWebSocketClient> mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();

        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::disconnected);

        connection->ensure_connected();

        stateChangeHelper->connectedEvent.wait();

        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connected);

        VERIFY_ARE_EQUAL_INT(stateChangeHelper->disconnected, 0);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connected, 1);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connecting, 1);

        // send test
        xsapi_internal_string sendMessage = "[0, 1, 1]";
        mockSocket->set_send_handler([sendMessage](xsapi_internal_string msg)
        {
            VERIFY_ARE_EQUAL(sendMessage, msg);
        });
        connection->send(sendMessage);

        // receive test
        xsapi_internal_string receiveMessage = "message receive";
        connection->set_received_handler([receiveMessage](xsapi_internal_string msg)
        {
            VERIFY_ARE_EQUAL(msg, receiveMessage);
        });
        mockSocket->m_receiveHandler(receiveMessage);

        connection->close();
    }

    DEFINE_TEST_CASE(ConnectFail)
    {
        DEFINE_TEST_CASE_PROPERTIES(ConnectFail);

        auto user = SignInUserWithMocks_WinRT();
        auto userContext = std::make_shared<user_context>(user);

        std::shared_ptr<web_socket_connection> connection = std::make_shared<web_socket_connection>(
            userContext,
            "wss://rta.xboxlive.com/connect",
            "rta.xboxlive.com",
            GetDefaultHttpSetting()
            );
        auto stateChangeHelper = SetupStateChangeHelper(connection);

        std::shared_ptr<MockWebSocketClient> mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        mockSocket->m_connectToFail = true;

        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::disconnected);

        connection->ensure_connected();
        stateChangeHelper->disconnectedEvent.wait();

        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::disconnected);

        VERIFY_ARE_EQUAL_INT(stateChangeHelper->disconnected, 1);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connected, 0);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connecting, 1);

        connection->close();
    }

    DEFINE_TEST_CASE(MultiConnecting)
    {
        DEFINE_TEST_CASE_PROPERTIES(MultiConnecting);

        auto user = SignInUserWithMocks_WinRT();
        auto userContext = std::make_shared<user_context>(user);

        std::shared_ptr<web_socket_connection> connection = std::make_shared<web_socket_connection>(
            userContext,
            "wss://rta.xboxlive.com/connect",
            "rta.xboxlive.com",
            GetDefaultHttpSetting()
            );
        // setup state change callback
        auto stateChangeHelper = SetupStateChangeHelper(connection);

        std::shared_ptr<MockWebSocketClient> mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();

        // make connection happen based on our signal
        mockSocket->m_waitForSignal = true;

        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::disconnected);

        connection->ensure_connected();
        connection->ensure_connected();
        connection->ensure_connected();
        connection->ensure_connected();

        stateChangeHelper->connectingEvent.wait();

        mockSocket->m_connectEvent.set();

        stateChangeHelper->connectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connected);

        connection->ensure_connected();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connected);

        VERIFY_ARE_EQUAL_INT(stateChangeHelper->disconnected, 0);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connected, 1);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connecting, 1);

        connection->close();
    }

    DEFINE_TEST_CASE(ConnectThenClientCloseThenConnect)
    {
        DEFINE_TEST_CASE_PROPERTIES(ConnectThenClientCloseThenConnect);

        auto user = SignInUserWithMocks_WinRT();
        auto userContext = std::make_shared<user_context>(user);

        std::shared_ptr<web_socket_connection> connection = std::make_shared<web_socket_connection>(
            userContext,
            "wss://rta.xboxlive.com/connect",
            "rta.xboxlive.com",
            GetDefaultHttpSetting()
            );

        // setup state change callback
        auto stateChangeHelper = SetupStateChangeHelper(connection);

        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::disconnected);

        stateChangeHelper->reset_events();
        connection->ensure_connected();
        stateChangeHelper->connectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connected);


        connection->close();
        stateChangeHelper->disconnectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::disconnected);

        stateChangeHelper->reset_events();
        connection->ensure_connected();
        stateChangeHelper->connectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connected);

        VERIFY_ARE_EQUAL_INT(stateChangeHelper->disconnected, 1);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connected, 2);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connecting, 2);

        connection->close();
    }

    DEFINE_TEST_CASE(ConnectThenServiceCloseThenConnect)
    {
        DEFINE_TEST_CASE_PROPERTIES(ConnectThenServiceCloseThenConnect);

        auto user = SignInUserWithMocks_WinRT();
        auto userContext = std::make_shared<user_context>(user);

        std::shared_ptr<web_socket_connection> connection = std::make_shared<web_socket_connection>(
            userContext,
            "wss://rta.xboxlive.com/connect",
            "rta.xboxlive.com",
            GetDefaultHttpSetting()
            );
        auto stateChangeHelper = SetupStateChangeHelper(connection);

        std::shared_ptr<MockWebSocketClient> mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();

        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::disconnected);

        // 1 connecting, 1 connected
        connection->ensure_connected();
        stateChangeHelper->connectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connected);

        // 2 connecting, 2 connected
        stateChangeHelper->reset_events();
        mockSocket->m_closeHandler(HCWebSocketCloseStatus_GoingAway);
        stateChangeHelper->connectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connected);

        connection->ensure_connected();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connected);

        VERIFY_ARE_EQUAL_INT(stateChangeHelper->disconnected, 0);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connected, 2);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connecting, 2);

        connection->close();
    }

    DEFINE_TEST_CASE(ConnectRetry)
    {
        DEFINE_TEST_CASE_PROPERTIES(ConnectRetry);

        auto user = SignInUserWithMocks_WinRT();
        auto userContext = std::make_shared<user_context>(user);

        // set 1 sec timeout window
        auto httpSetting = GetDefaultHttpSetting();
        httpSetting->set_websocket_timeout_window(std::chrono::seconds(1));

        std::shared_ptr<web_socket_connection> connection = std::make_shared<web_socket_connection>(
            userContext,
            "wss://rta.xboxlive.com/connect",
            "rta.xboxlive.com",
            httpSetting
            );

        // setup state change callback
        auto stateChangeHelper = SetupStateChangeHelper(connection);

        std::shared_ptr<MockWebSocketClient> mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        mockSocket->m_waitForSignal = true;
        mockSocket->m_connectToFail = true;

        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::disconnected);

        connection->ensure_connected();

        // first time set event should make connect to fail, but we would retry
        mockSocket->m_connectEvent.set();
        stateChangeHelper->connectingEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connecting);

        // now let it through
        mockSocket->m_connectToFail = false;
        mockSocket->m_connectEvent.set();

        stateChangeHelper->connectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connected);

        VERIFY_ARE_EQUAL_INT(stateChangeHelper->disconnected, 0);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connected, 1);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connecting, 1);

        connection->close();
    }

    DEFINE_TEST_CASE(ConnectRetryFail)
    {
        DEFINE_TEST_CASE_PROPERTIES_FAILING(ConnectRetryFail);

        auto user = SignInUserWithMocks_WinRT();
        auto userContext = std::make_shared<user_context>(user);

        // set 1 sec timeout window
        auto httpSetting = GetDefaultHttpSetting();
        httpSetting->set_websocket_timeout_window(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::milliseconds(100)));

        std::shared_ptr<web_socket_connection> connection = std::make_shared<web_socket_connection>(
            userContext,
            "wss://rta.xboxlive.com/connect",
            "rta.xboxlive.com",
            httpSetting
            );

        // setup state change callback
        auto stateChangeHelper = SetupStateChangeHelper(connection);

        std::shared_ptr<MockWebSocketClient> mockSocket = m_mockXboxSystemFactory->GetMockWebSocketClient();
        mockSocket->m_waitForSignal = true;
        mockSocket->m_connectToFail = true;

        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::disconnected);

        connection->ensure_connected();

        stateChangeHelper->connectingEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connecting);

        // let it timeout
        concurrency::wait(100);
        mockSocket->m_connectEvent.set();

        // should receive disconnected event
        stateChangeHelper->disconnectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::disconnected);

        // It should be keep retrying at background
        mockSocket->m_connectToFail = false;
        mockSocket->m_connectEvent.set();
        stateChangeHelper->connectedEvent.wait();
        VERIFY_ARE_EQUAL_INT(connection->state(), web_socket_connection_state::connected);

        VERIFY_ARE_EQUAL_INT(stateChangeHelper->disconnected, 1);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connected, 1);
        VERIFY_ARE_EQUAL_INT(stateChangeHelper->connecting, 1);

        connection->close();
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


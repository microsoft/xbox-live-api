// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(XboxLiveContextSettingsTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(XboxLiveContextSettingsTests);

    DEFINE_TEST_CASE(TestXboxLiveContextSettingsApis)
    {
        TEST_LOG(L"Test starting: TestXboxLiveContextSettingsApis");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();
        auto contextHandle{ xboxLiveContext.get() };

        // Verify Set/Get
        uint32_t desiredTimeout{ 60 }, actualTimeout{};
        VERIFY_SUCCEEDED(XblContextSettingsSetLongHttpTimeout(contextHandle, desiredTimeout));
        VERIFY_SUCCEEDED(XblContextSettingsGetLongHttpTimeout(contextHandle, &actualTimeout));
        VERIFY_ARE_EQUAL_UINT(desiredTimeout, actualTimeout);

        uint32_t desiredRetryDelay{ 60 }, actualRetryDelay{};
        VERIFY_SUCCEEDED(XblContextSettingsSetHttpRetryDelay(contextHandle, desiredRetryDelay));
        VERIFY_SUCCEEDED(XblContextSettingsGetHttpRetryDelay(contextHandle, &actualRetryDelay));
        VERIFY_ARE_EQUAL_UINT(desiredRetryDelay, actualRetryDelay);

        uint32_t desiredRetryWindow{ 60 }, actualRetryWindow{};
        VERIFY_SUCCEEDED(XblContextSettingsSetHttpTimeoutWindow(contextHandle, desiredRetryWindow));
        VERIFY_SUCCEEDED(XblContextSettingsGetHttpTimeoutWindow(contextHandle, &actualRetryWindow));
        VERIFY_ARE_EQUAL_UINT(desiredRetryWindow, actualRetryWindow);

        uint32_t desiredWebsocketTimeout{ 60 }, actualWebsocketTimeout{};
        VERIFY_SUCCEEDED(XblContextSettingsSetWebsocketTimeoutWindow(contextHandle, desiredWebsocketTimeout));
        VERIFY_SUCCEEDED(XblContextSettingsGetWebsocketTimeoutWindow(contextHandle, &actualWebsocketTimeout));
        VERIFY_ARE_EQUAL_UINT(desiredWebsocketTimeout, actualWebsocketTimeout);

        bool desiredUseXPlatQos{ true }, actualUserXPlatQos{};
        VERIFY_SUCCEEDED(XblContextSettingsSetUseCrossPlatformQosServers(contextHandle, desiredUseXPlatQos));
        VERIFY_SUCCEEDED(XblContextSettingsGetUseCrossPlatformQosServers(contextHandle, &actualUserXPlatQos));
        VERIFY_ARE_EQUAL_INT(desiredUseXPlatQos, actualUserXPlatQos);
    }

    static size_t callRoutedCount;

    static void ServiceCallRouted(
        _In_ XblServiceCallRoutedArgs args,
        _In_opt_ void* context
    )
    {
        UNREFERENCED_PARAMETER(context);

        LOGS_DEBUG << args.fullResponseFormatted;
        callRoutedCount++;
        LOGS_DEBUG << "callRoutedCount: " << callRoutedCount;
    }

    DEFINE_TEST_CASE(TestHttpTimeouts)
    {
        TEST_LOG(L"Test starting: TestHttpTimeouts");

        TestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext();

        callRoutedCount = 0;
        VERIFY_SUCCEEDED(XblAddServiceCallRoutedHandler(ServiceCallRouted, nullptr));

        uint32_t timeout{ 15 }, retryDelay{ 2 };
        VERIFY_SUCCEEDED(XblContextSettingsSetHttpTimeoutWindow(xboxLiveContext.get(), timeout));
        VERIFY_SUCCEEDED(XblContextSettingsSetHttpRetryDelay(xboxLiveContext.get(), retryDelay));

        HttpMock mock(String{}, "https://client-strings.xboxlive.com", 503);

        auto startTime{ std::chrono::high_resolution_clock::now() };

        XAsyncBlock async{};
        VERIFY_SUCCEEDED(XblStringVerifyStringAsync(xboxLiveContext.get(), "TestString", &async));
        VERIFY_FAILED(XAsyncGetStatus(&async, true));

        auto endTime{ std::chrono::high_resolution_clock::now() };
        auto callTime{ std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() };

        // Make sure at least one retry is attempted and that we get a call routed callback for each
        VERIFY_IS_TRUE(callRoutedCount > 1);
        VERIFY_IS_TRUE(callTime <= timeout);
    }

    static void CALLBACK RTAStateChanged(
        _In_opt_ void* context,
        _In_ XblRealTimeActivityConnectionState connectionState
    )
    {
        switch (connectionState)
        {
        case XblRealTimeActivityConnectionState::Connected:
        {
            assert(false);
            break;
        }
        case XblRealTimeActivityConnectionState::Disconnected:
        {
            auto event = static_cast<Event*>(context);
            event->Set();
            break;
        }
        default:
        {
            break;
        }
        }
    }
};

size_t XboxLiveContextSettingsTests::callRoutedCount{ 0 };

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


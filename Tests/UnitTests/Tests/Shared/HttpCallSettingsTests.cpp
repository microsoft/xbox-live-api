// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"XboxLiveContextSettings"
#include "UnitTestIncludes.h"
#include <xsapi/xbox_live_context.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring defaultStringVerifyResult =
LR"(
    {
    "verifyStringResult":
    [
        {
            "resultCode": 0
        },
        {
            "resultCode": 1,
            "offendingString":"sdfasdf"
        },
        {
            "resultCode": 7,
            "offendingString":"whatasfkjasl"
        }
    ]}
    )";

struct CallLogNode
{
public:
    CallLogNode(
        _In_ xbox::services::xbox_service_call_routed_event_args _args,
        _In_ std::chrono::steady_clock::time_point _time
        ) :
        m_args(_args),
        m_time(_time)
    {
    }

    xbox::services::xbox_service_call_routed_event_args m_args;
    std::chrono::steady_clock::time_point m_time;
};

static std::vector<CallLogNode> g_callLog;

DEFINE_TEST_CLASS(XboxLiveContextSettingsTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(XboxLiveContextSettingsTests)

    DEFINE_TEST_CASE(TestXboxLiveContextSettings)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestXboxLiveContextSettings);
        std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings = std::make_shared<xbox_live_context_settings>();

        // Verify defaults
        VERIFY_ARE_EQUAL(false, xboxLiveContextSettings->enable_service_call_routed_events());
        VERIFY_ARE_EQUAL_INT(30, xboxLiveContextSettings->http_timeout().count());
        VERIFY_ARE_EQUAL_INT(5 * 60, xboxLiveContextSettings->long_http_timeout().count());
        VERIFY_ARE_EQUAL_INT(2, xboxLiveContextSettings->http_retry_delay().count());
        VERIFY_ARE_EQUAL_INT(20, xboxLiveContextSettings->http_timeout_window().count());

        // Verify sets
        xboxLiveContextSettings->set_enable_service_call_routed_events(true);
        xboxLiveContextSettings->set_http_timeout(std::chrono::seconds(1));
        xboxLiveContextSettings->set_long_http_timeout(std::chrono::seconds(4));
        xboxLiveContextSettings->set_http_retry_delay(std::chrono::seconds(0));
        xboxLiveContextSettings->set_http_timeout_window(std::chrono::seconds(3));
        VERIFY_ARE_EQUAL(true, xboxLiveContextSettings->enable_service_call_routed_events());
        VERIFY_ARE_EQUAL_INT(1, xboxLiveContextSettings->http_timeout().count());
        VERIFY_ARE_EQUAL_INT(4, xboxLiveContextSettings->long_http_timeout().count());
        VERIFY_ARE_EQUAL_INT(2, xboxLiveContextSettings->http_retry_delay().count());
        VERIFY_ARE_EQUAL_INT(3, xboxLiveContextSettings->http_timeout_window().count());
    }

    static void TraceFunction(_In_ const xbox::services::xbox_service_call_routed_event_args& args)
    {
        CallLogNode n(args, std::chrono::high_resolution_clock::now());
        g_callLog.push_back(n);
    }

    DEFINE_TEST_CASE(TestHttpTimeoutWithNoRetry)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestHttpTimeoutWithNoRetry);
        g_callLog.clear();
        auto responseJson = web::json::value::parse(defaultStringVerifyResult);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto requestString = std::wstring(L"xboxUserId");
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        xboxLiveContext->settings()->add_service_call_routed_handler(TraceFunction);
        xboxLiveContext->settings()->set_enable_service_call_routed_events(true);
        xboxLiveContext->settings()->set_http_timeout_window(std::chrono::seconds(0));
        m_mockXboxSystemFactory->setup_mock_for_http_client();
        StockMocks::AddHttpMockResponse(responseJson.serialize(), 503);
        
        auto timeStart = std::chrono::high_resolution_clock::now();
        xboxLiveContext->string_service().verify_string(requestString)
        .then([timeStart](xbox_live_result<verify_string_result> result)
        {
            VERIFY_IS_TRUE(result.err() == xbox_live_error_code::http_status_503_service_unavailable);
            VERIFY_IS_TRUE(result.err() == xbox_live_error_condition::http);
        }).wait();

        LogCalls(timeStart);

        VERIFY_ARE_EQUAL_INT(1, g_callLog.size());
        VerifyDelay(g_callLog[0].m_time, timeStart, 0);
    }

    DEFINE_TEST_CASE(TestHttpTimeoutWithRetryAfter)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestHttpTimeoutWithRetryAfter);
        g_callLog.clear();
        auto responseJson = web::json::value::parse(defaultStringVerifyResult);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        auto requestString = std::wstring(L"xboxUserId");
        auto xboxLiveContext = GetMockXboxLiveContext_Cpp();
        xboxLiveContext->settings()->add_service_call_routed_handler(TraceFunction);
        xboxLiveContext->settings()->set_enable_service_call_routed_events(true);
        xboxLiveContext->settings()->set_http_timeout_window(std::chrono::seconds(1));
        m_mockXboxSystemFactory->setup_mock_for_http_client();

        http_headers headers;
        headers["Retry-After"] = "20";
        StockMocks::AddHttpMockResponse(responseJson.serialize(), 503, headers);

        xboxLiveContext->settings()->set_http_timeout_window(std::chrono::seconds(1));

        auto timeStart = std::chrono::high_resolution_clock::now();
        xboxLiveContext->string_service().verify_string(requestString)
        .then([timeStart](xbox_live_result<verify_string_result> result)
        {
            VERIFY_IS_TRUE(result.err() == xbox_live_error_code::http_status_503_service_unavailable);
            VERIFY_IS_TRUE(result.err() == xbox_live_error_condition::http);
        }).wait();

        while (true)
        {
            xboxLiveContext->string_service().verify_string(requestString)
            .then([timeStart](xbox_live_result<verify_string_result> result)
            {
                VERIFY_IS_TRUE(result.err() == xbox_live_error_code::http_status_503_service_unavailable);
                VERIFY_IS_TRUE(result.err() == xbox_live_error_condition::http);
            }).wait();

            auto timeCurrent = std::chrono::high_resolution_clock::now();
            auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(timeCurrent - timeStart);
            if (delay.count() > 1200)
            {
                break;
            }
        }

        LogCalls(timeStart);

        VERIFY_IS_TRUE(g_callLog.size() > 5);
        VerifyDelay(g_callLog[0].m_time, timeStart, 0);
        VerifyDelay(g_callLog[1].m_time, g_callLog[0].m_time, 0);
        VerifyDelay(g_callLog[2].m_time, g_callLog[1].m_time, 0);
    }

    static void LogCalls(_In_ const std::chrono::steady_clock::time_point& timeStart)
    {
        std::chrono::steady_clock::time_point timeLast = timeStart;
        for(const auto& node : g_callLog)
        {
            auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(node.m_time - timeLast);
            auto delayTotal = std::chrono::duration_cast<std::chrono::milliseconds>(node.m_time - timeStart);
            std::wstring str1 = FormatString(L"Delay: %d. ", delay.count());
            std::wstring str2 = FormatString(L"TotalDelay: %d.", delayTotal.count());
            TEST_LOG(FormatString(L"%s%s", str1.c_str(), str2.c_str()).c_str());
            timeLast = node.m_time;
        }
    }

    static void VerifyDelay(
        _In_ const std::chrono::steady_clock::time_point& timeCurrent,
        _In_ const std::chrono::steady_clock::time_point& timeStart,
        _In_ int expectedDelayTimeInMs
        )
    {
        auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(timeCurrent - timeStart);
        std::wstring str1 = FormatString(L"Delay: %d. ", delay.count());
        std::wstring str2 = FormatString(L"Expected: %d.", expectedDelayTimeInMs);
        TEST_LOG(FormatString(L"%s%s", str1.c_str(), str2.c_str()).c_str());
        VERIFY_IS_TRUE(delay.count() > expectedDelayTimeInMs - 500 && delay.count() < expectedDelayTimeInMs + 500);
    }

    static void VerifyDelayRange(
        _In_ const std::chrono::steady_clock::time_point& timeCurrent,
        _In_ const std::chrono::steady_clock::time_point& timeStart,
        _In_ int minExpectedDelayTimeInMs,
        _In_ int maxExpectedDelayTimeInMs
        )
    {
        auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(timeCurrent - timeStart);
        TEST_LOG(FormatString(L"Delay: %d", delay.count()).c_str());
        VERIFY_IS_TRUE(delay.count() > minExpectedDelayTimeInMs - 500 && delay.count() < maxExpectedDelayTimeInMs + 500);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


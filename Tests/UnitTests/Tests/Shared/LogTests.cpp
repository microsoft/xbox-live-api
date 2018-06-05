// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jicailiu"
#define TEST_CLASS_AREA L"Log"
#include "UnitTestIncludes.h"

#include "Logger\Log.h"
#include "Logger\console_output.h"
#include "xsapi/system.h"

#include "XboxLiveServicesSettings_WinRT.h"

using namespace xbox::services::system;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class test_log_output: public log_output
{
public:
    test_log_output(log_output_level_setting type, log_level level) : log_output(type, level)
    {}

    void write(_In_ const std::string& msg) override
    {
        m_logOutput.push_back(msg);
    }

    std::vector<std::string> m_logOutput;
};

static bool g_haslogged = false;
static bool g_hasloggedWinRT = false;

DEFINE_TEST_CLASS(LogTest)
{
public:
    DEFINE_TEST_CLASS_PROPS(LogTest);

    //TEST_METHOD_INITIALIZE(TestMethodSetup)
    //{
    //    logger::release_logger();
    //}

    bool StringCompareLastCharactors(std::string originStr, std::string compareStr)
    {
        char* startInOrigin = &originStr[0] + (originStr.size() - compareStr.size()) - 1; // remove the last endl

        return strncmp(compareStr.c_str(), startInOrigin, compareStr.size()) == 0;
    }

    DEFINE_TEST_CASE(WriteLog)
    {
        DEFINE_TEST_CASE_PROPERTIES(WriteLog);
        auto testLogger = std::make_shared<logger>();

        auto test_output = std::make_shared<test_log_output>(log_output_level_setting::use_logger_setting, log_level::off);

        testLogger->add_log_output(test_output);

        char testLogText[] = "testlog";

        testLogger->add_log(log_entry(log_level::error, "test", testLogText));
        testLogger->add_log(log_entry(log_level::error, "test") << testLogText << 1);

        char veryverylong[10240];
        int i = 0;
        for (; i < _countof(veryverylong) - 1; i++)
        {
            veryverylong[i] = 'a';
        }
        veryverylong[i] = 0;

        testLogger->add_log(log_entry(log_level::error, "test") << testLogText << testLogText);
        testLogger->add_log(log_entry(log_level::error, "test") << testLogText << veryverylong);

        // This will not be added into logger as it's level is higher than we set.
        testLogger->add_log(log_entry(log_level::debug, "test", "test"));

        VERIFY_ARE_EQUAL_INT(4, test_output->m_logOutput.size());
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[0], testLogText));
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[1], std::string(testLogText) + "1"));
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[2], std::string(testLogText) + testLogText));
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[3], std::string(testLogText) + veryverylong));

    }

    DEFINE_TEST_CASE(WriteLogStream)
    {
        DEFINE_TEST_CASE_PROPERTIES(WriteLogStream);
        auto testLogger = std::make_shared<logger>();

        auto test_output = std::make_shared<test_log_output>(log_output_level_setting::use_logger_setting, log_level::off);

        testLogger->add_log_output(test_output);

        char testLogText[] = "testlog";

        *testLogger += log_entry(log_level::error, "test") << testLogText;
        *testLogger += log_entry(log_level::error, "test") << testLogText << 1;

        VERIFY_ARE_EQUAL_INT(2, test_output->m_logOutput.size());
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[0], testLogText));
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[1], std::string(testLogText) + "1"));

    }

    DEFINE_TEST_CASE(WriteLogConcurrent)
    {
        DEFINE_TEST_CASE_PROPERTIES(WriteLogConcurrent);
        auto testLogger = std::make_shared<logger>();

        // Start 20 threads writing 100 logs
        auto test_output = std::make_shared<test_log_output>(log_output_level_setting::use_logger_setting, log_level::off);
        testLogger->add_log_output(test_output);

        int loopCount = 20;
        std::vector<task<void>> tasks;
        for (int i = 0; i < loopCount; i++)
        {
            auto task = create_task([loopCount, testLogger]()
            {
                for (int j = 0; j < loopCount; j++)
                testLogger->add_log(log_entry(log_level::error, "test", "a"));
            });
            tasks.push_back(task);
        }

        concurrency::when_all(tasks.begin(), tasks.end()).wait();

        VERIFY_ARE_EQUAL_INT(loopCount*loopCount, test_output->m_logOutput.size());
    }

    DEFINE_TEST_CASE(LogLevel)
    {
        DEFINE_TEST_CASE_PROPERTIES(LogLevel);
        auto testLogger = std::make_shared<logger>();
        auto test_output1 = std::make_shared<test_log_output>(log_output_level_setting::use_logger_setting, log_level::off);
        auto test_output2 = std::make_shared<test_log_output>(log_output_level_setting::use_own_setting, log_level::debug);
        testLogger->add_log_output(test_output1);
        testLogger->add_log_output(test_output2);

                testLogger->set_log_level(log_level::off);
        VERIFY_IS_FALSE(test_output1->log_level_enabled(log_level::error));
        VERIFY_IS_FALSE(test_output1->log_level_enabled(log_level::warn));
        VERIFY_IS_FALSE(test_output1->log_level_enabled(log_level::info));
        VERIFY_IS_FALSE(test_output1->log_level_enabled(log_level::debug));

        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::error));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::warn));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::info));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::debug));


        testLogger->set_log_level(log_level::error);
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::error));
        VERIFY_IS_FALSE(test_output1->log_level_enabled(log_level::warn));
        VERIFY_IS_FALSE(test_output1->log_level_enabled(log_level::info));
        VERIFY_IS_FALSE(test_output1->log_level_enabled(log_level::debug));

        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::error));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::warn));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::info));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::debug));


        testLogger->set_log_level(log_level::warn);
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::error));
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::warn));
        VERIFY_IS_FALSE(test_output1->log_level_enabled(log_level::info));
        VERIFY_IS_FALSE(test_output1->log_level_enabled(log_level::debug));

        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::error));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::warn));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::info));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::debug));


        testLogger->set_log_level(log_level::info);
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::error));
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::warn));
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::info));
        VERIFY_IS_FALSE(test_output1->log_level_enabled(log_level::debug));

        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::error));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::warn));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::info));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::debug));


        testLogger->set_log_level(log_level::debug);
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::error));
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::warn));
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::info));
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::debug));

        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::error));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::warn));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::info));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::debug));


        testLogger->set_log_level(log_level::debug);
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::error));
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::warn));
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::info));
        VERIFY_IS_TRUE(test_output1->log_level_enabled(log_level::debug));

        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::error));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::warn));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::info));
        VERIFY_IS_TRUE(test_output2->log_level_enabled(log_level::debug));
    }

    DEFINE_TEST_CASE(LogMacro)
    {
        DEFINE_TEST_CASE_PROPERTIES(LogMacro);

        std::shared_ptr<logger> testLogger = nullptr;
        // Nothing should happen without creating a logger
        VERIFY_IS_TRUE(testLogger == nullptr);
        LOG(testLogger, log_level::debug, "", "test");

        // create logger
        testLogger = std::make_shared<logger>();
        auto test_output = std::make_shared<test_log_output>(log_output_level_setting::use_logger_setting, log_level::off);
        testLogger->add_log_output(test_output);

        // default logging level is warn
        LOG(testLogger, log_level::debug, "", "debug");
        LOG(testLogger, log_level::info, "", "info");
        LOG(testLogger, log_level::warn, "", "warn");
        LOG(testLogger, log_level::error, "", "error");
        LOG(testLogger, log_level::error, "", "error");
        LOGS(testLogger, log_level::error, "") << "error" << L"error" << 2 << true;
        LOGS(testLogger, log_level::error, "") << std::string("error") << std::wstring(L"error") << std::showbase << std::hex << 15 << false;

        VERIFY_ARE_EQUAL_INT(5, test_output->m_logOutput.size());
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[0], "warn"));
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[1], "error"));
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[2], "error"));
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[3], "errorerror21"));
        VERIFY_IS_TRUE(StringCompareLastCharactors(test_output->m_logOutput[4], "errorerror0xf0"));
    }

    static void TraceFunction(_In_ xbox_services_diagnostics_trace_level level, _In_ const std::string& category, _In_ const std::string& message)
    {
        UNREFERENCED_PARAMETER(category);
        xbox_services_diagnostics_trace_level currentLevel = xbox_live_services_settings::get_singleton_instance()->diagnostics_trace_level();
        g_haslogged = true;
        VERIFY_IS_TRUE(level <= currentLevel);
        VERIFY_ARE_NOT_EQUAL(std::string(), message);
    }

    DEFINE_TEST_CASE(CustomLogs)
    {
        DEFINE_TEST_CASE_PROPERTIES(CustomLogs);

        auto instance = xbox_live_services_settings::get_singleton_instance();

        auto context = instance->add_logging_handler(TraceFunction);

        instance->set_diagnostics_trace_level(xbox_services_diagnostics_trace_level::off);
        LOG_ERROR("Error");
        LOG_WARN("Warn");
        LOG_INFO("Info");
        LOG_DEBUG("Verbose");

        instance->set_diagnostics_trace_level(xbox_services_diagnostics_trace_level::error);
        LOG_ERROR("Error");
        LOG_WARN("Warn");
        LOG_INFO("Info");
        LOG_DEBUG("Verbose");


        instance->set_diagnostics_trace_level(xbox_services_diagnostics_trace_level::warning);
        LOG_ERROR("Error");
        LOG_WARN("Warn");
        LOG_INFO("Info");
        LOG_DEBUG("Verbose");


        instance->set_diagnostics_trace_level(xbox_services_diagnostics_trace_level::info);
        LOG_ERROR("Error");
        LOG_WARN("Warn");
        LOG_INFO("Info");
        LOG_DEBUG("Verbose");


        instance->set_diagnostics_trace_level(xbox_services_diagnostics_trace_level::verbose);
        LOG_ERROR("Error");
        LOG_WARN("Warn");
        LOG_INFO("Info");
        LOG_DEBUG("Verbose");

        instance->remove_logging_handler(context);
        VERIFY_IS_TRUE(g_haslogged);
    }

    static void TraceFunctionWinRT(_In_ XboxServicesDiagnosticsTraceLevel level, _In_ Platform::String^ category, _In_ Platform::String^ message)
    {
        XboxLiveServicesSettings^ settings = XboxLiveServicesSettings::SingletonInstance;
        g_hasloggedWinRT = true;
        VERIFY_IS_TRUE(level <= settings->DiagnosticsTraceLevel);
        VERIFY_ARE_NOT_EQUAL(ref new Platform::String(), message);
    }

    DEFINE_TEST_CASE(CustomWinRTLogs)
    {
        DEFINE_TEST_CASE_PROPERTIES(CustomWinRTLogs);

        XboxLiveServicesSettings^ settings = XboxLiveServicesSettings::SingletonInstance;

        auto context = settings->LogCallRouted += ref new Windows::Foundation::EventHandler<Microsoft::Xbox::Services::XboxLiveLogCallEventArgs^>(
            [](Platform::Object^, Microsoft::Xbox::Services::XboxLiveLogCallEventArgs^ args)
        {
            TraceFunctionWinRT(args->Level, args->Category, args->Message);
        });
        
        settings->DiagnosticsTraceLevel = XboxServicesDiagnosticsTraceLevel::Off;
        LOG_ERROR("Error");
        LOG_WARN("Warn");
        LOG_INFO("Info");
        LOG_DEBUG("Verbose");
        
        settings->DiagnosticsTraceLevel = XboxServicesDiagnosticsTraceLevel::Error;
        LOG_ERROR("Error");
        LOG_WARN("Warn");
        LOG_INFO("Info");
        LOG_DEBUG("Verbose");
        
        settings->DiagnosticsTraceLevel = XboxServicesDiagnosticsTraceLevel::Warning;
        LOG_ERROR("Error");
        LOG_WARN("Warn");
        LOG_INFO("Info");
        LOG_DEBUG("Verbose");
        
        settings->DiagnosticsTraceLevel = XboxServicesDiagnosticsTraceLevel::Info;
        LOG_ERROR("Error");
        LOG_WARN("Warn");
        LOG_INFO("Info");
        LOG_DEBUG("Verbose");
        
        settings->DiagnosticsTraceLevel = XboxServicesDiagnosticsTraceLevel::Verbose;
        LOG_ERROR("Error");
        LOG_WARN("Warn");
        LOG_INFO("Info");
        LOG_DEBUG("Verbose");

        settings->LogCallRouted -= context;

        VERIFY_IS_TRUE(g_hasloggedWinRT);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END



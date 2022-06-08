// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "Logger/log.h"
#include "Logger/log_hc_output.h"

using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

DEFINE_TEST_CLASS(LogTest)
{
public:
    DEFINE_TEST_CLASS_PROPS(LogTest);

    class TestLogOutput : public log_output
    {
    public:
        TestLogOutput() = default;

        void write(_In_ HCTraceLevel level, _In_ const xsapi_internal_string& msg) override
        {
            UNREFERENCED_PARAMETER(level);
            m_logOutput.push_back(msg);
        }

        std::vector<xsapi_internal_string> m_logOutput;
    };

    DEFINE_TEST_CASE(WriteLog)
    {
        TEST_LOG(L"Test starting: WriteLog");

        VERIFY_SUCCEEDED(HCSettingsSetTraceLevel(HCTraceLevel::Error));

        auto testLogger = std::make_shared<logger>();
        auto testOutput = std::make_shared<TestLogOutput>();
        testLogger->add_log_output(testOutput);

        const char* testLogText = "testlog";

        testLogger->add_log(log_entry(HCTraceLevel::Error, "test", testLogText));
        testLogger->add_log(log_entry(HCTraceLevel::Error, "test") << testLogText << 1);

        char veryverylong[10240];
        int i = 0;
        for (; i < _countof(veryverylong) - 1; i++)
        {
            veryverylong[i] = 'a';
        }
        veryverylong[i] = 0;

        testLogger->add_log(log_entry(HCTraceLevel::Error, "test") << testLogText << testLogText);
        testLogger->add_log(log_entry(HCTraceLevel::Error, "test") << testLogText << veryverylong);

        // This will not be added into logger as it's level is higher than we set.
        testLogger->add_log(log_entry(HCTraceLevel::Verbose, "test", "test"));

        VERIFY_ARE_EQUAL_INT(4, testOutput->m_logOutput.size());
        VERIFY_ARE_EQUAL_STR(testLogText, testOutput->m_logOutput[0]);
        VERIFY_ARE_EQUAL_STR(xsapi_internal_string{ testLogText } +"1", testOutput->m_logOutput[1]);
        VERIFY_ARE_EQUAL_STR(xsapi_internal_string{ testLogText } +testLogText, testOutput->m_logOutput[2]);
        VERIFY_ARE_EQUAL_STR(xsapi_internal_string{ testLogText } +veryverylong, testOutput->m_logOutput[3]);
    }

    DEFINE_TEST_CASE(WriteLogStream)
    {
        TEST_LOG(L"Test starting: WriteLogStream");

        //TestEnvironment env{};
        
        auto testLogger = std::make_shared<logger>();
        auto testOutput = std::make_shared<TestLogOutput>();
        testLogger->add_log_output(testOutput);

        const char* testLogText = "testlog";

        *testLogger += log_entry(HCTraceLevel::Error, "test") << testLogText;
        *testLogger += log_entry(HCTraceLevel::Error, "test") << testLogText << 1;

        VERIFY_ARE_EQUAL_INT(2, testOutput->m_logOutput.size());
        VERIFY_ARE_EQUAL_STR(testLogText, testOutput->m_logOutput[0]);
        VERIFY_ARE_EQUAL_STR(xsapi_internal_string{ testLogText } +"1", testOutput->m_logOutput[1]);
    }

    DEFINE_TEST_CASE(WriteLogConcurrent)
    {
        TEST_LOG(L"Test starting: WriteLogConcurrent");

        auto testLogger = std::make_shared<logger>();

        // Start 20 threads writing 100 logs
        auto testOutput = std::make_shared<TestLogOutput>();
        testLogger->add_log_output(testOutput);

        struct LogTask
        {
            LogTask(std::shared_ptr<logger> logger, xsapi_internal_string message, size_t loopCount) noexcept
                : m_logger{ logger },
                m_message{ std::move(message) },
                m_loopCount{ loopCount }
            {
                auto hr = XAsyncRun(&m_async, [](XAsyncBlock* async)
                    {
                        auto pThis{ static_cast<LogTask*>(async->context) };
                        for (size_t i = 0; i < pThis->m_loopCount; ++i)
                        {
                            pThis->m_logger->add_log(log_entry(HCTraceLevel::Error, "test", pThis->m_message) << i);
                        }
                        pThis->Completed.Set();
                        return S_OK;
                    });

                assert(SUCCEEDED(hr));
            }

            Event Completed;

        private:
            XAsyncBlock m_async{ nullptr, this };
            std::shared_ptr<logger> m_logger;
            xsapi_internal_string m_message;
            size_t m_loopCount;
        };

        int loopCount = 20;
        std::vector<std::shared_ptr<LogTask>> tasks;

        for (int i = 0; i < loopCount; i++)
        {
            tasks.push_back(std::make_shared<LogTask>(testLogger, "a", loopCount));
        }

        for (auto task : tasks)
        {
            task->Completed.Wait();
        }

        VERIFY_ARE_EQUAL_INT(loopCount*loopCount, testOutput->m_logOutput.size());
    }

    DEFINE_TEST_CASE(HCLogging)
    {
        TEST_LOG(L"Test starting: HCLogging");

        VERIFY_SUCCEEDED(HCInitialize(nullptr));
        HCTraceSetTraceToDebugger(true);

        auto testLogger = std::make_shared<logger>();
        auto hcOutput = std::make_shared<log_hc_output>();
        testLogger->add_log_output(hcOutput);

        constexpr char formatString[]{ "%s%g%s" };
        testLogger->add_log(log_entry{ HCTraceLevel::Important, "" } << formatString);

        HCCleanup();
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#if UNIT_TEST_SERVICES
#define PERF_TESTING 0
#else
#define PERF_TESTING 0
#endif

#define PERF_THRESHOLD_MS .5f
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

struct perf_counter
{
    float totalTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> previousTime;
};

class perf_tester
{

public:
    perf_tester() { }

    perf_tester(_In_ xsapi_internal_string ownerName) :
        m_ownerName(std::move(ownerName))
    {
    }

    void start_timer(_In_ const xsapi_internal_string& logName)
    {
    #if PERF_TESTING
        perf_counter counter;
        counter.totalTime = 0.0f;

        std::lock_guard<std::mutex> lock(m_lock.get());
        counter.previousTime = std::chrono::high_resolution_clock::now();
        m_logNameToProcessTime[logName] = counter;
    #else
        UNREFERENCED_PARAMETER(logName);
    #endif
    }

    void stop_timer(_In_ const xsapi_internal_string& logName, _In_ bool shouldReportAnyways = false)
    {
#if PERF_TESTING
        std::lock_guard<std::mutex> lock(m_lock.get());
        auto iter = m_logNameToProcessTime.find(logName);
        if (iter != m_logNameToProcessTime.end())
        {
            std::chrono::nanoseconds totalTime = std::chrono::high_resolution_clock::now() - iter->second.previousTime;
            float totalTimeMS = totalTime.count() / 1000000.f;

            iter->second.totalTime = totalTimeMS;
            if (totalTimeMS > PERF_THRESHOLD_MS || shouldReportAnyways)
            {
                LOGS_ERROR << m_ownerName << " processing took : " << totalTimeMS << " ms";
                print();
            }
        }
        else
        {
            LOGS_WARN << "Perf logs cleared mid perf calculation!";
        }
    #else
        UNREFERENCED_PARAMETER(logName);
        UNREFERENCED_PARAMETER(shouldReportAnyways);
    #endif
    }

    void clear()
    {
    #if PERF_TESTING
        std::lock_guard<std::mutex> lock(m_lock.get());
        m_logNameToProcessTime.clear();
    #endif
    }

    void print()
    {
        LOG_ERROR("dumping logs");
        for (auto& log : m_logNameToProcessTime)
        {
            LOGS_ERROR << log.first << ": " << log.second.totalTime << "ms";
        }
        LOG_ERROR("");
    }

private:
    xsapi_internal_string m_ownerName;
    std::map<xsapi_internal_string, perf_counter> m_logNameToProcessTime;
    xbox::services::system::xbox_live_mutex m_lock;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
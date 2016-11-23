///*********************************************************
///
/// Copyright (c) Microsoft. All rights reserved.
/// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
/// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
/// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
/// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
///
///*********************************************************
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

    perf_tester(_In_ string_t ownerName) :
        m_ownerName(std::move(ownerName))
    {
    }

void start_timer(_In_ const string_t& logName)
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

void stop_timer(_In_ const string_t& logName, _In_ bool shouldReportAnyways = false)
{
#if PERF_TESTING
    std::chrono::nanoseconds totalTime = std::chrono::high_resolution_clock::now() - m_logNameToProcessTime[logName].previousTime;
    float totalTimeMS = totalTime.count() / 1000000.f;

    std::lock_guard<std::mutex> lock(m_lock.get());
    m_logNameToProcessTime[logName].totalTime = totalTimeMS;
    if (totalTimeMS > PERF_THRESHOLD_MS || shouldReportAnyways)
    {
        LOGS_ERROR << m_ownerName << " processing took : " << totalTimeMS << " ms";
        print();
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
    string_t m_ownerName;
    std::map<string_t, perf_counter> m_logNameToProcessTime;
    xbox::services::system::xbox_live_mutex m_lock;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <mutex>
#include <map>
#include <chrono>

#define ENABLE_PERF_PROFILING 0

#if ENABLE_PERF_PROFILING
#define PERF_START() xbox::services::detail::PerfTester::Instance().Start(__FUNCTION__)
#define PERF_STOP() xbox::services::detail::PerfTester::Instance().Stop(__FUNCTION__)
#define PERF_START_AREA(area) xbox::services::detail::PerfTester::Instance().Start(area)
#define PERF_STOP_AREA(area) xbox::services::detail::PerfTester::Instance().Stop(area)

namespace xbox{
namespace services{
namespace detail{

struct PerfTester
{
    static PerfTester& Instance()
    {
        static PerfTester instance;
        return instance;
    }

    void Start(const char* area)
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        m_stats[area].StartTime = std::chrono::high_resolution_clock::now();
    }

    void Stop(const char* area)
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        auto& stats{ m_stats[area] };

        ++stats.Count;
        auto duration{ std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - stats.StartTime).count() };
        stats.AverageTime = static_cast<uint64_t>(stats.AverageTime * ((double)(stats.Count - 1) / stats.Count) + duration * (1 / (double)stats.Count));
        if (duration > stats.MaxTime)
        {
            stats.MaxTime = duration;
        }
    }

    std::string FormatStats() const
    {
        std::stringstream ss;
        ss << __FUNCTION__ << std::endl;
        for (auto& entry : m_stats)
        {
            auto& s{ entry.second };
            ss << "Area:" << entry.first << " Count:" << s.Count << " MaxTime:" << s.MaxTime << " AverageTime:" << s.AverageTime << std::endl;
        }
        return ss.str();
    }

private:
    PerfTester() = default;

    struct Stats
    {
        int64_t Count{ 0 };
        int64_t MaxTime{ 0 };
        int64_t AverageTime{ 0 };
        std::chrono::time_point<std::chrono::high_resolution_clock> StartTime{};
    };

    mutable std::mutex m_mutex;
    std::map<std::string, Stats> m_stats;
};

}
}
}

#else
#define PERF_START()
#define PERF_STOP()
#define PERF_START_AREA(area)
#define PERF_STOP_AREA(area)
#endif

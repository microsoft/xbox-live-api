// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "PerformanceCounters.h"

#define UPDATE_AVG_AFTER_FRAMES 10
static std::mutex g_performanceCounterSingletonLock;
static std::shared_ptr<performance_counters> g_performanceCounterSingleton;

std::shared_ptr<performance_counters> performance_counters::get_singleton_instance()
{
    std::lock_guard<std::mutex> guard(g_performanceCounterSingletonLock);
    if (g_performanceCounterSingleton == nullptr)
    {
        g_performanceCounterSingleton = std::shared_ptr<performance_counters>(new performance_counters());
    }
    return g_performanceCounterSingleton;
}

performance_counters::performance_counters()
{
}

void
performance_counters::begin_capture( 
    _In_ const string_t& metadata
    )
{
    auto captureInstance = get_capture_instace(metadata);
    if (captureInstance != nullptr)
    {
        captureInstance->_Start();
    }
}

void
performance_counters::end_capture(
    _In_ const string_t& metadata
    )
{
    auto captureInstance = get_capture_instace(metadata);
    if (captureInstance != nullptr)
    {
        captureInstance->_End();
    }
}

std::shared_ptr<performance_capture>
performance_counters::get_capture_instace(
    _In_ const string_t& metadata
    )
{
    auto iter = m_performanceCaptureMap.find(metadata);
    if (iter == m_performanceCaptureMap.end())
    {
        auto capture = std::make_shared<performance_capture>(metadata);
        auto ret = m_performanceCaptureMap.insert(std::pair<string_t, std::shared_ptr<performance_capture>>(metadata, capture));
        iter = ret.first;
    }

    return iter->second;
}

performance_capture::performance_capture(
    _In_ string_t metadata
    ) :
    m_nFrames(0),
    m_metadata(std::move(metadata)),
    m_minTime(DBL_MAX),
    m_maxTime(DBL_MIN),
    m_averageTime(0.0),
    m_totalTime(0.0)
{
    m_startTime.QuadPart = 0;
    m_endTime.QuadPart = 0;
    m_frequency.QuadPart = 0;
    QueryPerformanceFrequency(&m_frequency);
}

void
performance_capture::_Start()
{
    QueryPerformanceCounter(&m_startTime);
}

void
performance_capture::_End()
{
    QueryPerformanceCounter(&m_endTime);
    update();
}

void
performance_capture::update()
{
    ++m_nFrames;
    auto deltaTime = get_delta_in_milliseconds(m_startTime, m_endTime);

    if (deltaTime < m_minTime)
    {
        m_minTime = deltaTime;
    }
    if (deltaTime > m_maxTime)
    {
        m_maxTime = deltaTime;
    }

    m_totalTime += deltaTime;
    if (m_nFrames > UPDATE_AVG_AFTER_FRAMES)
    {
        m_averageTime = m_totalTime / m_nFrames;
        m_totalTime = 0.0;
        m_nFrames = 0;
    }
}

double
performance_capture::get_delta_in_milliseconds(
    _In_ const LARGE_INTEGER& startTime,
    _In_ const LARGE_INTEGER& endTime
    )
{
    double deltaInSeconds = 0;

    if( startTime.QuadPart != 0 &&
        endTime.QuadPart != 0 &&
        m_frequency.QuadPart != 0)
    {
        LARGE_INTEGER deltaTicks;
        deltaTicks.QuadPart = endTime.QuadPart - startTime.QuadPart;
        deltaInSeconds = static_cast< double >( deltaTicks.QuadPart ) / static_cast< double >( m_frequency.QuadPart );
    }

    return deltaInSeconds * 1000.0f;
}
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

class performance_capture
{
public:
    performance_capture(
        _In_ string_t metadata
        );

    double min_time() const { return m_minTime; }
    double max_time() const { return m_maxTime; }
    double average_time() const { return m_averageTime; }
    const string_t& capture_metadata() const { return m_metadata; }

    void _Start();

    void _End();

private:
    void update();

    double get_delta_in_milliseconds(
        _In_ const LARGE_INTEGER& startTime,
        _In_ const LARGE_INTEGER& endTime
        );

    uint32_t m_nFrames;
    string_t m_metadata;
    double m_minTime;
    double m_maxTime;
    double m_averageTime;
    double m_totalTime;

    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_endTime;
    LARGE_INTEGER m_frequency;
};

class performance_counters
{
public:
    /// <summary>
    /// Gets the performance_counters singleton instance
    /// </summary>
    static std::shared_ptr<performance_counters> get_singleton_instance();

    /// <summary>
    /// Starts a user-defined event for a timing capture
    /// </summary>
    void begin_capture(
        _In_ const string_t& metadata
        );

    /// <summary>
    /// Defines the end of a user-defined event.
    /// </summary>
    void end_capture(
        _In_ const string_t& metadata
        );

    /// <summary>
    /// Returns the capture instance of a user-defined event.
    /// </summary>
    std::shared_ptr<performance_capture> get_capture_instace(
        _In_ const string_t& metadata
        );

private:

    performance_counters();
    performance_counters(const performance_counters&);
    void operator=(const performance_counters&);

    std::unordered_map<string_t, std::shared_ptr<performance_capture>> m_performanceCaptureMap;
};
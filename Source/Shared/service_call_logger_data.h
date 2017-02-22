// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#pragma once

#include <cpprest/json.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class service_call_logger_data
{
public:

    service_call_logger_data(
        _In_ string_t host,
        _In_ string_t uri,
        _In_ string_t xboxUserId,
        _In_ bool isGet,
        _In_ uint32_t httpStatusCode,
        _In_ string_t requestHeader,
        _In_ string_t requestBody,
        _In_ string_t responseHeader,
        _In_ string_t responseBody,
        _In_ std::chrono::milliseconds elapsedCallTime,
        _In_ chrono_clock_t::time_point requestTime
        );

    service_call_logger_data(
        _In_ string_t xboxUserId,
        _In_ string_t eventName,
        _In_ string_t eventPlayerSessionId,
        _In_ string_t eventDimensions,
        _In_ string_t eventMeasurements,
        _In_ chrono_clock_t::time_point requestTime
    );
    
    /// <summary>
    /// Output as multi-line string
    /// </summary>
    string_t to_string() const;

    static string_t get_csv_header();

private:

    void init();

    string_t m_host;
    string_t m_uri;

    string_t m_xboxUserId;

    string_t m_multiplayerCorrelationId;

    string_t m_requestHeader;
    string_t m_requestBody;

    string_t m_responseHeader;
    string_t m_responseBody;

    uint32_t m_httpStatusCode;

    std::chrono::milliseconds m_elapsedCallTime;
    chrono_clock_t::time_point m_requestTime;

    bool m_isGet;
    uint32_t m_id;

    bool m_isShoulderTap;
    uint64_t m_changeNumber;
    string_t m_sessionReferenceUriPath;

    bool m_isInGameEvent;
    string_t m_eventName;
    string_t m_playerSessionId;
    uint16_t m_version;
    string_t m_dimensions;
    string_t m_measurements;

    string_t m_breadCrumb;

    static uint32_t s_id;
    static const uint32_t s_invalidId = (uint32_t)-1;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
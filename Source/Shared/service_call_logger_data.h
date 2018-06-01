// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#pragma once

#include <cpprest/json.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class service_call_logger_data
{
public:

    service_call_logger_data(
        _In_ xsapi_internal_string host,
        _In_ xsapi_internal_string uri,
        _In_ xsapi_internal_string xboxUserId,
        _In_ bool isGet,
        _In_ uint32_t httpStatusCode,
        _In_ xsapi_internal_string requestHeader,
        _In_ xsapi_internal_string requestBody,
        _In_ xsapi_internal_string responseHeader,
        _In_ xsapi_internal_string responseBody,
        _In_ std::chrono::milliseconds elapsedCallTime,
        _In_ chrono_clock_t::time_point requestTime,
        _In_ xsapi_internal_string method
        );

    service_call_logger_data(
        _In_ xsapi_internal_string xboxUserId,
        _In_ xsapi_internal_string eventName,
        _In_ xsapi_internal_string eventPlayerSessionId,
        _In_ xsapi_internal_string eventDimensions,
        _In_ xsapi_internal_string eventMeasurements,
        _In_ chrono_clock_t::time_point requestTime
    );

    /// <summary>
    /// Output as multi-line string
    /// </summary>
    xsapi_internal_string to_string() const;

    static xsapi_internal_string get_csv_header();

private:

    void init();

    xsapi_internal_string m_host;
    xsapi_internal_string m_uri;

    xsapi_internal_string m_xboxUserId;

    xsapi_internal_string m_multiplayerCorrelationId;

    xsapi_internal_string m_requestHeader;
    xsapi_internal_string m_requestBody;

    xsapi_internal_string m_responseHeader;
    xsapi_internal_string m_responseBody;

    uint32_t m_httpStatusCode;

    std::chrono::milliseconds m_elapsedCallTime;
    chrono_clock_t::time_point m_requestTime;

    bool m_isGet;
    uint32_t m_id;

    bool m_isShoulderTap;
    uint64_t m_changeNumber;
    xsapi_internal_string m_sessionReferenceUriPath;

    bool m_isInGameEvent;
    xsapi_internal_string m_eventName;
    xsapi_internal_string m_playerSessionId;
    uint16_t m_version;
    xsapi_internal_string m_dimensions;
    xsapi_internal_string m_measurements;
    xsapi_internal_string m_method;

    xsapi_internal_string m_breadCrumb;

    static const uint32_t s_invalidId = (uint32_t)-1;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
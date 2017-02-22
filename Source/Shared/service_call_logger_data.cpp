// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "service_call_logger_data.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

//
// service_call_logger_data

uint32_t service_call_logger_data::s_id = 0;

service_call_logger_data::service_call_logger_data(
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
    ) :
    m_host(std::move(host)),
    m_uri(std::move(uri)),
    m_xboxUserId(std::move(xboxUserId)),
    m_isGet(std::move(isGet)),
    m_requestHeader(std::move(requestHeader)),
    m_requestBody(std::move(requestBody)),
    m_responseHeader(std::move(responseHeader)),
    m_responseBody(std::move(responseBody)),
    m_httpStatusCode(std::move(httpStatusCode)),
    m_elapsedCallTime(std::move(elapsedCallTime)),
    m_requestTime(std::move(requestTime)),
    m_isShoulderTap(false),
    m_isInGameEvent(false),
    m_changeNumber(0),
    m_version(0)
{
    init();
}

service_call_logger_data::service_call_logger_data(
    _In_ string_t xboxUserId,
    _In_ string_t eventName,
    _In_ string_t eventPlayerSessionId,
    _In_ string_t eventDimensions,
    _In_ string_t eventMeasurements,
    _In_ chrono_clock_t::time_point requestTime
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_host(_T("inGameEvents")),
    m_uri(_T("inGameEvents")),
    m_requestTime(std::move(requestTime)),
    m_eventName(std::move(eventName)),
    m_playerSessionId(std::move(eventPlayerSessionId)),
    m_dimensions(std::move(eventDimensions)),
    m_measurements(std::move(eventMeasurements)),
    m_elapsedCallTime(0),
    m_httpStatusCode(0),
    m_isGet(false),
    m_isShoulderTap(false),
    m_isInGameEvent(true),
    m_changeNumber(0),
    m_version(0)
{
    init();
}

void service_call_logger_data::init()
{
    m_id = ++s_id;
    m_breadCrumb = utils::create_guid(true).c_str();
}

string_t service_call_logger_data::to_string() const
{
    stringstream_t result;
    
    //Writing properties to string in a csv format. Order matters.

    //host
    result << _T('\"');
    result << m_host;
    result << _T("\",");

    //uri
    result << _T('\"');
    result << m_uri;
    result << _T("\",");

    //xboxUserId
    result << _T('\"');
    result << m_xboxUserId;
    result << _T("\",");

    //multiplayer correlation id
    result << _T('\"');
    result << m_multiplayerCorrelationId;
    result << _T("\",");

    //requestHeader
    result << _T('\"');
    result << utils::escape_special_characters(m_requestHeader);
    result << _T("\",");

    //requestBody
    result << _T('\"');
    result << utils::escape_special_characters(m_requestBody);
    result << _T("\",");

    //responseHeader
    result << _T('\"');
    result << utils::escape_special_characters(m_responseHeader);
    result << _T("\",");

    //responseBody
    result << _T('\"');
    result << utils::escape_special_characters(m_responseBody);
    result << _T("\",");

    //httpStatusCode
    result << _T('\"');
    result << m_httpStatusCode;
    result << _T("\",");

    //ellapsedCalltime
    result << _T('\"');
    result << m_elapsedCallTime.count();
    result << _T("\",");

    //requestTime
    result << _T('\"');
    result << utils::convert_timepoint_to_string(m_requestTime);
    result << _T("\",");

    //isGet
    result << _T('\"');
    result << ((m_isGet) ? _T("true"): _T("false"));
    result << _T("\",");

    //id
    result << _T('\"');
    result << m_id;
    result << _T("\",");

    //isShoulderTap
    result << _T('\"');
    result << ((m_isShoulderTap) ? _T("true") : _T("false"));
    result << _T("\",");

    //changeNumber
    result << _T('\"');
    result << m_changeNumber;
    result << _T("\",");

    //sessionReferenceUriPath
    result << _T('\"');
    result << m_sessionReferenceUriPath;
    result << _T("\",");

    //isInGameEvent
    result << _T('\"');
    result << ((m_isInGameEvent) ? _T("true") : _T("false"));
    result << _T("\",");

    //eventName
    result << _T('\"');
    result << m_eventName;
    result << _T("\",");

    //playerSessionId
    result << _T('\"');
    result << m_playerSessionId;
    result << _T("\",");

    //version
    result << _T('\"');
    result << m_version;
    result << _T("\",");

    //dimensions
    result << _T('\"');
    result << utils::escape_special_characters(m_dimensions);
    result << _T("\",");

    //measurements
    result << _T('\"');
    result << utils::escape_special_characters(m_measurements);
    result << _T("\",");

    //breadCrumb
    result << _T('\"');
    result << m_breadCrumb;
    result << _T("\"");

    result << _T("\n");

    return result.str();
}

string_t service_call_logger_data::get_csv_header()
{
    stringstream_t result;

    //headers
    result << _T("v1510\n");

    result << _T("\"Host\",");
    
    result << _T("\"Uri\",");
    
    result << _T("\"XboxUserId\",");
    
    result << _T("\"MultiplayerCorrelationId\",");

    result << _T("\"RequestHeaders\",");

    result << _T("\"RequestBody\",");

    result << _T("\"ResponseHeaders\",");

    result << _T("\"ResponseBody\",");

    result << _T("\"HttpStatusCode\",");

    result << _T("\"EllapsedCallTimeMs\",");

    result << _T("\"ReqTimeUTC\",");

    result << _T("\"IsGet\",");

    result << _T("\"LoggerId\",");

    result << _T("\"IsShoulderTap\",");

    result << _T("\"ChangeNumber\",");

    result << _T("\"SessionReferenceUriPath\",");

    result << _T("\"IsInGameEvent\",");

    result << _T("\"EventName\",");

    result << _T("\"EventPlayerSessionId\",");

    result << _T("\"EventVersion\",");

    result << _T("\"EventDimensionData\",");

    result << _T("\"EventMeasurementData\",");

    result << _T("\"BreadCrumb\"");

    result << _T("\n");

    return result.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

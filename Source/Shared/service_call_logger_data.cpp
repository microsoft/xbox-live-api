// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "service_call_logger_data.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

//
// service_call_logger_data

service_call_logger_data::service_call_logger_data(
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
    m_version(0),
    m_method(std::move(method))
{
    init();
}

service_call_logger_data::service_call_logger_data(
    _In_ xsapi_internal_string xboxUserId,
    _In_ xsapi_internal_string eventName,
    _In_ xsapi_internal_string eventPlayerSessionId,
    _In_ xsapi_internal_string eventDimensions,
    _In_ xsapi_internal_string eventMeasurements,
    _In_ chrono_clock_t::time_point requestTime
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_host("inGameEvents"),
    m_uri("inGameEvents"),
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
    m_id = ++get_xsapi_singleton()->m_loggerId;
    m_breadCrumb = utils::create_guid(true).c_str();
}

xsapi_internal_string service_call_logger_data::to_string() const
{
    xsapi_internal_stringstream result;
    
    //Writing properties to string in a csv format. Order matters.

    //host
    result << '\"';
    result << m_host;
    result << "\",";

    //uri
    result << '\"';
    result << m_uri;
    result << "\",";

    //xboxUserId
    result << '\"';
    result << m_xboxUserId;
    result << "\",";

    //multiplayer correlation id
    result << _T('\"');
    result << m_multiplayerCorrelationId;
    result << _T("\",");

    //requestHeader
    result << '\"';
    result << utils::escape_special_characters(m_requestHeader);
    result << "\",";

    //requestBody
    result << '\"';
    result << utils::escape_special_characters(m_requestBody);
    result << "\",";

    //responseHeader
    result << '\"';
    result << utils::escape_special_characters(m_responseHeader);
    result << "\",";

    //responseBody
    result << '\"';
    result << utils::escape_special_characters(m_responseBody);
    result << "\",";

    //httpStatusCode
    result << _T('\"');
    result << m_httpStatusCode;
    result << _T("\",");

    //ellapsedCalltime
    result << '\"';
    result << m_elapsedCallTime.count();
    result << "\",";

    //requestTime
    result << '\"';
    result << utils::convert_timepoint_to_string(m_requestTime);
    result << "\",";

    //isGet
    result << '\"';
    result << ((m_isGet) ? "true": "false");
    result << "\",";

    //id
    result << '\"';
    result << m_id;
    result << "\",";

    //isShoulderTap
    result << '\"';
    result << ((m_isShoulderTap) ? "true" : "false");
    result << "\",";

    //changeNumber
    result << '\"';
    result << m_changeNumber;
    result << "\",";

    //sessionReferenceUriPath
    result << '\"';
    result << m_sessionReferenceUriPath;
    result << "\",";

    //isInGameEvent
    result << '\"';
    result << ((m_isInGameEvent) ? "true" : "false");
    result << "\",";

    //eventName
    result << '\"';
    result << m_eventName;
    result << "\",";

    //playerSessionId
    result << '\"';
    result << m_playerSessionId;
    result << "\",";

    //version
    result << '\"';
    result << m_version;
    result << "\",";

    //dimensions
    result << '\"';
    result << utils::escape_special_characters(m_dimensions);
    result << "\",";

    //measurements
    result << '\"';
    result << utils::escape_special_characters(m_measurements);
    result << "\",";

    //breadCrumb
    result << '\"';
    result << m_breadCrumb;
    result << "\",";

    //breadCrumb
    result << '\"';
    result << m_method;
    result << "\"";

    result << "\n";

    return result.str();
}

xsapi_internal_string service_call_logger_data::get_csv_header()
{
    xsapi_internal_stringstream result;

    //headers
    result << "v1510\n";

    result << "\"Host\",";
    
    result << "\"Uri\",";
    
    result << "\"XboxUserId\",";
    
    result << "\"MultiplayerCorrelationId\",";

    result << "\"RequestHeaders\",";

    result << "\"RequestBody\",";

    result << "\"ResponseHeaders\",";

    result << "\"ResponseBody\",";

    result << "\"HttpStatusCode\",";

    result << "\"EllapsedCallTimeMs\",";

    result << "\"ReqTimeUTC\",";

    result << "\"IsGet\",";

    result << "\"LoggerId\",";

    result << "\"IsShoulderTap\",";

    result << "\"ChangeNumber\",";

    result << "\"SessionReferenceUriPath\",";

    result << "\"IsInGameEvent\",";

    result << "\"EventName\",";

    result << "\"EventPlayerSessionId\",";

    result << "\"EventVersion\",";

    result << "\"EventDimensionData\",";

    result << "\"EventMeasurementData\",";

    result << "\"BreadCrumb\",";

    result << "\"Method\"";

    result << "\n";

    return result.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/xbox_live_context_settings.h"
#include "xsapi/xbox_service_call_routed_event_args.h"
#include "http_call_response.h"
#include "user_context.h"
#include "utils.h"
#include "telemetry.h"
#ifdef _WIN32   
#include "service_call_logger_data.h"
#include "service_call_logger.h"
#endif
#include "http_call_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

const int RETRY_AFTER_CAP = 15;

http_call_response::http_call_response(
    _In_ const string_t& xboxUserId,
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const string_t& fullUrl,
    _In_ const web::http::http_request& request,
    _In_ const http_call_request_message& requestBody,
    _In_ xbox_live_api xboxLiveApi,
    _In_ const web::http::http_response& response
    ) :
    m_httpCallResponseBodyType(http_call_response_body_type::json_body),
    m_xboxUserId(xboxUserId),
    m_xboxLiveContextSettings(xboxLiveContextSettings),
    m_fullUrl(fullUrl),
    m_request(request),
    m_xboxLiveApi(xboxLiveApi),
    m_requestBody(requestBody)
{
    m_responseHeaders = response.headers();
    m_httpStatus = response.status_code();
    m_eTag = utils::extract_header_value(m_responseHeaders, ETAG_HEADER);
    m_responseDate = utils::extract_header_value(m_responseHeaders, DATE_HEADER);
    m_retryAfter = extract_retry_after_from_header(m_responseHeaders);
}

#ifndef DEFAULT_MOVE_ENABLED
http_call_response::http_call_response(http_call_response&& other)
{
    *this = std::move(other);
}

http_call_response& http_call_response::operator=(http_call_response&& other)
{
    if (this != &other)
    {
        m_httpCallResponseBodyType = std::move(other.m_httpCallResponseBodyType);
        m_responseBodyVector = std::move(other.m_responseBodyVector);
        m_responseBodyJson = std::move(other.m_responseBodyJson);
        m_responseBodyString = std::move(other.m_responseBodyString);
        m_responseHeaders = std::move(other.m_responseHeaders);
        m_httpStatus = std::move(other.m_httpStatus);
        m_errorCode = std::move(other.m_errorCode);
        m_errorMessage = std::move(other.m_errorMessage);
        m_eTag = std::move(other.m_eTag);
        m_responseDate = std::move(other.m_responseDate);
        m_retryAfter = std::move(other.m_retryAfter);
        m_requestTime = std::move(other.m_requestTime);
        m_responseTime = std::move(other.m_responseTime);
        m_xboxUserId = std::move(other.m_xboxUserId);
        m_xboxLiveContextSettings = std::move(other.m_xboxLiveContextSettings);
        m_fullUrl = std::move(other.m_fullUrl);
        m_request = std::move(other.m_request);
        m_requestBody = std::move(other.m_requestBody);
    }

    return *this;
}
#endif

std::chrono::seconds http_call_response::extract_retry_after_from_header(
    _In_ const web::http::http_headers& responseHeaders
    )
{
    std::chrono::seconds retryAfter = std::chrono::seconds();

    string_t retryAfterValue = utils::extract_header_value(responseHeaders, _T("Retry-After"), _T(""));
    if (retryAfterValue.length() > 0)
    {
        int value = 0;
        stringstream_t ss(retryAfterValue);
        ss >> value;

        if (!ss.fail())
        {
            if (value > RETRY_AFTER_CAP)
            {
                // cap the Retry-After header so users won't be locked out of an endpoint for a long time the limit is hit near the end of a period
                value = RETRY_AFTER_CAP;
            }

            retryAfter = std::chrono::seconds(value);
        }
    }

    return retryAfter;
}

string_t http_call_response::response_body_to_string() const
{
    switch (m_httpCallResponseBodyType)
    {
        case http_call_response_body_type::json_body: return m_responseBodyJson.serialize();
        case http_call_response_body_type::string_body: return m_responseBodyString;
        case http_call_response_body_type::vector_body: return _T("Binary data response");
        default: return _T("Unknown response");
    }
}

void http_call_response::_Set_response_body(_In_ const string_t& responseBodyString)
{
    m_responseBodyString = responseBodyString;
    m_httpCallResponseBodyType = http_call_response_body_type::string_body;
}

void http_call_response::_Set_response_body(_In_ const std::vector<unsigned char>& responseBodyVector)
{
    m_responseBodyVector = responseBodyVector;
    m_httpCallResponseBodyType = http_call_response_body_type::vector_body;
}

void http_call_response::_Set_response_body(_In_ const web::json::value& responseBodyJson)
{
    m_responseBodyJson = responseBodyJson;
    m_httpCallResponseBodyType = http_call_response_body_type::json_body;

    if (http_status() == static_cast<int>(xbox_live_error_code::http_status_429_too_many_requests))
    {
        m_errorMessage = get_throttling_error_message();
    }

}

void http_call_response::_Set_timing(
    _In_ const chrono_clock_t::time_point& requestTime,
    _In_ const chrono_clock_t::time_point& responseTime
    )
{
    m_requestTime = requestTime;
    m_responseTime = responseTime;
}

void http_call_response::_Set_error_info(
    _In_ const std::error_code& errCode,
    _In_ const std::string& errMessage
    )
{
    m_errorCode = errCode;
    m_errorMessage = errMessage;
}

const chrono_clock_t::time_point& http_call_response::_Local_response_time() const 
{ 
    return m_responseTime; 
}

std::shared_ptr<xbox_live_context_settings> http_call_response::_Context_settings() const
{
    return m_xboxLiveContextSettings;
}

const web::http::http_request& http_call_response::_Request() const
{
    return m_request;
}

void http_call_response::_Set_full_url(
    _In_ const string_t& fullUrl
    )
{
    m_fullUrl = fullUrl;
}

void http_call_response::_Route_service_call() const
{
    record_service_result();
#ifdef _WIN32   
    auto fullUrl = m_request.absolute_uri().to_string();
    if (!m_errorCode)
    {
        TraceLoggingWrite(
            g_hTraceLoggingProvider,
            "http_request",
            TraceLoggingWideString(fullUrl.c_str(), "url"),
            TraceLoggingInt32(m_httpStatus, "status"),
            TraceLoggingInt64(std::chrono::duration_cast<std::chrono::milliseconds>(m_responseTime - m_requestTime).count(), "duration"),
            TraceLoggingKeyword(XSAPI_TELEMETRY_KEYWORDS)
            );
    }
    else
    {
        TraceLoggingWrite(
            g_hTraceLoggingProvider,
            "http_request_exception",
            TraceLoggingWideString(fullUrl.c_str(), "url"),
            TraceLoggingInt32(m_errorCode.value(), "err"),
            TraceLoggingString(m_errorMessage.c_str(), "errMsg"),
            TraceLoggingInt64(std::chrono::duration_cast<std::chrono::milliseconds>(m_responseTime - m_requestTime).count(), "duration"),
            TraceLoggingKeyword(XSAPI_TELEMETRY_KEYWORDS)
            );
    }

    bool callFailed = FAILED(utils::convert_http_status_to_hresult(m_httpStatus));
    bool logCall = 
        (xbox::services::service_call_logger::get_singleton_instance()->is_enabled()) ||
        (system::xbox_live_services_settings::get_singleton_instance()->_Is_at_diagnostics_trace_level(xbox_services_diagnostics_trace_level::info)) ||
        (callFailed && system::xbox_live_services_settings::get_singleton_instance()->_Is_at_diagnostics_trace_level(xbox_services_diagnostics_trace_level::error));

    if (logCall || m_xboxLiveContextSettings->enable_service_call_routed_events())
    {
        uint32_t responseCount = InterlockedIncrement(&get_xsapi_singleton()->m_responseCount);

        web::http::http_headers headers = m_request.headers();

        string_t token = utils::extract_header_value(headers, AUTH_HEADER);
        string_t sig = utils::extract_header_value(headers, SIG_HEADER);
        headers.remove(AUTH_HEADER);
        headers.remove(SIG_HEADER);

        xbox::services::xbox_service_call_routed_event_args args(
            m_xboxUserId,
            m_request.method(),
            m_fullUrl,
            utils::headers_to_string(headers),
            m_requestBody,
            responseCount,
            utils::headers_to_string(m_responseHeaders),
            response_body_to_string(),
            e_tag(),
            token,
            sig,
            m_httpStatus,
            m_requestTime,
            m_responseTime
            );

        if (logCall)
        {
            std::shared_ptr<service_call_logger> tracker = service_call_logger::get_singleton_instance();

            const web::uri uri = args.uri();
            const string_t host = uri.host();
            const bool isGet = (utils::str_icmp(args.http_method(), L"GET") == 0);

            service_call_logger_data sharedData(
                host,
                fullUrl,
                args.xbox_user_id(),
                isGet,
                static_cast<uint32_t>(args.http_status()),
                args.request_headers(),
                args.request_body().request_message_string(),
                args.response_headers(),
                args.response_body(),
                args.elapsed_call_time(),
                args.request_time());

            tracker->log(sharedData.to_string());
        }

        if (m_xboxLiveContextSettings->enable_service_call_routed_events())
        {
            m_xboxLiveContextSettings->_Raise_service_call_routed_event(args);
        }
    }
#endif
}

void http_call_response::record_service_result() const
{
    // Only remember result if there was an error and there was a Retry-After header
    if (m_xboxLiveApi != xbox_live_api::unspecified &&
        http_status() >= 400 &&
        retry_after().count() > 0)
    {
        const auto& currentTime = _Local_response_time();

        http_retry_after_api_state state(
            currentTime + retry_after(),
            err_code(),
            err_message()
            );
        auto retryAfterManager = http_retry_after_manager::get_http_retry_after_manager_singleton();
        retryAfterManager->set_state(m_xboxLiveApi, state);
    }
}

std::string http_call_response::get_throttling_error_message() const
{
    _In_ const web::json::value& json = response_body_json();
    _In_ const std::chrono::seconds& retryAfter = retry_after();
        
    std::error_code errc;
    string_t limitType = utils::extract_json_string(json, _T("limitType"), errc);
    int currentRequests = utils::extract_json_int(json, _T("currentRequests"), errc);
    int maxRequests = utils::extract_json_int(json, _T("maxRequests"), errc);
    int periodInSeconds = utils::extract_json_int(json, _T("periodInSeconds"), errc);

    if (!errc && maxRequests > 0 && currentRequests > 0 && periodInSeconds > 0 && limitType == _T("Rate"))
    {
        std::stringstream errMessage;
        errMessage << "Too many requests sent. ";
        errMessage << currentRequests;
        errMessage << " of ";
        errMessage << maxRequests;
        errMessage << " in ";
        errMessage << periodInSeconds;
        errMessage << " seconds. ";
        if (retryAfter.count() > 0)
        {
            errMessage << "Retry after ";
            errMessage << retryAfter.count();
            errMessage << " seconds";
        }
        return errMessage.str();
    }

    return "";
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

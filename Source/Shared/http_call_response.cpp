// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/xbox_live_context_settings.h"
#include "xbox_service_call_routed_event_args_internal.h"
#include "http_call_response_internal.h"
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
    _In_ std::shared_ptr<http_call_response_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

std::shared_ptr<http_call_response_internal> http_call_response::_Internal_response() const
{
    return m_internalObj;
}

web::http::http_headers http_call_response::response_headers() const
{
    auto headers = web::http::http_headers();

    auto& internalHeaders = m_internalObj->response_headers();
    for (auto& kvp : internalHeaders)
    {
        headers.add(utils::string_t_from_internal_string(kvp.first), utils::string_t_from_internal_string(kvp.second));
    }
    return headers;
}

DEFINE_GET_ENUM_TYPE(http_call_response, http_call_response_body_type, body_type);
DEFINE_GET_STRING(http_call_response, response_body_string);
DEFINE_GET_OBJECT_REF(http_call_response, web::json::value, response_body_json);
DEFINE_GET_VECTOR(http_call_response, unsigned char, response_body_vector);
DEFINE_GET_UINT32(http_call_response, http_status);
DEFINE_GET_OBJECT_REF(http_call_response, std::error_code, err_code);
DEFINE_GET_STD_STRING(http_call_response, err_message);
DEFINE_GET_STRING(http_call_response, e_tag);
DEFINE_GET_STRING(http_call_response, response_date);
DEFINE_GET_OBJECT_REF(http_call_response, std::chrono::seconds, retry_after);

http_call_response_internal::http_call_response_internal(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& fullUrl,
    _In_ const http_call_request_message_internal& requestBody,
    _In_ xbox_live_api xboxLiveApi,
    _In_ uint32_t responseStatusCode
    ) :
    m_httpCallResponseBodyType(http_call_response_body_type::string_body),
    m_xboxUserId(xboxUserId),
    m_xboxLiveContextSettings(xboxLiveContextSettings),
    m_httpMethod(httpMethod),
    m_fullUrl(fullUrl),
    m_xboxLiveApi(xboxLiveApi),
    m_requestBody(requestBody),
    m_httpStatus(responseStatusCode)
{
}

http_call_response_internal::http_call_response_internal(
    _In_ const std::shared_ptr<http_call_data> httpCallData
    )
{
    HRESULT hr = S_OK;
    uint32_t platformErrorCode = 0;
    HCHttpCallResponseGetNetworkErrorCode(httpCallData->callHandle, &hr, &platformErrorCode);
    HCHttpCallResponseGetStatusCode(httpCallData->callHandle, &m_httpStatus);

    if (httpCallData->userContext != nullptr)
    {
        m_xboxUserId = httpCallData->userContext->xbox_user_id();
    }
    m_xboxLiveContextSettings = httpCallData->xboxLiveContextSettings;
    m_httpMethod = httpCallData->httpMethod;
    m_fullUrl = httpCallData->serverName + utils::internal_string_from_string_t(httpCallData->pathQueryFragment.to_string());
    m_requestBody = httpCallData->requestBody;
    m_xboxLiveApi = httpCallData->xboxLiveApi;
    m_requestTime = httpCallData->requestStartTime;
    m_responseTime = chrono_clock_t::now();

    uint32_t numHeaders;
    HCHttpCallResponseGetNumHeaders(httpCallData->callHandle, &numHeaders);
    for (uint32_t i = 0; i < numHeaders; ++i)
    {
        const char* headerName;
        const char* headerValue;
        HCHttpCallResponseGetHeaderAtIndex(httpCallData->callHandle, i, &headerName, &headerValue);
        add_response_header(headerName, headerValue);
    }

    PCSTR responseBody = nullptr;
    if (FAILED(hr))
    {
        m_errorCode = static_cast<xbox_live_error_code>(hr);
        HCHttpCallResponseGetResponseString(httpCallData->callHandle, &responseBody);
        if (responseBody != nullptr)
        {
            m_errorMessage = "HTTP Response Body: " + xsapi_internal_string(responseBody);
        }
    }
    else
    {
#pragma warning(suppress: 4244)
        m_errorCode = get_xbox_live_error_code_from_http_status(m_httpStatus);

        if (httpCallData->httpCallResponseBodyType == http_call_response_body_type::vector_body)
        {
            size_t responseSize;
            HCHttpCallResponseGetResponseBodyBytesSize(httpCallData->callHandle, &responseSize);

            xsapi_internal_vector<uint8_t> responseBodyVector(responseSize);
            HCHttpCallResponseGetResponseBodyBytes(httpCallData->callHandle, responseSize, &responseBodyVector[0], nullptr);

            set_response_body(responseBodyVector);
        }
        else
        {
            HCHttpCallResponseGetResponseString(httpCallData->callHandle, &responseBody);
            if (httpCallData->httpCallResponseBodyType == http_call_response_body_type::json_body)
            {
                web::json::value responseBodyJson;
                std::error_code errCode;
                responseBodyJson = web::json::value::parse(utils::string_t_from_internal_string(responseBody), errCode);
                if (!errCode)
                {
                    set_response_body(responseBodyJson);
                }
            }
            else if (httpCallData->httpCallResponseBodyType == http_call_response_body_type::string_body)
            {
                set_response_body(responseBody);
            }
        }
    }
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

xsapi_internal_string http_call_response_internal::response_body_to_string() const
{
    switch (m_httpCallResponseBodyType)
    {
        case http_call_response_body_type::json_body: return utils::internal_string_from_string_t(m_responseBodyJson.serialize());
        case http_call_response_body_type::string_body: return m_responseBodyString;
        case http_call_response_body_type::vector_body: return "Binary data response";
        default: return "Unknown response";
    }
}

void http_call_response_internal::add_response_header(
    _In_ const xsapi_internal_string& headerName,
    _In_ const xsapi_internal_string& headerValue
    )
{
    m_responseHeaders[headerName] = headerValue;

    // special handling for certain headers
    if (headerName == ETAG_HEADER)
    {
        m_eTag = headerValue;
    }
    else if (headerName == DATE_HEADER)
    {
        m_responseDate = headerValue;
    }
    else if (headerName == RETRY_AFTER_HEADER && headerValue.length() > 0)
    {
        int value = 0;
        xsapi_internal_stringstream ss(headerValue);
        ss >> value;

        if (!ss.fail())
        {
            if (value > RETRY_AFTER_CAP)
            {
                // cap the Retry-After header so users won't be locked out of an endpoint for a long time the limit is hit near the end of a period
                value = RETRY_AFTER_CAP;
            }

            m_retryAfter = std::chrono::seconds(value);
        }
    }
}

void http_call_response_internal::set_response_body(_In_ const xsapi_internal_string& responseBodyString)
{
    m_responseBodyString = responseBodyString;
    m_httpCallResponseBodyType = http_call_response_body_type::string_body;
}

void http_call_response_internal::set_response_body(_In_ const xsapi_internal_vector<unsigned char>& responseBodyVector)
{
    m_responseBodyVector = responseBodyVector;
    m_httpCallResponseBodyType = http_call_response_body_type::vector_body;
}

void http_call_response_internal::set_response_body(_In_ const web::json::value& responseBodyJson)
{
    m_responseBodyJson = responseBodyJson;
    m_httpCallResponseBodyType = http_call_response_body_type::json_body;

    if (http_status() == static_cast<int>(xbox_live_error_code::http_status_429_too_many_requests))
    {
        m_errorMessage = get_throttling_error_message();
    }

}

void http_call_response_internal::set_timing(
    _In_ const chrono_clock_t::time_point& requestTime,
    _In_ const chrono_clock_t::time_point& responseTime
    )
{
    m_requestTime = requestTime;
    m_responseTime = responseTime;
}

void http_call_response_internal::set_error_info(
    _In_ const std::error_code& errCode,
    _In_ const xsapi_internal_string& errMessage
    )
{
    m_errorCode = errCode;
    m_errorMessage = errMessage;
}

void http_call_response_internal::set_full_url(_In_ const xsapi_internal_string& fullUrl)
{
    m_fullUrl = fullUrl;
}

const chrono_clock_t::time_point& http_call_response_internal::local_response_time() const 
{ 
    return m_responseTime;
}

std::shared_ptr<xbox_live_context_settings> http_call_response_internal::context_settings() const
{
    return m_xboxLiveContextSettings;
}

void http_call_response_internal::route_service_call() const
{
    record_service_result();
#ifdef _WIN32
    if (!m_errorCode)
    {
        TraceLoggingWrite(
            g_hTraceLoggingProvider,
            "http_request",
            TraceLoggingString(m_fullUrl.c_str(), "url"),
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
            TraceLoggingString(m_fullUrl.c_str(), "url"),
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

        http_headers headers = m_responseHeaders;

        xsapi_internal_string token;
        auto iter = headers.find(AUTH_HEADER);
        if (iter != headers.end())
        {
            token = iter->second;
            headers.erase(iter);
        }
        xsapi_internal_string sig;
        iter = headers.find(SIG_HEADER);
        if (iter != headers.end())
        {
            sig = iter->second;
            headers.erase(iter);
        }

        auto args = xsapi_allocate_shared<xbox::services::xbox_service_call_routed_event_args_internal>(
            m_xboxUserId,
            m_httpMethod,
            m_fullUrl,
            utils::headers_to_string(headers),
            m_requestBody,
            responseCount,
            utils::headers_to_string(m_responseHeaders),
            response_body_to_string(),
            m_eTag,
            token,
            sig,
            m_httpStatus,
            m_requestTime,
            m_responseTime
            );

        if (logCall)
        {
            std::shared_ptr<service_call_logger> tracker = service_call_logger::get_singleton_instance();

            web::uri uri = utils::string_t_from_internal_string(args->uri());
            const xsapi_internal_string host = utils::internal_string_from_string_t(uri.host());
            const bool isGet = (utils::str_icmp(args->http_method(), "GET") == 0);

            service_call_logger_data sharedData(
                host,
                args->uri(),
                args->xbox_user_id(),
                isGet,
                args->http_status(),
                args->request_headers(),
                m_requestBody.request_message_string(),
                args->response_headers(),
                args->response_body(),
                args->elapsed_call_time(),
                args->request_time(),
                args->http_method());

            tracker->log(sharedData.to_string());
        }

        if (m_xboxLiveContextSettings->enable_service_call_routed_events())
        {
            m_xboxLiveContextSettings->_Raise_service_call_routed_event(xbox_service_call_routed_event_args(args));
        }
    }
#endif
}

void http_call_response_internal::record_service_result() const
{
    // Only remember result if there was an error and there was a Retry-After header
    if (m_xboxLiveApi != xbox_live_api::unspecified &&
        http_status() >= 400 &&
        retry_after().count() > 0)
    {
        const auto& currentTime = m_responseTime;

        http_retry_after_api_state state(
            currentTime + retry_after(),
            err_code(),
            err_message()
            );
        auto retryAfterManager = http_retry_after_manager::get_http_retry_after_manager_singleton();
        retryAfterManager->set_state(m_xboxLiveApi, state);
    }
}

xsapi_internal_string http_call_response_internal::get_throttling_error_message() const
{
    _In_ const web::json::value& json = response_body_json();
    _In_ const std::chrono::seconds& retryAfter = retry_after();

    std::error_code errc;
    xsapi_internal_string limitType = utils::extract_json_string(json, "limitType", errc);
    int currentRequests = utils::extract_json_int(json, "currentRequests", errc);
    int maxRequests = utils::extract_json_int(json, "maxRequests", errc);
    int periodInSeconds = utils::extract_json_int(json, "periodInSeconds", errc);

    if (!errc && maxRequests > 0 && currentRequests > 0 && periodInSeconds > 0 && limitType == "Rate")
    {
        xsapi_internal_stringstream errMessage;
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

xbox_live_error_code http_call_response_internal::get_xbox_live_error_code_from_http_status(
    _In_ uint32_t statusCode
    )
{
    if (statusCode < 300 || statusCode >= 600)
    {
        // Treat as success so 
        //      if (!result.err()) 
        // works properly which requires all non-errors to be 0.
        return xbox_live_error_code::no_error;
    }
    else
    {
        return static_cast<xbox_live_error_code>(statusCode);
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

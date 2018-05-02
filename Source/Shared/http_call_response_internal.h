// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/http_call_request_message.h"
#include "xsapi/http_call.h"
#include "http_call_request_message_internal.h"
#include "shared_macros.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

struct http_call_data;

class http_call_response_internal
{
public:
    http_call_response_internal(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& baseUrl,
        _In_ const http_call_request_message_internal& requestBody,
        _In_ xbox_live_api xboxLiveApi,
        _In_ uint32_t responseStatusCode = 0
        );

    http_call_response_internal(
        _In_ const std::shared_ptr<http_call_data> callData
        );

#ifndef DEFAULT_MOVE_ENABLED
    http_call_response(http_call_response&& other);
    http_call_response& operator=(http_call_response&& other);
#endif

    http_call_response_body_type body_type() const { return m_httpCallResponseBodyType; }

    const xsapi_internal_string& response_body_string() const { return m_responseBodyString; }

    const web::json::value& response_body_json() const { return m_responseBodyJson; }

    const xsapi_internal_vector<unsigned char>& response_body_vector() const { return m_responseBodyVector; }

    const http_headers& response_headers() const { return m_responseHeaders; }

    uint32_t http_status() const { return m_httpStatus; }

    const std::error_code& err_code() const { return m_errorCode; }

    const xsapi_internal_string& err_message() const { return m_errorMessage; }

    const xsapi_internal_string& e_tag() const { return m_eTag; }

    const xsapi_internal_string& response_date() const { return m_responseDate; }

    const std::chrono::seconds& retry_after() const { return m_retryAfter; }

    void add_response_header(
        _In_ const xsapi_internal_string& headerName,
        _In_ const xsapi_internal_string& headerValue
        );

    void remove_response_header(_In_ const xsapi_internal_string& headerName)
    {
        m_responseHeaders.erase(headerName);
    }

    void set_response_body(_In_ const xsapi_internal_string& responseBodyString);

    void set_response_body(_In_ const xsapi_internal_vector<unsigned char>& responseBodyVector);

    void set_response_body(_In_ const web::json::value& responseBodyJson);

    void set_timing(
        _In_ const chrono_clock_t::time_point& requestTime,
        _In_ const chrono_clock_t::time_point& responseTime
        );

    void set_error_info(
        _In_ const std::error_code& errCode,
        _In_ const xsapi_internal_string& errMessage = xsapi_internal_string()
        );

    void set_full_url(_In_ const xsapi_internal_string& fullUrl);

    void route_service_call() const;

    const chrono_clock_t::time_point& local_response_time() const;

    std::shared_ptr<xbox_live_context_settings> context_settings() const;

private:
    void record_service_result() const;
    xsapi_internal_string get_throttling_error_message() const;

    static xbox_live_error_code get_xbox_live_error_code_from_http_status(_In_ uint32_t statusCode);

    http_call_response_body_type m_httpCallResponseBodyType;
    xsapi_internal_vector<unsigned char> m_responseBodyVector;
    xsapi_internal_string m_responseBodyString;
    web::json::value m_responseBodyJson;

    uint32_t m_httpStatus;
    std::error_code m_errorCode;
    xsapi_internal_string m_errorMessage;

    xbox::services::http_headers m_responseHeaders;
    xsapi_internal_string m_httpMethod;
    xsapi_internal_string m_eTag;
    xsapi_internal_string m_responseDate;
    std::chrono::seconds m_retryAfter;
    chrono_clock_t::time_point m_requestTime;
    chrono_clock_t::time_point m_responseTime;

    xsapi_internal_string m_xboxUserId;
    std::shared_ptr<xbox_live_context_settings> m_xboxLiveContextSettings;
    xsapi_internal_string m_fullUrl;
    http_call_request_message_internal m_requestBody;
    xbox_live_api m_xboxLiveApi;

    xsapi_internal_string response_body_to_string() const;
};

template<typename T>
xbox::services::xbox_live_result<T>
get_xbl_result_from_response(_In_ std::shared_ptr<http_call_response> response, _In_ std::function<T(_In_ const web::json::value&)> deserializeFn)
{
    if (response->response_body_json().size() != 0)
    {
        return xbox::services::xbox_live_result<T>(deserializeFn(response->response_body_json()), response->err_code(), response->err_message());
    }
    else
    {
        return xbox::services::xbox_live_result<T>(response->err_code(), response->err_message());
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

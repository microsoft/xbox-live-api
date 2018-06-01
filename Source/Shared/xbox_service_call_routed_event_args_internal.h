// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "http_call_request_message_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xbox_service_call_routed_event_args_internal
{
public:
    _XSAPIIMP const xsapi_internal_string& xbox_user_id() const { return m_xboxUserId; }

    _XSAPIIMP const xsapi_internal_string& http_method() const { return m_httpMethod; }

    _XSAPIIMP const xsapi_internal_string& uri() const { return m_uri; }

    _XSAPIIMP const xsapi_internal_string& request_headers() const { return m_requestHeaders; }

    _XSAPIIMP const http_call_request_message_internal& request_body() const { return m_requestBody; }

    _XSAPIIMP uint32_t response_count() const { return m_responseCount; }

    _XSAPIIMP const xsapi_internal_string& response_headers() const { return m_responseHeaders; }

    _XSAPIIMP const xsapi_internal_string& response_body() const { return m_responseBody; }

    _XSAPIIMP const xsapi_internal_string& etag() const { return m_etag; }

    _XSAPIIMP const xsapi_internal_string& token() const { return m_token; }

    _XSAPIIMP const xsapi_internal_string& signature() const { return m_signature; }

    _XSAPIIMP uint32_t http_status() const { return m_httpStatus; }

    _XSAPIIMP xsapi_internal_string full_response_formatted() const;

    _XSAPIIMP const chrono_clock_t::time_point& request_time() const { return m_requestTime; }

    _XSAPIIMP const chrono_clock_t::time_point& response_time() const { return m_responseTime; }

    _XSAPIIMP const std::chrono::milliseconds& elapsed_call_time() const { return m_elapsedCallTime; }

    xbox_service_call_routed_event_args_internal(
        _In_ xsapi_internal_string xboxUserId,
        _In_ xsapi_internal_string httpMethod,
        _In_ xsapi_internal_string uri,
        _In_ xsapi_internal_string requestHeaders,
        _In_ http_call_request_message_internal requestBody,
        _In_ uint32_t responseCount,
        _In_ xsapi_internal_string responseHeaders,
        _In_ xsapi_internal_string responseBody,
        _In_ xsapi_internal_string etag,
        _In_ xsapi_internal_string token,
        _In_ xsapi_internal_string signature,
        _In_ uint32_t httpStatus,
        _In_ chrono_clock_t::time_point requestTime,
        _In_ chrono_clock_t::time_point responseTime
        );

    xbox_service_call_routed_event_args_internal(
        _In_ hc_call_handle_t hcCallHandle
        );

private:
    xsapi_internal_string m_xboxUserId;
    xsapi_internal_string m_httpMethod;
    xsapi_internal_string m_uri;
    xsapi_internal_string m_requestHeaders;
    http_call_request_message_internal m_requestBody;
    uint32_t m_responseCount;
    xsapi_internal_string m_responseHeaders;
    xsapi_internal_string m_responseBody;
    xsapi_internal_string m_etag;
    xsapi_internal_string m_token;
    xsapi_internal_string m_signature;
    uint32_t m_httpStatus;
    std::chrono::milliseconds m_elapsedCallTime;
    chrono_clock_t::time_point m_requestTime;
    chrono_clock_t::time_point m_responseTime;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
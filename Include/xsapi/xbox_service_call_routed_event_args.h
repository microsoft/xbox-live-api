// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/http_call_request_message.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

/// <summary>
/// Contains information about a service call.
/// </summary>
class xbox_service_call_routed_event_args
{
public:

    /// <summary>
    /// Returns the XboxUserId of the User contacting the service
    /// </summary>
    _XSAPIIMP const string_t& xbox_user_id() const { return m_xboxUserId; }

    /// <summary>
    /// Returns the HTTP method used to contact the service.  For example, "GET".
    /// </summary>
    _XSAPIIMP const string_t& http_method() const { return m_httpMethod; }

    /// <summary>
    /// Returns the URI used to contact the service.
    /// </summary>
    _XSAPIIMP const string_t& uri() const { return m_uri; }

    /// <summary>
    /// Returns the request headers that were sent to the service.
    /// </summary>
    _XSAPIIMP const string_t& request_headers() const { return m_requestHeaders; }

    /// <summary>
    /// Returns the request body that was sent to the service.
    /// </summary>
    _XSAPIIMP const http_call_request_message& request_body() const { return m_requestBody; }

    /// <summary>
    /// Returns the number of responses in this session.
    /// </summary>
    _XSAPIIMP uint32_t response_count() const { return m_responseCount; }

    /// <summary>
    /// Returns the response headers returned by the service.
    /// </summary>
    _XSAPIIMP const string_t& response_headers() const { return m_responseHeaders; }

    /// <summary>
    /// Returns the response body returned by the service.
    /// </summary>
    _XSAPIIMP const string_t& response_body() const { return m_responseBody; }

    /// <summary>
    /// Returns the ETag returned by the service.
    /// </summary>
    _XSAPIIMP const string_t& etag() const { return m_etag; }

    /// <summary>
    /// Returns the authentication token returned by GetTokenAndSignatureAsync.
    /// </summary>
    _XSAPIIMP const string_t& token() const { return m_token; }

    /// <summary>
    /// Returns the authentication signature returned by GetTokenAndSignatureAsync.
    /// </summary>
    _XSAPIIMP const string_t& signature() const { return m_signature; }

    /// <summary>
    /// Returns the HTTP status code. For example, 200.
    /// </summary>
    _XSAPIIMP uint32_t http_status() const { return m_httpStatus; }

    /// <summary>
    /// Returns the a full response log formatted message of all the properties in XboxServiceCallRoutedEventArgs.
    /// </summary>
    _XSAPIIMP const string_t full_response_formatted() const;

    /// <summary>
    /// Returns the  time when the request was sent.
    /// </summary>
    _XSAPIIMP const chrono_clock_t::time_point& request_time() const { return m_requestTime; }

    /// <summary>
    /// Returns the  time when the response was received.
    /// </summary>
    _XSAPIIMP const chrono_clock_t::time_point& response_time() const { return m_responseTime; }

    /// <summary>
    /// Returns the elapsed time span between sending a request and receiving a response.
    /// </summary>
    _XSAPIIMP const std::chrono::milliseconds& elapsed_call_time() const { return m_elapsedCallTime; }

    /// <summary>
    /// Internal function
    /// </summary>
    xbox_service_call_routed_event_args(
        _In_ string_t xboxUserId,
        _In_ string_t httpMethod,
        _In_ string_t uri,
        _In_ string_t requestHeaders,
        _In_ http_call_request_message requestBody,
        _In_ uint32_t responseCount,
        _In_ string_t responseHeaders,
        _In_ string_t responseBody,
        _In_ string_t etag,
        _In_ string_t token,
        _In_ string_t signature,
        _In_ uint32_t httpStatus,
        _In_ chrono_clock_t::time_point requestTime,
        _In_ chrono_clock_t::time_point responseTime
        );

private:
    string_t m_xboxUserId;
    string_t m_httpMethod;
    string_t m_uri;
    string_t m_requestHeaders;
    http_call_request_message m_requestBody;
    uint32_t m_responseCount;
    string_t m_responseHeaders;
    string_t m_responseBody;
    string_t m_etag;
    string_t m_token;
    string_t m_signature;
    uint32_t m_httpStatus;
    std::chrono::milliseconds m_elapsedCallTime;
    chrono_clock_t::time_point m_requestTime;
    chrono_clock_t::time_point m_responseTime;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
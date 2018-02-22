// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/http_call_request_message.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xbox_service_call_routed_event_args_internal;

/// <summary>
/// Contains information about a service call.
/// </summary>
class xbox_service_call_routed_event_args
{
public:

    /// <summary>
    /// Returns the XboxUserId of the User contacting the service
    /// </summary>
    _XSAPIIMP string_t xbox_user_id() const;

    /// <summary>
    /// Returns the HTTP method used to contact the service.  For example, "GET".
    /// </summary>
    _XSAPIIMP string_t http_method() const;

    /// <summary>
    /// Returns the URI used to contact the service.
    /// </summary>
    _XSAPIIMP string_t uri() const;

    /// <summary>
    /// Returns the request headers that were sent to the service.
    /// </summary>
    _XSAPIIMP string_t request_headers() const;

    /// <summary>
    /// Returns the request body that was sent to the service.
    /// </summary>
    _XSAPIIMP const http_call_request_message& request_body() const { return m_requestBody; }

    /// <summary>
    /// Returns the number of responses in this session.
    /// </summary>
    _XSAPIIMP uint32_t response_count() const;

    /// <summary>
    /// Returns the response headers returned by the service.
    /// </summary>
    _XSAPIIMP string_t response_headers() const;

    /// <summary>
    /// Returns the response body returned by the service.
    /// </summary>
    _XSAPIIMP string_t response_body() const;

    /// <summary>
    /// Returns the ETag returned by the service.
    /// </summary>
    _XSAPIIMP string_t etag() const;

    /// <summary>
    /// Returns the authentication token returned by GetTokenAndSignatureAsync.
    /// </summary>
    _XSAPIIMP string_t token() const;

    /// <summary>
    /// Returns the authentication signature returned by GetTokenAndSignatureAsync.
    /// </summary>
    _XSAPIIMP string_t signature() const;

    /// <summary>
    /// Returns the HTTP status code. For example, 200.
    /// </summary>
    _XSAPIIMP uint32_t http_status() const;

    /// <summary>
    /// Returns the a full response log formatted message of all the properties in XboxServiceCallRoutedEventArgs.
    /// </summary>
    _XSAPIIMP string_t full_response_formatted() const;

    /// <summary>
    /// Returns the  time when the request was sent.
    /// </summary>
    _XSAPIIMP const chrono_clock_t::time_point& request_time() const;

    /// <summary>
    /// Returns the  time when the response was received.
    /// </summary>
    _XSAPIIMP const chrono_clock_t::time_point& response_time() const;

    /// <summary>
    /// Returns the elapsed time span between sending a request and receiving a response.
    /// </summary>
    _XSAPIIMP const std::chrono::milliseconds& elapsed_call_time() const;

    /// <summary>
    /// Internal function
    /// </summary>
    xbox_service_call_routed_event_args(
        _In_ std::shared_ptr<xbox_service_call_routed_event_args_internal> internalObj
        );

private:
    std::shared_ptr<xbox_service_call_routed_event_args_internal> m_internalObj;
    http_call_request_message m_requestBody;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
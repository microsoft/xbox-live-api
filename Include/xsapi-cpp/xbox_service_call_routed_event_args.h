// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi-cpp/http_call_request_message.h"
#include "xsapi-c/xbox_live_context_settings_c.h"

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
    inline string_t xbox_user_id() const;

    /// <summary>
    /// Returns the HTTP method used to contact the service.  For example, "GET".
    /// </summary>
    inline string_t http_method() const;

    /// <summary>
    /// Returns the URI used to contact the service.
    /// </summary>
    inline string_t uri() const;

    /// <summary>
    /// Returns the request headers that were sent to the service.
    /// </summary>
    inline string_t request_headers() const;

    /// <summary>
    /// Returns the request body that was sent to the service.
    /// </summary>
    inline http_call_request_message request_body() const;

    /// <summary>
    /// Returns the number of responses in this session.
    /// </summary>
    inline uint32_t response_count() const;

    /// <summary>
    /// Returns the response headers returned by the service.
    /// </summary>
    inline string_t response_headers() const;

    /// <summary>
    /// Returns the response body returned by the service.
    /// </summary>
    inline string_t response_body() const;

    /// <summary>
    /// Returns the ETag returned by the service.
    /// </summary>
    inline string_t etag() const;

    /// <summary>
    /// Returns the authentication token returned by GetTokenAndSignatureAsync.
    /// </summary>
    inline string_t token() const;

    /// <summary>
    /// Returns the authentication signature returned by GetTokenAndSignatureAsync.
    /// </summary>
    inline string_t signature() const;

    /// <summary>
    /// Returns the HTTP status code. For example, 200.
    /// </summary>
    inline uint32_t http_status() const;

    /// <summary>
    /// Returns the a full response log formatted message of all the properties in xbox_service_call_routed_event_args.
    /// </summary>
    inline const string_t& full_response_formatted() const;

    /// <summary>
    /// Returns the  time when the request was sent.
    /// </summary>
    inline chrono_clock_t::time_point request_time() const;

    /// <summary>
    /// Returns the  time when the response was received.
    /// </summary>
    inline chrono_clock_t::time_point response_time() const;

    /// <summary>
    /// Returns the elapsed time span between sending a request and receiving a response.
    /// </summary>
    inline std::chrono::milliseconds elapsed_call_time() const;

    inline xbox_service_call_routed_event_args(
        _In_ const XblServiceCallRoutedArgs& internalArgs
    );

    inline ~xbox_service_call_routed_event_args();
    inline xbox_service_call_routed_event_args(const xbox_service_call_routed_event_args& other);
    inline xbox_service_call_routed_event_args& operator=(xbox_service_call_routed_event_args other);

private:
    HCCallHandle m_callHandle{ nullptr };
    long m_responseCount{ 0 };
    string_t m_fullResponseFormatted;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#include "impl/xbox_service_call_routed_event_args.hpp"
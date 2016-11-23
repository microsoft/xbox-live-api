//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "shared_macros.h"
#include "Macros_WinRT.h"
#include "xsapi/system.h"
#include "Utils_WinRT.h"
#include "HttpCallRequestMessage_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

/// <summary>
/// Contains information about a service call.
/// </summary>
public ref class XboxServiceCallRoutedEventArgs sealed
{
public:
    /// <summary>
    /// Returns the HTTP method used to contact the service.  For example, "GET".
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(HttpMethod, http_method);

    /// <summary>
    /// Returns the URI used to contact the service.
    /// </summary>
    property Windows::Foundation::Uri^ Url { Windows::Foundation::Uri^ get(); }

    /// <summary>
    /// Returns the request headers that were sent to the service.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(RequestHeaders, request_headers);

    /// <summary>
    /// Returns the request body that was sent to the service.
    /// </summary>
    property HttpCallRequestMessage^ RequestBody { HttpCallRequestMessage^ get(); }

    /// <summary>
    /// Returns the number of responses in this session.
    /// </summary>
    DEFINE_PROP_GET_OBJ(ResponseCount, response_count, uint32);

    /// <summary>
    /// Returns the response headers returned by the service.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ResponseHeaders, response_headers);

    /// <summary>
    /// Returns the response body returned by the service.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ResponseBody, response_body);

    /// <summary>
    /// Returns the ETag returned by the service.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ETag, etag);

    /// <summary>
    /// Returns the authentication token returned by GetTokenAndSignatureAsync.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Token, token);

    /// <summary>
    /// Returns the authentication signature returned by GetTokenAndSignatureAsync.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Signature, signature);

    /// <summary>
    /// Returns the HTTP status code. For example, 200.
    /// </summary>
    DEFINE_PROP_GET_OBJ(HttpStatus, http_status, uint32);

    /// <summary>
    /// Returns the a full response log formatted message of all the properties in XboxServiceCallRoutedEventArgs.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(FullResponseToString, full_response_formatted);

    /// <summary>
    /// Returns the UTC time when the request was sent through the IXHR layer.
    /// </summary>
    property Windows::Foundation::DateTime RequestTimeUTC { Windows::Foundation::DateTime get(); }

    /// <summary>
    /// Returns the UTC time when the response was received through the IXHR layer.
    /// </summary>
    property Windows::Foundation::DateTime ResponseTimeUTC { Windows::Foundation::DateTime get(); }

    /// <summary>
    /// Returns the elapsed time span between sending a request and receiving a response through the IXHR layer.
    /// </summary>
    property Windows::Foundation::TimeSpan ElapsedCallTime { Windows::Foundation::TimeSpan get(); }

    /// <summary>
    /// Returns the Xbox User Id in XboxServiceCallRoutedEventArgs.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

internal:
    XboxServiceCallRoutedEventArgs(
        _In_ xbox::services::xbox_service_call_routed_event_args cppObj
        );

private:
    xbox::services::xbox_service_call_routed_event_args m_cppObj;
    HttpCallRequestMessage^ m_requestBody;
    Windows::Foundation::Uri^ m_uri;
    Windows::Foundation::DateTime m_requestTimeUTC;
    Windows::Foundation::DateTime m_responseTimeUTC;
    Windows::Foundation::TimeSpan m_ellapsedCallTime;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_END

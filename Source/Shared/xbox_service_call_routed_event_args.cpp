// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/http_call_request_message.h"
#include "http_call_request_message_internal.h"
#include "xbox_service_call_routed_event_args_internal.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xbox_service_call_routed_event_args::xbox_service_call_routed_event_args(
    _In_ std::shared_ptr<xbox_service_call_routed_event_args_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj)),
    m_requestBody(&(m_internalObj->request_body()))
{
}

DEFINE_GET_STRING(xbox_service_call_routed_event_args, xbox_user_id);
DEFINE_GET_STRING(xbox_service_call_routed_event_args, http_method);
DEFINE_GET_STRING(xbox_service_call_routed_event_args, uri);
DEFINE_GET_STRING(xbox_service_call_routed_event_args, request_headers);
DEFINE_GET_UINT32(xbox_service_call_routed_event_args, response_count);
DEFINE_GET_STRING(xbox_service_call_routed_event_args, response_headers);
DEFINE_GET_STRING(xbox_service_call_routed_event_args, response_body);
DEFINE_GET_STRING(xbox_service_call_routed_event_args, etag);
DEFINE_GET_STRING(xbox_service_call_routed_event_args, token);
DEFINE_GET_STRING(xbox_service_call_routed_event_args, signature);
DEFINE_GET_UINT32(xbox_service_call_routed_event_args, http_status);
DEFINE_GET_STRING(xbox_service_call_routed_event_args, full_response_formatted);
DEFINE_GET_OBJECT_REF(xbox_service_call_routed_event_args, chrono_clock_t::time_point, request_time);
DEFINE_GET_OBJECT_REF(xbox_service_call_routed_event_args, chrono_clock_t::time_point, response_time);
DEFINE_GET_OBJECT_REF(xbox_service_call_routed_event_args, std::chrono::milliseconds, elapsed_call_time);

xbox_service_call_routed_event_args_internal::xbox_service_call_routed_event_args_internal(
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
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_httpMethod(std::move(httpMethod)),
    m_uri(std::move(uri)),
    m_requestHeaders(std::move(requestHeaders)),
    m_requestBody(std::move(requestBody)),
    m_responseCount(std::move(responseCount)),
    m_responseHeaders(std::move(responseHeaders)),
    m_responseBody(std::move(responseBody)),
    m_etag(std::move(etag)),
    m_token(std::move(token)),
    m_signature(std::move(signature)),
    m_httpStatus(std::move(httpStatus)),
    m_requestTime(std::move(requestTime)),
    m_responseTime(std::move(responseTime))
{
    m_elapsedCallTime = std::chrono::duration_cast<std::chrono::milliseconds>(m_responseTime - m_requestTime);
}

xbox_service_call_routed_event_args_internal::xbox_service_call_routed_event_args_internal(
    _In_ hc_call_handle_t hcCallHandle
    )
{
    // Possible TODO. For intermediate call routed calls we don't have access to many of the
    // fields. Could expose them in libHttpClient if they are needed.

    const char* url;
    HCHttpCallGetRequestUrl(hcCallHandle, &url);
    m_uri = url;

    const char* responseBody;
    HCHttpCallResponseGetResponseString(hcCallHandle, &responseBody);
    m_responseBody = responseBody;

    HCHttpCallResponseGetStatusCode(hcCallHandle, &m_httpStatus);

    uint32_t numHeaders;
    HCHttpCallResponseGetNumHeaders(hcCallHandle, &numHeaders);

    xsapi_internal_stringstream ss;
    for (uint32_t i = 0; i < numHeaders; ++i)
    {
        const char* headerName;
        const char* headerValue;
        HCHttpCallResponseGetHeaderAtIndex(hcCallHandle, i, &headerName, &headerValue);
        ss << headerName << " : " << headerValue << "\r\n";
    }
    m_responseHeaders = ss.str();
}

xsapi_internal_string xbox_service_call_routed_event_args_internal::full_response_formatted() const
{
    xsapi_internal_stringstream response;

    response << "== [XBOX SERVICE CALL] #";
    response << m_responseCount;
    response << "\r\n";

    response << "[HTTP Method]: ";
    response << m_httpMethod;

    response << "\r\n[URI]: ";
    response << m_uri;

    response << "\r\n[Request Headers]: ";
    xsapi_internal_string requestHeaders = m_requestHeaders;
    std::replace(requestHeaders.begin(), requestHeaders.end(), '\r', ';');
    std::replace(requestHeaders.begin(), requestHeaders.end(), '\n', ' ');
    response << requestHeaders;

    if (!m_token.empty())
    {
        response << "\r\n[Authorization Header]: ";
        response << m_token;
    }

    if (!m_signature.empty())
    {
        response << "\r\n[Signature Header]: ";
        response << m_signature;
    }

    auto messageType = m_requestBody.get_http_request_message_type();
    if (messageType != http_request_message_type::empty_message)
    {
        response << "\r\n[Request Body]: ";

        if (messageType == http_request_message_type::vector_message)
        {
            response << &m_requestBody.request_message_vector()[0];
        }
        else
        {
            response << m_requestBody.request_message_string();
        }
    }

    response << "\r\n[HTTP Status]: ";
    response << m_httpStatus;
#ifdef _WIN32
    HRESULT hr = utils::convert_http_status_to_hresult(m_httpStatus);
    response << " [";
    response << utils::convert_hresult_to_error_name(hr);
    response << _T("] ");
#endif

    if (!m_responseHeaders.empty())
    {
        response << "\r\n[Response Headers]: ";
        xsapi_internal_string responseHeaders = m_responseHeaders;
        std::replace(responseHeaders.begin(), responseHeaders.end(), '\r', ';');
        std::replace(responseHeaders.begin(), responseHeaders.end(), '\n', ' ');
        response << responseHeaders;
    }

    if (!m_etag.empty())
    {
        response << "\r\n[ETag]: ";
        response << m_etag;
    }

    if (!m_responseBody.empty())
    {
        response << "\r\n[Response Body]: ";
        response << m_responseBody;
    }

    response << "\r\n";

    return response.str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
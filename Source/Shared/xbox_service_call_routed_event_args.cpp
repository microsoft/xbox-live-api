//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/xbox_service_call_routed_event_args.h"
#include "utils.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
#endif

xbox_service_call_routed_event_args::xbox_service_call_routed_event_args(
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

const string_t xbox_service_call_routed_event_args::full_response_formatted() const
{
    stringstream_t response;

    response << _T("== [XBOX SERVICE CALL] #");
    response << m_responseCount;
    response << _T("\r\n");

    response << _T("[HTTP Method]: ");
    response << m_httpMethod;

    response << _T("\r\n[URI]: ");
    response << m_uri;

    response << _T("\r\n[Request Headers]: ");
    string_t requestHeaders = m_requestHeaders;
    std::replace(requestHeaders.begin(), requestHeaders.end(), _T('\r'), _T(';'));
    std::replace(requestHeaders.begin(), requestHeaders.end(), _T('\n'), _T(' '));
    response << requestHeaders;

    if (!m_token.empty())
    {
        response << _T("\r\n[Authorization Header]: ");
        response << m_token;
    }

    if (!m_signature.empty())
    {
        response << _T("\r\n[Signature Header]: ");
        response << m_signature;
    }

    auto messageType = m_requestBody.get_http_request_message_type();
    if (messageType != http_request_message_type::empty_message)
    {
        response << _T("\r\n[Request Body]: ");

        if (messageType == http_request_message_type::vector_message)
        {
            response << &m_requestBody.request_message_vector()[0];
        }
        else
        {
            response << m_requestBody.request_message_string();
        }
    }

    response << _T("\r\n[HTTP Status]: ");
    response << m_httpStatus;
#ifdef _WIN32
    HRESULT hr = utils::convert_http_status_to_hresult(m_httpStatus);
    response << _T(" [");
    response << utils::convert_hresult_to_error_name(hr);
    response << _T("] ");
#endif

    if (!m_responseHeaders.empty())
    {
        response << _T("\r\n[Response Headers]: ");
        string_t responseHeaders = m_responseHeaders;
        std::replace(responseHeaders.begin(), responseHeaders.end(), _T('\r'), _T(';'));
        std::replace(responseHeaders.begin(), responseHeaders.end(), _T('\n'), _T(' '));
        response << responseHeaders;
    }

    if (!m_etag.empty())
    {
        response << _T("\r\n[ETag]: ");
        response << m_etag;
    }

    if (!m_responseBody.empty())
    {
        response << _T("\r\n[Response Body]: ");
        response << m_responseBody;
    }

    response << _T("\r\n");

    return response.str();
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#endif
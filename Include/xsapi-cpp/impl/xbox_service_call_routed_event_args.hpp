// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xbox_service_call_routed_event_args::xbox_service_call_routed_event_args(
    _In_ const XblServiceCallRoutedArgs& internalArgs
)
{
    m_callHandle = HCHttpCallDuplicateHandle(internalArgs.call);
    m_responseCount = static_cast<long>(internalArgs.responseCount);
    m_fullResponseFormatted = Utils::StringTFromUtf8(internalArgs.fullResponseFormatted);
}

xbox_service_call_routed_event_args::xbox_service_call_routed_event_args(const xbox_service_call_routed_event_args& other)
    : m_responseCount(other.m_responseCount),
    m_fullResponseFormatted(other.m_fullResponseFormatted)
{
    m_callHandle = HCHttpCallDuplicateHandle(other.m_callHandle);
}

xbox_service_call_routed_event_args& xbox_service_call_routed_event_args::operator=(xbox_service_call_routed_event_args other)
{
    std::swap(m_callHandle, other.m_callHandle);
    m_responseCount = other.m_responseCount;
    m_fullResponseFormatted = other.m_fullResponseFormatted;
    return *this;
}

xbox_service_call_routed_event_args::~xbox_service_call_routed_event_args()
{
    HCHttpCallCloseHandle(m_callHandle);
}

string_t xbox_service_call_routed_event_args::xbox_user_id() const
{
    // TODO a lot of these fields are not exposed from the hc_call_handle. Need to expose in libHttpClient to expose them from xsapi.
    return string_t{};
}

string_t xbox_service_call_routed_event_args::http_method() const
{
    return string_t();
}

string_t xbox_service_call_routed_event_args::uri() const
{
    const char* url;
    HCHttpCallGetRequestUrl(m_callHandle, &url);
    return Utils::StringTFromUtf8(url);
}

string_t xbox_service_call_routed_event_args::request_headers() const
{
    return string_t();
}

http_call_request_message xbox_service_call_routed_event_args::request_body() const
{
    return http_call_request_message();
}

uint32_t xbox_service_call_routed_event_args::response_count() const
{
    return static_cast<uint32_t>(m_responseCount);
}

string_t xbox_service_call_routed_event_args::response_headers() const
{
    uint32_t numHeaders;
    HCHttpCallResponseGetNumHeaders(m_callHandle, &numHeaders);

    stringstream_t ss;
    for (uint32_t i = 0; i < numHeaders; ++i)
    {
        const char* headerName;
        const char* headerValue;
        HCHttpCallResponseGetHeaderAtIndex(m_callHandle, i, &headerName, &headerValue);
        ss << Utils::StringTFromUtf8(headerName) << _T(" : ") << Utils::StringTFromUtf8(headerValue) << _T("\r\n");
    }
    return ss.str();
}

string_t xbox_service_call_routed_event_args::response_body() const
{
    const char* responseBody;
    HCHttpCallResponseGetResponseString(m_callHandle, &responseBody);
    return Utils::StringTFromUtf8(responseBody);
}

string_t xbox_service_call_routed_event_args::etag() const
{
    const char* etag;
    HCHttpCallResponseGetHeader(m_callHandle, "ETag", &etag);
    return Utils::StringTFromUtf8(etag);
}

string_t xbox_service_call_routed_event_args::token() const
{
    const char* token;
    HCHttpCallResponseGetHeader(m_callHandle, "Authorization", &token);
    return Utils::StringTFromUtf8(token);
}

string_t xbox_service_call_routed_event_args::signature() const
{
    const char* signature;
    HCHttpCallResponseGetHeader(m_callHandle, "Signature", &signature);
    return Utils::StringTFromUtf8(signature);
}

uint32_t xbox_service_call_routed_event_args::http_status() const
{
    uint32_t httpStatus;
    HCHttpCallResponseGetStatusCode(m_callHandle, &httpStatus);
    return httpStatus;
}

const string_t& xbox_service_call_routed_event_args::full_response_formatted() const
{
    return m_fullResponseFormatted;
}

chrono_clock_t::time_point xbox_service_call_routed_event_args::request_time() const
{
    return chrono_clock_t::time_point();
}

chrono_clock_t::time_point xbox_service_call_routed_event_args::response_time() const
{
    return chrono_clock_t::time_point();
}

std::chrono::milliseconds xbox_service_call_routed_event_args::elapsed_call_time() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(request_time() - request_time());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
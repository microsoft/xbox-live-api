// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"
#include "../xbox_live_context_settings.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
http_call_response::http_call_response(
    _In_ XblHttpCallHandle call,
    _In_ XblHttpCallResponseBodyType type
)
{
    XblHttpCallDuplicateHandle(call, &m_handle);
    m_bodyType = static_cast<http_call_response_body_type>(type);
}

http_call_response_body_type http_call_response::body_type() const
{
    return m_bodyType;
}

string_t http_call_response::response_body_string()
{
    if (m_responseBodyString != string_t())
    {
        return m_responseBodyString;
    }

    const char* responseString;
    HRESULT hr = XblHttpCallGetResponseString(m_handle, &responseString);
    if (FAILED(hr))
    {
        return string_t();
    }

    m_responseBodyString = Utils::StringTFromUtf8(responseString);
    return m_responseBodyString;
}

web::json::value http_call_response::response_body_json()
{
    auto str = response_body_string();
    m_responseBodyJson = web::json::value::parse(str);
    return m_responseBodyJson;
}

std::vector<unsigned char> http_call_response::response_body_vector()
{
    if (!m_responseBodyVector.empty())
    {
        return m_responseBodyVector;
    }

    size_t bufferSize;
    HRESULT hr = XblHttpCallGetResponseBodyBytesSize(m_handle, &bufferSize);
    if (FAILED(hr))
    {
        return {};
    }

    uint8_t* buffer = new uint8_t[bufferSize];
    size_t bufferUsed;
    hr = XblHttpCallGetResponseBodyBytes(m_handle, bufferSize, buffer, &bufferUsed);
    if (FAILED(hr))
    {
        return {};
    }

    auto bufferInput = std::vector<uint8_t>(buffer, buffer + bufferUsed);
    m_responseBodyVector = std::vector<unsigned char>(bufferUsed);
    std::transform(bufferInput.begin(), bufferInput.end(), m_responseBodyVector.begin(),
        [](uint8_t c)
    {
        return static_cast<unsigned char>(c);
    });

    return m_responseBodyVector;
}

web::http::http_headers http_call_response::response_headers()
{
    if (!m_responseHeaders.empty())
    {
        return m_responseHeaders;
    }

    uint32_t numHeader;
    HRESULT hr = XblHttpCallGetNumHeaders(m_handle, &numHeader);
    if (FAILED(hr))
    {
        return web::http::http_headers();
    }

    const char* headerName;
    const char* headerValue;
    auto headers = web::http::http_headers();
    for (auto i = 0u; i < numHeader; i++)
    {
        hr = XblHttpCallGetHeaderAtIndex(m_handle, i, &headerName, &headerValue);
        if (SUCCEEDED(hr))
        {
            headers.add(Utils::StringTFromUtf8(headerName), Utils::StringTFromUtf8(headerValue));
        }
    }

    m_responseHeaders = headers;
    return headers;
}

uint32_t http_call_response::http_status()
{
    uint32_t statusCode;
    HRESULT hr = XblHttpCallGetStatusCode(m_handle, &statusCode);
    if (FAILED(hr))
    {
        return 0;
    }

    m_httpStatus = statusCode;
    return m_httpStatus;
}

std::error_code http_call_response::err_code()
{
    if (m_errorCode == std::error_code())
    {
        return m_errorCode;
    }

    HRESULT networkErrorCode;
    uint32_t platformNetworkErrorCode;

    HRESULT hr = XblHttpCallGetNetworkErrorCode(m_handle, &networkErrorCode, &platformNetworkErrorCode);
    if (FAILED(hr))
    {
        return std::make_error_code(Utils::ConvertHrToXblErrorCode(hr));
    }

    m_errorCode = std::make_error_code(Utils::ConvertHrToXblErrorCode(networkErrorCode));
    return m_errorCode;
}

std::string http_call_response::err_message()
{
    if (m_errMessage != std::string())
    {
        return m_errMessage;
    }

    const char* errorMessage;
    HRESULT hr = XblHttpCallGetPlatformNetworkErrorMessage(m_handle, &errorMessage);
    if (FAILED(hr))
    {
        return std::string();
    }

    m_errMessage = std::string(errorMessage);
    return m_errMessage;
}

string_t http_call_response::e_tag() const
{
    // return default value because there is no matching method
    return string_t();
}

string_t http_call_response::response_date() const
{
    // return default value because there is no matching method
    return string_t();
}

std::chrono::seconds http_call_response::retry_after() const
{
    // return default value because there is no matching method
    return std::chrono::seconds();
}

http_call_response::http_call_response(const http_call_response& other)
    : m_bodyType(other.m_bodyType),
    m_responseBodyString(other.m_responseBodyString),
    m_responseBodyJson(other.m_responseBodyJson),
    m_responseBodyVector(other.m_responseBodyVector),
    m_responseHeaders(other.m_responseHeaders),
    m_httpStatus(other.m_httpStatus),
    m_errorCode(other.m_errorCode),
    m_errMessage(other.m_errMessage)
{
    XblHttpCallDuplicateHandle(other.m_handle, &m_handle);
}

http_call_response& http_call_response::operator=(http_call_response other)
{
    std::swap(m_handle, other.m_handle);
    m_responseBodyString = other.m_responseBodyString;
    m_responseBodyJson = other.m_responseBodyJson;
    m_responseBodyVector = other.m_responseBodyVector;
    m_httpStatus = other.m_httpStatus;
    m_responseHeaders = other.m_responseHeaders;
    m_errorCode = other.m_errorCode;
    m_errMessage = other.m_errMessage;

    return *this;
}

http_call_response::~http_call_response()
{
    XblHttpCallCloseHandle(m_handle);
}

http_call::http_call(
    _In_ XblHttpCallHandle callHandle,
    _In_ string_t httpMethod,
    _In_ string_t serverName,
    _In_ web::uri pathQueryFragment
) :
    m_callHandle(std::move(callHandle)),
    m_httpMethod(std::move(httpMethod)),
    m_serverName(std::move(serverName)),
    m_pathQueryFragment(std::move(pathQueryFragment))
{
}

#if !XSAPI_NO_PPL
pplx::task<std::shared_ptr<http_call_response>> http_call::get_response_with_auth(
    _In_ http_call_response_body_type httpCallResponseBodyType
)
{
    XblHttpCallHandle xblHttpCall = m_callHandle;

    auto asyncWrapper = new AsyncWrapper<std::shared_ptr<http_call_response>>(
        [xblHttpCall, httpCallResponseBodyType](
            _In_ XAsyncBlock* async, 
            _In_ std::shared_ptr<http_call_response>& result)
    {
        HRESULT hr = XAsyncGetStatus(async, false);
        if (SUCCEEDED(hr))
        {
            result = std::make_shared<http_call_response>(xblHttpCall, static_cast<XblHttpCallResponseBodyType>(httpCallResponseBodyType));
        }
        return hr;
    });

    auto hr = XblHttpCallPerformAsync(
        m_callHandle,
        static_cast<XblHttpCallResponseBodyType>(httpCallResponseBodyType),
        &asyncWrapper->async
    );

    pplx::task_completion_event<std::shared_ptr<http_call_response>> taskCompletionEvent;

    auto response = asyncWrapper->Task(hr).then([taskCompletionEvent](xbox_live_result<std::shared_ptr<http_call_response>> result) {
        taskCompletionEvent.set(result.payload());
    });
    return pplx::task<std::shared_ptr<http_call_response>>(taskCompletionEvent);
}

pplx::task<std::shared_ptr<http_call_response>> http_call::get_response_with_auth(
    _In_ XalUserHandle user,
    _In_ http_call_response_body_type httpCallResponseBodyType,
    _In_ bool allUsersAuthRequired
)
{
    UNREFERENCED_PARAMETER(user);
    UNREFERENCED_PARAMETER(allUsersAuthRequired);

    auto xblHttpCall = m_callHandle;

    auto asyncWrapper = new AsyncWrapper<std::shared_ptr<http_call_response>>(
        [xblHttpCall, httpCallResponseBodyType](XAsyncBlock* async, std::shared_ptr<http_call_response>& response)
    {
        HRESULT hr = XAsyncGetStatus(async, false);
        if (SUCCEEDED(hr))
        {
            response = std::make_shared<http_call_response>(xblHttpCall, static_cast<XblHttpCallResponseBodyType>(httpCallResponseBodyType));
        }
        return hr;
    });

    auto hr = XblHttpCallPerformAsync(
        m_callHandle,
        static_cast<XblHttpCallResponseBodyType>(httpCallResponseBodyType),
        &asyncWrapper->async
    );

    pplx::task_completion_event<std::shared_ptr<http_call_response>> taskCompletionEvent;

    auto response = asyncWrapper->Task(hr).then([taskCompletionEvent](xbox_live_result<std::shared_ptr<http_call_response>> result) {
        taskCompletionEvent.set(result.payload());
    });
    return pplx::task<std::shared_ptr<http_call_response>>(taskCompletionEvent);
}

pplx::task<std::shared_ptr<http_call_response>> http_call::get_response(
    _In_ http_call_response_body_type httpCallResponseBodyType
)
{
    // no matching function
    UNREFERENCED_PARAMETER(httpCallResponseBodyType);
    return pplx::task<std::shared_ptr<http_call_response>>();
}
#endif // !XSAPI_NO_PPL

void http_call::set_request_body(
    _In_ const string_t& value
)
{
    auto convertedValue = Utils::StringFromStringT(value);
    XblHttpCallRequestSetRequestBodyString(m_callHandle, convertedValue.c_str());
}

void http_call::set_request_body( _In_ const web::json::value& value )
{
    auto convertedValue = Utils::StringFromStringT(value.serialize());
    XblHttpCallRequestSetRequestBodyString(m_callHandle, convertedValue.c_str());
}

void http_call::set_request_body(
    _In_ const std::vector<uint8_t>& value
)
{
    uint8_t* buffer{ nullptr };
    auto neededSize = value.size();
    std::copy(value.begin(), value.end(), buffer);
    XblHttpCallRequestSetRequestBodyBytes(m_callHandle, buffer, static_cast<uint32_t>(neededSize));
}


void http_call::set_custom_header(
    _In_ const string_t& Name,
    _In_ const string_t& Value
)
{
    const auto headerName = Utils::StringFromStringT(Name);
    const auto headerValue = Utils::StringFromStringT(Value);

    XblHttpCallRequestSetHeader(
        m_callHandle, 
        headerName.c_str(),
        headerValue.c_str(),
        false
    );
}

void http_call::set_long_http_call(
    _In_ bool value
)
{
    XblHttpCallRequestSetLongHttpCall(m_callHandle, value);
    m_longHttpCall = value;
}

bool http_call::long_http_call() const
{
    return m_longHttpCall;
}

void http_call::set_retry_allowed(
    _In_ bool value
)
{
    XblHttpCallRequestSetLongHttpCall(m_callHandle, value);
    m_retryAllowed = value;
}

bool http_call::retry_allowed() const
{
    return m_retryAllowed;
}

void http_call::set_content_type_header_value(
    _In_ const string_t& value
)
{
    XblHttpCallRequestSetHeader(m_callHandle, "Content-Type", Utils::StringFromStringT(value).c_str(), true);
}

string_t http_call::content_type_header_value() const
{
    //no matching method
    return string_t();
}

void http_call::set_xbox_contract_version_header_value(
    _In_ const string_t& value
)
{
    XblHttpCallRequestSetHeader(m_callHandle, "x-xbl-contract-version", Utils::StringFromStringT(value).c_str(), true);
}

string_t http_call::xbox_contract_version_header_value() const
{
    //no matching method
    return string_t();
}

string_t http_call::server_name() const
{
    return m_serverName;
}

const web::uri& http_call::path_query_fragment() const
{
    return m_pathQueryFragment;
}

string_t http_call::http_method() const
{
    return m_httpMethod;
}

void http_call::set_add_default_headers(
    _In_ bool value
)
{
    UNREFERENCED_PARAMETER(value);
    //no matching method
}

bool http_call::add_default_headers() const
{
    //no matching method
    return false;
}

http_call::~http_call()
{
    XblHttpCallCloseHandle(m_callHandle);
}

std::shared_ptr<http_call> create_xbox_live_http_call(
    _In_ const std::shared_ptr<xbox_live_context_settings>& xboxLiveContextSettings,
    _In_ const string_t& httpMethod,
    _In_ const string_t& serverName,
    _In_ const web::uri& pathQueryFragment
)
{
    // access context handle via friend function
    auto xblContext = xboxLiveContextSettings->m_xblContextHandle;

    auto httpMethod_c = Utils::StringFromStringT(httpMethod);

    auto fullUrl = serverName;
    if (!pathQueryFragment.is_empty())
    {
        fullUrl += pathQueryFragment.to_string();
    }
    auto fullUrl_c = Utils::StringFromStringT(fullUrl);

    XblHttpCallHandle callHandle;

    HRESULT hr = XblHttpCallCreate(xblContext, httpMethod_c.c_str(), fullUrl_c.c_str(), &callHandle);
    if (FAILED(hr))
    {
        return nullptr;
    }

    return std::make_shared<http_call>(callHandle, httpMethod, serverName, pathQueryFragment);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
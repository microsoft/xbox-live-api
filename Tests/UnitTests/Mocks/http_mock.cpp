// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "http_mock.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

struct HttpMockContext
{
    std::weak_ptr<HttpMock> pWeakThis;
};

std::vector< std::shared_ptr<HttpMockContext> > g_httpMockContextList;

HttpMock::HttpMock(
    _In_ const xsapi_internal_string& method,
    _In_ const xsapi_internal_string& url,
    _In_ uint32_t httpStatus,
    _In_ const JsonValue& responseBodyJson,
    _In_ const HttpHeaders& responseHeaders
) noexcept
{
    auto hr = HCMockCallCreate(&m_handle);
    assert(SUCCEEDED(hr));

    hr = HCMockAddMock(m_handle, method.c_str(), url.c_str(), nullptr, 0);
    assert(SUCCEEDED(hr));

    SetResponseHttpStatus(httpStatus);
    SetResponseBody(responseBodyJson);
    SetResponseHeaders(responseHeaders);

    UNREFERENCED_PARAMETER(hr);
}

HttpMock::HttpMock(HttpMock&& other)
    : m_handle(std::move(other.m_handle)),
    m_matchedCallback(std::move(other.m_matchedCallback))
{
    other.m_handle = nullptr;
    other.m_matchedCallback = nullptr;
}

HttpMock & HttpMock::operator=(HttpMock&& other)
{
    m_handle = std::move(other.m_handle);
    m_matchedCallback = std::move(other.m_matchedCallback);

    other.m_handle = nullptr;
    other.m_matchedCallback = nullptr;
    return *this;
}

HttpMock::~HttpMock()
{
    HCMockRemoveMock(m_handle);
    HCMockCallCloseHandle(m_handle);
}

void HttpMock::SetResponseHttpStatus(uint32_t httpStatus) const noexcept
{
    auto hr = HCMockResponseSetStatusCode(m_handle, httpStatus);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);
}

void HttpMock::SetResponseBody(const String& responseBodyString) const noexcept
{
    if (!responseBodyString.empty())
    {
        Vector<uint8_t> bodyBytes{ responseBodyString.begin(), responseBodyString.end() };
        SetResponseBody(bodyBytes.data(), bodyBytes.size());
    }
}

void HttpMock::SetResponseBody(const JsonValue& responseBodyJson) const noexcept
{
    if (!responseBodyJson.IsNull())
    {
        SetResponseBody(JsonUtils::SerializeJson(responseBodyJson));
    }
}

void HttpMock::SetResponseBody(
    const uint8_t* responseBodyBytes,
    size_t responseBodySize
) const noexcept
{
    if (responseBodyBytes && responseBodySize)
    {
        auto hr = HCMockResponseSetResponseBodyBytes(m_handle, responseBodyBytes, static_cast<uint32_t>(responseBodySize));
        assert(SUCCEEDED(hr));
        UNREFERENCED_PARAMETER(hr);
    }
}

void HttpMock::ClearReponseBody() const noexcept
{
    // libHttpClient doesn't allow setting body with nullptr, but it does allow size = 0
    uint8_t body{};
    SetResponseBody(&body, 0);
}

void HttpMock::SetResponseHeaders(const HttpHeaders& responseHeaders) const noexcept
{
    for (const auto& header : responseHeaders)
    {
        auto hr = HCMockResponseSetHeader(m_handle, header.first.data(), header.second.data());
        assert(SUCCEEDED(hr));
        UNREFERENCED_PARAMETER(hr);
    }
}

void HttpMock::SetMockMatchedCallback(MockMatchedCallback mockMatched) noexcept
{
    m_matchedCallback = std::move(mockMatched);

    // m_sharedFromThis will be destroyed when HttpMock is destroyed.  
    // pass a weak version of it via context and check if its still around inside callback
    // g_httpMockContextList will be cleaned up during shutdown
    auto contextBlock = std::make_shared<HttpMockContext>();
    g_httpMockContextList.push_back(contextBlock);
    contextBlock->pWeakThis = shared_from_this();

    HCMockSetMockMatchedCallback(m_handle,
        [](HCMockCallHandle matchedMock,
            const char* method,
            const char* url,
            const uint8_t* requestBodyBytes,
            uint32_t requestBodySize,
            void* context
            )
        {
            UNREFERENCED_PARAMETER(matchedMock);
            UNREFERENCED_PARAMETER(method);

            HttpMockContext* contextBlockPtr{ static_cast<HttpMockContext*>(context) };
            std::shared_ptr<HttpMock> sharedFromThis{ contextBlockPtr->pWeakThis.lock() };
            if (sharedFromThis != nullptr)
            {
                sharedFromThis->m_matchedCallback(sharedFromThis.get(), url, xsapi_internal_string{ requestBodyBytes, requestBodyBytes + requestBodySize });
            }
        },
        contextBlock.get()
    );
}

xsapi_internal_string HttpMock::GetUriPath(const xsapi_internal_string& uriString) noexcept
{
    xbox::services::uri uri{ uriString.data() };
    return uri.path();
}

xsapi_internal_string HttpMock::GetUriQuery(const xsapi_internal_string& uriString) noexcept
{
    xbox::services::uri uri{ uriString.data() };
    return uri.query();
}

std::map<xsapi_internal_string, xsapi_internal_string> HttpMock::GetQueryParams(const xsapi_internal_string& uriString) noexcept
{
    xbox::services::uri uri{ uriString.data() };
    auto params = xbox::services::uri::split_query(uri.query());

    std::map<xsapi_internal_string, xsapi_internal_string> out;
    for (auto& param : params)
    {
        out[param.first.c_str()] = param.second.c_str();
    }
    return out;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END




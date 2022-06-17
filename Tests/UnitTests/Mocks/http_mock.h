// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "pch.h"
#include "httpClient/httpClient.h"
#include "xsapi-cpp/http_call.h"
#include "shared_macros.h"
#include "xbox_live_context_settings_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

typedef Callback<class HttpMock* /*matchedMock*/, xsapi_internal_string /*actualRequestUrl*/, xsapi_internal_string /*requestBody*/> MockMatchedCallback;

// RAII wrapper around HCMockCallHandle
class HttpMock : public std::enable_shared_from_this<HttpMock>
{
public:
    HttpMock(
        _In_ const xsapi_internal_string& method,
        _In_ const xsapi_internal_string& url,
        _In_ uint32_t httpStatus = 200,
        _In_ const JsonValue& responseBodyJson = JsonValue{ rapidjson::kNullType },
        _In_ const HttpHeaders& responseHeaders = HttpHeaders{}
    ) noexcept;

    HttpMock(const HttpMock&) = delete;
    HttpMock& operator=(HttpMock) = delete;
    HttpMock(HttpMock&& other);
    HttpMock& operator=(HttpMock&& other);
    virtual ~HttpMock();

    void SetResponseHttpStatus(uint32_t httpStatus) const noexcept;
    void SetResponseBody(const String& responseBodyString) const noexcept;
    void SetResponseBody(const JsonValue& responseBodyJson) const noexcept;
    void SetResponseBody(const uint8_t* responseBodyBytes, size_t responseBodySize) const noexcept;
    void ClearReponseBody() const noexcept;
    void SetResponseHeaders(const HttpHeaders& responseHeaders) const noexcept;

    void SetMockMatchedCallback(MockMatchedCallback callback) noexcept;

    // Helper methods for parsing URI
    static xsapi_internal_string GetUriPath(const xsapi_internal_string& uri) noexcept;
    static xsapi_internal_string GetUriQuery(const xsapi_internal_string& uri) noexcept;
    static std::map<xsapi_internal_string, xsapi_internal_string> GetQueryParams(const xsapi_internal_string& uriString) noexcept;

private:
    HCMockCallHandle m_handle{ nullptr };
    MockMatchedCallback m_matchedCallback{ nullptr };
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


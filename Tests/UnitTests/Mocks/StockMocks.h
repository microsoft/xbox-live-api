// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class StockMocks
{
public:
    static web::json::value GetStockJsonMockXTokenResult();
    static web::json::value GetStockJsonMockUserTokenResult();

    static web::json::value GetStockNsal();

    static web::json::value GetCreateAccountXErrorResponse();

    // TODO remove non-internal versions
    static std::shared_ptr<http_call_response> CreateMockHttpCallResponse(
        _In_ web::json::value responseBodyJson,
        _In_ web::http::http_response httpResponse
        );

    static std::shared_ptr<http_call_response> CreateMockHttpCallResponse(
        _In_ web::json::value responseBodyJson,
        _In_ int statusCode = 200,
        _In_ web::http::http_response httpResponse = web::http::http_response(),
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings = nullptr
        );

    static std::shared_ptr<http_call_response> CreateMockHttpCallResponse(
        _In_ string_t responseString,
        _In_ int statusCode = 200,
        _In_ web::http::http_response httpResponse = web::http::http_response()
        );

    static std::shared_ptr<http_call_response> CreateMockHttpCallResponse(
        _In_ std::vector<unsigned char> responseVector,
        _In_ int statusCode = 200,
        _In_ web::http::http_response httpResponse = web::http::http_response()
        );

    static std::shared_ptr<http_call_response_internal> CreateMockHttpCallResponseInternal(
        _In_ web::json::value responseBodyJson,
        _In_ const http_headers& responseHeaders
        );

    static std::shared_ptr<http_call_response_internal> CreateMockHttpCallResponseInternal(
        _In_ web::json::value responseBodyJson,
        _In_ int statusCode = 200,
        _In_ const http_headers& responseHeaders = http_headers(),
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings = nullptr
        );

    static std::shared_ptr<http_call_response_internal> CreateMockHttpCallResponseInternal(
        _In_ string_t responseString,
        _In_ int statusCode = 200,
        _In_ const http_headers& responseHeaders = http_headers()
        );

    static std::shared_ptr<http_call_response_internal> CreateMockHttpCallResponseInternal(
        _In_ std::vector<unsigned char> responseVector,
        _In_ int statusCode = 200,
        _In_ const http_headers& responseHeaders = http_headers()
        );

    // TODO extend
    static void AddHttpMockResponse(
        _In_ const string_t& responseBody,
        _In_ uint32_t statusCode = 200,
        _In_ const http_headers& responseHeaders = http_headers()
        );

private:
    static void InitializeResponse(_In_ int statusCode, _In_ web::http::http_response& response);

    static http_headers HttpHeadersFromResponse(_In_ const web::http::http_response& response);
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


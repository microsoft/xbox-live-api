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
#include "pch.h"
#include "http_call_response.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class StockMocks
{
public:
    static web::json::value GetStockJsonMockXTokenResult();
    static web::json::value GetStockJsonMockUserTokenResult();

    static web::json::value GetStockNsal();

    static web::json::value GetCreateAccountXErrorResponse();

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

private:
    static void InitializeResponse(_In_ int statusCode, _In_ web::http::http_response& response);
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


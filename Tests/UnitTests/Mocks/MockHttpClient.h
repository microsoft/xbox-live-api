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
#include "http_client.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockHttpClient : public xbox_http_client
{
public:
    virtual pplx::task<web::http::http_response> get_request(
        _In_ web::http::http_request request,
        _In_ pplx::cancellation_token token = pplx::cancellation_token::none()
        ) override;

    MockHttpClient();
    void reinit();

    HRESULT ResultHR;
    web::http::http_response ResultValue;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


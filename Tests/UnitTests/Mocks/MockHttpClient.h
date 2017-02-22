// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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


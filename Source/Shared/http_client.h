// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "http_call_response.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xbox_http_client
{
public:
    virtual pplx::task<web::http::http_response> get_request(
        _In_ web::http::http_request request,
        _In_ pplx::cancellation_token token = pplx::cancellation_token::none()
        ) = 0;
};

class xbox_http_client_impl : public xbox_http_client
{
public:
    xbox_http_client_impl(
        _In_ web::http::uri base_uri,
        _In_ web::http::client::http_client_config client_config
        );

    virtual pplx::task<web::http::http_response> get_request(
        _In_ web::http::http_request request,
        _In_ pplx::cancellation_token token = pplx::cancellation_token::none()
        );

private:
    std::shared_ptr<web::http::client::http_client> m_client;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

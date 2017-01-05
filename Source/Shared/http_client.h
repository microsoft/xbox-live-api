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
#include "http_call_response.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
#endif

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

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#endif

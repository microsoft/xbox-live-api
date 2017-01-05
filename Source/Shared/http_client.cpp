//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "http_client.h"
#include "utils.h"

using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
#endif
 
xbox_http_client_impl::xbox_http_client_impl(
    _In_ web::http::uri base_uri,
    _In_ web::http::client::http_client_config client_config
    )
{
    m_client = std::make_shared<http_client>(std::move(base_uri), std::move(client_config));
}

pplx::task<web::http::http_response>
xbox_http_client_impl::get_request(
    _In_ web::http::http_request request,
    _In_ pplx::cancellation_token token
    )
{
    return m_client->request(std::move(request), std::move(token));
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#endif

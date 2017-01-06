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
#include "xsapi/system.h"
#include "xbox_live_server_impl.h"

using namespace XBOX_LIVE_NAMESPACE;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN


xbox_live_server::xbox_live_server()
{
    m_server_impl = std::make_shared<xbox_live_server_impl>();
}

pplx::task<xbox_live_result<void>>
xbox_live_server::signin(_In_ cert_context cert)
{
    return m_server_impl->signin(std::move(cert), false);
}

pplx::task<xbox_live_result<token_and_signature_result>>
xbox_live_server::get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers
    )
{
    return m_server_impl->get_token_and_signature(httpMethod, url, headers);
}

pplx::task<xbox_live_result<token_and_signature_result>>
xbox_live_server::get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const string_t& requestBodyString
    )
{
    return m_server_impl->get_token_and_signature(httpMethod, url, headers, requestBodyString);
}

pplx::task<xbox_live_result<token_and_signature_result>>
xbox_live_server::get_token_and_signature_array(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& requestBodyArray
    )
{
    return m_server_impl->get_token_and_signature_array(httpMethod, url, headers, requestBodyArray);
}

bool
xbox_live_server::is_signed_in() const
{
    return m_server_impl->is_signed_in();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

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
#include "xsapi/system.h"
#include "xbox_system_factory.h"
#include "shared_macros.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif


class xbox_live_server_impl : public std::enable_shared_from_this<xbox_live_server_impl>
{
public:
    xbox_live_server_impl();

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<void>> signin(
        _In_ cert_context cert,
        _In_ bool forceRefresh
        );

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result>>
    get_token_and_signature(
        _In_ string_t httpMethod,
        _In_ string_t url,
        _In_ string_t headers
        );

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result>>
    get_token_and_signature(
        _In_ string_t httpMethod,
        _In_ string_t url,
        _In_ string_t headers,
        _In_ string_t requestBodyString
        );

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result>>
    get_token_and_signature_array(
        _In_ string_t httpMethod,
        _In_ string_t url,
        _In_ string_t headers,
        _In_ const std::vector<unsigned char>& requestBodyArray
        );

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result>>
    internal_get_token_and_signature(
        _In_ string_t httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& endpointForNsal,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh
        );

    bool is_signed_in() const;

private:
    bool m_isSignedIn;
    std::shared_ptr<auth_config> m_authConfig;
    std::shared_ptr<auth_manager> m_authManager;
};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

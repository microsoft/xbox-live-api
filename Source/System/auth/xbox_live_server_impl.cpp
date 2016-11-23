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
#include <wincrypt.h>
#include "xbox_live_server_impl.h"
#include "request_signer.h"
#include <WinHttp.h>

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

xbox_live_server_impl::xbox_live_server_impl()
    :m_isSignedIn(false)
{
    auto localConfig = xbox_system_factory::get_factory()->create_local_config();

    m_authConfig = std::make_shared<auth_config>(
        localConfig->sandbox(),
        localConfig->environment_prefix(),
        localConfig->environment(),
        localConfig->use_first_party_token()
        );
    m_authConfig->set_xtoken_composition({ token_identity_type::s_token});

    m_authManager = std::make_shared<auth_manager>(m_authConfig);
}

pplx::task<xbox_live_result<void>>
xbox_live_server_impl::signin(
    _In_ cert_context cert,
    _In_ bool forceRefresh
    )
{
    xbox_live_context_settings::_Set_SSL_cert(std::move(cert));
    std::weak_ptr<xbox_live_server_impl> thisWeakPtr = shared_from_this();

    return m_authManager->initialize_default_nsal()
    .then([thisWeakPtr, forceRefresh](xbox_live_result<void> result)
    {
        auto pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            return pplx::task_from_result(xbox_live_result<token_and_signature_result>(xbox_live_error_code::runtime_error, "xbox_live_server_impl shutting down"));
        }

        if (result.err())
        {
            return pplx::task_from_result(xbox_live_result<token_and_signature_result>(result.err(), result.err_message()));
        }

        return pThis->m_authManager->internal_get_token_and_signature(
            _T("GET"),
            pThis->m_authConfig->xbox_live_endpoint(),
            pThis->m_authConfig->xbox_live_endpoint(),
            string_t(),
            std::vector<uint8_t>(),
            false,
            forceRefresh
            );
    })
    .then([thisWeakPtr](xbox_live_result<token_and_signature_result> result)
    {
        auto pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            return xbox_live_result<void>(xbox_live_error_code::runtime_error, "xbox_live_server_impl shutting down");
        }

        if (result.err())
        {
            return xbox_live_result<void>(result.err(), result.err_message());
        }
        pThis->m_isSignedIn = true;
        return xbox_live_result<void>();

    });
}

pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
xbox_live_server_impl::get_token_and_signature(
    _In_ string_t httpMethod,
    _In_ string_t url,
    _In_ string_t headers
    )
{
    return internal_get_token_and_signature(
        std::move(httpMethod),
        url,
        url,
        std::move(headers),
        std::vector<unsigned char>(),
        false,
        false
        );
}

pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
xbox_live_server_impl::get_token_and_signature(
    _In_ string_t httpMethod,
    _In_ string_t url,
    _In_ string_t headers,
    _In_ string_t requestBodyString
    )
{
    std::string utf8Body(utility::conversions::to_utf8string(requestBodyString));
    std::vector<unsigned char> utf8Vec(utf8Body.begin(), utf8Body.end());
    return internal_get_token_and_signature(
        std::move(httpMethod),
        url,
        url,
        std::move(headers),
        utf8Vec,
        false,
        false
        );
}

pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
xbox_live_server_impl::get_token_and_signature_array(
    _In_ string_t httpMethod,
    _In_ string_t url,
    _In_ string_t headers,
    _In_opt_ const std::vector<unsigned char>& requestBodyArray
    )
{
    return internal_get_token_and_signature(
        std::move(httpMethod),
        url,
        url,
        std::move(headers),
        requestBodyArray,
        false,
        false
        );
}

pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
xbox_live_server_impl::internal_get_token_and_signature(
    _In_ string_t httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& endpointForNsal,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh
    )
{
    return m_authManager->internal_get_token_and_signature(
        httpMethod,
        url,
        endpointForNsal,
        headers,
        bytes,
        promptForCredentialsIfNeeded,
        forceRefresh
        );
}

bool
xbox_live_server_impl::is_signed_in() const
{
    return m_isSignedIn;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"

#include "user_impl.h"

using namespace pplx;
using xbox::services;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

pplx::task<xbox_live_result<sign_in_result>>
user_impl_server::sign_in_impl(_In_ bool showUI, _In_ bool forceRefresh)
{
    throw std::exception("not supported.");
}

user_impl_server::user_impl_server(void *) :
    user_impl()
{
    m_authConfig->set_xtoken_composition({ token_identity_type::s_token, token_identity_type::u_token });
    m_authManager = std::make_shared<auth_manager>(m_authConfig);
}

pplx::task<xbox_live_result<token_and_signature_result> >
user_impl_server::internal_get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& endpointForNsal,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh
    )
{
    UNREFERENCED_PARAMETER(endpointForNsal);

    auto task = m_authManager->internal_get_token_and_signature(
        _T("GET"),
        m_authConfig->xbox_live_endpoint(),
        m_authConfig->xbox_live_endpoint(),
        string_t(),
        std::vector<uint8_t>(),
        false,
        forceRefresh
        );

    return utils::create_exception_free_task<token_and_signature_result>(
        task
        );
}

// Not supported for user_impl_server
pplx::task<xbox_live_result<void>>
user_impl_server::sign_in_impl(
    _In_ const string_t& userDelegationTicket,
    _In_ bool forceRefresh
    )
{
    m_authManager->auth_token_manager()->set_rps_ticket(userDelegationTicket);
    std::weak_ptr<user_impl_server> thisWeakPtr = std::dynamic_pointer_cast<user_impl_server>(shared_from_this());
    return m_authManager->initialize_default_nsal()
    .then([thisWeakPtr, forceRefresh](xbox_live_result<void> result)
    {
        auto pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            return pplx::task_from_result(xbox_live_result<token_and_signature_result>(xbox_live_error_code::runtime_error, "user_impl_server shutting down"));
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

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

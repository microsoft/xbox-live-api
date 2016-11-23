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
#include "MockUser.h"
#include "StockMocks.h"
#include "user_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

MockUser::MockUser() :
    ResultHR(S_OK),
    user_impl()
{
}

pplx::task<xbox_live_result<sign_in_result>>
MockUser::sign_in_impl(
    _In_ bool showUI,
    _In_ bool forceRefresh
    )
{
    user_signed_in(
        L"TestXboxUserId",
        L"TestGamerTag",
        L"Adult",
        L"191 192",
        L""
        );
    return pplx::task_from_result(xbox_live_result<sign_in_result>());
}


pplx::task<xbox_live_result<void>> 
MockUser::sign_in_impl(
    _In_ const string_t& userDelegationTicket,
    _In_ bool forceRefresh
    )
{
    user_signed_in(
        L"TestXboxUserId",
        L"TestGamerTag",
        L"Adult",
        L"191 192",
        L""
        );
    return pplx::task_from_result(xbox_live_result<void>());
}

pplx::task<xbox_live_result<token_and_signature_result> >
MockUser::internal_get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& endpointForNsal,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh
    )
{
    HRESULT hr = ResultHR;
    if (FAILED(hr))
    {
        throw hr;
    }

    auto result = token_and_signature_result(
        L"TestToken",
        L"",
        m_xboxUserId,
        m_gamertag,
        L"TestXboxUserHash",
        m_ageGroup,
        m_privileges,
        L"",
        L""
        );

    return pplx::task_from_result(xbox_live_result<token_and_signature_result>(result));

}

std::shared_ptr<xbox::services::system::auth_config> MockUser::auth_config()
{
    return std::make_shared<xbox::services::system::auth_config>(_T("MockSandbox"), _T("MockPrefix-"), _T("MockEnv"), false);
}

std::shared_ptr<xbox::services::xbox_live_context_settings> MockUser::xbox_live_context_settings()
{
    return std::make_shared<xbox::services::xbox_live_context_settings>();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

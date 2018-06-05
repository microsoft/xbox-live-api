// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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

void MockUser::sign_in_impl(
    _In_ bool showUI,
    _In_ bool forceRefresh,
    _In_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<sign_in_result>> callback
    )
{
    UNREFERENCED_PARAMETER(queue);
    UNREFERENCED_PARAMETER(forceRefresh);
    UNREFERENCED_PARAMETER(showUI);
    user_signed_in(
        "TestXboxUserId",
        "TestGamerTag",
        "Adult",
        "191 192",
        ""
        );
    callback(xbox_live_result<sign_in_result>());
}

void MockUser::internal_get_token_and_signature(
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& url,
    _In_ const xsapi_internal_string& endpointForNsal,
    _In_ const xsapi_internal_string& headers,
    _In_ const xsapi_internal_vector<unsigned char>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh,
    _In_ async_queue_handle_t queue,
    _In_ token_and_signature_callback callback
    )
{
    UNREFERENCED_PARAMETER(bytes);
    UNREFERENCED_PARAMETER(headers);
    UNREFERENCED_PARAMETER(endpointForNsal);
    UNREFERENCED_PARAMETER(url);
    UNREFERENCED_PARAMETER(httpMethod);
    UNREFERENCED_PARAMETER(queue);
    UNREFERENCED_PARAMETER(forceRefresh);
    UNREFERENCED_PARAMETER(promptForCredentialsIfNeeded);
    HRESULT hr = ResultHR;
    if (FAILED(hr))
    {
        throw hr;
    }

    callback(xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>(
        xsapi_allocate_shared<token_and_signature_result_internal>(
            "TestToken",
            "",
            m_xboxUserId,
            m_gamertag,
            "TestXboxUserHash",
            m_ageGroup,
            m_privileges,
            "",
            ""
        )));
}


std::shared_ptr<xbox::services::system::auth_config> MockUser::auth_config()
{
    return std::make_shared<xbox::services::system::auth_config>("MockSandbox", "MockPrefix-", "MockEnv", false, false, "MockScope");
}

std::shared_ptr<xbox::services::xbox_live_context_settings> MockUser::xbox_live_context_settings()
{
    return std::make_shared<xbox::services::xbox_live_context_settings>();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

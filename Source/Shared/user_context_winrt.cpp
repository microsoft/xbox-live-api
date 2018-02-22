// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_system_factory.h"
#include "user_context.h"
#include "shared_macros.h"

#if defined __cplusplus_winrt
using namespace Platform;
using namespace Windows::Foundation::Collections;
#if !XSAPI_CPP
using namespace Microsoft::Xbox::Services::System;
#endif
#endif

using namespace xbox::services::system;

#if !TV_API && !XSAPI_CPP // Non-XDK WinRT

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// XDK's Windows.* user object

user_context::user_context(_In_ XboxLiveUser^ user) :
    m_user(user),
    m_callerContextType(xbox::services::caller_context_type::title)
{
    XSAPI_ASSERT(m_user != nullptr);
    m_xboxUserId = m_user->XboxUserId->Data();
}

user_context::user_context(_In_ std::shared_ptr<xbox_live_user> user) :
    m_user(user_context::user_convert(user)),
    m_callerContextType(xbox::services::caller_context_type::title)
{
    XSAPI_ASSERT(m_user != nullptr);
    m_xboxUserId = m_user->XboxUserId->Data();
}

XboxLiveUser^ user_context::user() const
{
    return m_user;
}

pplx::task<xbox_live_result<user_context_auth_result>> user_context::get_auth_result(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const string_t& requestBodyString,
    _In_ bool allUsersAuthRequired
    )
{
    UNREFERENCED_PARAMETER(allUsersAuthRequired);
    auto asyncTask = pplx::create_task([this, httpMethod, url, headers, requestBodyString]()
    {
        return m_user->GetTokenAndSignatureAsync(
            ref new String(httpMethod.c_str()),
            ref new String(url.c_str()),
            ref new String(headers.c_str()),
            ref new String(requestBodyString.c_str())
        );
    });

    return asyncTask
    .then([](pplx::task<GetTokenAndSignatureResult^> t)
    {
        try
        {
            GetTokenAndSignatureResult^ result = t.get();
            user_context_auth_result userContextResult(result->Token->Data(), result->Signature->Data());
            return xbox_live_result<user_context_auth_result>(userContextResult);
        }
        catch (Exception^ ex)
        {
            xbox_live_error_code err = xbox::services::utils::convert_exception_to_xbox_live_error_code();
            return xbox_live_result<user_context_auth_result>(err, "Failed getting auth token");
        }
    }, pplx::task_continuation_context::use_arbitrary());
}

pplx::task<xbox_live_result<user_context_auth_result>> user_context::get_auth_result(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& requestBody,
    _In_ bool allUsersAuthRequired
    )
{
    UNREFERENCED_PARAMETER(allUsersAuthRequired);
    Array<byte>^ byteArray = ref new Array<byte>(static_cast<uint32_t>(requestBody.size()));
    memcpy(&byteArray->Data[0], &requestBody[0], requestBody.size());

    auto asyncTask = pplx::create_task([this, httpMethod, url, headers, byteArray]()
    {
        return m_user->GetTokenAndSignatureArrayAsync(
            ref new String(httpMethod.c_str()),
            ref new String(url.c_str()),
            ref new String(headers.c_str()),
            byteArray
        );
    });

    return asyncTask
    .then([](pplx::task<GetTokenAndSignatureResult^> t)
    {
        try
        {
            GetTokenAndSignatureResult^ result = t.get();
            user_context_auth_result userContextResult(result->Token->Data(), result->Signature->Data());
            return xbox_live_result<user_context_auth_result>(userContextResult);
        }
        catch (Exception^ ex)
        {
            xbox_live_error_code err = xbox::services::utils::convert_exception_to_xbox_live_error_code();
            return xbox_live_result<user_context_auth_result>(err, "Failed getting auth token");
        }
    });
}

void user_context::get_auth_result(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const string_t& requestBodyString,
    _In_ bool allUsersAuthRequired,
    _In_ uint64_t taskGroupId,
    _In_ xbox_live_callback<xbox::services::xbox_live_result<user_context_auth_result>> callback
    )
{
    // TODO
}

void user_context::get_auth_result(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& requestBodyVector,
    _In_ bool allUsersAuthRequired,
    _In_ uint64_t taskGroupId,
    _In_ xbox_live_callback<xbox::services::xbox_live_result<user_context_auth_result>> callback
    )
{
    // TODO
}

pplx::task<xbox_live_result<void>> user_context::refresh_token()
{
    auto authConfig = m_user->_User_impl()->get_auth_config();

    return m_user->_User_impl()->internal_get_token_and_signature(
        _T("GET"),
        authConfig->xbox_live_endpoint(),
        string_t(),
        string_t(),
        std::vector<unsigned char>(),
        false,
        true)
        .then([](xbox_live_result<token_and_signature_result> result)
    {
        if (!result.err())
        {
            return xbox_live_result<void>();
        }
        else
        {
            return xbox_live_result<void>(result.err(), result.err_message());
        }
    });
}

bool user_context::is_signed_in() const
{
    if (!user()) return false;
    return user()->IsSignedIn;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#endif
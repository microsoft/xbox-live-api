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

// XDK's Windows.* user object
#if TV_API

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

user_context::user_context(_In_ Windows::Xbox::System::User^ user) :
    m_user(user),
    m_callerContextType(xbox::services::caller_context_type::title)
{
    XSAPI_ASSERT(m_user != nullptr);
    if (m_user == nullptr)
    {
        m_xboxUserId = string_t();
    }

    m_xboxUserId = m_user->XboxUserId->Data();
}

Windows::Xbox::System::User^ user_context::user() const
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
    auto platformHttp = ref new String(httpMethod.c_str());
    auto platformUrl = ref new String(url.c_str());
    auto platformHeaders = ref new String(headers.c_str());
    auto platformRequestBody = ref new String(requestBodyString.c_str());
    pplx::task<Windows::Xbox::System::GetTokenAndSignatureResult^> asyncTask;

    if(allUsersAuthRequired)
    {
        if(platformRequestBody == nullptr)
        {
            asyncTask = pplx::create_task([this, platformHttp, platformUrl, platformHeaders]()
            {
                return m_user->GetTokenAndSignatureForAllUsersAsync(
                    platformHttp,
                    platformUrl,
                    platformHeaders
                );
            });
        }
        else
        {
            asyncTask = pplx::create_task([this, platformHttp, platformUrl, platformHeaders, platformRequestBody]()
            {
                return m_user->GetTokenAndSignatureForAllUsersAsync(
                    platformHttp,
                    platformUrl,
                    platformHeaders,
                    platformRequestBody
                );
            });
        }
    }
    else
    {
        if(platformRequestBody == nullptr)
        {
            asyncTask = pplx::create_task([this, platformHttp, platformUrl, platformHeaders]()
            {
                return m_user->GetTokenAndSignatureAsync(
                    platformHttp,
                    platformUrl,
                    platformHeaders
                );
            });
        }
        else
        {
            asyncTask = pplx::create_task([this, platformHttp, platformUrl, platformHeaders, platformRequestBody]()
            {
                return m_user->GetTokenAndSignatureAsync(
                    platformHttp,
                    platformUrl,
                    platformHeaders,
                    platformRequestBody
                );
            });
        }
    }

    return asyncTask
    .then([](pplx::task<Windows::Xbox::System::GetTokenAndSignatureResult^> result) 
    {
        try
        {
            auto tokenAndSig = result.get();
            user_context_auth_result userContextResult( tokenAndSig->Token->ToString()->Data(), tokenAndSig->Signature->ToString()->Data() );
            return xbox_live_result<user_context_auth_result>(userContextResult);
        }
        catch(Exception^ ex)
        {
            xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
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
    auto byteArr = ref new Array<unsigned char, 1U>(static_cast<uint32_t>(requestBody.size()));
    memcpy(&byteArr->Data[0], &requestBody[0], requestBody.size());
    pplx::task<Windows::Xbox::System::GetTokenAndSignatureResult^> asyncTask;

    if(allUsersAuthRequired)
    {
        asyncTask = pplx::create_task([this, httpMethod, url, headers, byteArr]()
        {
            return m_user->GetTokenAndSignatureForAllUsersAsync(
                ref new String(httpMethod.c_str()),
                ref new String(url.c_str()),
                ref new String(headers.c_str()),
                byteArr
            );
        });
    }
    else
    {
        asyncTask = pplx::create_task([this, httpMethod, url, headers, byteArr]()
        {
            return m_user->GetTokenAndSignatureAsync(
                ref new String(httpMethod.c_str()),
                ref new String(url.c_str()),
                ref new String(headers.c_str()),
                byteArr
            );
        });
    }

    return asyncTask
    .then([](pplx::task<Windows::Xbox::System::GetTokenAndSignatureResult^> result) 
    {
        try
        {
            auto tokenAndSig = result.get();
            user_context_auth_result userContextResult( tokenAndSig->Token->ToString()->Data(), tokenAndSig->Signature->ToString()->Data() );
            return xbox_live_result<user_context_auth_result>(userContextResult);
        }
        catch(Exception^ ex)
        {
            xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
            return xbox_live_result<user_context_auth_result>(err, "Failed getting auth token");
        }
    }, pplx::task_continuation_context::use_arbitrary());
}

// Console OS will auto refresh tokens, we don't need to do anything here.
pplx::task<xbox_live_result<void>> user_context::refresh_token()
{
    return pplx::task_from_result(xbox_live_result<void>());
}

bool user_context::is_signed_in() const
{
    if (!user()) return false;
    return user()->IsSignedIn;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#endif
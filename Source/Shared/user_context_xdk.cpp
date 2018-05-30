// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_system_factory.h"
#include "user_context.h"
#include "shared_macros.h"

#if TV_API

#if defined __cplusplus_winrt
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Xbox::System;
#if !XSAPI_CPP
using namespace Microsoft::Xbox::Services::System;
#endif
#endif

using namespace xbox::services::system;

// XDK's Windows.* user object

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

user_context::user_context(_In_ Windows::Xbox::System::User^ user) :
    m_user(user),
    m_callerContextType(xbox::services::caller_context_type::title)
{
    XSAPI_ASSERT(m_user != nullptr);
    if (m_user == nullptr)
    {
        m_xboxUserId = xsapi_internal_string();
    }

    m_xboxUserId = utils::internal_string_from_char_t(m_user->XboxUserId->Data());
}

Windows::Xbox::System::User^ user_context::user() const
{
    return m_user;
}

void user_context::get_auth_result(
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& url,
    _In_ const xsapi_internal_string& headers,
    _In_ const xsapi_internal_string& requestBodyString,
    _In_ bool allUsersAuthRequired,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<user_context_auth_result>> callback
    )
{
    UNREFERENCED_PARAMETER(queue);

    auto platformHttp = PLATFORM_STRING_FROM_INTERNAL_STRING(httpMethod);
    auto platformUrl = PLATFORM_STRING_FROM_INTERNAL_STRING(url);
    auto platformHeaders = PLATFORM_STRING_FROM_INTERNAL_STRING(headers);
    auto platformRequestBody = PLATFORM_STRING_FROM_INTERNAL_STRING(requestBodyString);

    IAsyncOperation<GetTokenAndSignatureResult^>^ asyncOp;

    if (allUsersAuthRequired)
    {
        if (platformRequestBody == nullptr)
        {
            asyncOp = m_user->GetTokenAndSignatureForAllUsersAsync(
                platformHttp,
                platformUrl,
                platformHeaders
            );
        }
        else
        {
            asyncOp = m_user->GetTokenAndSignatureForAllUsersAsync(
                platformHttp,
                platformUrl,
                platformHeaders,
                platformRequestBody
            );
        }
    }
    else
    {
        if (platformRequestBody == nullptr)
        {
            asyncOp = m_user->GetTokenAndSignatureAsync(
                platformHttp,
                platformUrl,
                platformHeaders
            );
        }
        else
        {
            asyncOp = m_user->GetTokenAndSignatureAsync(
                platformHttp,
                platformUrl,
                platformHeaders,
                platformRequestBody
            );
        }
    }

    asyncOp->Completed = ref new AsyncOperationCompletedHandler<GetTokenAndSignatureResult^>(
        [asyncOp, callback](IAsyncOperation<GetTokenAndSignatureResult^>^ asyncInfo, AsyncStatus asyncStatus)
    {
        UNREFERENCED_PARAMETER(asyncStatus);
        try
        {
            auto tokenAndSig = asyncInfo->GetResults();
            user_context_auth_result userContextResult(
                utils::internal_string_from_char_t(tokenAndSig->Token->ToString()->Data()), 
                utils::internal_string_from_char_t(tokenAndSig->Signature->ToString()->Data())
                );
            callback(xbox_live_result<user_context_auth_result>(userContextResult));
        }
        catch (Exception^ ex)
        {
            xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
            callback(xbox_live_result<user_context_auth_result>(err, "Failed getting auth token"));
        }
    });
}

void user_context::get_auth_result(
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& url,
    _In_ const xsapi_internal_string& headers,
    _In_ const xsapi_internal_vector<unsigned char>& requestBodyVector,
    _In_ bool allUsersAuthRequired,
    _In_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<user_context_auth_result>> callback
    )
{
    UNREFERENCED_PARAMETER(queue);

    auto byteArr = ref new Array<unsigned char, 1U>(static_cast<uint32_t>(requestBodyVector.size()));
    memcpy(&byteArr->Data[0], &requestBodyVector[0], requestBodyVector.size());
    IAsyncOperation<GetTokenAndSignatureResult^>^ asyncOp;

    if (allUsersAuthRequired)
    {
        asyncOp = m_user->GetTokenAndSignatureForAllUsersAsync(
            PLATFORM_STRING_FROM_INTERNAL_STRING(httpMethod),
            PLATFORM_STRING_FROM_INTERNAL_STRING(url),
            PLATFORM_STRING_FROM_INTERNAL_STRING(headers),
            byteArr
            );
    }
    else
    {
        asyncOp = m_user->GetTokenAndSignatureAsync(
            PLATFORM_STRING_FROM_INTERNAL_STRING(httpMethod),
            PLATFORM_STRING_FROM_INTERNAL_STRING(url),
            PLATFORM_STRING_FROM_INTERNAL_STRING(headers),
            byteArr
            );
    }

    asyncOp->Completed = ref new AsyncOperationCompletedHandler<GetTokenAndSignatureResult^>(
        [asyncOp, callback](IAsyncOperation<GetTokenAndSignatureResult^>^ asyncInfo, AsyncStatus asyncStatus)
    {
        UNREFERENCED_PARAMETER(asyncStatus);
        try
        {
            auto tokenAndSig = asyncInfo->GetResults();
            user_context_auth_result userContextResult(
                utils::internal_string_from_char_t(tokenAndSig->Token->ToString()->Data()), 
                utils::internal_string_from_char_t(tokenAndSig->Signature->ToString()->Data())
                );
            callback(xbox_live_result<user_context_auth_result>(userContextResult));
        }
        catch (Exception^ ex)
        {
            xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
            callback(xbox_live_result<user_context_auth_result>(err, "Failed getting auth token"));
        }
    });
}

// Console OS will auto refresh tokens, we don't need to do anything here.
void user_context::refresh_token(
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<xbox::services::system::token_and_signature_result_internal>>> callback
    )
{
    UNREFERENCED_PARAMETER(queue);
    callback(xbox_live_result<std::shared_ptr<xbox::services::system::token_and_signature_result_internal>>());
}

bool user_context::is_signed_in() const
{
    if (!user()) return false;
    return user()->IsSignedIn;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#endif
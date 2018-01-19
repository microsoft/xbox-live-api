// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_system_factory.h"
#include "user_context.h"
#include "shared_macros.h"
#include "xsapi/system.h"
#include "xsapi/types.h"

#if defined __cplusplus_winrt
using namespace Platform;
using namespace Windows::Foundation::Collections;
#if !XSAPI_CPP
using namespace Microsoft::Xbox::Services::System;
#endif
#endif

using namespace xbox::services::system;

#if !TV_API && XSAPI_CPP && !UNIT_TEST_SERVICES // Non-XDK C++


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

user_context::user_context(_In_ std::shared_ptr< xbox::services::system::xbox_live_user > user) :
    m_user(user),
    m_callerContextType(xbox::services::caller_context_type::title)
{
    XSAPI_ASSERT(user != nullptr);
    m_xboxUserId = m_user->xbox_user_id();
}

std::shared_ptr< xbox::services::system::xbox_live_user > user_context::user() const
{
    return m_user;
}

void user_context::get_auth_result(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const string_t& requestBodyString,
    _In_ bool allUsersAuthRequired,
    _In_ uint64_t taskGroupId,
    _In_ xsapi_callback<xbox::services::xbox_live_result<user_context_auth_result>> callback
    )
{
    UNREFERENCED_PARAMETER(allUsersAuthRequired);

    std::string utf8Body(utility::conversions::to_utf8string(requestBodyString));
    std::vector<unsigned char> utf8Vec(utf8Body.begin(), utf8Body.end());

    get_auth_result(
        httpMethod,
        url,
        headers,
        utf8Vec,
        allUsersAuthRequired,
        taskGroupId,
        callback
    );
}

void user_context::get_auth_result(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& requestBodyVector,
    _In_ bool allUsersAuthRequired,
    _In_ uint64_t taskGroupId,
    _In_ xsapi_callback<xbox::services::xbox_live_result<user_context_auth_result>> callback
    )
{
    UNREFERENCED_PARAMETER(allUsersAuthRequired);

#if XSAPI_SERVER
    if (m_server != nullptr)
    {
        m_server->get_token_and_signature(httpMethod, url, headers, requestBodyString)
            .then([completionRoutine, completionRoutineContext, taskGroupId](xbox_live_result<token_and_signature_result> xblResult)
        {
            const auto& tokenResult = xblResult.payload();
            user_context_auth_result userContextResult(tokenResult.token(), tokenResult.signature());
            callback(xbox_live_result<user_context_auth_result>(userContextResult, xblResult.err(), xblResult.err_message()));
        });
        return;
    }
#endif
    if (m_user != nullptr)
    {
        m_user->_User_impl()->get_token_and_signature(httpMethod, url, headers, requestBodyVector, taskGroupId,
            [callback](xbox_live_result<token_and_signature_result> result)
        {
            const auto& tokenResult = result.payload();
            user_context_auth_result userContextResult(tokenResult.token(), tokenResult.signature());
            callback(xbox_live_result<user_context_auth_result>(userContextResult, result.err(), result.err_message()));
        });
    }
}

pplx::task<xbox_live_result<user_context_auth_result> >
user_context::get_auth_result(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const string_t& requestBodyString,
    _In_ bool allUsersAuthRequired
    )
{
    UNREFERENCED_PARAMETER(allUsersAuthRequired);

    pplx::task<xbox_live_result<token_and_signature_result>> tokenTask;
    if (tokenTask._GetImpl() == nullptr && m_user != nullptr)
    {
        tokenTask = m_user->get_token_and_signature(httpMethod, url, headers, requestBodyString);
    }

    return tokenTask
    .then([](xbox_live_result<xbox::services::system::token_and_signature_result> xblResult)
    {
        const auto& tokenResult = xblResult.payload();
        user_context_auth_result userContextResult(tokenResult.token(), tokenResult.signature());
        return xbox_live_result<user_context_auth_result>(userContextResult, xblResult.err(), xblResult.err_message());
    });
}

pplx::task<xbox_live_result<user_context_auth_result>> user_context::get_auth_result(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& requestBodyArray,
    _In_ bool allUsersAuthRequired
    )
{
    UNREFERENCED_PARAMETER(allUsersAuthRequired);

    pplx::task<xbox_live_result<token_and_signature_result>> tokenTask;
    if (tokenTask._GetImpl() == nullptr && m_user != nullptr)
    {
        tokenTask = m_user->get_token_and_signature_array(httpMethod, url, headers, requestBodyArray);
    }

    return tokenTask
    .then([](xbox_live_result<xbox::services::system::token_and_signature_result> xblResult)
    {
        const auto& tokenResult = xblResult.payload();
        user_context_auth_result userContextResult(tokenResult.token(), tokenResult.signature());
        return xbox_live_result<user_context_auth_result>(userContextResult, xblResult.err(), xblResult.err_message());
    });
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
    return user()->is_signed_in();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#endif
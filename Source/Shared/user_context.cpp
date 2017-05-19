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

using namespace XBOX_LIVE_NAMESPACE::system;


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// XDK's Windows.* user object
#if XSAPI_SERVER

user_context::user_context(_In_ std::shared_ptr<system::xbox_live_server> server) :
    m_server(server),
    m_callerContextType(xbox::services::caller_context_type::title)
{
    XSAPI_ASSERT(server != nullptr);
}

std::shared_ptr<system::xbox_live_server> user_context::server() const
{
    return m_server;
}

#endif

#if TV_API | XBOX_UWP
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
            if (userContextResult.token().length() == 0)
            {
                return xbox_live_result<user_context_auth_result>(xbox_live_error_code::auth_unknown_error, "No auth token");
            }
            else
            {
                return xbox_live_result<user_context_auth_result>(userContextResult);
            }
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
            if (userContextResult.token().length() == 0)
            {
                return xbox_live_result<user_context_auth_result>(xbox_live_error_code::auth_unknown_error, "No auth token");
            }
            else
            {
                return xbox_live_result<user_context_auth_result>(userContextResult);
            }
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

#elif XSAPI_CPP
user_context::user_context(_In_ std::shared_ptr< xbox::services::system::xbox_live_user > user) :
    m_user(user),
    m_callerContextType(xbox::services::caller_context_type::title)
{
    XSAPI_ASSERT(user != nullptr);
    m_xboxUserId = m_user->xbox_user_id();
}

std::shared_ptr< xbox::services::system::xbox_live_user > user_context::user()
{
    return m_user;
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
#if XSAPI_SERVER
    if (m_server != nullptr)
    {
        tokenTask = m_server->get_token_and_signature(httpMethod, url, headers, requestBodyString);
    }
#endif
    if (tokenTask._GetImpl() == nullptr && m_user != nullptr)
    {
        tokenTask = m_user->get_token_and_signature(httpMethod, url, headers, requestBodyString);
    }

    return tokenTask
    .then([](xbox_live_result<xbox::services::system::token_and_signature_result> xblResult)
    {
        const auto& tokenResult = xblResult.payload();
        user_context_auth_result userContextResult(tokenResult.token(), tokenResult.signature());
        if (userContextResult.token().length() == 0 && !xblResult.err())
        {
            return xbox_live_result<user_context_auth_result>(xbox_live_error_code::auth_unknown_error, "No auth token");
        }
        else
        {
            return xbox_live_result<user_context_auth_result>(userContextResult, xblResult.err(), xblResult.err_message());
        }
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

#if XSAPI_SERVER
    if (m_server != nullptr)
    {
        tokenTask = m_server->get_token_and_signature_array(httpMethod, url, headers, requestBodyArray);
    }
#endif
    if (tokenTask._GetImpl() == nullptr && m_user != nullptr)
    {
        tokenTask = m_user->get_token_and_signature_array(httpMethod, url, headers, requestBodyArray);
    }

    return tokenTask
    .then([](xbox_live_result<xbox::services::system::token_and_signature_result> xblResult)
    {
        const auto& tokenResult = xblResult.payload();
        user_context_auth_result userContextResult(tokenResult.token(), tokenResult.signature());
        if (userContextResult.token().length() == 0 && !xblResult.err())
        {
            return xbox_live_result<user_context_auth_result>(xbox_live_error_code::auth_unknown_error, "No auth token");
        }
        else
        {
            return xbox_live_result<user_context_auth_result>(userContextResult, xblResult.err(), xblResult.err_message());
        }
    });
}

pplx::task<xbox_live_result<void>> user_context::refresh_token()
{
#if XSAPI_SERVER
    if (m_server != nullptr)
    {
        return m_server->m_server_impl->signin(nullptr, true);
    }
#endif

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
            return xbox_live_result<void>(result.err(), "Refresh token failed.");
        }
    });
}

#else 
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
            if (userContextResult.token().length() == 0)
            {
                return xbox_live_result<user_context_auth_result>(xbox_live_error_code::auth_unknown_error, "No auth token");
            }
            else
            {
                return xbox_live_result<user_context_auth_result>(userContextResult);
            }
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
            if (userContextResult.token().length() == 0)
            {
                return xbox_live_result<user_context_auth_result>(xbox_live_error_code::auth_unknown_error, "No auth token");
            }
            else
            {
                return xbox_live_result<user_context_auth_result>(userContextResult);
            }
        }
        catch (Exception^ ex)
        {
            xbox_live_error_code err = xbox::services::utils::convert_exception_to_xbox_live_error_code();
            return xbox_live_result<user_context_auth_result>(err, "Failed getting auth token");
        }
    });
}

pplx::task<xbox_live_result<void>> user_context::refresh_token()
{
    auto authConfig = m_user->GetUserImpl()->get_auth_config();

    return m_user->GetUserImpl()->internal_get_token_and_signature(
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
            return xbox_live_result<void>(result.err(), "Refresh token failed.");
        }
    });
}

#endif

const string_t&
user_context::caller_context() const
{
    return m_callerContext;
}

caller_context_type
user_context::caller_context_type() const
{
    return m_callerContextType;
}

void
user_context::set_caller_context_type(xbox::services::caller_context_type context)
{
    m_callerContextType = context;
    if (context == caller_context_type::multiplayer_manager)
    {
        m_callerContext = _T("MultiplayerManager");
    }
    else if (context == caller_context_type::social_manager)
    {
        m_callerContext = _T("SocialManager");
    }
}

const string_t& user_context::xbox_user_id() const
{
    return m_xboxUserId;
}

user_context_auth_result::user_context_auth_result()
{
}

user_context_auth_result::user_context_auth_result(
    _In_ string_t token,
    _In_ string_t signature) :
    m_token(std::move(token)),
    m_signature(std::move(signature))
{
}

const string_t& user_context_auth_result::token() const
{
    return m_token;
}

const string_t& user_context_auth_result::signature() const
{
    return m_signature;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

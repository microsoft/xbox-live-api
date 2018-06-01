// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "system_internal.h"

#if !TV_API
    #if !XSAPI_CPP
        // This header is required for C++ Microsoft.* user object
        #include "User_WinRT.h"
    #else
        #include "xsapi/system.h"
        #include "user_impl.h"
    #endif
#endif
#include "xsapi/types.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
class token_and_signature_result_internal;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

enum class caller_context_type
{
    title,
    multiplayer_manager,
    social_manager,
    stats_manager
};

enum class caller_api_type
{
    api_unknown,
    api_c,
    api_cpp,
    api_winrt
};

class user_context_auth_result
{
public:
    user_context_auth_result();

    user_context_auth_result(
        _In_ xsapi_internal_string token,
        _In_ xsapi_internal_string signature
        );

    const xsapi_internal_string& token() const;
    const xsapi_internal_string& signature() const;

private:
    xsapi_internal_string m_token;
    xsapi_internal_string m_signature;
};

class user_context
{
public:
    user_context() :
        m_callerContextType(xbox::services::caller_context_type::title),
        m_apiType(xbox::services::caller_api_type::api_unknown)
    {};

    const xsapi_internal_string& xbox_user_id() const;

    const xsapi_internal_string& caller_context() const;
    caller_context_type caller_context_type() const;
    caller_api_type api_type() const { return m_apiType; }
    void set_caller_context_type(xbox::services::caller_context_type context);
    void set_caller_api_type(xbox::services::caller_api_type apiType);

    bool is_signed_in() const;

    void get_auth_result(
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& url,
        _In_ const xsapi_internal_string& headers,
        _In_ const xsapi_internal_string& requestBodyString,
        _In_ bool allUsersAuthRequired,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<user_context_auth_result>> callback
        );

    void get_auth_result(
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& url,
        _In_ const xsapi_internal_string& headers,
        _In_ const xsapi_internal_vector<unsigned char>& requestBodyVector,
        _In_ bool allUsersAuthRequired,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<user_context_auth_result>> callback
        );

    void refresh_token(
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<xbox::services::system::token_and_signature_result_internal>>> callback
        );

    // inline helper functions
    static xsapi_internal_string get_user_id(xbox_live_user_t user)
    {
#if TV_API
        return utils::internal_string_from_char_t(user->XboxUserId->Data());
#else
        return user->_User_impl()->xbox_user_id();
#endif
    }
    
#if XSAPI_XDK_AUTH
public:
    static Windows::Xbox::System::User^ user_convert(Windows::Xbox::System::User^ user)
    {
        return user;
    }

    // XDK's Windows.* user object
public:
    user_context(_In_ Windows::Xbox::System::User^ user);
    Windows::Xbox::System::User^ user() const;

private:
    Windows::Xbox::System::User^ m_user;
#endif 


#if XSAPI_NONXDK_CPP_AUTH && !UNIT_TEST_SERVICES
    // C++ Microsoft.* user object
public:
    user_context(_In_ std::shared_ptr< xbox::services::system::xbox_live_user > user);
    std::shared_ptr< xbox::services::system::xbox_live_user > user() const;

private:
    std::shared_ptr< xbox::services::system::xbox_live_user > m_user;
#endif 

#if XSAPI_NONXDK_WINRT_AUTH
    static std::shared_ptr<xbox::services::system::xbox_live_user> user_convert(Microsoft::Xbox::Services::System::XboxLiveUser^ user)
    {
        return std::make_shared<xbox::services::system::xbox_live_user>(user->_User_impl());
    }

    static Microsoft::Xbox::Services::System::XboxLiveUser^ user_convert(std::shared_ptr<xbox::services::system::xbox_live_user> user)
    {
        return ref new Microsoft::Xbox::Services::System::XboxLiveUser(user->_User_impl());
    }

    // WinRT Microsoft.* user object
public:
    user_context(_In_ std::shared_ptr< xbox::services::system::xbox_live_user > user);
    user_context(_In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user);
    Microsoft::Xbox::Services::System::XboxLiveUser^ user() const;

private:
    Microsoft::Xbox::Services::System::XboxLiveUser^ m_user;
#endif


private:
    xsapi_internal_string m_xboxUserId;
    xsapi_internal_string m_callerContext;
    xbox::services::caller_api_type m_apiType;
    xbox::services::caller_context_type m_callerContextType;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

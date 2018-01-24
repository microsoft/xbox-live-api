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
    #endif
#endif
#include "xsapi/types.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

enum class caller_context_type
{
    title,
    multiplayer_manager,
    social_manager,
    stats_manager
};

class user_context_auth_result
{
public:
    user_context_auth_result();

    user_context_auth_result(
        _In_ string_t token,
        _In_ string_t signature
        );

    const string_t& token() const;
    const string_t& signature() const;

private:
    string_t m_token;
    string_t m_signature;
};

class user_context
{
public:
    user_context() {};
    const string_t& xbox_user_id() const;

    const string_t& caller_context() const;
    caller_context_type caller_context_type() const;
    void set_caller_context_type(xbox::services::caller_context_type context);

    bool is_signed_in() const;

    void get_auth_result(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_ const string_t& requestBodyString,
        _In_ bool allUsersAuthRequired,
        _In_ uint64_t taskGroupId,
        _In_ xbox_live_callback<xbox::services::xbox_live_result<user_context_auth_result>> callback
        );

    void get_auth_result(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& requestBodyVector,
        _In_ bool allUsersAuthRequired,
        _In_ uint64_t taskGroupId,
        _In_ xbox_live_callback<xbox::services::xbox_live_result<user_context_auth_result>> callback
        );

    /// TODO eventually remove
    pplx::task<xbox::services::xbox_live_result<user_context_auth_result>> get_auth_result(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_ const string_t& requestBodyString,
        _In_ bool allUsersAuthRequired = false
        );

    /// TODO eventually remove
    pplx::task<xbox::services::xbox_live_result<user_context_auth_result>> get_auth_result(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& requestBodyVector,
        _In_ bool allUsersAuthRequired = false
        );

    pplx::task<xbox::services::xbox_live_result<void>> refresh_token();

    // inline helper functions
    static string_t get_user_id(xbox_live_user_t user)
    {
#if TV_API
        return user->XboxUserId->Data();
#else
        return user->xbox_user_id();
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
    string_t m_xboxUserId;
    string_t m_callerContext;
    xbox::services::caller_context_type m_callerContextType;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

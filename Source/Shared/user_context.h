//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

#if !TV_API

#if !XSAPI_CPP
    // This header is required for C++ Microsoft.* user object
    #include "User_WinRT.h"
#else
    #include "xsapi/system.h"
#endif

#endif

#if !XSAPI_CPP
#if TV_API | XBOX_UWP
typedef Windows::Xbox::System::User^ XboxLiveUser_t;
#else
typedef Microsoft::Xbox::Services::System::XboxLiveUser^ XboxLiveUser_t;
#endif
#endif

#if TV_API | XBOX_UWP
typedef  Windows::Xbox::System::User^ xbox_live_user_t;
#else
typedef std::shared_ptr<XBOX_LIVE_NAMESPACE::system::xbox_live_user> xbox_live_user_t;
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

enum class caller_context_type
{
    title,
    multiplayer_manager,
    social_manager
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
    void set_caller_context_type(XBOX_LIVE_NAMESPACE::caller_context_type context);

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<user_context_auth_result>> get_auth_result(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_ const string_t& requestBodyString,
        _In_ bool allUsersAuthRequired = false
        );

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<user_context_auth_result>> get_auth_result(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& requestBodyVector,
        _In_ bool allUsersAuthRequired = false
        );

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<void>> refresh_token();

    // inline helper functions
    static string_t get_user_id(xbox_live_user_t user)
    {
#if TV_API | XBOX_UWP
        return user->XboxUserId->Data();
#else
        return user->xbox_user_id();
#endif
    }

#if !XSAPI_CPP
#if TV_API | XBOX_UWP
    static Windows::Xbox::System::User^ user_convert(Windows::Xbox::System::User^ user)
    {
        return user;
    }
#else
    static std::shared_ptr<XBOX_LIVE_NAMESPACE::system::xbox_live_user> user_convert(Microsoft::Xbox::Services::System::XboxLiveUser^ user)
    {
        return std::make_shared<XBOX_LIVE_NAMESPACE::system::xbox_live_user>(user->GetUserImpl());
    }

    static Microsoft::Xbox::Services::System::XboxLiveUser^ user_convert(std::shared_ptr<XBOX_LIVE_NAMESPACE::system::xbox_live_user> user)
    {
        return ref new Microsoft::Xbox::Services::System::XboxLiveUser(user->_User_impl());
    }
#endif
#endif

#if XSAPI_SERVER
public:
    user_context(_In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::system::xbox_live_server> server);
    std::shared_ptr<XBOX_LIVE_NAMESPACE::system::xbox_live_server> server() const;

private:
    std::shared_ptr<XBOX_LIVE_NAMESPACE::system::xbox_live_server> m_server;
#endif

#if TV_API | XBOX_UWP
    // XDK's Windows.* user object
    public:
        user_context(_In_ Windows::Xbox::System::User^ user);
        Windows::Xbox::System::User^ user() const;

    private:
        Windows::Xbox::System::User^ m_user;
#elif XSAPI_CPP
    // C++ Microsoft.* user object
    public:
        user_context(_In_ std::shared_ptr< XBOX_LIVE_NAMESPACE::system::xbox_live_user > user);
        std::shared_ptr< XBOX_LIVE_NAMESPACE::system::xbox_live_user > user();
    private:
        std::shared_ptr< XBOX_LIVE_NAMESPACE::system::xbox_live_user > m_user;
#else
        // WinRT Microsoft.* user object
    public:
        user_context(_In_ std::shared_ptr< XBOX_LIVE_NAMESPACE::system::xbox_live_user > user);

        user_context(_In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user);
        
        Microsoft::Xbox::Services::System::XboxLiveUser^ user() const;
    private:
        Microsoft::Xbox::Services::System::XboxLiveUser^ m_user;
#endif

private:
    string_t m_xboxUserId;
    string_t m_callerContext;
    XBOX_LIVE_NAMESPACE::caller_context_type m_callerContextType;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

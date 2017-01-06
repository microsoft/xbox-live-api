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

#include <string>
#include <regex>
#include <chrono>

#ifdef _WIN32
#include <Windows.h>

#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10 0x0A00
#endif

#ifndef TV_API
#define TV_API (WINAPI_FAMILY == WINAPI_FAMILY_TV_APP || WINAPI_FAMILY == WINAPI_FAMILY_TV_TITLE) 
#endif

#ifndef UWP_API
#define UWP_API (WINAPI_FAMILY == WINAPI_FAMILY_APP && _WIN32_WINNT >= _WIN32_WINNT_WIN10)
#endif

#endif //#ifdef _WIN32

#ifndef _WIN32
    #ifdef _In_
        #undef _In_
    #endif
    #define _In_
    #ifdef _Ret_maybenull_
        #undef _Ret_maybenull_
    #endif
    #define _Ret_maybenull_
    #ifdef _Post_writable_byte_size_
        #undef _Post_writable_byte_size_
    #endif
    #define _Post_writable_byte_size_(X)
#endif

#ifndef _T
    #ifdef _WIN32
        #define _T(x) L ## x
    #else
        #define _T(x) x
    #endif
#endif

#if defined _WIN32
  #ifdef _NO_XSAPIIMP
    #define _XSAPIIMP
    #define _XSAPIIMP_DEPRECATED __declspec(deprecated)
  #else
    #ifdef _XSAPIIMP_EXPORT
      #define _XSAPIIMP __declspec(dllexport)
      #define _XSAPIIMP_DEPRECATED __declspec(dllexport, deprecated)
    #else
      #define _XSAPIIMP __declspec(dllimport)
      #define _XSAPIIMP_DEPRECATED __declspec(dllimport, deprecated)
    #endif
  #endif
#else
  #if defined _NO_XSAPIIMP || __GNUC__ < 4
     #define _XSAPIIMP
     #define _XSAPIIMP_DEPRECATED __attribute__ ((deprecated))
  #else
    #define _XSAPIIMP __attribute__ ((visibility ("default")))
    #define _XSAPIIMP_DEPRECATED __attribute__ ((visibility ("default"), deprecated))
  #endif
#endif

#if !defined(_WIN32) | (defined(_MSC_VER) && (_MSC_VER >= 1900))
// VS2013 doesn't support default move constructor and assignment, so we implemented this.
// However, a user defined move constructor and assignment will implicitly delete default copy 
// constructor and assignment in other compiler like clang. So we only define this in Win32 under VS2013
#define DEFAULT_MOVE_ENABLED
#endif

typedef int32_t function_context;
#ifdef _WIN32
typedef wchar_t char_t;
typedef std::wstring string_t;
typedef std::wstringstream stringstream_t;
typedef std::wregex regex_t;
typedef std::wsmatch smatch_t;
#else
typedef char char_t;
typedef std::string string_t;
typedef std::stringstream stringstream_t;
typedef std::regex regex_t;
typedef std::smatch smatch_t;
#endif

#if _MSC_VER <= 1800
typedef std::chrono::system_clock chrono_clock_t;
#else
typedef std::chrono::steady_clock chrono_clock_t;
#endif

// Forward declarations
namespace xbox { namespace services {
#if BEAM_API
namespace beam {
#endif
    class user_context;
    class xbox_live_context_settings;
    class local_config;
#if BEAM_API
}
#endif
}}

#if !TV_API
// SSL client certificate context
#ifdef _WIN32
#include <wincrypt.h>
typedef PCCERT_CONTEXT cert_context;
#else
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/context.hpp>
typedef boost::asio::ssl::context* cert_context;
#endif
#endif

#if UWP_API || UNIT_TEST_SERVICES
typedef Windows::System::User^ user_creation_context;
#else
typedef void* user_creation_context;
#endif

#if TV_API | XBOX_UWP
typedef  Windows::Xbox::System::User^ xbox_live_user_t;
#endif

#if defined(XSAPI_CPPWINRT)
#if TV_API
inline Windows::Xbox::System::User^ convert_user_to_cppcx(_In_ const winrt::Windows::Xbox::System::User& user)
{
    winrt::ABI::Windows::Xbox::System::IUser* abiUser = winrt::get(user);
    return reinterpret_cast<Windows::Xbox::System::User^>(abiUser);
}

#if XBOX_UWP
inline Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ convert_eventargs_to_cppcx(
    _In_ const winrt::Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs& eventArgs
)
{
    winrt::ABI::Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs* abiEventArgs = winrt::get(eventArgs);
    return reinterpret_cast<Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^>(abiEventArgs);
}
#endif

inline std::vector<Windows::Xbox::System::User^> convert_user_vector_to_cppcx(
    _In_ const std::vector<winrt::Windows::Xbox::System::User>& users
)
{
    std::vector<Windows::Xbox::System::User^> cppCxUsers;
    for (winrt::Windows::Xbox::System::User u : users)
    {
        cppCxUsers.push_back(convert_user_to_cppcx(u));
    }
    return cppCxUsers;
}

inline winrt::Windows::Xbox::System::User convert_user_to_cppwinrt(_In_ Windows::Xbox::System::User^ user)
{
    winrt::Windows::Xbox::System::User cppWinrtUser(nullptr);
    winrt::copy_from(cppWinrtUser, reinterpret_cast<winrt::ABI::Windows::Xbox::System::IUser*>(user));
    return cppWinrtUser;
}

inline std::vector<winrt::Windows::Xbox::System::User> convert_user_vector_to_cppwinrt(
    _In_ const std::vector<xbox_live_user_t>& users
)
{
    std::vector<winrt::Windows::Xbox::System::User> cppWinRtUsers;
    for (xbox_live_user_t u : users)
    {
        cppWinRtUsers.push_back(convert_user_to_cppwinrt(u));
    }
    return cppWinRtUsers;
}
#endif
#endif

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    #if _MSC_VER >= 1900
        #define _XSAPIIMP_DEPRECATED __declspec(deprecated)
    #else
        #define _XSAPIIMP_DEPRECATED
    #endif
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
namespace xbox {
    namespace services {
        class user_context;
        class xbox_live_context_settings;
        class local_config;
        namespace system {
            class xbox_live_user;
        }
    }
}

#if !XSAPI_CPP || UNIT_TEST_SERVICES
namespace Microsoft {
    namespace Xbox {
        namespace Services {
            namespace System {
                ref class XboxLiveUser;
            }
        }
    }
}
#endif

#if TV_API
#define XSAPI_XDK_AUTH 1
#endif
#if TV_API && defined(XSAPI_CPPWINRT)
#define XSAPI_XDK_AUTH_WITH_CPPWINRT 1
#endif
#if !TV_API && XSAPI_CPP // Non-XDK C++
#define XSAPI_NONXDK_CPP_AUTH 1
#endif
#if !TV_API && (!XSAPI_CPP || UNIT_TEST_SERVICES) // Non-XDK WinRT
#define XSAPI_NONXDK_WINRT_AUTH 1
#endif

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

#if !XSAPI_CPP || UNIT_TEST_SERVICES
    #if TV_API
        typedef Windows::Xbox::System::User^ XboxLiveUser_t;
    #else
        typedef Microsoft::Xbox::Services::System::XboxLiveUser^ XboxLiveUser_t;
    #endif
#endif

#if TV_API
    typedef  Windows::Xbox::System::User^ xbox_live_user_t;
#else
    typedef std::shared_ptr<xbox::services::system::xbox_live_user> xbox_live_user_t;
#endif

#if defined(XSAPI_CPPWINRT)
#if TV_API
inline Windows::Xbox::System::User^ convert_user_to_cppcx(_In_ const winrt::Windows::Xbox::System::User& user)
{
    winrt::ABI::Windows::Xbox::System::IUser* abiUser = winrt::get_abi(user);
    return reinterpret_cast<Windows::Xbox::System::User^>(abiUser);
}

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
    winrt::copy_from_abi(cppWinrtUser, reinterpret_cast<winrt::ABI::Windows::Xbox::System::IUser*>(user));
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


#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN                     namespace xbox { namespace services {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END                       }}
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN                         namespace Microsoft { namespace Xbox { namespace Services { 
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_END                           }}}

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN              NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace system {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END                NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN                  NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace System { 
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END                    NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN              NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace social {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END                NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_BEGIN                  NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Social {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_END                    NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_BEGIN        NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace achievements {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_CPP_END          NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN            NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Achievements {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END              NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN         NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace leaderboard {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END           NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_BEGIN             NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Leaderboard {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_END               NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN      NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace user_statistics {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END        NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN          NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace UserStatistics {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END            NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN         NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace multiplayer {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END           NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN             NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Multiplayer {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END               NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN         NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace matchmaking {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END           NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_BEGIN             NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Matchmaking {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_END               NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_BEGIN         NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace marketplace {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_CPP_END           NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN             NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Marketplace {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END               NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN             NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace privacy {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END               NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_BEGIN                 NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Privacy {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_END                   NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN                 NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace real_time_activity {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END                   NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_BEGIN                     NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace RealTimeActivity {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_END                       NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN            NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace presence {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END              NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN                NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Presence {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END                  NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_BEGIN  NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace game_server_platform {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_CPP_END    NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN      NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace GameServerPlatform {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END        NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_BEGIN       NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace title_storage {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_CPP_END         NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_BEGIN           NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace TitleStorage {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_TITLE_STORAGE_END             NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_BEGIN              NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace events {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_CPP_END                NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_BEGIN                  NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Events {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_EVENTS_END                    NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN   NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace contextual_search {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END     NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_BEGIN       NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace ContextualSearch {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_END         NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_CPP_BEGIN   NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace entertainment_profile {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_CPP_END     NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_BEGIN       NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace EntertainmentProfile {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_ENTERTAINMENT_PROFILE_END         NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN        NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace notification {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END          NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace multiplayer { namespace manager {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END   NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END } }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN     NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Multiplayer { namespace Manager {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END       NAMESPACE_MICROSOFT_XBOX_SERVICES_END } }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_EXPERIMENTAL_CPP_BEGIN        NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace experimental {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_EXPERIMENTAL_CPP_END          NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_EXPERIMENTAL_BEGIN            NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Experimental {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_EXPERIMENTAL_END              NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN     NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace social { namespace manager {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END       NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END } }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_BEGIN         NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Social { namespace Manager {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_END           NAMESPACE_MICROSOFT_XBOX_SERVICES_END } }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN        NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace tournaments {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END          NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN            NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Tournaments {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END              NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN     NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace stats { namespace manager {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END       NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END } }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_BEGIN         NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Statistics { namespace Manager {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_STATISTIC_MANAGER_END           NAMESPACE_MICROSOFT_XBOX_SERVICES_END } }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_CPP_BEGIN   NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace player_state {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_CPP_END     NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_BEGIN       NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace PlayerState {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_PLAYER_STATE_END         NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN          NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN namespace clubs {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END            NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END }
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN              NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN namespace Clubs {
#define NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END                NAMESPACE_MICROSOFT_XBOX_SERVICES_END }

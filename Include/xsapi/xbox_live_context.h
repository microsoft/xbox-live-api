// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if !TV_API
    #if !XSAPI_CPP
        #include "User_WinRT.h"
    #else
        #include "xsapi/system.h"
    #endif
#endif

#include "xsapi/contextual_search_service.h"
#if TV_API || UNIT_TEST_SERVICES
#include "xsapi/entertainment_profile.h"
#endif
#include "xsapi/multiplayer.h"
#include "xsapi/tournaments.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
class xbox_live_context_server_impl;

/// <summary>
/// Defines pointers to objects that access Xbox Live to create features for player 
/// interactions.
///
/// Note: the XboxLiveContext is unique per instance. Changing state on one instance for a 
/// user does not affect a second instance of the context for the same user. Using multiple
/// instances can therefore result in unexpected behavior. Titles should ensure to only use 
/// one instance of the XboxLiveContext per user.
/// </summary>
class xbox_live_context
{
public:

#if XSAPI_XDK_AUTH
    /// <summary>
    /// Creates an xbox_live_context from a Windows::Xbox::System::User^
    /// </summary>
    _XSAPIIMP xbox_live_context(
        _In_ Windows::Xbox::System::User^ user
        );

    /// <summary>
    /// Returns the associated system User 
    /// </summary>
    _XSAPIIMP Windows::Xbox::System::User^ user();
#endif // XSAPI_XDK_AUTH

#if XSAPI_XDK_AUTH_WITH_CPPWINRT
    _XSAPIIMP xbox_live_context(
        _In_ winrt::Windows::Xbox::System::User user
    ) : xbox_live_context(convert_user_to_cppcx(user))
    {
    }

    inline winrt::Windows::Xbox::System::User user_cppwinrt()
    {
        winrt::Windows::Xbox::System::User cppWinrtUser(nullptr);
        winrt::copy_from_abi(cppWinrtUser, reinterpret_cast<winrt::ABI::Windows::Xbox::System::IUser*>(user()));
        return cppWinrtUser;
    }
#endif // XSAPI_XDK_AUTH_WITH_CPPWINRT

#if XSAPI_NONXDK_CPP_AUTH && !UNIT_TEST_SERVICES
    /// <summary>
    /// Creates an xbox_live_context from a xbox_live_user
    /// </summary>
    _XSAPIIMP xbox_live_context(
        _In_ std::shared_ptr<system::xbox_live_user> user
        );

    /// <summary>
    /// Returns the associated system User.
    /// </summary>
    _XSAPIIMP std::shared_ptr<system::xbox_live_user> user();
#endif // XSAPI_NONXDK_CPP_AUTH

#if XSAPI_NONXDK_WINRT_AUTH
    /// <summary>
    /// Creates an xbox_live_context from a Microsoft::Xbox::Services::System::XboxLiveUser^
    /// </summary>
    _XSAPIIMP xbox_live_context(
        _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user
        );

    /// <summary>
    /// Returns the associated system XboxLiveUser.
    /// </summary>
    _XSAPIIMP Microsoft::Xbox::Services::System::XboxLiveUser^ user();
#endif // XSAPI_NONXDK_WINRT_AUTH

    /// <summary>
    /// Returns the current user's Xbox Live User ID.
    /// </summary>
    _XSAPIIMP string_t xbox_live_user_id();

    /// <summary>
    /// Returns an object containing settings that apply to all REST calls made such as retry and diagnostic settings.
    /// </summary>
    _XSAPIIMP std::shared_ptr<xbox_live_context_settings> settings();

    /// <summary>
    /// Returns an object containing Xbox Live app config such as title ID
    /// </summary>
    _XSAPIIMP std::shared_ptr<xbox_live_app_config> application_config();

    /// <summary>
    /// A service for storing data in the cloud.
    /// </summary>
    _XSAPIIMP title_storage::title_storage_service& title_storage_service();

    /// <summary>
    /// A service for managing user profiles.
    /// </summary>
    _XSAPIIMP social::profile_service& profile_service();

    /// <summary>
    /// A service for managing privacy settings.
    /// </summary>
    _XSAPIIMP privacy::privacy_service& privacy_service();

#if !defined(XBOX_LIVE_CREATORS_SDK)
    /// <summary>
    /// A service for managing leaderboards.
    /// </summary>
    _XSAPIIMP leaderboard::leaderboard_service& leaderboard_service();

    /// <summary>
    /// A service for managing social networking links.
    /// </summary>
    _XSAPIIMP social::social_service& social_service();

    /// <summary>
    /// A service for managing reputation reports.
    /// </summary>
    _XSAPIIMP social::reputation_service& reputation_service();

    /// <summary>
    /// A service for managing achievements.
    /// </summary>
    _XSAPIIMP achievements::achievement_service& achievement_service();

    /// <summary>
    /// A service for managing user statistics.
    /// </summary>
    _XSAPIIMP user_statistics::user_statistics_service&  user_statistics_service();

    /// <summary>
    /// A service for managing multiplayer games.
    /// </summary>
    _XSAPIIMP multiplayer::multiplayer_service& multiplayer_service();

    /// <summary>
    /// A service for managing matchmaking sessions.
    /// </summary>
    _XSAPIIMP matchmaking::matchmaking_service& matchmaking_service();

    /// <summary>
    /// A service for managing tournaments.
    /// </summary>
    _XSAPIIMP tournaments::tournament_service& tournament_service();

    /// <summary>
    /// A service for managing real-time activity.
    /// </summary>
    _XSAPIIMP const std::shared_ptr<real_time_activity::real_time_activity_service>& real_time_activity_service();

    /// <summary>
    /// A service for using the Game Server Platform.
    /// </summary>
    _XSAPIIMP game_server_platform::game_server_platform_service& game_server_platform_service();

    /// <summary>
    /// A service for contextual search.
    /// </summary>
    _XSAPIIMP contextual_search::contextual_search_service& contextual_search_service();

    /// <summary>
    /// A service used to check for offensive strings.
    /// </summary>
    _XSAPIIMP system::string_service& string_service();

    /// <summary>
    /// A service for managing Rich Presence.
    /// </summary>
    _XSAPIIMP presence::presence_service& presence_service();

    /// <summary>
    /// A service for managing Title Clubs.
    /// </summary>
    _XSAPIIMP clubs::clubs_service& clubs_service();

#if UWP_API || XSAPI_U
    /// <summary>
    /// A service used to write in game events.
    /// </summary>
    _XSAPIIMP events::events_service& events_service();
#endif // UWP_API || XSAPI_U

#if TV_API || UNIT_TEST_SERVICES
    /// <summary>
    /// A service for managing catalogs.
    /// </summary>
    _XSAPIIMP marketplace::catalog_service& catalog_service();

    /// <summary>
    /// A service for managing inventory.
    /// </summary>
    _XSAPIIMP marketplace::inventory_service& inventory_service();

    /// <summary>
    /// A service for the entertainment profile.
    /// </summary>
    _XSAPIIMP entertainment_profile::entertainment_profile_list_service& entertainment_profile_list_service();
#endif // TV_API || UNIT_TEST_SERVICES

#endif // !defined(XBOX_LIVE_CREATORS_SDK)

#if !XSAPI_CPP
    /// <summary>
    /// Internal method
    /// </summary>
    std::shared_ptr<user_context> _User_context() const;
#endif

private:
    std::shared_ptr<xbox::services::xbox_live_context_impl> m_xboxLiveContextImpl;

    void init();

    achievements::achievement_service m_achievementService;
    social::profile_service m_profileService;
    social::social_service m_socialService;
    social::reputation_service m_reputationService;
    presence::presence_service m_presenceService;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END


// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>
#if !TV_API

#if !XSAPI_CPP
#include "User_WinRT.h"
#else
#include "xsapi/system.h"
#endif

#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xbox_live_context_impl : public std::enable_shared_from_this < xbox_live_context_impl >
{
public:

#if TV_API | XBOX_UWP
    xbox_live_context_impl(
        _In_ Windows::Xbox::System::User^ user
        );

    /// <summary>
    /// Returns the associated system User 
    /// </summary>
    Windows::Xbox::System::User^ user();

#else
    xbox_live_context_impl(
        _In_ std::shared_ptr<system::xbox_live_user> user
        );
#if XSAPI_CPP
    /// <summary>
    /// Returns the associated system User.
    /// </summary>
    std::shared_ptr<system::xbox_live_user> user();

#else
    xbox_live_context_impl(
        _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user
        );

    /// <summary>
    /// Returns the associated system XboxLiveUser.
    /// </summary>
    Microsoft::Xbox::Services::System::XboxLiveUser^ user();
#endif
#endif

    ~xbox_live_context_impl();

    /// <summary>
    /// Returns the current user context.
    /// </summary>
    std::shared_ptr<user_context> user_context();

    /// <summary>
    /// Returns the current user's Xbox Live User ID.
    /// </summary>
    const string_t& xbox_live_user_id();

    /// <summary>
    /// A service for managing user profiles.
    /// </summary>
    social::profile_service& profile_service();

    /// <summary>
    /// A service for managing social networking links.
    /// </summary>
    social::social_service& social_service();

    /// <summary>
    /// A service for managing reputation reports.
    /// </summary>
    social::reputation_service& reputation_service();

    /// <summary>
    /// A service for managing leaderboards.
    /// </summary>
    leaderboard::leaderboard_service& leaderboard_service();

    /// <summary>
    /// A service for managing achievements.
    /// </summary>
    achievements::achievement_service& achievement_service();

    /// <summary>
    /// A service for managing user statistics.
    /// </summary>
    user_statistics::user_statistics_service&  user_statistics_service();

    /// <summary>
    /// A service for managing multiplayer games.
    /// </summary>
    multiplayer::multiplayer_service& multiplayer_service();

    /// <summary>
    /// A service for managing matchmaking sessions.
    /// </summary>
    matchmaking::matchmaking_service& matchmaking_service();

    /// <summary>
    /// A service for managing tournaments.
    /// </summary>
    tournaments::tournament_service& tournament_service();

    /// <summary>
    /// A service for managing real-time activity.
    /// </summary>
    const std::shared_ptr<real_time_activity::real_time_activity_service>& real_time_activity_service();

    /// <summary>
    /// A service for using the Game Server Platform.
    /// </summary>
    game_server_platform::game_server_platform_service& game_server_platform_service();

    /// <summary>
    /// A service for managing Rich Presence.
    /// </summary>
    presence::presence_service& presence_service();

    /// <summary>
    /// A service for storing data in the cloud.
    /// </summary>
    title_storage::title_storage_service& title_storage_service();

    /// <summary>
    /// A service for managing privacy settings.
    /// </summary>
    privacy::privacy_service& privacy_service();

    /// <summary>
    /// A service for contextual search.
    /// </summary>
    contextual_search::contextual_search_service& contextual_search_service();

    /// <summary>
    /// Returns an object containing Xbox Live app config such as title ID
    /// </summary>
    std::shared_ptr<xbox_live_app_config> application_config();

    /// <summary>
    /// Returns an object containing settings that apply to all REST calls made such as retry and diagnostic settings.
    /// </summary>
    std::shared_ptr<xbox_live_context_settings> settings();

    /// <summary>
    /// A service used to check for offensive strings.
    /// </summary>
    system::string_service& string_service();

#if UWP_API || XSAPI_U
    /// <summary>
    /// A service used to write in game events.
    /// </summary>
    events::events_service& events_service();
#endif

#if TV_API || UNIT_TEST_SERVICES
    /// <summary>
    /// A service for managing catalogs.
    /// </summary>
    marketplace::catalog_service& catalog_service();

    /// <summary>
    /// A service for managing inventory.
    /// </summary>
    marketplace::inventory_service& inventory_service();

    /// <summary>
    /// A service for the entertainment profile.
    /// </summary>
    _XSAPIIMP entertainment_profile::entertainment_profile_list_service& entertainment_profile_list_service();
#endif

    void init();
    void init_real_time_activity_service_instance();

private:
    std::shared_ptr<XBOX_LIVE_NAMESPACE::user_context> m_userContext;
    std::shared_ptr<XBOX_LIVE_NAMESPACE::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox_live_app_config> m_appConfig;

    social::profile_service m_profileService;
    social::social_service m_socialService;
    social::reputation_service m_reputationService;
    leaderboard::leaderboard_service m_leaderboardService;
    achievements::achievement_service m_achievementService;
    user_statistics::user_statistics_service m_userStatisticsService;
    multiplayer::multiplayer_service m_multiplayerService;
    matchmaking::matchmaking_service m_matchmakingService;
    tournaments::tournament_service m_tournamentService;
    std::shared_ptr<real_time_activity::real_time_activity_service> m_realTimeActivityService;
    presence::presence_service m_presenceService;
    game_server_platform::game_server_platform_service m_gameServerPlatformService;
    title_storage::title_storage_service m_titleStorageService;
    privacy::privacy_service m_privacyService;
    contextual_search::contextual_search_service m_contextualSearchService;
    system::string_service m_stringService;

#if UWP_API || XSAPI_U
    events::events_service m_eventsService;
#endif
#if TV_API || UNIT_TEST_SERVICES
    marketplace::catalog_service m_catalogService;
    marketplace::inventory_service m_inventoryService;
    entertainment_profile::entertainment_profile_list_service m_entertainmentProfileService;
#endif

    function_context m_signInContext;
    function_context m_signOutContext;

    friend class xbox_live_context;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
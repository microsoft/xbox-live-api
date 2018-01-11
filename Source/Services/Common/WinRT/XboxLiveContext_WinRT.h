// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#if !TV_API
#include "User_WinRT.h"
#endif
#include "ProfileService_WinRT.h"
#include "SocialService_WinRT.h"
#include "ReputationService_WinRT.h"
#include "AppConfiguration_WinRT.h"
#include "Achievementservice_WinRT.h"
#include "LeaderboardService.h"
#include "UserStatisticsService_WinRT.h"
#include "MultiplayerService_WinRT.h"
#include "MatchmakingService_WinRT.h"
#include "TournamentService_WinRT.h"
#include "XboxLiveContextSettings_WinRT.h"
#include "RealTimeActivityService_WinRT.h"
#include "PresenceService_WinRT.h"
#include "TitleStorageService_WinRT.h"
#include "GameServerPlatformService_WinRT.h"
#include "PrivacyService_WinRT.h"
#include "StringService_WinRT.h"
#include "EventsService_WinRT.h"
#include "ContextualSearchService_WinRT.h"
#include "ClubsService_WinRT.h"
#if TV_API || UNIT_TEST_SERVICES
#include "CatalogService_WinRT.h"
#include "InventoryService_WinRT.h"
#include "EntertainmentProfileListService_WinRT.h"
#endif
#include "local_config.h"

namespace xbox { namespace services { class xbox_live_context; } }

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

/// <summary>
/// Defines methods used to retrieve objects that in turn provide access to different Xbox Live service components.
/// 
/// Note: the XboxLiveContext is unique per instance. Changing state on one instance for a user does not affect a second instance of the context for the same user. Using multiple instances can therefore result in unexpected behavior. Titles should ensure to only use one instance of the XboxLiveContext per user.
/// </summary> 
public ref class XboxLiveContext sealed
{
public:

#if TV_API
    XboxLiveContext(_In_ Windows::Xbox::System::User^ user);

    /// <summary>
    /// Returns the associated system user.
    /// </summary>
    property Windows::Xbox::System::User^ User { Windows::Xbox::System::User^ get(); }

#else
    XboxLiveContext(_In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user);

    /// <summary>
    /// Returns the associated system user. 
    /// </summary>
    property Microsoft::Xbox::Services::System::XboxLiveUser^ User { Microsoft::Xbox::Services::System::XboxLiveUser^ get(); }

#endif

    /// <summary>
    /// Returns object containing access methods to the Xbox Profile service.
    /// </summary>
    property Social::ProfileService^ ProfileService { Social::ProfileService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Xbox Social service.
    /// </summary>
    property Social::SocialService^ SocialService { Social::SocialService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Xbox Reputation service.
    /// </summary>
    property Social::ReputationService^ ReputationService { Social::ReputationService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Xbox Achievement service.
    /// </summary>
    property Achievements::AchievementService^ AchievementService { Achievements::AchievementService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Xbox Leaderboard service.
    /// </summary>
    property Leaderboard::LeaderboardService^ LeaderboardService { Leaderboard::LeaderboardService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Xbox User Stats service.
    /// </summary>
    property UserStatistics::UserStatisticsService^ UserStatisticsService { UserStatistics::UserStatisticsService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Xbox Matchmaking service.
    /// </summary>
    property Matchmaking::MatchmakingService^ MatchmakingService { Matchmaking::MatchmakingService^ get(); }
    
    /// <summary>
    /// Returns object containing access methods to the Xbox Multiplayer service.
    /// </summary>
    property Multiplayer::MultiplayerService^ MultiplayerService { Multiplayer::MultiplayerService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Xbox Tournament service.
    /// </summary>
    property Tournaments::TournamentService^ TournamentService { Tournaments::TournamentService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Xbox Real Time Activity service.
    /// </summary>
    property RealTimeActivity::RealTimeActivityService^ RealTimeActivityService { RealTimeActivity::RealTimeActivityService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Presence service.
    /// </summary>
    property Presence::PresenceService^ PresenceService { Presence::PresenceService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the GameServerPlatform service.
    /// </summary>
    property GameServerPlatform::GameServerPlatformService^ GameServerPlatformService { GameServerPlatform::GameServerPlatformService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the TitleStorage service.
    /// </summary>
    property TitleStorage::TitleStorageService^ TitleStorageService{ TitleStorage::TitleStorageService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Privacy service.
    /// </summary>
    property Privacy::PrivacyService^ PrivacyService { Privacy::PrivacyService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Xbox String service.
    /// </summary>
    property System::StringService^ StringService { System::StringService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the contextual search service.
    /// </summary>
    property ContextualSearch::ContextualSearchService^ ContextualSearchService { ContextualSearch::ContextualSearchService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the title clubs service.
    /// </summary>
    property Clubs::ClubsService^ ClubsService { Clubs::ClubsService^ get(); }

#if UWP_API
    /// <summary>
    /// Returns object containing access methods to the events service.
    /// </summary>
    property Events::EventsService^ EventsService { Events::EventsService^ get(); }
#endif

#if TV_API || UNIT_TEST_SERVICES
    /// <summary>
    /// Returns object containing access methods to the Catalog service.
    /// </summary>
    property Marketplace::CatalogService^ CatalogService { Marketplace::CatalogService^ get(); }

    /// <summary>
    /// Returns object containing access methods to the Inventory service.
    /// </summary>
    property Marketplace::InventoryService^ InventoryService { Marketplace::InventoryService^ get(); }
    
    /// <summary>
    /// Returns object containing access methods to the Xbox String service.
    /// </summary>
    property EntertainmentProfile::EntertainmentProfileListService^ EntertainmentProfileListService { EntertainmentProfile::EntertainmentProfileListService^ get(); }
#endif // #if TV_API || UNIT_TEST_SERVICES

    /// <summary>
    /// Returns an object containing settings that apply to all REST calls made such as retry and diagnostic settings.
    /// </summary>
    property Microsoft::Xbox::Services::XboxLiveContextSettings^ Settings { Microsoft::Xbox::Services::XboxLiveContextSettings^ get(); }

    /// <summary>
    /// Returns an object containing Xbox Live app config such as title ID
    /// </summary>
    property Microsoft::Xbox::Services::XboxLiveAppConfiguration^ AppConfig { Microsoft::Xbox::Services::XboxLiveAppConfiguration^ get(); }

internal:
    std::shared_ptr<xbox::services::xbox_live_context> GetCppObj() const;

private:
    void Initialize();

    std::shared_ptr<xbox::services::xbox_live_context> m_cppObj;
    Microsoft::Xbox::Services::XboxLiveContextSettings^ m_xboxLiveContextSettings;
    Microsoft::Xbox::Services::XboxLiveAppConfiguration^ m_appConfig;
    Social::ProfileService^ m_profileService;
    Social::SocialService^ m_socialService;
    Social::ReputationService^ m_reputationService;
    Achievements::AchievementService^ m_achievementService;
    Leaderboard::LeaderboardService^ m_leaderboardService;
    UserStatistics::UserStatisticsService^ m_userStatisticsService;
    Multiplayer::MultiplayerService^ m_multiplayerService;
    Matchmaking::MatchmakingService^ m_matchmakingService;
    Tournaments::TournamentService^ m_tournamentService;
    RealTimeActivity::RealTimeActivityService^ m_realTimeActivityService;
    Presence::PresenceService^ m_presenceService;
    GameServerPlatform::GameServerPlatformService^ m_gameServerPlatformService;
    TitleStorage::TitleStorageService^ m_titleStorageService;
    Privacy::PrivacyService^ m_privacyService;
    ContextualSearch::ContextualSearchService^ m_contextualSearchService;
    System::StringService^ m_stringService;
    Clubs::ClubsService^ m_clubsService;
#if UWP_API
    Events::EventsService^ m_eventsService;
#endif
#if TV_API || UNIT_TEST_SERVICES
    Marketplace::CatalogService^ m_catalogService;
    Marketplace::InventoryService^ m_inventoryService;
    EntertainmentProfile::EntertainmentProfileListService^ m_entertainmentProfileListService;
#endif

    function_context m_signInContext;
    function_context m_signOutContext;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END

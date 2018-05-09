// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/services.h"
#include "../Common/xbox_live_context_impl.h"
#include "XboxLiveContext_WinRT.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#if !TV_API
#include "User_WinRT.h"
#endif
#include "xsapi/profile.h"
#include "xsapi/contextual_search_service.h"
#include "xsapi/xbox_live_app_config.h"
#if !TV_API
#include "notification_service.h"
#endif
#include "presence_internal.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;


NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

#if TV_API
XboxLiveContext::XboxLiveContext(
    _In_ Windows::Xbox::System::User^ user
    ) 
{
    m_cppObj = std::make_shared<xbox::services::xbox_live_context>(user);
    m_cppObj->_User_context()->set_caller_api_type(xbox::services::caller_api_type::api_winrt);

    Initialize();
}


Windows::Xbox::System::User^
XboxLiveContext::User::get()
{
    return m_cppObj->user();
}

#else
XboxLiveContext::XboxLiveContext(
    _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user
    ) 
{
    m_cppObj = std::make_shared<xbox::services::xbox_live_context>(user);
    m_cppObj->_User_context()->set_caller_api_type(xbox::services::caller_api_type::api_winrt);

    Initialize();
}

Microsoft::Xbox::Services::System::XboxLiveUser^
XboxLiveContext::User::get()
{
    return m_cppObj->user();
}
#endif

void
XboxLiveContext::Initialize()
{
    m_xboxLiveContextSettings = ref new Microsoft::Xbox::Services::XboxLiveContextSettings(m_cppObj->settings());

    auto appConfig = xbox::services::xbox_live_app_config::get_app_config_singleton();

    m_appConfig = ref new Microsoft::Xbox::Services::XboxLiveAppConfiguration(appConfig);

    m_profileService = ref new Microsoft::Xbox::Services::Social::ProfileService(
        m_cppObj->profile_service()
        );

    m_reputationService = ref new Microsoft::Xbox::Services::Social::ReputationService(
        m_cppObj->reputation_service()
        );

    m_achievementService = ref new Microsoft::Xbox::Services::Achievements::AchievementService(
        m_cppObj->achievement_service()
        );

    m_leaderboardService = ref new Leaderboard::LeaderboardService(
        m_cppObj->leaderboard_service()
        );

    m_realTimeActivityService = ref new RealTimeActivity::RealTimeActivityService(
        m_cppObj->real_time_activity_service()
        );

    m_multiplayerService = ref new Multiplayer::MultiplayerService(
        m_cppObj->multiplayer_service()
        );

    m_tournamentService = ref new Tournaments::TournamentService(
        m_cppObj->tournament_service()
        );

    m_userStatisticsService = ref new UserStatistics::UserStatisticsService(
        m_cppObj->user_statistics_service()
        );

    m_matchmakingService = ref new Matchmaking::MatchmakingService(
        m_cppObj->matchmaking_service()
        );

    m_presenceService = ref new Presence::PresenceService(
        m_cppObj->presence_service()
        );

    m_gameServerPlatformService = ref new GameServerPlatform::GameServerPlatformService(
        m_cppObj->game_server_platform_service()
        );

    m_titleStorageService = ref new TitleStorage::TitleStorageService(
        m_cppObj->title_storage_service()
        );

    m_privacyService = ref new Privacy::PrivacyService(
        m_cppObj->privacy_service()
        );

    m_socialService = ref new Microsoft::Xbox::Services::Social::SocialService(
        m_cppObj->social_service()
        );

#if UWP_API
    m_eventsService = ref new Events::EventsService(
        m_cppObj->events_service()
        );
#endif

#if TV_API || UNIT_TEST_SERVICES
    m_catalogService = ref new Marketplace::CatalogService(
        m_cppObj->catalog_service()
        );

    m_inventoryService = ref new Marketplace::InventoryService(
        m_cppObj->inventory_service()
        );

    m_entertainmentProfileListService = ref new Microsoft::Xbox::Services::EntertainmentProfile::EntertainmentProfileListService(
        m_cppObj->entertainment_profile_list_service()
        );
#endif
    m_stringService = ref new System::StringService(
        m_cppObj->string_service()
        );

    m_contextualSearchService = ref new ContextualSearch::ContextualSearchService(
        m_cppObj->contextual_search_service()
        );

    m_clubsService = ref new Clubs::ClubsService(
        m_cppObj->clubs_service()
        );
}

Social::ProfileService^ 
XboxLiveContext::ProfileService::get()
{ 
    return m_profileService;
}

Social::SocialService^
XboxLiveContext::SocialService::get()
{
    return m_socialService;
}

Social::ReputationService^ 
XboxLiveContext::ReputationService::get()
{ 
    return m_reputationService;
}

Achievements::AchievementService^
XboxLiveContext::AchievementService::get()
{
    return m_achievementService;
}

Leaderboard::LeaderboardService^
XboxLiveContext::LeaderboardService::get()
{
    return m_leaderboardService;
}

UserStatistics::UserStatisticsService^
XboxLiveContext::UserStatisticsService::get()
{
    return m_userStatisticsService;
}

Multiplayer::MultiplayerService^ 
XboxLiveContext::MultiplayerService::get()
{
    return m_multiplayerService;
}

Matchmaking::MatchmakingService^
XboxLiveContext::MatchmakingService::get()
{
    return m_matchmakingService;
}

Tournaments::TournamentService^
XboxLiveContext::TournamentService::get()
{
    return m_tournamentService;
}

RealTimeActivity::RealTimeActivityService^
XboxLiveContext::RealTimeActivityService::get()
{
    return m_realTimeActivityService;
}

Presence::PresenceService^
XboxLiveContext::PresenceService::get()
{
    return m_presenceService;
}

GameServerPlatform::GameServerPlatformService^
XboxLiveContext::GameServerPlatformService::get()
{
    return m_gameServerPlatformService;
}

TitleStorage::TitleStorageService^
XboxLiveContext::TitleStorageService::get()
{
    return m_titleStorageService;
}

Privacy::PrivacyService^
XboxLiveContext::PrivacyService::get()
{
    return m_privacyService;
}

System::StringService^
XboxLiveContext::StringService::get()
{
    return m_stringService;
}

ContextualSearch::ContextualSearchService^
XboxLiveContext::ContextualSearchService::get()
{
    return m_contextualSearchService;
}

Clubs::ClubsService^
XboxLiveContext::ClubsService::get()
{
    return m_clubsService;
}

#if UWP_API
Events::EventsService^
XboxLiveContext::EventsService::get()
{
    return m_eventsService;
}
#endif

#if TV_API || UNIT_TEST_SERVICES
Marketplace::CatalogService^
XboxLiveContext::CatalogService::get()
{
    return m_catalogService;
}

Marketplace::InventoryService^
XboxLiveContext::InventoryService::get()
{
    return m_inventoryService;
}

EntertainmentProfile::EntertainmentProfileListService^
XboxLiveContext::EntertainmentProfileListService::get()
{
    return m_entertainmentProfileListService;
}
#endif

Microsoft::Xbox::Services::XboxLiveContextSettings^
XboxLiveContext::Settings::get()
{ 
    return m_xboxLiveContextSettings;
}

Microsoft::Xbox::Services::XboxLiveAppConfiguration^
XboxLiveContext::AppConfig::get()
{ 
    return m_appConfig;
}

std::shared_ptr<xbox::services::xbox_live_context>
XboxLiveContext::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_END
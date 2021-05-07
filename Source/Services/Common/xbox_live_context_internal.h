// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>

#include "achievements_internal.h"
#include "multiplayer_internal.h"
#include "matchmaking_internal.h"
#include "privacy_service_internal.h"
#include "user_statistics_internal.h"
#include "title_managed_statistics_internal.h"
#include "leaderboard_internal.h"
#include "events_service.h"
#include "presence_internal.h"
#include "profile_internal.h"
#include "title_storage_internal.h"
#include "string_service_internal.h"
#include "social_internal.h"
#include "multiplayer_activity_internal.h"
#if XSAPI_NOTIFICATION_SERVICE
#include "notification_internal.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_BEGIN
namespace legacy
{
    class matchmaking_service;
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_CPP_END

struct XblContext : public std::enable_shared_from_this<XblContext>, public xbox::services::RefCounter
{
public:
    static std::shared_ptr<XblContext> Make(
        _In_ xbox::services::User&&  user
    ) noexcept;

    ~XblContext();

    const xbox::services::User& User() const noexcept;
    uint64_t Xuid() const;

    /// <summary>
    /// A service for managing user profiles.
    /// </summary>
    std::shared_ptr<xbox::services::social::ProfileService> ProfileService();

    /// <summary>
    /// A service for managing social networking links.
    /// </summary>
    std::shared_ptr<xbox::services::social::SocialService> SocialService();

    /// <summary>
    /// A service for managing reputation reports.
    /// </summary>
    std::shared_ptr<xbox::services::social::ReputationService> ReputationService();

    /// <summary>
    /// A service for managing achievements.
    /// </summary>
    std::shared_ptr<xbox::services::achievements::AchievementsService> AchievementsService();

    /// <summary>
    /// A service for managing multiplayer games.
    /// </summary>
    std::shared_ptr<xbox::services::multiplayer::MultiplayerService> MultiplayerService();


    /// <summary>
    /// A service for managing Matchmaking Requests.
    /// </summary>
    std::shared_ptr<xbox::services::matchmaking::MatchmakingService> MatchmakingService();

    /// <summary>
    /// A service for managing privacy settings.
    /// </summary>
    std::shared_ptr<xbox::services::privacy::PrivacyService> PrivacyService();

    /// <summary>
    /// A service for verifying strings.
    /// </summary>
    std::shared_ptr<xbox::services::system::StringService> StringService();

    /// <summary>
    /// A service for managing document statistics.
    /// </summary>
    std::shared_ptr<xbox::services::user_statistics::TitleManagedStatisticsService> TitleManagedStatisticsService();

    /// <summary>
    /// A service for managing user statistics.
    /// </summary>
    std::shared_ptr<xbox::services::user_statistics::UserStatisticsService> UserStatisticsService();

    /// <summary>
    /// A service for managing leaderboards.
    /// </summary>
    std::shared_ptr<xbox::services::leaderboard::LeaderboardService> LeaderboardService();

    /// <summary>
    /// A service for managing Rich Presence.
    /// </summary>
    std::shared_ptr<xbox::services::presence::PresenceService> PresenceService();

#if XSAPI_NOTIFICATION_SERVICE
    /// <summary>
    /// A service used for delivering notifications.
    /// </summary>
    std::shared_ptr<xbox::services::notification::NotificationService> NotificationService();
#endif

    /// <summary>
    /// A service for storing data in the cloud.
    /// </summary>
    std::shared_ptr<xbox::services::title_storage::TitleStorageService> TitleStorageService();

    /// <summary>
    /// Service for tracking multiplayer activity, recent players, and sending multiplayer invites.
    /// </summary>
    std::shared_ptr<xbox::services::multiplayer_activity::MultiplayerActivityService> MultiplayerActivityService() noexcept;

#if XSAPI_EVENTS_SERVICE
    /// <summary>
    /// A service used to write in game events.
    /// </summary>
    std::shared_ptr<xbox::services::events::IEventsService> EventsService();
#endif

    std::shared_ptr<xbox::services::AppConfig> ApplicationConfig() const;

    /// <summary>
    /// Returns an object containing settings that apply to all REST calls made such as retry and diagnostic settings.
    /// </summary>
    std::shared_ptr<xbox::services::XboxLiveContextSettings> Settings();

    HRESULT Initialize(std::shared_ptr<real_time_activity::RealTimeActivityManager> rtaManager);

protected:
    std::shared_ptr<xbox::services::RefCounter> GetSharedThis() override;
    XblContext(_In_ xbox::services::User&& user) noexcept
        : m_user{ std::move(user) }
    {}

private:
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;

    std::shared_ptr<xbox::services::achievements::AchievementsService> m_achievementService;
    std::shared_ptr<xbox::services::social::ProfileService> m_profileService;
    std::shared_ptr<xbox::services::social::ReputationService> m_reputationServiceImpl;
    std::shared_ptr<xbox::services::social::SocialService> m_socialService;
    std::shared_ptr<xbox::services::system::StringService> m_stringService;
    std::shared_ptr<xbox::services::multiplayer::MultiplayerService> m_multiplayerService;
    std::shared_ptr<xbox::services::matchmaking::MatchmakingService> m_matchmakingService;
    std::shared_ptr<xbox::services::privacy::PrivacyService> m_privacyService;
    std::shared_ptr<xbox::services::user_statistics::TitleManagedStatisticsService> m_titleManagedStatisticsService;
    std::shared_ptr<xbox::services::user_statistics::UserStatisticsService> m_userStatisticsService;
    std::shared_ptr<xbox::services::leaderboard::LeaderboardService> m_leaderboardService;
    std::shared_ptr<xbox::services::title_storage::TitleStorageService> m_titleStorageService;
    std::shared_ptr<xbox::services::multiplayer_activity::MultiplayerActivityService> m_multiplayerActivityService;

    std::shared_ptr<xbox::services::presence::PresenceService> m_presenceService;
#if !XSAPI_UNIT_TESTS && (HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL)
    std::shared_ptr<xbox::services::notification::RTANotificationService> m_notificationService;
#elif HC_PLATFORM == HC_PLATFORM_UWP
    std::shared_ptr<xbox::services::notification::UWPNotificationService> m_notificationService;
#elif HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM == HC_PLATFORM_IOS
    std::shared_ptr<xbox::services::notification::MobileNotificationService> m_notificationService;
#elif XSAPI_NOTIFICATION_SERVICE
    std::shared_ptr<xbox::services::notification::NotificationService> m_notificationService;
#endif

#if XSAPI_EVENTS_SERVICE
    std::shared_ptr<xbox::services::events::IEventsService> m_eventsService;
#endif

    uint64_t m_userChangeEventToken{ 0 };
    uint64_t m_xuid{ 0 };

    xbox::services::User m_user;
};
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xbox_system_factory.h"
#include "xbox_live_context_internal.h"
#include "presence_internal.h"

#include "Achievements/achievements_internal.h"
#include "profile_internal.h"
#include "social_internal.h"
#include "string_service_internal.h"
#include "multiplayer_internal.h"
#include "xbox_live_app_config_internal.h"
#include "xbox_live_context_settings_internal.h"
#include "title_storage_internal.h"
#include "notification_internal.h"

using namespace xbox::services;
using namespace xbox::services::system;

XblContext::XblContext(_In_ xbox::services::User user) noexcept
    : m_user{ std::move(user) }
{
}

const xbox::services::User& XblContext::User() const noexcept
{
    return m_user;
}

XblContext::~XblContext()
{
    if (m_userChangeEventToken)
    {
        User::UnregisterChangeEventHandle(m_userChangeEventToken);
    }
}

std::shared_ptr<xbox::services::RefCounter> XblContext::GetSharedThis()
{
    return shared_from_this();
}

HRESULT XblContext::Initialize(
    std::shared_ptr<real_time_activity::RealTimeActivityManager> rtaManager
)
{
    m_xboxLiveContextSettings = MakeShared<xbox::services::XboxLiveContextSettings>();

    std::weak_ptr<XblContext> thisWeakPtr = shared_from_this();

    m_achievementService = MakeShared<xbox::services::achievements::AchievementsService>(m_user, m_xboxLiveContextSettings, AppConfig::Instance(), thisWeakPtr, rtaManager);
    m_profileService = MakeShared<xbox::services::social::ProfileService>(m_user, m_xboxLiveContextSettings, AppConfig::Instance());
    m_reputationServiceImpl = MakeShared<xbox::services::social::ReputationService>(m_user, m_xboxLiveContextSettings);
    m_presenceService = MakeShared<xbox::services::presence::PresenceService>(m_user, m_xboxLiveContextSettings, rtaManager);
    m_socialService = MakeShared<xbox::services::social::SocialService>(m_user, m_xboxLiveContextSettings, rtaManager);
    m_stringService = MakeShared<xbox::services::system::StringService>(m_user, m_xboxLiveContextSettings);
    m_multiplayerService = MakeShared<xbox::services::multiplayer::MultiplayerService>(m_user, m_xboxLiveContextSettings, AppConfig::Instance(), rtaManager);
    m_privacyService = MakeShared<privacy::PrivacyService>(m_user, m_xboxLiveContextSettings);
    m_titleManagedStatisticsService = MakeShared<xbox::services::user_statistics::TitleManagedStatisticsService>(m_user, m_xboxLiveContextSettings);
    m_userStatisticsService = MakeShared<xbox::services::user_statistics::UserStatisticsService>(m_user, m_xboxLiveContextSettings, rtaManager);
    m_leaderboardService = MakeShared<xbox::services::leaderboard::LeaderboardService>(m_user, m_xboxLiveContextSettings, AppConfig::Instance());
    m_matchmakingService = MakeShared<xbox::services::matchmaking::MatchmakingService>(m_user, m_xboxLiveContextSettings);
    m_titleStorageService = MakeShared<xbox::services::title_storage::TitleStorageService>(m_user, m_xboxLiveContextSettings);
    m_multiplayerActivityService = MakeShared<multiplayer_activity::MultiplayerActivityService>(m_user, get_xsapi_singleton_async_queue(), m_xboxLiveContextSettings);

#if XSAPI_EVENTS_SERVICE
    m_eventsService = MakeShared<events::EventsService>(
        m_user
#if XSAPI_INTERNAL_EVENTS_SERVICE
        , get_xsapi_singleton_async_queue()
#endif
    );
    HRESULT hr = m_eventsService->Initialize();
    RETURN_HR_IF_FAILED(hr);
#endif

#if XSAPI_NOTIFICATION_SERVICE
#if !XSAPI_UNIT_TESTS && (HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL)
	m_notificationService = MakeShared<xbox::services::notification::RTANotificationService>(m_user, m_xboxLiveContextSettings, rtaManager);
    m_notificationService->RegisterForSpopNotificationEvents();
#elif HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM == HC_PLATFORM_IOS
	m_notificationService = MakeShared<xbox::services::notification::MobileNotificationService>(m_user, m_xboxLiveContextSettings);
#elif HC_PLATFORM == HC_PLATFORM_UWP
	m_notificationService = MakeShared<xbox::services::notification::UWPNotificationService>(m_user, m_xboxLiveContextSettings);
	m_notificationService->RegisterWithNotificationService(AsyncContext<HRESULT>{TaskQueue{ get_xsapi_singleton_async_queue() }});
#endif
#endif

    auto userChangedRegistrationResult = User::RegisterChangeEventHandler(
        [
            thisWeakPtr
        ]
    (UserLocalId localId, UserChangeType changeType)
    {
        auto sharedThis{ thisWeakPtr.lock() };
        if (sharedThis)
        {
            if (sharedThis->m_user.LocalId() == localId.value && changeType == XalUserChange_SignedOut)
            {
#if XSAPI_NOTIFICATION_SERVICE
                sharedThis->NotificationService()->UnregisterFromNotificationService(
                    AsyncContext<HRESULT> 
                    { 
                        TaskQueue{ get_xsapi_singleton_async_queue() }
                    });
#endif
            }
        }
    });

    return userChangedRegistrationResult.Hresult();
}

uint64_t XblContext::Xuid() const
{
    return m_user.Xuid();
}

std::shared_ptr<social::ProfileService> XblContext::ProfileService()
{
    return m_profileService;
}

std::shared_ptr<social::SocialService> XblContext::SocialService()
{
    return m_socialService;
}

std::shared_ptr<social::ReputationService> XblContext::ReputationService()
{
    return m_reputationServiceImpl;
}

std::shared_ptr<achievements::AchievementsService> XblContext::AchievementsService()
{
    return m_achievementService;
}

std::shared_ptr<multiplayer::MultiplayerService> XblContext::MultiplayerService()
{
    return m_multiplayerService;
}

std::shared_ptr<xbox::services::system::StringService> XblContext::StringService()
{
    return m_stringService;
}

std::shared_ptr<matchmaking::MatchmakingService> XblContext::MatchmakingService()
{
    return m_matchmakingService;
}

std::shared_ptr<privacy::PrivacyService> XblContext::PrivacyService()
{
    return m_privacyService;
}

std::shared_ptr<user_statistics::TitleManagedStatisticsService> XblContext::TitleManagedStatisticsService()
{
    return m_titleManagedStatisticsService;
}

std::shared_ptr<user_statistics::UserStatisticsService> XblContext::UserStatisticsService()
{
    return m_userStatisticsService;
}

std::shared_ptr<leaderboard::LeaderboardService> XblContext::LeaderboardService()
{
    return m_leaderboardService;
}

std::shared_ptr<title_storage::TitleStorageService> XblContext::TitleStorageService()
{
    return m_titleStorageService;
}

#if XSAPI_EVENTS_SERVICE
std::shared_ptr<events::IEventsService> XblContext::EventsService()
{
    return m_eventsService;
}
#endif 

std::shared_ptr<presence::PresenceService> XblContext::PresenceService()
{
    return m_presenceService;
}

std::shared_ptr<notification::NotificationService> XblContext::NotificationService()
{
    return m_notificationService;
}

std::shared_ptr<multiplayer_activity::MultiplayerActivityService> XblContext::MultiplayerActivityService() noexcept
{
    return m_multiplayerActivityService;
}

std::shared_ptr<XboxLiveContextSettings> XblContext::Settings()
{
    return m_xboxLiveContextSettings;
}

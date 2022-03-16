// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
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
#if XSAPI_NOTIFICATION_SERVICE
#include "notification_internal.h"
#endif

using namespace xbox::services;
using namespace xbox::services::system;

/*static*/ std::shared_ptr<XblContext> XblContext::Make(
    _In_ xbox::services::User&&  user
) noexcept
{
    auto xblContext = std::shared_ptr<XblContext>(
        new (Alloc(sizeof(XblContext))) XblContext{ std::move(user) },
        Deleter<XblContext>(),
        Allocator<XblContext>()
        );
    return xblContext;
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
    TaskQueue globalQueue;
    {
        auto state = GlobalState::Get();
        if (state)
        {
            globalQueue = state->Queue();
        }
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_achievementService = MakeShared<xbox::services::achievements::AchievementsService>(userResult.ExtractPayload(), m_xboxLiveContextSettings, AppConfig::Instance(), thisWeakPtr, rtaManager);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_profileService = MakeShared<xbox::services::social::ProfileService>(userResult.ExtractPayload(), m_xboxLiveContextSettings, AppConfig::Instance());
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_reputationServiceImpl = MakeShared<xbox::services::social::ReputationService>(userResult.ExtractPayload(), m_xboxLiveContextSettings);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_presenceService = MakeShared<xbox::services::presence::PresenceService>(userResult.ExtractPayload(), globalQueue, m_xboxLiveContextSettings, rtaManager);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_socialService = MakeShared<xbox::services::social::SocialService>(userResult.ExtractPayload(), m_xboxLiveContextSettings, rtaManager);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_stringService = MakeShared<xbox::services::system::StringService>(userResult.ExtractPayload(), m_xboxLiveContextSettings);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_multiplayerService = MakeShared<xbox::services::multiplayer::MultiplayerService>(userResult.ExtractPayload(), m_xboxLiveContextSettings, AppConfig::Instance(), rtaManager);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_privacyService = MakeShared<privacy::PrivacyService>(userResult.ExtractPayload(), m_xboxLiveContextSettings);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_titleManagedStatisticsService = MakeShared<xbox::services::user_statistics::TitleManagedStatisticsService>(userResult.ExtractPayload(), m_xboxLiveContextSettings);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_userStatisticsService = MakeShared<xbox::services::user_statistics::UserStatisticsService>(userResult.ExtractPayload(), globalQueue, m_xboxLiveContextSettings, rtaManager);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_leaderboardService = MakeShared<xbox::services::leaderboard::LeaderboardService>(userResult.ExtractPayload(), m_xboxLiveContextSettings, AppConfig::Instance());
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_matchmakingService = MakeShared<xbox::services::matchmaking::MatchmakingService>(userResult.ExtractPayload(), m_xboxLiveContextSettings);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_titleStorageService = MakeShared<xbox::services::title_storage::TitleStorageService>(userResult.ExtractPayload(), m_xboxLiveContextSettings);
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
        m_multiplayerActivityService = MakeShared<multiplayer_activity::MultiplayerActivityService>(userResult.ExtractPayload(), globalQueue, m_xboxLiveContextSettings);
    }
    
    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
#if XSAPI_EVENTS_SERVICE
        m_eventsService = MakeShared<events::EventsService>(
            userResult.ExtractPayload()
#if XSAPI_INTERNAL_EVENTS_SERVICE
            , globalQueue
#endif
            );
        RETURN_HR_IF_FAILED(m_eventsService->Initialize());
#endif
    }

    {
        Result<xbox::services::User> userResult = m_user.Copy();
        RETURN_HR_IF_FAILED(userResult.Hresult());
#if XSAPI_NOTIFICATION_SERVICE
#if !XSAPI_UNIT_TESTS && (HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL)
        m_notificationService = MakeShared<xbox::services::notification::RTANotificationService>(userResult.ExtractPayload(), globalQueue, m_xboxLiveContextSettings, rtaManager);
        RETURN_HR_IF_FAILED(m_notificationService->Initialize());
#elif HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM == HC_PLATFORM_IOS
        m_notificationService = MakeShared<xbox::services::notification::MobileNotificationService>(userResult.ExtractPayload(), m_xboxLiveContextSettings);
#elif HC_PLATFORM == HC_PLATFORM_UWP
        m_notificationService = MakeShared<xbox::services::notification::UWPNotificationService>(userResult.ExtractPayload(), m_xboxLiveContextSettings);
        m_notificationService->RegisterWithNotificationService(AsyncContext<HRESULT>{ globalQueue });
#endif
#endif
    }

#if XSAPI_NOTIFICATION_SERVICE
    auto userChangedRegistrationResult = User::RegisterChangeEventHandler(
        [
            thisWeakPtr,
            queue{ globalQueue.DeriveWorkerQueue() }
        ]
    (UserLocalId localId, UserChangeType changeType)
    {
        auto sharedThis{ thisWeakPtr.lock() };
        if (sharedThis)
        {
            if (sharedThis->m_user.LocalId() == localId.value && changeType == XalUserChange_SignedOut)
            {
                sharedThis->NotificationService()->UnregisterFromNotificationService(AsyncContext<HRESULT>{ queue });
            }
        }
    });

    RETURN_HR_IF_FAILED(userChangedRegistrationResult.Hresult());
    m_userChangeEventToken = userChangedRegistrationResult.Payload();
#endif

    return S_OK;
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

#if XSAPI_NOTIFICATION_SERVICE
std::shared_ptr<notification::NotificationService> XblContext::NotificationService()
{
    return m_notificationService;
}
#endif

std::shared_ptr<multiplayer_activity::MultiplayerActivityService> XblContext::MultiplayerActivityService() noexcept
{
    return m_multiplayerActivityService;
}

std::shared_ptr<XboxLiveContextSettings> XblContext::Settings()
{
    return m_xboxLiveContextSettings;
}

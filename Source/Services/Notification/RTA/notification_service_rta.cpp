// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "notification_internal.h"
#include "real_time_activity_manager.h"
#include "multiplayer_internal.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

RTANotificationService::RTANotificationService(
    _In_ User&& user,
    _In_ const TaskQueue& queue,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings,
    _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
) noexcept :
    NotificationService(std::move(user), contextSettings),
    m_taskQueue{ queue.DeriveWorkerQueue() },
    m_rtaManager{ std::move(rtaManager) }
{
}

RTANotificationService::~RTANotificationService() noexcept
{
    m_rtaManager->RemoveSubscription(m_user, m_rtaSubscription);
    m_rtaManager->Deactivate(m_user);
}

HRESULT RTANotificationService::Initialize() noexcept
{
    // Always register with notification service as it powers SPOP for Win32

    // Subscribing to events requires two separate steps:
    // 1) Creating and adding an RTA subscription to the notification endpoint
    // 2) Registering with the notification service

    std::lock_guard<std::recursive_mutex> lock{ m_mutex };

    auto copyUserResult = m_user.Copy();
    RETURN_HR_IF_FAILED(copyUserResult.Hresult());

    m_rtaManager->Activate(m_user);
    m_rtaSubscription = MakeShared<NotificationSubscription>(copyUserResult.ExtractPayload(), m_taskQueue, AppConfig::Instance()->TitleId());
    RETURN_HR_IF_FAILED(m_rtaManager->AddSubscription(m_user, m_rtaSubscription));

    return RegisterWithNotificationService(
        m_rtaSubscription->ResourceUri(),
        AsyncContext<HRESULT>{ m_taskQueue,
        [](HRESULT hr)
        {
            if (FAILED(hr))
            {
                LOGS_ERROR << "Failed to register with Notification Service hr=" << std::hex << hr;
            }
        }
    });
}

HRESULT RTANotificationService::RegisterWithNotificationService(
    _In_ const String& uriData,
    _In_ AsyncContext<HRESULT> async
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };

    Stringstream titleId;
    titleId << AppConfig::Instance()->TitleId();

    const char platform[]{
#if HC_PLATFORM == HC_PLATFORM_WIN32
    "Win32",
#elif HC_PLATFORM == HC_PLATFORM_NINTENDO_SWITCH
    "NintendoSwitch",
#endif
    };

    Vector<NotificationFilter> notificationFilterList;
    notificationFilterList.push_back({ NotificationTypeFilterSourceType::Multiplayer, 1 });
    notificationFilterList.push_back({ NotificationTypeFilterSourceType::Multiplayer, 8 });
    notificationFilterList.push_back({ NotificationTypeFilterSourceType::Achievements, 1 });

    return NotificationService::RegisterForNotificationsHelper(
        utils::create_guid(true), // applicationInstanceId
        uriData,
        platform,
        "",
        "",
        notificationFilterList,
        std::move(async)
    );
}

XblFunctionContext RTANotificationService::AddGameInviteHandler(
    _In_ NotificationSubscription::MPSDInviteHandler handler
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    assert(m_rtaSubscription);
    return m_rtaSubscription->AddHandler(std::move(handler));
}

XblFunctionContext RTANotificationService::AddGameInviteHandler(
    _In_ NotificationSubscription::MultiplayerActivityInviteHandler handler
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    assert(m_rtaSubscription);
    return m_rtaSubscription->AddHandler(std::move(handler));
}

XblFunctionContext RTANotificationService::AddAchievementUnlockNotificationHandler(
    _In_ NotificationSubscription::AchievementUnlockHandler handler
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    assert(m_rtaSubscription);
    return m_rtaSubscription->AddHandler(std::move(handler));
}

void RTANotificationService::RemoveNotificationHandler(
    _In_ XblFunctionContext token
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    m_rtaSubscription->RemoveHandler(token);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
#endif

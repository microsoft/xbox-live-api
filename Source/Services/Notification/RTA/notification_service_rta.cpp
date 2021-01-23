// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "notification_internal.h"
#include "real_time_activity_manager.h"
#include "multiplayer_internal.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

template<typename T>
HRESULT RTANotificationService::RegisterForEvents(std::shared_ptr<T>&        subscription,
    AsyncContext<HRESULT> async) noexcept
{
    // Subscribing to events requires two separate steps:
    // 1) Creating and adding an RTA subscription to the notification endpoint
    // 2) Registering with the notification service

    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    if (!subscription)
    {
        m_rtaManager->Activate(m_user);
        subscription = MakeShared<T>(m_user.Xuid(), AppConfig::Instance()->TitleId());
        RETURN_HR_IF_FAILED(m_rtaManager->AddSubscription(m_user, subscription));
    }

    return RegisterWithNotificationService(
        subscription->ResourceUri(),
        std::move(async));
};


template<typename T>
HRESULT RTANotificationService::RTANotificationService::UnregisterForEvents(std::shared_ptr<T>&        subscription,
    AsyncContext<HRESULT> async) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    if (subscription)
    {
        m_rtaManager->RemoveSubscription(m_user, subscription);
        subscription.reset();
        m_rtaManager->Deactivate(m_user);
    }

    if (!m_gameInviteSubscription && !m_achievementUnlockSubscription && !m_spopNotificationSubscription)
    {
        return UnregisterFromNotificationService(
            std::move(async));
    }

    return S_OK;
}

RTANotificationService::RTANotificationService(
    _In_ User&& user,
    _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings,
    _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
) noexcept :
    NotificationService(std::move(user), contextSettings),
    m_rtaManager{ std::move(rtaManager) }
{
}

HRESULT RTANotificationService::RegisterForSpopNotificationEvents() noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };

    auto copiedUserResult = m_user.Copy();
    RETURN_HR_IF_FAILED(copiedUserResult.Hresult());
    m_rtaManager->Activate(copiedUserResult.ExtractPayload());

    {
        auto subscriptionUserCopyResult = m_user.Copy();
        RETURN_HR_IF_FAILED(subscriptionUserCopyResult.Hresult());
        m_spopNotificationSubscription = MakeShared<SpopKickSubscription>(subscriptionUserCopyResult.ExtractPayload(), AppConfig::Instance()->TitleId());
    }

    {
        auto subscriptionUserCopyResult = m_user.Copy();
        RETURN_HR_IF_FAILED(subscriptionUserCopyResult.Hresult());
        RETURN_HR_IF_FAILED(m_rtaManager->AddSubscription(subscriptionUserCopyResult.ExtractPayload(), m_spopNotificationSubscription));
    }

    return RegisterWithNotificationService(
        m_spopNotificationSubscription->ResourceUri(),
        {});
}

HRESULT RTANotificationService::RegisterForGameInviteEvents(
    _In_ AsyncContext<HRESULT> async
) noexcept
{
    return RegisterForEvents(m_gameInviteSubscription, async);
};

HRESULT RTANotificationService::UnregisterForGameInviteEvents(
    _In_ AsyncContext<HRESULT> async
) noexcept
{
    return UnregisterForEvents(m_gameInviteSubscription, async);
}


HRESULT RTANotificationService::RegisterForAchievementUnlockEvents(
    _In_ AsyncContext<HRESULT> async
) noexcept
{
    return RegisterForEvents(m_achievementUnlockSubscription, async);
};


HRESULT RTANotificationService::UnregisterForAchievementUnlockEvents(
    _In_ AsyncContext<HRESULT> async
) noexcept
{
    return UnregisterForEvents(m_achievementUnlockSubscription, async);
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
            AsyncContext<HRESULT>{
                async.Queue(),
                [thisWeakPtr = std::weak_ptr<NotificationService>{ shared_from_this() }, async](HRESULT hr)
                {
                    if (auto pThis{ thisWeakPtr.lock() })
                    {
                        auto derivedPtr = dynamic_cast<RTANotificationService*>(pThis.get());
                        if (SUCCEEDED(hr))
                        {
                            if (pThis->m_registrationStatus == RegistrationStatus::PendingUnregistration)
                            {
                                // Immediately kick off unregistration
                                derivedPtr->UnregisterForGameInviteEvents();
                                derivedPtr->UnregisterForAchievementUnlockEvents();
                            }

                            return async.Complete(hr);
                        }
                        else
                        {

                            return async.Complete(E_XBL_AUTH_RUNTIME_ERROR);
                        }
                    }
                }
            });
}

XblFunctionContext RTANotificationService::AddGameInviteHandler(
    _In_ GameInviteSubscription::MPSDInviteHandler handler
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };

    RegisterForGameInviteEvents();

    // Subscription should always be created after registering for invites
    assert(m_gameInviteSubscription);
    return m_gameInviteSubscription->AddHandler(std::move(handler));
}

XblFunctionContext RTANotificationService::AddGameInviteHandler(
    _In_ GameInviteSubscription::MultiplayerActivityInviteHandler handler
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };

    RegisterForGameInviteEvents();

    // Subscription should always be created after registering for invites
    assert(m_gameInviteSubscription);
    return m_gameInviteSubscription->AddHandler(std::move(handler));
}

void RTANotificationService::RemoveGameInviteHandler(
    _In_ XblFunctionContext token
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    if (m_gameInviteSubscription)
    {
        size_t remainingHandlers = m_gameInviteSubscription->RemoveHandler(token);
        if (!remainingHandlers)
        {
            // Unregister if that was the last client handler
            UnregisterForGameInviteEvents();
        }
    }
}

XblFunctionContext RTANotificationService::AddAchievementUnlockNotificationHandler(
    _In_ AchievementUnlockSubscription::EventHandler handler
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };

    RegisterForAchievementUnlockEvents();

    // Subscription should always be created after registering for invites
    assert(m_achievementUnlockSubscription);
    return m_achievementUnlockSubscription->AddHandler(std::move(handler));
}

void RTANotificationService::RemoveAchievementUnlockNotificationHandler(
    _In_ XblFunctionContext token
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    if (m_achievementUnlockSubscription)
    {
        size_t remainingHandlers = m_achievementUnlockSubscription->RemoveHandler(token);
        if (!remainingHandlers)
        {
            // Unregister if that was the last client handler
            UnregisterForAchievementUnlockEvents();
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
#endif

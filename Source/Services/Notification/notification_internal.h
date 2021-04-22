// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL
#include "real_time_activity_subscription.h"
#include "RTA/notification_subscription.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

enum NotificationTypeFilterSourceType
{
    Media_Presence = 1,
    Presence_Online = 2,
    Broadcast = 3,
    Message = 4,
    Party_Invite_360 = 5,
    Multiplayer = 6,
    Achievements = 8
};

struct NotificationFilter
{
    NotificationTypeFilterSourceType sourceType;
    uint32_t type;
};

class NotificationService : public std::enable_shared_from_this<NotificationService>
{
public:
    enum class RegistrationStatus : uint32_t
    {
        Unregistered,
        Unregistering,
        PendingUnregistration,
        Registered,
        Registering
    } m_registrationStatus{ RegistrationStatus::Unregistered };

    NotificationService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings);
    virtual ~NotificationService() = default;

    virtual HRESULT RegisterWithNotificationService(
#if HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL
        _In_ const String& endpointId,
#endif
        _In_ AsyncContext<HRESULT> async) = 0;

    virtual HRESULT UnregisterFromNotificationService(
        _In_ AsyncContext<HRESULT> async);

protected:
    AsyncContext<HRESULT> m_registrationAsync;
    AsyncContext<HRESULT> m_unregistrationAsync;

    HRESULT RegisterForNotificationsHelper(
        _In_ const String& applicationInstanceId,
        _In_ const String& uriData,
        _In_ const String& platform,
        _In_ const String& deviceName,
        _In_ const String& platformVersion,
        _In_ const Vector<NotificationFilter> notificationFilterEnum,
        _In_ AsyncContext<HRESULT> async
    );

    HRESULT UnregisterFromNotificationHelper(
        _In_ const String& endpointId,
        _In_ AsyncContext<HRESULT> async);

    User m_user;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_contextSettings;

    String m_endpointId;
    bool m_isInitialized{ false };

    std::recursive_mutex m_mutex;
};

#if HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM == HC_PLATFORM_IOS
class MobileNotificationService : public NotificationService
{
public:
    MobileNotificationService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings);

    HRESULT RegisterWithNotificationService(
        _In_ const String& uriData,
        _In_ AsyncContext<HRESULT> async
    ) noexcept override;
};
#endif

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

class RTANotificationService : public NotificationService
{
public:
    RTANotificationService(
        _In_ User&& user,
        _In_ const TaskQueue& taskQueue,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings,
        _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
    ) noexcept;

    ~RTANotificationService() noexcept;

    HRESULT Initialize() noexcept;

    XblFunctionContext AddGameInviteHandler(
        _In_ NotificationSubscription::MPSDInviteHandler handler
    ) noexcept;

    XblFunctionContext AddGameInviteHandler(
        _In_ NotificationSubscription::MultiplayerActivityInviteHandler handler
    ) noexcept;

    void RemoveNotificationHandler(
        _In_ XblFunctionContext token
    ) noexcept;

    XblFunctionContext AddAchievementUnlockNotificationHandler(
        _In_ NotificationSubscription::AchievementUnlockHandler handler
    ) noexcept;

    HRESULT RegisterWithNotificationService(
        _In_ const String& endpointId,
        _In_ AsyncContext<HRESULT> async
    ) noexcept override;

private:
    TaskQueue m_taskQueue;
    std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> m_rtaManager;
    std::shared_ptr<NotificationSubscription> m_rtaSubscription;
};
#elif HC_PLATFORM == HC_PLATFORM_UWP
class UWPNotificationService : public NotificationService
{
public:
    UWPNotificationService(
        _In_ User&& user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings
    ) noexcept;

    HRESULT RegisterWithNotificationService(
        _In_ AsyncContext<HRESULT> async
    ) noexcept override;

private:
    void OnPushNotificationReceived(
        _In_ Windows::Networking::PushNotifications::PushNotificationChannel ^sender,
        _In_ Windows::Networking::PushNotifications::PushNotificationReceivedEventArgs ^args
    );
};
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END

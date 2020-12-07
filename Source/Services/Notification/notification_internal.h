// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL
#include "xsapi-c/game_invite_c.h"
#include "xsapi-c/multiplayer_activity_c.h"
#include "real_time_activity_subscription.h"
#include "RTA/achievement_unlock_subscription.h"
#include "RTA/spop_kick_subscription.h"
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
    } m_registrationStatus;

    NotificationService(
        _In_ User user,
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
    bool m_isInitialized;

    std::recursive_mutex m_mutex;
};

#if HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM == HC_PLATFORM_IOS
class MobileNotificationService : public NotificationService
{
public:
    MobileNotificationService(
        _In_ User user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings);

    HRESULT RegisterWithNotificationService(
        _In_ const String& uriData,
        _In_ AsyncContext<HRESULT> async
    ) noexcept override;
};
#endif

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL
// Event args for MPSD game invites
struct GameInviteNotificationEventArgs : public XblGameInviteNotificationEventArgs
{
public:
    GameInviteNotificationEventArgs() noexcept = default;
    GameInviteNotificationEventArgs(const GameInviteNotificationEventArgs& other) noexcept;
    GameInviteNotificationEventArgs& operator=(GameInviteNotificationEventArgs other) noexcept = delete;

    static Result<GameInviteNotificationEventArgs> Deserialize(
        const JsonValue& json
    ) noexcept;

private:
    String m_inviteHandleId;
    String m_inviteProtocol;
    String m_senderImageUrl;
};

struct MultiplayerActivityInviteData : public XblMultiplayerActivityInviteData
{
public:
    MultiplayerActivityInviteData() noexcept = default;
    MultiplayerActivityInviteData(const MultiplayerActivityInviteData& other) noexcept;
    MultiplayerActivityInviteData& operator=(MultiplayerActivityInviteData other) noexcept = delete;

    static Result<MultiplayerActivityInviteData> Deserialize(
        const JsonValue& json
    ) noexcept;

private:
    String m_senderImageUrl;
    String m_titleName;
    String m_titleImageUrl;
    String m_connectionString;
};

class GameInviteSubscription : public real_time_activity::Subscription
{
public:
    GameInviteSubscription(
        _In_ uint64_t xuid,
        _In_ uint32_t titleId
    ) noexcept;

    const String& ResourceUri() const noexcept;

    typedef Function<void(const GameInviteNotificationEventArgs&)> MPSDInviteHandler;
    typedef Function<void(const MultiplayerActivityInviteData&)> MultiplayerActivityInviteHandler;

    XblFunctionContext AddHandler(MPSDInviteHandler handler) noexcept;
    XblFunctionContext AddHandler(MultiplayerActivityInviteHandler handler) noexcept;
    size_t RemoveHandler(XblFunctionContext token) noexcept;

protected:
    void OnEvent(_In_ const JsonValue& data) noexcept override;
    void OnResync() noexcept override;

private:
    UnorderedMap<XblFunctionContext, MPSDInviteHandler> m_mpsdInviteHandlers;
    UnorderedMap<XblFunctionContext, MultiplayerActivityInviteHandler> m_mpaInviteHandlers;
    XblFunctionContext m_nextToken{ 0 };

    std::mutex m_mutex;
};

class RTANotificationService : public NotificationService
{
public:
    RTANotificationService(
        _In_ User user,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> contextSettings,
        _In_ std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> rtaManager
    ) noexcept;

    HRESULT RegisterForSpopNotificationEvents() noexcept;

    // Keeping these APIs public to continue supporting XblGameInviteRegisterForEventResult and XblGameInviteUnregisterForEventAsync
    HRESULT RegisterForGameInviteEvents(
        _In_ AsyncContext<HRESULT> async = {}
    ) noexcept;

    HRESULT UnregisterForGameInviteEvents(
        _In_ AsyncContext<HRESULT> async = {}
    ) noexcept;

    HRESULT RegisterForAchievementUnlockEvents(
        _In_ AsyncContext<HRESULT> async = {}
    ) noexcept;

    HRESULT UnregisterForAchievementUnlockEvents(
        _In_ AsyncContext<HRESULT> async = {}
    ) noexcept;

    XblFunctionContext AddGameInviteHandler(
        _In_ GameInviteSubscription::MPSDInviteHandler handler
    ) noexcept;

    XblFunctionContext AddGameInviteHandler(
        _In_ GameInviteSubscription::MultiplayerActivityInviteHandler handler
    ) noexcept;

    void RemoveGameInviteHandler(
        _In_ XblFunctionContext token
    ) noexcept;

    XblFunctionContext AddAchievementUnlockNotificationHandler(
        _In_ AchievementUnlockSubscription::EventHandler handler
    ) noexcept;

    void RemoveAchievementUnlockNotificationHandler(
        _In_ XblFunctionContext token
    ) noexcept;

    HRESULT RegisterWithNotificationService(
        _In_ const String& endpointId,
        _In_ AsyncContext<HRESULT> async
    ) noexcept override;

private:
    template<typename T>
    HRESULT RegisterForEvents(std::shared_ptr<T>&        subscription,
        AsyncContext<HRESULT> async) noexcept;

    template<typename T>
    HRESULT UnregisterForEvents(std::shared_ptr<T>&        subscription,
        AsyncContext<HRESULT> async) noexcept;

    std::shared_ptr<xbox::services::real_time_activity::RealTimeActivityManager> m_rtaManager;

    std::shared_ptr<GameInviteSubscription> m_gameInviteSubscription;
    std::shared_ptr<AchievementUnlockSubscription> m_achievementUnlockSubscription;
    std::shared_ptr<SpopKickSubscription> m_spopNotificationSubscription;
};
#elif HC_PLATFORM == HC_PLATFORM_UWP
class UWPNotificationService : public NotificationService
{
public:
    UWPNotificationService(
        _In_ User user,
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

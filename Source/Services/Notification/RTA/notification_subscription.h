// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/game_invite_c.h"
#include "xsapi-c/achievements_c.h"
#include "xsapi-c/multiplayer_activity_c.h"
#include "real_time_activity_subscription.h"

#if HC_PLATFORM == HC_PLATFORM_WIN32 || HC_PLATFORM_IS_EXTERNAL

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

// Event args for MPSD game invites
struct GameInviteNotificationEventArgs : public XblGameInviteNotificationEventArgs
{
public:
    GameInviteNotificationEventArgs() noexcept = default;
    GameInviteNotificationEventArgs(const GameInviteNotificationEventArgs& other) noexcept;
    GameInviteNotificationEventArgs& operator=(GameInviteNotificationEventArgs other) noexcept = delete;

    static Result<GameInviteNotificationEventArgs> Deserialize(
        _In_ const JsonValue& json
    ) noexcept;

private:
    String m_inviteHandleId;
    String m_inviteProtocol;
    String m_inviteContext;
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

struct AchievementUnlockEvent : public XblAchievementUnlockEvent
{
public:

    AchievementUnlockEvent() = default;
    AchievementUnlockEvent(AchievementUnlockEvent&& event) noexcept;
    AchievementUnlockEvent(const AchievementUnlockEvent& event);

    static Result<AchievementUnlockEvent> Deserialize(_In_ const JsonValue& json) noexcept;

private:

    String m_achievementId;
    String m_achievementDescription;
    String m_achievementName;
    String m_achievementIconUri;
    String m_deepLink;
};

class NotificationSubscription : public real_time_activity::Subscription
{
public:
    NotificationSubscription(
        User&& user,
        TaskQueue queue,
        uint32_t titleId
    ) noexcept;

    const String& ResourceUri() const noexcept;

    using MPSDInviteHandler = Function<void(GameInviteNotificationEventArgs&)>;
    using MultiplayerActivityInviteHandler = Function<void(MultiplayerActivityInviteData&)>;
    using AchievementUnlockHandler = Function<void(const AchievementUnlockEvent&)>;

    XblFunctionContext AddHandler(MPSDInviteHandler&& handler) noexcept;
    XblFunctionContext AddHandler(MultiplayerActivityInviteHandler&& handler) noexcept;
    XblFunctionContext AddHandler(AchievementUnlockHandler&& handler) noexcept;

    size_t RemoveHandler(XblFunctionContext token) noexcept;

protected:
    void OnEvent(_In_ const JsonValue& event) noexcept override;

private:
    User m_user;
    TaskQueue m_queue;
    std::mutex m_mutex;
    XblFunctionContext m_nextToken{ 1 };

    Map<XblFunctionContext, MPSDInviteHandler> m_mpsdInviteHandlers;
    Map<XblFunctionContext, MultiplayerActivityInviteHandler> m_mpaInviteHandlers;
    Map<XblFunctionContext, AchievementUnlockHandler> m_achievementUnlockHandlers;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
#endif
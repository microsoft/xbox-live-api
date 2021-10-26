// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <xsapi-c/notification_c.h>

#if HC_PLATFORM == HC_PLATFORM_WIN32
#include <xsapi-c/achievements_c.h>
#include <xsapi-c/game_invite_c.h>
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN
class xbox_live_context;

#if HC_PLATFORM == HC_PLATFORM_WIN32
class invite_notification_event_args
{
public:
    inline string_t invited_xbox_user_id() const;
    inline string_t sender_xbox_user_id() const;
    inline string_t sender_gamertag() const;
    inline string_t invite_handle_id() const;
    inline string_t invite_protocol() const;
    inline string_t invite_context() const;
    inline utility::datetime expiration() const;
    inline const multiplayer::multiplayer_session_reference session_reference() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline invite_notification_event_args(_In_ const XblGameInviteNotificationEventArgs& gameInviteargs);

 private:
     XblGameInviteNotificationEventArgs m_gameInviteArgs;
};

class achievement_unlocked_notification_event_args
{
public:
    inline string_t name() const;
    inline string_t id() const;
    inline string_t description() const;
    inline string_t icon_url() const;
    inline uint64_t gamerscore() const;
    inline string_t deeplink() const;
    inline string_t xbox_user_id() const;
    inline utility::datetime unlockTime() const;

    achievement_unlocked_notification_event_args(_In_ const XblAchievementUnlockEvent& achievementUnlockEvent);

private:
    XblAchievementUnlockEvent m_achievementUnlock;
};

#endif // WIN32

enum notification_filter_source_type
{
    media_presence = 1,
    presence_online = 2,
    broadcast = 3,
    message = 4,
    party_invite_360 = 5,
    multiplayer = 6,
    achievements = 8
};

struct notification_filter
{
    notification_filter_source_type sourceType;
    uint32_t type;
};

class notification_service : public std::enable_shared_from_this<notification_service>
{
public:
    inline notification_service(
        _In_ XblContextHandle contextHandle);

    inline ~notification_service();

    inline pplx::task<xbox_live_result<void>> subscribe_to_notifications(
#if HC_PLATFORM == HC_PLATFORM_WIN32 && !XSAPI_UNIT_TESTS
        _In_ const std::function<void(achievement_unlocked_notification_event_args&)>& achievementUnlockHandler,
        _In_ const std::function<void(invite_notification_event_args&)>& multiplayerInviteHandler
#elif (HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID)
       _In_ const string_t deviceToken
#endif
    );

    inline pplx::task<xbox_live_result<void>> unsubscribe_from_notifications();

#if HC_PLATFORM == HC_PLATFORM_WIN32 && !XSAPI_UNIT_TESTS
    inline std::function<void(invite_notification_event_args&)>& game_invite_handler();

    inline std::function<void(achievement_unlocked_notification_event_args&)>& achievement_unlock_handler();
#endif

private:
    XblContextHandle m_xblContext;

#if HC_PLATFORM == HC_PLATFORM_WIN32 && !XSAPI_UNIT_TESTS
    XblFunctionContext m_gameinviteFunctionContext;
    XblFunctionContext m_achievementUnlockFunctionContext;

    std::function<void(achievement_unlocked_notification_event_args&)> m_achievementUnlockedHandler;
    std::function<void(invite_notification_event_args&)> m_inviteHandler;
#endif
};
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END

#if !XSAPI_NO_PPL
#include "impl/notification.hpp"
#endif
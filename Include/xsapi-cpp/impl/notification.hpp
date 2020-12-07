// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN
notification_service::notification_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_xblContext);
}

notification_service::~notification_service()
{
    XblContextCloseHandle(m_xblContext);
}

#if (HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID)
pplx::task<xbox_live_result<void>> notification_service::subscribe_to_notifications(
    _In_ const string_t deviceToken
) 
{
    auto xblContext = m_xblContext;
    auto asyncWrapper = new AsyncWrapper<void>();
    auto hr = XblNotificationSubscribeToNotificationsAsync(
        xblContext, 
        &asyncWrapper->async,
        deviceToken.c_str());

    return asyncWrapper->Task(hr);
}
#elif HC_PLATFORM == HC_PLATFORM_UWP
pplx::task<xbox_live_result<void>> notification_service::subscribe_to_notifications(
)
{
    auto xblContext = m_xblContext;
    auto asyncWrapper = new AsyncWrapper<void>();
    auto hr = XblNotificationSubscribeToNotificationsAsync(
        xblContext,
        &asyncWrapper->async);

    return asyncWrapper->Task(hr);
}
#elif HC_PLATFORM == HC_PLATFORM_WIN32 && !XSAPI_UNIT_TESTS

inline invite_notification_event_args::invite_notification_event_args(_In_ const XblGameInviteNotificationEventArgs& gameInviteargs)
    :m_gameInviteArgs(gameInviteargs)
{
}

const string_t& invite_notification_event_args::invited_xbox_user_id() const
{
    return Utils::StringTFromUint64(m_gameInviteArgs.invitedXboxUserId);
}

const string_t& invite_notification_event_args::sender_xbox_user_id() const
{
    return Utils::StringTFromUint64(m_gameInviteArgs.senderXboxUserId);
}

const string_t& invite_notification_event_args::sender_gamertag() const
{
    return Utils::StringTFromUtf8(m_gameInviteArgs.senderGamertag);
}

const string_t& invite_notification_event_args::invite_handle_id() const
{
    return Utils::StringTFromUtf8(m_gameInviteArgs.inviteHandleId);
}

const string_t& invite_notification_event_args::invite_protocol() const
{
    return Utils::StringTFromUtf8(m_gameInviteArgs.inviteProtocol);
}

const utility::datetime& invite_notification_event_args::expiration() const
{
    return Utils::DatetimeFromTimeT(m_gameInviteArgs.expiration);
}

const multiplayer::multiplayer_session_reference invite_notification_event_args::session_reference() const
{
    return multiplayer::multiplayer_session_reference(m_gameInviteArgs.sessionReference);
}

inline achievement_unlocked_notification_event_args::achievement_unlocked_notification_event_args(_In_ const XblAchievementUnlockEvent& achievementUnlockEvent)
    : m_achievementUnlock(achievementUnlockEvent)
{
}

const string_t& achievement_unlocked_notification_event_args::name() const
{
    return Utils::StringTFromUtf8(m_achievementUnlock.achievementName);
}

const string_t achievement_unlocked_notification_event_args::id() const
{
    return Utils::StringTFromUtf8(m_achievementUnlock.achievementId);
}

const string_t& achievement_unlocked_notification_event_args::description() const
{
    return Utils::StringTFromUtf8(m_achievementUnlock.achievementDescription);
}

const string_t& achievement_unlocked_notification_event_args::icon_url() const
{
    return Utils::StringTFromUtf8(m_achievementUnlock.achievementIcon);
}

const uint64_t achievement_unlocked_notification_event_args::gamerscore() const
{
    return m_achievementUnlock.gamerscore;
}

const string_t& achievement_unlocked_notification_event_args::deeplink() const
{
    return Utils::StringTFromUtf8(m_achievementUnlock.deepLink);
}

const string_t& achievement_unlocked_notification_event_args::xbox_user_id() const
{
    return Utils::StringTFromUint64(m_achievementUnlock.xboxUserId);
}

const utility::datetime& achievement_unlocked_notification_event_args::unlockTime() const
{
    return Utils::DatetimeFromTimeT(m_achievementUnlock.timeUnlocked);
}

std::function<void(invite_notification_event_args&)>& notification_service::game_invite_handler()
{
    return m_inviteHandler;
}

std::function<void(achievement_unlocked_notification_event_args&)>& notification_service::achievement_unlock_handler()
{
    return m_achievementUnlockedHandler;
}

pplx::task<xbox_live_result<void>> notification_service::subscribe_to_notifications(
    _In_ const std::function<void(achievement_unlocked_notification_event_args&)>& achievementUnlockHandler,
    _In_ const std::function<void(invite_notification_event_args&)>& multiplayerInviteHandler
)
{
    auto xblContext = m_xblContext;
    void* context = m_xblContext;

    m_inviteHandler = multiplayerInviteHandler;
    m_achievementUnlockedHandler = achievementUnlockHandler;

    auto asyncWrapper = new AsyncWrapper<void>();
    m_gameinviteFunctionContext = XblGameInviteAddNotificationHandler(
        xblContext,
        [](_In_ const XblGameInviteNotificationEventArgs* args, _In_opt_ void* context)
        {
            invite_notification_event_args resultInviteArgs(*args);
            auto service = static_cast<notification_service*>(context);
            service->game_invite_handler()(resultInviteArgs);
        },
        context
        );
    delete(asyncWrapper);

    asyncWrapper = new AsyncWrapper<void>();
    m_achievementUnlockFunctionContext = XblAchievementUnlockAddNotificationHandler(
        xblContext,
        [](_In_ const XblAchievementUnlockEvent* args, _In_opt_ void* context)
        {
            achievement_unlocked_notification_event_args achievementUnlockArgs(*args);
            auto service = static_cast<notification_service*>(context);
            service->achievement_unlock_handler()(achievementUnlockArgs);
        },
        context
        );

    return asyncWrapper->Task(S_OK);
}
#endif


#if (HC_PLATFORM == HC_PLATFORM_IOS || HC_PLATFORM == HC_PLATFORM_ANDROID || HC_PLATFORM == HC_PLATFORM_UWP)
pplx::task<xbox_live_result<void>> notification_service::unsubscribe_from_notifications()
{
    auto xblContext = m_xblContext;
    auto asyncWrapper = new AsyncWrapper<void>();
    auto hr = XblNotificationUnsubscribeFromNotificationsAsync(xblContext, &asyncWrapper->async);

    return asyncWrapper->Task(hr);
}
#elif HC_PLATFORM == HC_PLATFORM_WIN32 && !XSAPI_UNIT_TESTS
pplx::task<xbox_live_result<void>> notification_service::unsubscribe_from_notifications()
{
    auto xblContext = m_xblContext;
    auto asyncWrapper = new AsyncWrapper<void>();

    XblGameInviteRemoveNotificationHandler(xblContext, m_gameinviteFunctionContext);
    XblAchievementUnlockRemoveNotificationHandler(xblContext, m_achievementUnlockFunctionContext);

    return asyncWrapper->Task(S_OK);
}
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
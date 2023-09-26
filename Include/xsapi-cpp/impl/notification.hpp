// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN 
#if XSAPI_NOTIFICATION_SERVICE
notification_service::notification_service(_In_ XblContextHandle contextHandle)
{
    XblContextDuplicateHandle(contextHandle, &m_xblContext);
}

notification_service::~notification_service()
{
#if !XSAPI_UNIT_TESTS
    unsubscribe_from_notifications().wait();
#endif
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
#elif HC_PLATFORM == HC_PLATFORM_WIN32 && !XSAPI_UNIT_TESTS

inline invite_notification_event_args::invite_notification_event_args(_In_ const XblGameInviteNotificationEventArgs& gameInviteargs)
    :m_gameInviteArgs(gameInviteargs)
{
}

string_t invite_notification_event_args::invited_xbox_user_id() const
{
    return Utils::StringTFromUint64(m_gameInviteArgs.invitedXboxUserId);
}

string_t invite_notification_event_args::sender_xbox_user_id() const
{
    return Utils::StringTFromUint64(m_gameInviteArgs.senderXboxUserId);
}

string_t invite_notification_event_args::sender_gamertag() const
{
    return Utils::StringTFromUtf8(m_gameInviteArgs.senderGamertag);
}

string_t invite_notification_event_args::invite_handle_id() const
{
    return Utils::StringTFromUtf8(m_gameInviteArgs.inviteHandleId);
}

string_t invite_notification_event_args::invite_protocol() const
{
    return Utils::StringTFromUtf8(m_gameInviteArgs.inviteProtocol);
}

string_t invite_notification_event_args::invite_context() const
{
    return Utils::StringTFromUtf8(m_gameInviteArgs.inviteContext);
}

utility::datetime invite_notification_event_args::expiration() const
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

string_t achievement_unlocked_notification_event_args::name() const
{
    return Utils::StringTFromUtf8(m_achievementUnlock.achievementName);
}

string_t achievement_unlocked_notification_event_args::id() const
{
    return Utils::StringTFromUtf8(m_achievementUnlock.achievementId);
}

string_t achievement_unlocked_notification_event_args::description() const
{
    return Utils::StringTFromUtf8(m_achievementUnlock.achievementDescription);
}

string_t achievement_unlocked_notification_event_args::icon_url() const
{
    return Utils::StringTFromUtf8(m_achievementUnlock.achievementIcon);
}

uint64_t achievement_unlocked_notification_event_args::gamerscore() const
{
    return m_achievementUnlock.gamerscore;
}

string_t achievement_unlocked_notification_event_args::deeplink() const
{
    return Utils::StringTFromUtf8(m_achievementUnlock.deepLink);
}

string_t achievement_unlocked_notification_event_args::xbox_user_id() const
{
    return Utils::StringTFromUint64(m_achievementUnlock.xboxUserId);
}

utility::datetime achievement_unlocked_notification_event_args::unlockTime() const
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
        shared_from_this().get()
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
        shared_from_this().get()
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
    XblGameInviteRemoveNotificationHandler(m_xblContext, m_gameinviteFunctionContext);
    XblAchievementUnlockRemoveNotificationHandler(m_xblContext, m_achievementUnlockFunctionContext);

    return pplx::task_from_result(xbox::services::xbox_live_result<void>());
}
#endif
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if !XSAPI_NO_PPL

#include "xsapi-cpp/system.h"
#include "xsapi-cpp/multiplayer.h"
#include "xsapi-cpp/title_storage.h"
#include "xsapi-cpp/profile.h"
#include "xsapi-cpp/privacy.h"
#include "xsapi-cpp/leaderboard.h"
#include "xsapi-cpp/social.h"
#include "xsapi-cpp/presence.h"
#include "xsapi-cpp/achievements.h"
#include "xsapi-cpp/matchmaking.h"
#include "xsapi-cpp/user_statistics.h"
#include "xsapi-cpp/string_verify.h"
#if XSAPI_NOTIFICATION_SERVICE
#include "xsapi-cpp/notification_service.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

/// <summary>
/// Defines pointers to objects that access Xbox Live to create features for player 
/// interactions.
///
/// Note: the XboxLiveContext is unique per instance. Changing state on one instance for a 
/// user does not affect a second instance of the context for the same user. Using multiple
/// instances can therefore result in unexpected behavior. Titles should ensure to only use 
/// one instance of the XboxLiveContext per user.
/// </summary>
class xbox_live_context
{
public:
    /// <summary>
    /// Creates an xbox_live_context from a xbox_live_user
    /// </summary>
    inline xbox_live_context(
        _In_ XblUserHandle user
    );

    /// <summary>
    /// Returns the associated system User.
    /// </summary>
    inline XblUserHandle user();

    /// <summary>
    /// Returns a copy of the associated Xbox Live context handle.
    /// </summary>
    /// <remarks>
    /// It is the caller's responsibility to close the returned handle.
    /// </remarks>
    inline XblContextHandle handle();

    /// <summary>
    /// Returns the current user's Xbox Live User ID.
    /// </summary>
    inline string_t xbox_live_user_id();

    /// <summary>
    /// Returns an object containing settings that apply to all REST calls made such as retry and diagnostic settings.
    /// </summary>
    inline std::shared_ptr<xbox_live_context_settings> settings();

    /// <summary>
    /// Returns an object containing Xbox Live app config such as title ID
    /// </summary>
    inline std::shared_ptr<xbox_live_app_config> application_config();

    /// <summary>
    /// A service for storing data in the cloud.
    /// </summary>
    inline title_storage::title_storage_service title_storage_service();

    /// <summary>
    /// A service for managing user profiles.
    /// </summary>
    inline social::profile_service profile_service();

    /// <summary>
    /// A service for managing privacy settings.
    /// </summary>
    inline privacy::privacy_service privacy_service();

#if !defined(XBOX_LIVE_CREATORS_SDK)
    /// <summary>
    /// A service for managing leaderboards.
    /// </summary>
    inline leaderboard::leaderboard_service leaderboard_service();

    /// <summary>
    /// A service for managing social networking links.
    /// </summary>
    inline social::social_service social_service();

    /// <summary>
    /// A service for managing reputation reports.
    /// </summary>
    inline social::reputation_service reputation_service();

    /// <summary>
    /// A service for managing achievements.
    /// </summary>
    inline achievements::achievement_service achievement_service();

    /// <summary>
    /// A service for managing user statistics.
    /// </summary>
    inline user_statistics::user_statistics_service user_statistics_service();

    /// <summary>
    /// A service for managing multiplayer games.
    /// </summary>
    inline multiplayer::multiplayer_service multiplayer_service();

    /// <summary>
    /// A service for managing matchmaking sessions.
    /// </summary>
    inline matchmaking::matchmaking_service matchmaking_service();

    /// <summary>
    /// A service for managing real-time activity.
    /// </summary>
    inline std::shared_ptr<real_time_activity::real_time_activity_service> real_time_activity_service();

    /// <summary>
    /// A service used to check for offensive strings.
    /// </summary>
    inline system::string_service string_service();

    /// <summary>
    /// A service for managing Rich Presence.
    /// </summary>
    inline presence::presence_service presence_service();

#if XSAPI_NOTIFICATION_SERVICE
    /// <summary>
    /// A service for receiving notifications.
    /// </summary>
    inline std::shared_ptr<notification::notification_service> notification_service();
#endif

#if XSAPI_EVENTS_SERVICE
    /// <summary>
    /// A service used to write in game events.
    /// </summary>
    inline events::events_service events_service();
#endif // !XDK_API && !XSAPI_UNIT_TESTS

#endif // !defined(XBOX_LIVE_CREATORS_SDK)

    /// <summary>
    /// Internal function
    /// </summary>
    inline xbox_live_context(_In_ XblContextHandle xboxLiveContextHandle);
    inline ~xbox_live_context();

private:
    XblContextHandle m_handle = nullptr;

#if XSAPI_NOTIFICATION_SERVICE
    std::shared_ptr <notification::notification_service> m_notificationService;
#endif
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#endif

#if !XSAPI_NO_PPL
#include "impl/xbox_live_context.hpp"
#endif
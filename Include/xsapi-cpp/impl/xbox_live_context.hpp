// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// TODO exclude this from build and include in xbox_live_context.h. Circular dependency currently prevents this,
// but after services are migrated that will work.
#if !XSAPI_NO_PPL

#include "xsapi-c/xbox_live_context_c.h"

#include "public_utils.h"

#include "xsapi-cpp/xbox_live_context.h"
#include "xsapi-cpp/events.h"
#if XSAPI_NOTIFICATION_SERVICE
#include "xsapi-cpp/notification_service.h"
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xbox_live_context::xbox_live_context(_In_ XblUserHandle user)
{
    HRESULT hr = XblContextCreateHandle(user, &m_handle);
    if (FAILED(hr)) throw std::runtime_error("XblContextCreateHandle failed");
    XblSetApiType(XblApiType::XblCPPApi);

#if XSAPI_NOTIFICATION_SERVICE
    // Unlike the rest of the services, notification_service needs to maintain state
    // That is because of the differences between C-APIs for subscribing for RTA events
    // and the C++ subscribe_to_notifications call
    m_notificationService = std::make_shared<notification::notification_service>(m_handle);
#endif
}

xbox_live_context::xbox_live_context(_In_ XblContextHandle xboxLiveContextHandle)
{
    HRESULT hr = XblContextDuplicateHandle(xboxLiveContextHandle, &m_handle);
    if (FAILED(hr)) throw std::runtime_error("XblContextCreateHandle failed");
    XblSetApiType(XblApiType::XblCPPApi);

#if XSAPI_NOTIFICATION_SERVICE
    // Unlike the rest of the services, notification_service needs to maintain state
    // That is because of the differences between C-APIs for subscribing for RTA events
    // and the C++ subscribe_to_notifications call
    m_notificationService = std::make_shared<notification::notification_service>(m_handle);
#endif
}

xbox_live_context::~xbox_live_context()
{
    XblContextCloseHandle(m_handle);
}

XblUserHandle xbox_live_context::user()
{
    XblUserHandle userHandle = nullptr;
    XblContextGetUser(m_handle, &userHandle);
    return userHandle;
}

XblContextHandle xbox_live_context::handle()
{
    XblContextHandle contextHandle = nullptr;
    XblContextDuplicateHandle(m_handle, &contextHandle);
    return contextHandle;
}

string_t xbox_live_context::xbox_live_user_id()
{
    stringstream_t ss;
    uint64_t xuid;
    XblContextGetXboxUserId(m_handle, &xuid);
    ss << xuid;
    return ss.str();
}

std::shared_ptr<xbox_live_context_settings> xbox_live_context::settings()
{
    return std::make_shared<xbox_live_context_settings>(m_handle);
}

std::shared_ptr<xbox_live_app_config> xbox_live_context::application_config()
{
    return xbox_live_app_config::get_app_config_singleton();
}

title_storage::title_storage_service xbox_live_context::title_storage_service()
{
    return title_storage::title_storage_service(m_handle);
}

social::profile_service xbox_live_context::profile_service()
{
    return social::profile_service(m_handle);
}

privacy::privacy_service xbox_live_context::privacy_service()
{
    return privacy::privacy_service(m_handle);
}

#if !defined(XBOX_LIVE_CREATORS_SDK)
leaderboard::leaderboard_service xbox_live_context::leaderboard_service()
{
    return leaderboard::leaderboard_service(m_handle);
}

social::social_service xbox_live_context::social_service()
{
    return social::social_service(m_handle);
}

social::reputation_service xbox_live_context::reputation_service()
{
    return social::reputation_service(m_handle);
}

achievements::achievement_service xbox_live_context::achievement_service()
{
    return achievements::achievement_service(m_handle);
}

user_statistics::user_statistics_service xbox_live_context::user_statistics_service()
{
    return user_statistics::user_statistics_service(m_handle);
}

multiplayer::multiplayer_service xbox_live_context::multiplayer_service()
{
    return multiplayer::multiplayer_service(m_handle);
}

matchmaking::matchmaking_service xbox_live_context::matchmaking_service()
{
    return matchmaking::matchmaking_service(m_handle);
}

std::shared_ptr<real_time_activity::real_time_activity_service> xbox_live_context::real_time_activity_service()
{
    return std::shared_ptr<real_time_activity::real_time_activity_service>(new real_time_activity::real_time_activity_service(m_handle));
}

system::string_service xbox_live_context::string_service()
{
    return system::string_service(m_handle);
}

presence::presence_service xbox_live_context::presence_service()
{
    return presence::presence_service(m_handle);
}

#if XSAPI_NOTIFICATION_SERVICE
std::shared_ptr<notification::notification_service> xbox_live_context::notification_service()
{
    return m_notificationService;
}
#endif

#if XSAPI_EVENTS_SERVICE
events::events_service xbox_live_context::events_service()
{
    return events::events_service(m_handle);
}
#endif // XSAPI_EVENTS_SERVICE
#endif // !defined(XBOX_LIVE_CREATORS_SDK)

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#endif
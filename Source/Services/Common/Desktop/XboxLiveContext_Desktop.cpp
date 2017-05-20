// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/services.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "xbox_live_context_impl.h"
#if !TV_API && !UNIT_TEST_SERVICES && !XSAPI_SERVER && !XSAPI_U
#include "Misc/notification_service.h"
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if TV_API | XBOX_UWP

xbox_live_context::xbox_live_context(
    _In_ Windows::Xbox::System::User^ user
    )
{
    m_xboxLiveContextImpl = std::make_shared<xbox_live_context_impl>(user);
    m_xboxLiveContextImpl->init();
}

Windows::Xbox::System::User^
xbox_live_context::user()
{
    return m_xboxLiveContextImpl->user();
}

#elif XSAPI_CPP

xbox_live_context::xbox_live_context(
    _In_ std::shared_ptr<system::xbox_live_user> user
    )
{
    user->_User_impl()->set_user_pointer(user);
    m_xboxLiveContextImpl = std::make_shared<xbox_live_context_impl>(user);
    m_xboxLiveContextImpl->init();
}

std::shared_ptr<system::xbox_live_user>
xbox_live_context::user()
{
    return m_xboxLiveContextImpl->user();
}

#else
xbox_live_context::xbox_live_context(
    _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user
    )
{
    m_xboxLiveContextImpl = std::make_shared<xbox_live_context_impl>(user);
    m_xboxLiveContextImpl->init();
}

Microsoft::Xbox::Services::System::XboxLiveUser^
xbox_live_context::user()
{
    return m_xboxLiveContextImpl->user();
}
#endif

const string_t& xbox_live_context::xbox_live_user_id()
{
    return m_xboxLiveContextImpl->xbox_live_user_id();
}

social::profile_service&
xbox_live_context::profile_service()
{
    return m_xboxLiveContextImpl->profile_service();
}

social::social_service&
xbox_live_context::social_service()
{
    return m_xboxLiveContextImpl->social_service();
}

social::reputation_service&
xbox_live_context::reputation_service()
{
    return m_xboxLiveContextImpl->reputation_service();
}

leaderboard::leaderboard_service&
xbox_live_context::leaderboard_service()
{
    return m_xboxLiveContextImpl->leaderboard_service();
}

achievements::achievement_service&
xbox_live_context::achievement_service()
{
    return m_xboxLiveContextImpl->achievement_service();
}

multiplayer::multiplayer_service&
xbox_live_context::multiplayer_service()
{
    return m_xboxLiveContextImpl->multiplayer_service();
}

matchmaking::matchmaking_service&
xbox_live_context::matchmaking_service()
{
    return m_xboxLiveContextImpl->matchmaking_service();
}

tournaments::tournament_service&
xbox_live_context::tournament_service()
{
    return m_xboxLiveContextImpl->tournament_service();
}

user_statistics::user_statistics_service&
xbox_live_context::user_statistics_service()
{
    return m_xboxLiveContextImpl->user_statistics_service();
}

const std::shared_ptr<real_time_activity::real_time_activity_service>&
xbox_live_context::real_time_activity_service()
{
    return m_xboxLiveContextImpl->real_time_activity_service();
}

presence::presence_service&
xbox_live_context::presence_service()
{
    return m_xboxLiveContextImpl->presence_service();
}

game_server_platform::game_server_platform_service&
xbox_live_context::game_server_platform_service()
{
    return m_xboxLiveContextImpl->game_server_platform_service();
}

title_storage::title_storage_service&
xbox_live_context::title_storage_service()
{
    return m_xboxLiveContextImpl->title_storage_service();
}

privacy::privacy_service&
xbox_live_context::privacy_service()
{
    return m_xboxLiveContextImpl->privacy_service();
}

system::string_service&
xbox_live_context::string_service()
{
	return m_xboxLiveContextImpl->string_service();
}

contextual_search::contextual_search_service&
xbox_live_context::contextual_search_service()
{
	return m_xboxLiveContextImpl->contextual_search_service();
}

#if UWP_API || XSAPI_U
events::events_service&
xbox_live_context::events_service()
{
    return m_xboxLiveContextImpl->events_service();
}
#endif

#if TV_API || UNIT_TEST_SERVICES
marketplace::catalog_service&
xbox_live_context::catalog_service()
{
    return m_xboxLiveContextImpl->catalog_service();
}

marketplace::inventory_service&
    xbox_live_context::inventory_service()
{
    return m_xboxLiveContextImpl->inventory_service();
}

entertainment_profile::entertainment_profile_list_service&
xbox_live_context::entertainment_profile_list_service()
{
    return m_xboxLiveContextImpl->entertainment_profile_list_service();
}
#endif

std::shared_ptr<xbox_live_context_settings> 
xbox_live_context::settings()
{
    return m_xboxLiveContextImpl->settings();
}

std::shared_ptr<xbox_live_app_config> 
xbox_live_context::application_config()
{
    return m_xboxLiveContextImpl->application_config();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
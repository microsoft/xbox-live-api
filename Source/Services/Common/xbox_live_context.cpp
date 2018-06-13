// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/services.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "xbox_live_context_impl.h"
#if !TV_API && !UNIT_TEST_SERVICES && !XSAPI_U
#include "Misc/notification_service.h"
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN


#if XSAPI_XDK_AUTH
xbox_live_context::xbox_live_context(
    _In_ Windows::Xbox::System::User^ user
    )
{
    m_xboxLiveContextImpl = xsapi_allocate_shared<xbox_live_context_impl>(user);
    m_xboxLiveContextImpl->user_context()->set_caller_api_type(xbox::services::caller_api_type::api_cpp);
    init();
}

Windows::Xbox::System::User^
xbox_live_context::user()
{
    return m_xboxLiveContextImpl->user();
}
#endif

#if XSAPI_NONXDK_CPP_AUTH && !UNIT_TEST_SERVICES
xbox_live_context::xbox_live_context(
    _In_ std::shared_ptr<system::xbox_live_user> user
    )
{
    user->_User_impl()->set_user_pointer(user);
    m_xboxLiveContextImpl = xsapi_allocate_shared<xbox_live_context_impl>(user);
    m_xboxLiveContextImpl->user_context()->set_caller_api_type(xbox::services::caller_api_type::api_cpp);
    this->init();
}

std::shared_ptr<system::xbox_live_user>
xbox_live_context::user()
{
    return m_xboxLiveContextImpl->user();
}
#endif

#if XSAPI_NONXDK_WINRT_AUTH
xbox_live_context::xbox_live_context(
    _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user
    )
{
    m_xboxLiveContextImpl = xsapi_allocate_shared<xbox_live_context_impl>(user);
    m_xboxLiveContextImpl->user_context()->set_caller_api_type(xbox::services::caller_api_type::api_cpp);
    this->init();
}

Microsoft::Xbox::Services::System::XboxLiveUser^
xbox_live_context::user()
{
    return m_xboxLiveContextImpl->user();
}

#endif

void xbox_live_context::init()
{
    m_xboxLiveContextImpl->init();

    m_achievementService = achievements::achievement_service(m_xboxLiveContextImpl->achievement_service_internal());
    m_profileService = social::profile_service(m_xboxLiveContextImpl->profile_service_impl());
    m_socialService = social::social_service(settings(), m_xboxLiveContextImpl->social_service_impl());
    m_reputationService = social::reputation_service(m_xboxLiveContextImpl->reputation_service_impl());
    m_presenceService = presence::presence_service(m_xboxLiveContextImpl->presence_service());
}

string_t xbox_live_context::xbox_live_user_id()
{
    return utils::string_t_from_internal_string(m_xboxLiveContextImpl->xbox_live_user_id());
}

social::profile_service&
xbox_live_context::profile_service()
{
    return m_profileService;
}

social::social_service&
xbox_live_context::social_service()
{
    return m_socialService;
}

social::reputation_service&
xbox_live_context::reputation_service()
{
    return m_reputationService;
}

leaderboard::leaderboard_service&
xbox_live_context::leaderboard_service()
{
    return m_xboxLiveContextImpl->leaderboard_service();
}

achievements::achievement_service&
xbox_live_context::achievement_service()
{
    return m_achievementService;
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
    return m_presenceService;
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

clubs::clubs_service&
xbox_live_context::clubs_service()
{
    return m_xboxLiveContextImpl->clubs_service();
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

#if !XSAPI_CPP
std::shared_ptr<user_context>
xbox_live_context::_User_context() const
{
    return m_xboxLiveContextImpl->user_context();
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
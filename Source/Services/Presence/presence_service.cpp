//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/presence.h"
#include "utils.h"
#include "presence_internal.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_BEGIN

presence_service::presence_service()
{
}

presence_service::presence_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService
    ) : 
    m_presenceServiceImpl(std::make_shared<presence_service_impl>(
        realTimeActivityService,
        userContext, 
        xboxLiveContextSettings,
        appConfig))
{
}

task<xbox_live_result<void>>
presence_service::set_presence(
    _In_ bool isUserActiveInTitle
    )
{
    return m_presenceServiceImpl->set_presence(isUserActiveInTitle);
}

task<xbox_live_result<void>>
presence_service::set_presence(
    _In_ bool isUserActiveInTitle,
    _In_ presence_data presenceData
    )
{
    return m_presenceServiceImpl->set_presence(isUserActiveInTitle, presenceData);
}

std::shared_ptr<xbox_live_context_settings>
presence_service::_Xbox_live_context_settings()
{
    return m_presenceServiceImpl->_Xbox_live_context_settings();
}

task<xbox_live_result<presence_record>> 
presence_service::get_presence(
    _In_ const string_t& xboxUserId
    )
{
    return m_presenceServiceImpl->get_presence(xboxUserId);
}

task<xbox_live_result<std::vector<presence_record>>>
presence_service::get_presence_for_multiple_users(
    _In_ const std::vector<string_t>& xboxUserIds
    )
{
    return m_presenceServiceImpl->get_presence_for_multiple_users(xboxUserIds);
}

task<xbox_live_result<std::vector<presence_record>>>
presence_service::get_presence_for_multiple_users(
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const std::vector<presence_device_type>& deviceTypes,
    _In_ const std::vector<uint32_t>& titleIds,
    _In_ presence_detail_level presenceDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
    )
{
    return m_presenceServiceImpl->get_presence_for_multiple_users(
        xboxUserIds,
        deviceTypes,
        titleIds,
        presenceDetailLevel,
        onlineOnly,
        broadcastingOnly
        );
}

task<xbox_live_result<std::vector<presence_record>>> 
presence_service::get_presence_for_social_group(
    _In_ const string_t& socialGroup
    )
{
    return m_presenceServiceImpl->get_presence_for_social_group(socialGroup);
}

task<xbox_live_result<std::vector<presence_record>>>
presence_service::get_presence_for_social_group(
    _In_ const string_t& socialGroup,
    _In_ const string_t& socialGroupOwnerXboxUserId,
    _In_ const std::vector<presence_device_type>& deviceTypes,
    _In_ const std::vector<uint32_t>& titleIds,
    _In_ presence_detail_level peoplehubDetailLevel,
    _In_ bool onlineOnly,
    _In_ bool broadcastingOnly
    )
{
    return m_presenceServiceImpl->get_presence_for_social_group(
        socialGroup,
        socialGroupOwnerXboxUserId,
        deviceTypes,
        titleIds,
        peoplehubDetailLevel,
        onlineOnly,
        broadcastingOnly
        );
}

function_context
presence_service::add_device_presence_changed_handler(
    _In_ std::function<void(const device_presence_change_event_args&)> handler
    )
{
    return m_presenceServiceImpl->add_device_presence_changed_handler(
        handler
        );
}

void
presence_service::remove_device_presence_changed_handler(
    _In_ function_context context
    )
{
    return m_presenceServiceImpl->remove_device_presence_changed_handler(
        context
        );
}

function_context
presence_service::add_title_presence_changed_handler(
    _In_ std::function<void(const title_presence_change_event_args&)> handler
    )
{
    return m_presenceServiceImpl->add_title_presence_changed_handler(
        handler
        );
}

void
presence_service::remove_title_presence_changed_handler(
    _In_ function_context context
    )
{
    m_presenceServiceImpl->remove_title_presence_changed_handler(
        context
        );
}

xbox_live_result<std::shared_ptr<device_presence_change_subscription>>
presence_service::subscribe_to_device_presence_change(
    _In_ const string_t& xboxUserId
    )
{
    return m_presenceServiceImpl->subscribe_to_device_presence_change(
        xboxUserId
        );
}

xbox_live_result<void>
presence_service::unsubscribe_from_device_presence_change(
    _In_ std::shared_ptr<device_presence_change_subscription> subscription
    )
{
    return m_presenceServiceImpl->unsubscribe_from_device_presence_change(
        subscription
        );
}

xbox_live_result<std::shared_ptr<title_presence_change_subscription>>
presence_service::subscribe_to_title_presence_change(
    _In_ const string_t& xboxUserId,
    _In_ uint32_t titleId
    )
{
    return m_presenceServiceImpl->subscribe_to_title_presence_change(
        xboxUserId,
        titleId
        );
}

xbox_live_result<void>
presence_service::unsubscribe_from_title_presence_change(
    _In_ std::shared_ptr<title_presence_change_subscription> subscription
    )
{
    return m_presenceServiceImpl->unsubscribe_from_title_presence_change(
        subscription
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_CPP_END
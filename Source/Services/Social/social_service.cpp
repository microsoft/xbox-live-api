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
#include "xsapi/social.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"
#include "social_internal.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

social_service::social_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService
    ) :
    m_userContext(userContext),
    m_xboxLiveContextSettings(xboxLiveContextSettings),
    m_socialServiceImpl(std::make_shared<social_service_impl>(userContext, xboxLiveContextSettings, appConfig, realTimeActivityService))
{
}

pplx::task<xbox_live_result<xbox_social_relationship_result>> 
social_service::get_social_relationships()
{
    return get_social_relationships(m_userContext->xbox_user_id(), xbox_social_relationship_filter::all, 0, 0);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>>
social_service::get_social_relationships(
    _In_ const string_t& xboxUserId
    )
{
    return get_social_relationships(xboxUserId, xbox_social_relationship_filter::all, 0, 0);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>> 
social_service::get_social_relationships(
    _In_ xbox_social_relationship_filter socialRelationshipFilter
    )
{
    return get_social_relationships(m_userContext->xbox_user_id(), socialRelationshipFilter, 0, 0);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>>
social_service::get_social_relationships(
    _In_ xbox_social_relationship_filter filter,
    _In_ unsigned int startIndex,
    _In_ unsigned int maxItems
    )
{
    return m_socialServiceImpl->get_social_relationships(
        filter,
        startIndex,
        maxItems
        );
}

pplx::task<xbox_live_result<xbox_social_relationship_result>>
social_service::get_social_relationships(
    _In_ const string_t& xboxUserId,
    _In_ xbox_social_relationship_filter filter,
    _In_ unsigned int startIndex,
    _In_ unsigned int maxItems
    )
{
    return m_socialServiceImpl->get_social_relationships(
        xboxUserId,
        filter,
        startIndex,
        maxItems
        );
}

xbox_live_result<std::shared_ptr<social_relationship_change_subscription>>
social_service::subscribe_to_social_relationship_change(
    _In_ const string_t& xboxUserId
    )
{
    return m_socialServiceImpl->subscribe_to_social_relationship_change(
        xboxUserId
        );
}

xbox_live_result<void>
social_service::unsubscribe_from_social_relationship_change(
    _In_ std::shared_ptr<social_relationship_change_subscription> subscription
    )
{
    return m_socialServiceImpl->unsubscribe_from_social_relationship_change(
        subscription
        );
}

function_context
social_service::add_social_relationship_changed_handler(
    _In_ std::function<void(social_relationship_change_event_args)> handler
    )
{
    return m_socialServiceImpl->add_social_relationship_changed_handler(
        std::move(handler)
        );
}

void
social_service::remove_social_relationship_changed_handler(
    _In_ function_context context
    )
{
    m_socialServiceImpl->remove_social_relationship_changed_handler(
        context
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END
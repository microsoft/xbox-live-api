// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"
#include "social_internal.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

social_service::social_service(
    _In_ std::shared_ptr<social_service_impl> serviceImpl
    ) :
    m_socialServiceImpl(std::move(serviceImpl))
{
}

pplx::task<xbox_live_result<xbox_social_relationship_result>> 
social_service::get_social_relationships()
{
    return get_social_relationships(xbox_social_relationship_filter::all, 0, 0);
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
    return get_social_relationships(socialRelationshipFilter, 0, 0);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>>
social_service::get_social_relationships(
    _In_ xbox_social_relationship_filter filter,
    _In_ uint32_t startIndex,
    _In_ uint32_t maxItems
    )
{
    task_completion_event<xbox_live_result<xbox_social_relationship_result>> tce;

    auto result = m_socialServiceImpl->get_social_relationships(
        filter,
        startIndex,
        maxItems,
        XSAPI_DEFAULT_TASKGROUP,
        [tce](xbox_live_result<xbox_social_relationship_result> result) { tce.set(result); }
        );

    return pplx::task<xbox_live_result<xbox_social_relationship_result>>(tce);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>>
social_service::get_social_relationships(
    _In_ const string_t& xboxUserId,
    _In_ xbox_social_relationship_filter filter,
    _In_ unsigned int startIndex,
    _In_ unsigned int maxItems
    )
{
    task_completion_event<xbox_live_result<xbox_social_relationship_result>> tce;

    auto result = m_socialServiceImpl->get_social_relationships(
        utils::internal_string_from_external_string(xboxUserId),
        filter,
        startIndex,
        maxItems,
        XSAPI_DEFAULT_TASKGROUP,
        [tce](xbox_live_result<xbox_social_relationship_result> result) { tce.set(result); }
    );

    return pplx::task<xbox_live_result<xbox_social_relationship_result>>(tce);
}

xbox_live_result<std::shared_ptr<social_relationship_change_subscription>>
social_service::subscribe_to_social_relationship_change(
    _In_ const string_t& xboxUserId
    )
{
    return m_socialServiceImpl->subscribe_to_social_relationship_change(
        utils::internal_string_from_external_string(xboxUserId)
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
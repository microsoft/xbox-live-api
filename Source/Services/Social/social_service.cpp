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
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> contextSettings,
    _In_ std::shared_ptr<social_service_impl> serviceImpl
    ) :
    m_xboxLiveContextSettings(std::move(contextSettings)),
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
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>> result)
        {
            tce.set(CREATE_EXTERNAL_XBOX_LIVE_RESULT(xbox_social_relationship_result, result));
        });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<xbox_social_relationship_result>(result.err(), result.err_message()));
    }
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
        utils::internal_string_from_string_t(xboxUserId),
        filter,
        startIndex,
        maxItems,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>> result)
        {
            tce.set(CREATE_EXTERNAL_XBOX_LIVE_RESULT(xbox_social_relationship_result, result));
        });

    if (result.err())
    {
        return pplx::task_from_result(xbox_live_result<xbox_social_relationship_result>(result.err(), result.err_message()));
    }
    return pplx::task<xbox_live_result<xbox_social_relationship_result>>(tce);
}

xbox_live_result<std::shared_ptr<social_relationship_change_subscription>>
social_service::subscribe_to_social_relationship_change(
    _In_ const string_t& xboxUserId
    )
{
    auto result = m_socialServiceImpl->subscribe_to_social_relationship_change(
        utils::internal_string_from_string_t(xboxUserId)
        );

    if (result.err())
    {
        return xbox_live_result<std::shared_ptr<social_relationship_change_subscription>>(result.err(), result.err_message());
    }
    return xbox_live_result<std::shared_ptr<social_relationship_change_subscription>>(
        xsapi_allocate_shared<social_relationship_change_subscription>(result.payload())
        );
}

xbox_live_result<void>
social_service::unsubscribe_from_social_relationship_change(
    _In_ std::shared_ptr<social_relationship_change_subscription> subscription
    )
{
    return m_socialServiceImpl->unsubscribe_from_social_relationship_change(
        subscription->m_internalObj
        );
}

function_context
social_service::add_social_relationship_changed_handler(
    _In_ std::function<void(social_relationship_change_event_args)> handler
    )
{
    return m_socialServiceImpl->add_social_relationship_changed_handler(
        [handler](std::shared_ptr<social_relationship_change_event_args_internal> internalArgs)
    {
        handler(social_relationship_change_event_args(internalArgs));
    });
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
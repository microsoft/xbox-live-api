// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "social_internal.h"
#include "social_manager_internal.h"
#include "http_call_impl.h"
#include "user_context.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

social_service_impl::social_service_impl(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_realTimeActivityService(realTimeActivityService),
    m_socialRelationshipChangeHandlerCounter(0)
{
}

xbox_live_result<std::shared_ptr<social_relationship_change_subscription_internal>>
social_service_impl::subscribe_to_social_relationship_change(
    _In_ const xsapi_internal_string& xboxUserId
    )
{
    std::weak_ptr<social_service_impl> thisWeakPtr = shared_from_this();
    auto socialRelationshipSub = xsapi_allocate_shared<social_relationship_change_subscription_internal>(
        xboxUserId,
        ([thisWeakPtr](std::shared_ptr<social_relationship_change_event_args_internal> eventArgs)
        {
            std::shared_ptr<social_service_impl> pThis(thisWeakPtr);
            if (pThis)
            {
                pThis->social_relationship_changed(eventArgs);
            }
        }),
        ([thisWeakPtr](const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& eventArgs)
        {
            std::shared_ptr<social_service_impl> pThis(thisWeakPtr);
            if (pThis)
            {
                pThis->m_realTimeActivityService->_Trigger_subscription_error(eventArgs);
            }
        })
        );

    auto subscriptionSucceed = m_realTimeActivityService->_Add_subscription(
        socialRelationshipSub
        );

    if (!subscriptionSucceed.err())
    {
        return xbox_live_result<std::shared_ptr<social_relationship_change_subscription_internal>>(socialRelationshipSub);
    }

    return xbox_live_result<std::shared_ptr<social_relationship_change_subscription_internal>>(subscriptionSucceed.err(), subscriptionSucceed.err_message());
}

xbox_live_result<void>
social_service_impl::unsubscribe_from_social_relationship_change(
    _In_ std::shared_ptr<social_relationship_change_subscription_internal> subscription
    )
{
    return m_realTimeActivityService->_Remove_subscription(
        subscription
        );
}

function_context
social_service_impl::add_social_relationship_changed_handler(
    _In_ xbox_live_callback<std::shared_ptr<social_relationship_change_event_args_internal>> handler
    )
{
    std::lock_guard<std::mutex> lock(m_socialRelationshipChangeHandlerLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_socialRelationshipChangeHandlerCounter;
        m_socialRelationshipChangeHandler[m_socialRelationshipChangeHandlerCounter] = std::move(handler);
    }

    return context;
}

void
social_service_impl::remove_social_relationship_changed_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_socialRelationshipChangeHandlerLock.get());

    m_socialRelationshipChangeHandler.erase(context);
}

void
social_service_impl::social_relationship_changed(
    _In_ std::shared_ptr<social_relationship_change_event_args_internal> eventArgs
    )
{
    xsapi_internal_unordered_map<uint32_t, xbox_live_callback<std::shared_ptr<social_relationship_change_event_args_internal>>> socialRelationshipChangedHandlersCopy;
    {
        std::lock_guard<std::mutex> lock(m_socialRelationshipChangeHandlerLock.get());
        socialRelationshipChangedHandlersCopy = m_socialRelationshipChangeHandler;
    }

    for (auto& handler : socialRelationshipChangedHandlersCopy)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(eventArgs);
            }
            catch (...)
            {
                LOG_ERROR_IF(
                    manager::social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error, 
                    "social_relationship_changed call threw an exception"
                );
            }
        }
    }
}

xbox_live_result<void>
social_service_impl::get_social_relationships(
    _In_ xbox_social_relationship_filter filter,
    _In_ uint32_t startIndex,
    _In_ uint32_t maxItems,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>>> callback
    )
{
    return get_social_relationships(
        m_userContext->xbox_user_id(), 
        filter,
        startIndex,
        maxItems,
        queue,
        callback
        );
}

xbox_live_result<void>
social_service_impl::get_social_relationships(
    _In_ const xsapi_internal_string& xboxUserId,
    _In_ xbox_social_relationship_filter filter,
    _In_ uint32_t startIndex,
    _In_ uint32_t maxItems,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>>> callback
    )
{
    RETURN_CPP_INVALIDARGUMENT_IF(xboxUserId.empty(), void, "xboxUserId is empty");
    bool includeViewFilter = (filter != xbox_social_relationship_filter::all);

    xsapi_internal_string pathAndQuery = pathandquery_social_subpath(
        xboxUserId,
        includeViewFilter,
        xbox_social_relationship_filter_to_string(filter),
        startIndex,
        maxItems
        );

    std::shared_ptr<http_call_internal> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        "GET",
        utils::create_xboxlive_endpoint("social", m_appConfig),
        utils::string_t_from_internal_string(pathAndQuery),
        xbox_live_api::get_social_relationships
        );

    std::shared_ptr<social_service_impl> thisShared = shared_from_this();

    httpCall->get_response_with_auth(
        m_userContext, 
        http_call_response_body_type::json_body,
        false,
        queue,
        [thisShared, startIndex, filter, callback](std::shared_ptr<http_call_response_internal> response)
    {
        auto result = xbox_live_result<std::shared_ptr<xbox_social_relationship_result_internal>>(response->err_code());
        if (!result.err())
        {
            result = xbox_social_relationship_result_internal::deserialize(response->response_body_json());

            auto socialRelationship = result.payload();
            uint32_t itemSize = static_cast<uint32_t>(socialRelationship->items().size());
            if (itemSize > 0)
            {
                unsigned continuationSkip = startIndex + itemSize;

                // Initialize the request params for get_next()
                socialRelationship->init_next_page_info(
                    thisShared,
                    filter,
                    continuationSkip
                );
            }
        }
        callback(result);
    });

    return xbox_live_result<void>();
}

const xsapi_internal_string
social_service_impl::xbox_social_relationship_filter_to_string(
    _In_ xbox_social_relationship_filter xboxSocialRelationshipFilter
    )
{
    switch (xboxSocialRelationshipFilter)
    {
    case xbox_social_relationship_filter::favorite:
        return "Favorite";

    case xbox_social_relationship_filter::legacy_xbox_live_friends:
        return "LegacyXboxLiveFriends";

    default:
    case xbox_social_relationship_filter::all:
        return xsapi_internal_string();
    }
}

xsapi_internal_string
social_service_impl::pathandquery_social_subpath(
    _In_ const xsapi_internal_string& ownerId,
    _In_ bool includeViewFilter,
    _In_ const xsapi_internal_string& view,
    _In_ uint64_t startIndex,
    _In_ uint64_t maxItems
    )
{
    xsapi_internal_stringstream source;
    source << "/users/xuid(";
    source << ownerId;
    source << ")/people";

    xsapi_internal_string nextDelimiter = "?";

    if (includeViewFilter)
    {
        source << nextDelimiter;
        source << "view=";
        source << view;
        nextDelimiter = "&";
    }

    if (startIndex > 0)
    {
        source << nextDelimiter;
        source << "startIndex=";
        source << startIndex;
        nextDelimiter = "&";
    }

    if (maxItems > 0)
    {
        source << nextDelimiter;
        source << "maxItems=";
        source << maxItems;
        nextDelimiter = "&";
    }

    return source.str();
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END
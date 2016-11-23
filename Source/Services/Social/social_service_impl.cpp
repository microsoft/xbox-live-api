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
#include "social_internal.h"
#include "http_call_impl.h"
#include "user_context.h"
#include "xbox_system_factory.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

social_service_impl::social_service_impl(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_realTimeActivityService(realTimeActivityService),
    m_socialRelationshipChangeHandlerCounter(0)
{
}

xbox_live_result<std::shared_ptr<social_relationship_change_subscription>>
social_service_impl::subscribe_to_social_relationship_change(
    _In_ const string_t& xboxUserId
    )
{
    std::weak_ptr<social_service_impl> thisWeakPtr = shared_from_this();
    auto socialRelationshipSub = std::make_shared<social_relationship_change_subscription>(
        xboxUserId,
        ([thisWeakPtr](social_relationship_change_event_args eventArgs)
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
        return xbox_live_result<std::shared_ptr<social_relationship_change_subscription>>(socialRelationshipSub);
    }

    return xbox_live_result<std::shared_ptr<social_relationship_change_subscription>>(subscriptionSucceed.err(), subscriptionSucceed.err_message());
}

xbox_live_result<void>
social_service_impl::unsubscribe_from_social_relationship_change(
    _In_ std::shared_ptr<social_relationship_change_subscription> subscription
    )
{
    return m_realTimeActivityService->_Remove_subscription(
        subscription
        );
}

function_context
social_service_impl::add_social_relationship_changed_handler(
    _In_ std::function<void(social_relationship_change_event_args)> handler
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
    _In_ social_relationship_change_event_args eventArgs
    )
{
    std::unordered_map<uint32_t, std::function<void(const social_relationship_change_event_args&)>> socialRelationshipChangedHandlersCopy;
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
                LOG_ERROR("social_relationship_changed call threw an exception");
            }
        }
    }
}

pplx::task<xbox_live_result<xbox_social_relationship_result>>
social_service_impl::get_social_relationships(
    _In_ xbox_social_relationship_filter filter,
    _In_ unsigned int startIndex,
    _In_ unsigned int maxItems
    )
{
    return get_social_relationships(m_userContext->xbox_user_id(), filter, startIndex, maxItems);
}

pplx::task<xbox_live_result<xbox_social_relationship_result>>
social_service_impl::get_social_relationships(
    _In_ const string_t& xboxUserId,
    _In_ xbox_social_relationship_filter filter,
    _In_ unsigned int startIndex,
    _In_ unsigned int maxItems
    )
{
    bool includeViewFilter = (filter != xbox_social_relationship_filter::all);

    string_t pathAndQuery = pathandquery_social_subpath(
        xboxUserId,
        includeViewFilter,
        xbox_social_relationship_filter_to_string(filter),
        startIndex,
        maxItems
        );

    std::shared_ptr<http_call> httpCall = xbox::services::system::xbox_system_factory::get_factory()->create_http_call(
        m_xboxLiveContextSettings,
        _T("GET"),
        utils::create_xboxlive_endpoint(_T("social"), m_appConfig),
        pathAndQuery,
        xbox_live_api::get_social_relationships
        );

    auto userContext = m_userContext;
    auto xboxLiveContextSettings = m_xboxLiveContextSettings;
    auto appConfig = m_appConfig;

    std::shared_ptr<social_service_impl> thisShared = shared_from_this();
    auto task = httpCall->get_response_with_auth(m_userContext)
    .then([thisShared, userContext, xboxLiveContextSettings, appConfig, startIndex, filter](std::shared_ptr<http_call_response> response)
    {
        auto result = xbox_social_relationship_result::_Deserialize(response->response_body_json());

        auto& socialRelationship = result.payload();
        uint32_t itemSize = static_cast<uint32_t>(socialRelationship.items().size());
        if (itemSize > 0)
        {
            unsigned continuationSkip = startIndex + itemSize;
            
            // Initialize the request params for get_next()
            socialRelationship._Init_next_page_info(
                thisShared,
                filter,
                continuationSkip
                );
        }

        return utils::generate_xbox_live_result<xbox_social_relationship_result>(
            result,
            response
            );
    });

    return utils::create_exception_free_task<xbox_social_relationship_result>(
        task
        );
}

const string_t
social_service_impl::xbox_social_relationship_filter_to_string(
    _In_ xbox_social_relationship_filter xboxSocialRelationshipFilter
    )
{
    switch (xboxSocialRelationshipFilter)
    {
    case xbox_social_relationship_filter::favorite:
        return _T("Favorite");

    case xbox_social_relationship_filter::legacy_xbox_live_friends:
        return _T("LegacyXboxLiveFriends");

    default:
    case xbox_social_relationship_filter::all:
        return string_t();
    }
}

string_t
social_service_impl::pathandquery_social_subpath(
    _In_ const string_t& ownerId,
    _In_ bool includeViewFilter,
    _In_ const string_t& view,
    _In_ uint64_t startIndex,
    _In_ uint64_t maxItems
    )
{
    stringstream_t source;
    source << _T("/users/xuid(");
    source << ownerId;
    source << ")/people";

    string_t nextDelimiter = _T("?");

    if (includeViewFilter)
    {
        source << nextDelimiter;
        source << _T("view=");
        source << view;
        nextDelimiter = _T("&");
    }

    if (startIndex > 0)
    {
        source << nextDelimiter;
        source << _T("startIndex=");
        source << startIndex;
        nextDelimiter = _T("&");
    }

    if (maxItems > 0)
    {
        source << nextDelimiter;
        source << _T("maxItems=");
        source << maxItems;
        nextDelimiter = _T("&");
    }

    return source.str();
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END
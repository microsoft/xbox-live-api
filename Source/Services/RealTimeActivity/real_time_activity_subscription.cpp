// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/real_time_activity.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

real_time_activity_subscription::real_time_activity_subscription(
    _In_ std::function<void(const real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
    ) :
    m_subscriptionErrorHandler(std::move(subscriptionErrorHandler)),
    m_state(real_time_activity_subscription_state::unknown),
    m_guid(utils::string_t_from_internal_string(xbox::services::utils::create_guid(true)))
{
    XSAPI_ASSERT(m_subscriptionErrorHandler != nullptr);
}

real_time_activity_subscription_state 
real_time_activity_subscription::state() const
{
    return m_state;
}

void 
real_time_activity_subscription::_Set_state(
    _In_ real_time_activity_subscription_state newState
    )
{
    // if state is change from subscribed, reset subscription id
    if (m_state == subscribed && newState != real_time_activity_subscription_state::subscribed)
    {
        m_subscriptionId = 0;
    }
    m_state = newState;
    on_state_changed(m_state);
}

const string_t& 
real_time_activity_subscription::resource_uri() const
{
    return m_resourceUri;
}

void 
real_time_activity_subscription::set_resource_uri(
    _In_ string_t uri
    )
{
    m_resourceUri = std::move(uri);
}

uint32_t 
real_time_activity_subscription::subscription_id() const
{
    return m_subscriptionId;
}

void 
real_time_activity_subscription::set_subscription_id(
    _In_ uint32_t id
    )
{
    m_subscriptionId = id;
}

void
real_time_activity_subscription::on_event_received(
    _In_ const web::json::value& data
    )
{
    // Nothing need to do on the base class.
    UNREFERENCED_PARAMETER(data);
}

void 
real_time_activity_subscription::on_subscription_created(
    _In_ uint32_t id, 
    _In_ const web::json::value& data
    )
{
    UNREFERENCED_PARAMETER(data);
    m_subscriptionId = id;
    _Set_state(real_time_activity_subscription_state::subscribed);
}

void
real_time_activity_subscription::on_state_changed(
    _In_ real_time_activity_subscription_state state
    )
{
    // Nothing need to do on the base class.
    UNREFERENCED_PARAMETER(state);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END
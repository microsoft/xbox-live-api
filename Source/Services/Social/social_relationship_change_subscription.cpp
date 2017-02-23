// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"

using namespace xbox::services::real_time_activity;
NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

social_relationship_change_subscription::social_relationship_change_subscription(
    _In_ string_t xboxUserId,
    _In_ std::function<void(const social_relationship_change_event_args&)> handler,
    _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
    ) :
    real_time_activity_subscription(subscriptionErrorHandler),
    m_xboxUserId(std::move(xboxUserId)),
    m_handler(std::move(handler))
{
    XSAPI_ASSERT(!m_xboxUserId.empty());
    XSAPI_ASSERT(m_handler != nullptr); 

    stringstream_t uri;
    uri << _T("http://social.xboxlive.com/users/xuid(") << m_xboxUserId << _T(")/friends");

    m_resourceUri = uri.str();
}

const string_t&
social_relationship_change_subscription::xbox_user_id() const
{
    return m_xboxUserId;
}

void
social_relationship_change_subscription::on_subscription_created(
    _In_ uint32_t id,
    _In_ const web::json::value& data
    )
{
    real_time_activity_subscription::on_subscription_created(id, data);

    if (!data.is_null())
    {
        m_subscriptionErrorHandler(
            real_time_activity::real_time_activity_subscription_error_event_args(
                *this,
                xbox_live_error_code::json_error,
                "JSON Deserialization Failure"
                )
            );
    }

}

void
social_relationship_change_subscription::on_event_received(_In_ const web::json::value& data)
{
    std::error_code errc;
    auto notificationTypeString = utils::extract_json_string(data, _T("NotificationType"), errc);
    auto notificationType = convert_string_type_to_notification_type(notificationTypeString);
    auto xboxUserIds = utils::extract_json_vector<string_t>(
        utils::json_string_extractor,
        data,
        _T("Xuids"),
        errc,
        false
        );

    if (errc || notificationType == social_notification_type::unknown)
    {
        m_subscriptionErrorHandler(
            real_time_activity_subscription_error_event_args(
                *this,
                xbox_live_error_code::json_error,
                "JSON Deserialization Failure"
                )
            );
    }
    else
    {
        m_handler(
            social_relationship_change_event_args(
                m_xboxUserId,
                notificationType,
                xboxUserIds
                )
            );
    }
}

social_notification_type
social_relationship_change_subscription::convert_string_type_to_notification_type(
    _In_ const string_t& notificationTypeString
    ) const
{
    if (utils::str_icmp(notificationTypeString, _T("added")) == 0) return social_notification_type::added;
    else if (utils::str_icmp(notificationTypeString, _T("removed")) == 0) return social_notification_type::removed;
    else if (utils::str_icmp(notificationTypeString, _T("changed")) == 0) return social_notification_type::changed;
    else return social_notification_type::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END
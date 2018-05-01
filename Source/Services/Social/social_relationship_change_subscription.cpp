// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "social_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

using namespace xbox::services::real_time_activity;

social_relationship_change_subscription::social_relationship_change_subscription(
    _In_ std::shared_ptr<social_relationship_change_subscription_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(social_relationship_change_subscription, xbox_user_id);
DEFINE_GET_ENUM_TYPE(social_relationship_change_subscription, real_time_activity_subscription_state, state);
DEFINE_GET_UINT32(social_relationship_change_subscription, subscription_id);

const string_t& social_relationship_change_subscription::resource_uri() const
{
    return m_internalObj->resource_uri();
}

social_relationship_change_subscription_internal::social_relationship_change_subscription_internal(
    _In_ xsapi_internal_string xboxUserId,
    _In_ xbox_live_callback<std::shared_ptr<social_relationship_change_event_args_internal>> handler,
    _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
    ) :
    real_time_activity_subscription(subscriptionErrorHandler),
    m_xboxUserId(std::move(xboxUserId)),
    m_handler(std::move(handler))
{
    XSAPI_ASSERT(!m_xboxUserId.empty());
    XSAPI_ASSERT(m_handler != nullptr);

    stringstream_t uri;
    uri << _T("http://social.xboxlive.com/users/xuid(") << utils::string_t_from_internal_string(m_xboxUserId) << _T(")/friends");

    m_resourceUri = uri.str();
}

const xsapi_internal_string& social_relationship_change_subscription_internal::xbox_user_id() const
{
    return m_xboxUserId;
}

void
social_relationship_change_subscription_internal::on_subscription_created(
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
social_relationship_change_subscription_internal::on_event_received(_In_ const web::json::value& data)
{
    std::error_code errc;
    auto notificationTypeString = utils::extract_json_string(data, "NotificationType", errc);
    auto notificationType = convert_string_type_to_notification_type(notificationTypeString);
    auto xboxUserIds = utils::extract_json_vector<xsapi_internal_string>(
        utils::json_internal_string_extractor,
        data,
        "Xuids",
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
            xsapi_allocate_shared<social_relationship_change_event_args_internal>(
                m_xboxUserId,
                notificationType,
                xboxUserIds
                )
            );
    }
}

social_notification_type
social_relationship_change_subscription_internal::convert_string_type_to_notification_type(
    _In_ const xsapi_internal_string& notificationTypeString
    ) const
{
    if (utils::str_icmp(notificationTypeString, "added") == 0) return social_notification_type::added;
    else if (utils::str_icmp(notificationTypeString, "removed") == 0) return social_notification_type::removed;
    else if (utils::str_icmp(notificationTypeString, "changed") == 0) return social_notification_type::changed;
    else return social_notification_type::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END
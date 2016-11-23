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
#include "xsapi/user_statistics.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

statistic_change_subscription::statistic_change_subscription(
    _In_ string_t xboxUserId,
    _In_ string_t serviceConfigurationId,
    _In_ xbox::services::user_statistics::statistic initialStatistic,
    _In_ std::function<void(const statistic_change_event_args&)> handler,
    _In_ std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)> subscriptionErrorHandler
    ) :
    real_time_activity_subscription(subscriptionErrorHandler),
    m_xboxUserId(std::move(xboxUserId)),
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_statistic(std::move(initialStatistic)),
    m_statisticChangeHandler(handler)
{
    XSAPI_ASSERT(handler != nullptr);

    stringstream_t uriPath;
    uriPath << _T("https://userstats.xboxlive.com/users/xuid(") << m_xboxUserId << _T(")/scids/") << m_serviceConfigurationId << _T("/stats/") << m_statistic.statistic_name();
    m_resourceUri = uriPath.str();
}

void
statistic_change_subscription::on_subscription_created(
    _In_ uint32_t id,
    _In_ const web::json::value& data
    )
{
    real_time_activity_subscription::on_subscription_created(
        id,
        data
        );

    if (m_statisticChangeHandler && !data.is_null())
    {
        std::error_code errc;
        m_statistic._Set_statistic_name(
            utils::extract_json_string(
                data,
                _T("name")
                )
            );

        m_statistic._Set_statistic_type(
            utils::extract_json_string(
                data,
                _T("type")
                )
            );

        string_t value;
        web::json::value field(utils::extract_json_field(data, _T("value"), false));
        if (field.is_integer())
        {
            stringstream_t stream;
            stream << field.as_integer();
            value = stream.str();
        }
        else if (field.is_string())
        {
            value = field.as_string();
        }

        m_statistic._Set_statistic_value(
            value
            );

        m_statisticChangeHandler(
            statistic_change_event_args(
                m_xboxUserId,
                m_serviceConfigurationId,
                m_statistic
                )
            );
    }
    else
    {
        if (m_subscriptionErrorHandler != nullptr)
        {
            m_subscriptionErrorHandler(
                    xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                    *this,
                    xbox_live_error_code::json_error, 
                    "JSON not found on subscription"
                    )
                );
        }
    }
}

void
statistic_change_subscription::on_event_received(
    _In_ const web::json::value& data
    )
{
    statistic_change_event_args statisticChangeEventArgs;

    if (m_statisticChangeHandler && !data.is_null())
    {
        m_statistic._Set_statistic_value(
            data.serialize()
            );

        m_statisticChangeHandler(
            statistic_change_event_args(
                m_xboxUserId,
                m_serviceConfigurationId,
                m_statistic
                )
            );
    }
    else
    {
        if (m_subscriptionErrorHandler != nullptr)
        {
            m_subscriptionErrorHandler(
                xbox::services::real_time_activity::real_time_activity_subscription_error_event_args(
                    *this,
                    xbox_live_error_code::json_error, 
                    "JSON not found on change event"
                    )
                );
        }
    }
}

const string_t&
statistic_change_subscription::xbox_user_id() const
{
    return m_xboxUserId;
}

const string_t&
statistic_change_subscription::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const xbox::services::user_statistics::statistic&
statistic_change_subscription::statistic() const
{
    return m_statistic;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END
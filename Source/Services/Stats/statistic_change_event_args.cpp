// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/user_statistics.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

statistic_change_event_args::statistic_change_event_args()
{
}

statistic_change_event_args::statistic_change_event_args(
    _In_ string_t xboxUserId, 
    _In_ string_t serviceConfigurationId,
    _In_ statistic latestStatistic
    ) : 
    m_xboxUserId(std::move(xboxUserId)),
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_latestStatistic(std::move(latestStatistic))
{
}

const string_t& 
statistic_change_event_args::xbox_user_id() const
{
    return m_xboxUserId;
}

const string_t& 
statistic_change_event_args::service_configuration_id() const
{
    return m_serviceConfigurationId;
}

const statistic& 
statistic_change_event_args::latest_statistic() const
{
    return m_latestStatistic;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END
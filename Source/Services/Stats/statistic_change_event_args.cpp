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
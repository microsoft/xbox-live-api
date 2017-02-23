// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "StatisticChangeEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

StatisticChangeEventArgs::StatisticChangeEventArgs(
    _In_ xbox::services::user_statistics::statistic_change_event_args cppObj
    ) : 
    m_cppObj(std::move(cppObj))
{
    m_latestStatistic = ref new Statistic(m_cppObj.latest_statistic());
}

Statistic^ 
StatisticChangeEventArgs::LatestStatistic::get()
{
    return m_latestStatistic;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "user_statistics_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

RequestedStatistics::RequestedStatistics()
{ }

RequestedStatistics::RequestedStatistics(
    _In_ xsapi_internal_string serviceConfigurationId,
    _In_ xsapi_internal_vector<xsapi_internal_string> statistics
    ) :
    m_serviceConfigurationId(serviceConfigurationId),
    m_statistics(statistics)
{ }

RequestedStatistics::RequestedStatistics(
    _In_ XblRequestedStatistics requestedStatistics
    )
{
    m_serviceConfigurationId = xsapi_internal_string(requestedStatistics.serviceConfigurationId);
    m_statistics = utils::string_array_to_internal_string_vector(requestedStatistics.statistics, requestedStatistics.statisticsCount);
}

const xsapi_internal_string& RequestedStatistics::ServiceConfigurationId() const
{
    return m_serviceConfigurationId;
}

const xsapi_internal_vector<xsapi_internal_string>& RequestedStatistics::Statistics() const
{
    return m_statistics;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END
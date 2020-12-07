// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi_utils.h"
#include "user_statistics_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

UserStatisticsResult::UserStatisticsResult()
{ }

UserStatisticsResult::UserStatisticsResult(
    _In_ xsapi_internal_string xboxUserId,
    _In_ xsapi_internal_vector<ServiceConfigurationStatistic> serviceConfigStatistics
    ) :
    m_xboxUserId(std::move(xboxUserId)),
    m_serviceConfigStatistics(std::move(serviceConfigStatistics))
{ }

const xsapi_internal_string&
UserStatisticsResult::XboxUserId() const
{
    return m_xboxUserId;
}

const xsapi_internal_vector<ServiceConfigurationStatistic>&
UserStatisticsResult::ServiceConfigurationStatistics() const
{
    return m_serviceConfigStatistics;
}

void
UserStatisticsResult::SetServiceConfigurationId(_In_ xsapi_internal_string serviceConfigId)
{
    for (auto& stat : m_serviceConfigStatistics)
    {
        stat.SetServiceConfigurationId(serviceConfigId);
    }
}

/* static */ Result<UserStatisticsResult>
UserStatisticsResult::Deserialize(
    _In_ const JsonValue& json
)
{
    UserStatisticsResult returnResult;
    if (json.IsNull())
    {
        return returnResult;
    }

    xsapi_internal_vector<ServiceConfigurationStatistic> serviceConfigStatisticses;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<ServiceConfigurationStatistic>(ServiceConfigurationStatistic::Deserialize, json, "scids", serviceConfigStatisticses, false));
    if (serviceConfigStatisticses.size() == 0)
    {
        xsapi_internal_vector<Statistic> statistics;
        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<Statistic>(Statistic::Deserialize, json, "stats", statistics, false));
        ServiceConfigurationStatistic serviceConfigStatistics(xsapi_internal_string(), statistics);
        serviceConfigStatisticses.push_back(serviceConfigStatistics);
    }
    xsapi_internal_string xuid;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "xuid", xuid, true));
    returnResult = UserStatisticsResult(
        xuid,
        serviceConfigStatisticses
    );

    return Result<UserStatisticsResult>(returnResult, S_OK);
}

size_t 
UserStatisticsResult::SizeOf() const
{
    size_t size = sizeof(XblUserStatisticsResult);
    
    for (ServiceConfigurationStatistic serviceConfigStat : m_serviceConfigStatistics)
    {
        size += serviceConfigStat.SizeOf();
    }

    return size;
}
char*
UserStatisticsResult::Serialize(char* buffer) const
{
    XblUserStatisticsResult* userStatResult = reinterpret_cast<XblUserStatisticsResult*>(buffer);
    buffer += sizeof(XblUserStatisticsResult);
    return Serialize(userStatResult, buffer);
}

char*
UserStatisticsResult::Serialize(XblUserStatisticsResult* userStatResult, char* buffer) const
{
    userStatResult->xboxUserId = utils::internal_string_to_uint64(m_xboxUserId);

    userStatResult->serviceConfigStatisticsCount = (uint32_t)m_serviceConfigStatistics.size();
    userStatResult->serviceConfigStatistics = reinterpret_cast<XblServiceConfigurationStatistic*>(buffer);
    buffer += sizeof(XblServiceConfigurationStatistic) * m_serviceConfigStatistics.size();

    for (size_t i = 0; i < m_serviceConfigStatistics.size(); i++)
    {
        buffer = m_serviceConfigStatistics[i].Serialize(&userStatResult->serviceConfigStatistics[i], buffer);
    }

    return buffer;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END
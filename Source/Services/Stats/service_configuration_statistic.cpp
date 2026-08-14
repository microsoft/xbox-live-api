// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi_utils.h"
#include "user_statistics_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

ServiceConfigurationStatistic::ServiceConfigurationStatistic()
{ }

ServiceConfigurationStatistic::ServiceConfigurationStatistic(
    _In_ xsapi_internal_string serviceConfigurationId,
    _In_ xsapi_internal_vector<Statistic> stats
    ) :
    m_serviceConfigurationId(serviceConfigurationId),
    m_stats(stats)
{ }

const xsapi_internal_string& 
ServiceConfigurationStatistic::ServiceConfigurationId() const
{
    return m_serviceConfigurationId;
}

const xsapi_internal_vector<Statistic>&
ServiceConfigurationStatistic::Statistics() const
{
    return m_stats;
}

void
ServiceConfigurationStatistic::SetServiceConfigurationId(_In_ xsapi_internal_string serviceConfigId)
{
    m_serviceConfigurationId = std::move(serviceConfigId);
}

/* static */ Result<ServiceConfigurationStatistic>
ServiceConfigurationStatistic::Deserialize(
    _In_ const JsonValue& json
)
{
    ServiceConfigurationStatistic returnResult;

    if (json.IsNull())
    {
        return returnResult;
    }

    xsapi_internal_string scid;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "scid", scid, true));
    if (scid.size() >= sizeof(XblServiceConfigurationStatistic::serviceConfigurationId))
    {
        return Result<ServiceConfigurationStatistic>(returnResult, WEB_E_INVALID_JSON_STRING);
    }
    xsapi_internal_vector<Statistic> stats;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonVector<Statistic>(Statistic::Deserialize, json, "stats", stats, true));
    returnResult = ServiceConfigurationStatistic(
        scid,
        stats
    );

    return Result<ServiceConfigurationStatistic>(returnResult, S_OK);
}

size_t 
ServiceConfigurationStatistic::SizeOf() const
{
    size_t size = sizeof(XblServiceConfigurationStatistic);

    for (Statistic stat : m_stats)
    {
        size += stat.SizeOf();
    }

    return size;
}

char*
ServiceConfigurationStatistic::Serialize(XblServiceConfigurationStatistic* serviceConfigStat, char* buffer) const
{
    // serviceConfigurationId is a fixed-size inline array within XblServiceConfigurationStatistic,
    // and SizeOf() accounts for it at that fixed size. Bound the copy by the destination rather
    // than by the source so an oversized id can never write past the flat result buffer.
    if (m_serviceConfigurationId.size() < sizeof(serviceConfigStat->serviceConfigurationId))
    {
        utils::strcpy(serviceConfigStat->serviceConfigurationId, sizeof(serviceConfigStat->serviceConfigurationId), m_serviceConfigurationId.c_str());
    }
    else
    {
        // Serialize cannot report failure, so an over-long id is surfaced as an empty one.
        // Deserialize rejects these up front; ids supplied directly via SetServiceConfigurationId
        // are not yet validated at their own entry point, so this branch remains reachable.
        serviceConfigStat->serviceConfigurationId[0] = '\0';
    }

    serviceConfigStat->statisticsCount = (uint32_t)m_stats.size();
    serviceConfigStat->statistics = reinterpret_cast<XblStatistic*>(buffer);
    buffer += sizeof(XblStatistic) * m_stats.size();
    
    for (size_t i = 0; i < m_stats.size(); i++)
    {
        buffer = m_stats[i].Serialize(&serviceConfigStat->statistics[i], buffer);
    }

    return buffer;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi_utils.h"
#include "user_statistics_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

Statistic::Statistic()
{ }

// class statistic
Statistic::Statistic(
    _In_ xsapi_internal_string name,
    _In_ xsapi_internal_string type,
    _In_ xsapi_internal_string value
    ) :
    m_statName(std::move(name)),
    m_statType(std::move(type)),
    m_value(std::move(value))
{ }

const xsapi_internal_string&
Statistic::StatisticName() const
{
    return m_statName;
}

const xsapi_internal_string&
Statistic::StatisticType() const
{
    return m_statType;
}

const xsapi_internal_string&
Statistic::Value() const
{
    return m_value;
}

void
Statistic::SetStatisticName(
    _In_ xsapi_internal_string name
    )
{
    m_statName = std::move(name);
}

void 
Statistic::SetStatisticType(
    _In_ xsapi_internal_string type
    )
{
    m_statType = std::move(type);
}

void
Statistic::SetStatisticValue(
    _In_ xsapi_internal_string value
    )
{
    m_value = std::move(value);
}

/* static */ Result<Statistic>
Statistic::Deserialize(
    _In_ const JsonValue& json
)
{
    Statistic returnResult;
    if (json.IsNull())
    {
        return returnResult;
    }

    xsapi_internal_string statname, type, value;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "statname", statname, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "type", type, true));
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "value", value, true));
    returnResult = Statistic(
        statname,
        type,
        value
    );

    return Result<Statistic>(returnResult, S_OK);
}

size_t 
Statistic::SizeOf() const
{
    size_t size = sizeof(XblStatistic);
    size += m_statName.size() + 1;
    size += m_statType.size() + 1;
    size += m_value.size() + 1;
    return size;
}

char*
Statistic::Serialize(XblStatistic* statistic, char* buffer) const
{
    utils::strcpy(buffer, m_statName.size() + 1, m_statName.c_str());
    statistic->statisticName = static_cast<char*>(buffer);
    buffer += m_statName.size() + 1;

    utils::strcpy(buffer, m_statType.size() + 1, m_statType.c_str());
    statistic->statisticType = static_cast<char*>(buffer);
    buffer += m_statType.size() + 1;

    utils::strcpy(buffer, m_value.size() + 1, m_value.c_str());
    statistic->value = static_cast<char*>(buffer);
    buffer += m_value.size() + 1;

    return buffer;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END
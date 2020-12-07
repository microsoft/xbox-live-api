// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "leaderboard_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

LeaderboardColumn::LeaderboardColumn()
{
    m_statName = xsapi_internal_string();
    m_statType = legacy::leaderboard_stat_type::stat_other;
}

LeaderboardColumn::LeaderboardColumn(
    _In_ xsapi_internal_string statName,
    _In_ legacy::leaderboard_stat_type stat_type
    ) :
    m_statName(std::move(statName)),
    m_statType(std::move(stat_type))
{ }

const xsapi_internal_string&
LeaderboardColumn::StatName() const
{
    return m_statName;
}

legacy::leaderboard_stat_type
LeaderboardColumn::StatType() const
{
    return m_statType;
}

/* static */ xbox::services::Result<xbox::services::leaderboard::LeaderboardColumn> LeaderboardColumn::Deserialize(
    _In_ const JsonValue& json
)
{
    xsapi_internal_string statName;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "statName", statName, true));
    xsapi_internal_string statTypeStr;
    RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(json, "type", statTypeStr, true));
    legacy::leaderboard_stat_type statType = legacy::leaderboard_stat_type::stat_other;

    if (utils::str_icmp_internal(statTypeStr, "Integer") == 0)
    {
        statType = legacy::leaderboard_stat_type::stat_uint64;
    }
    else if (utils::str_icmp_internal(statTypeStr, "Double") == 0)
    {
        statType = legacy::leaderboard_stat_type::stat_double;
    }
    else if (utils::str_icmp_internal(statTypeStr, "String") == 0)
    {
        statType = legacy::leaderboard_stat_type::stat_string;
    }

    return LeaderboardColumn(
        std::move(statName),
        statType
    );
}

size_t 
LeaderboardColumn::SizeOf()
{
    size_t size = sizeof(XblLeaderboardColumn);
    size += m_statName.size() + 1;
    size = static_cast<size_t>((size + XBL_ALIGN_SIZE - 1) / XBL_ALIGN_SIZE) * XBL_ALIGN_SIZE;
    return size;
}

char* 
LeaderboardColumn::Serialize(XblLeaderboardColumn* column, char* buffer)
{
    utils::strcpy(buffer, m_statName.size() + 1, m_statName.c_str());
    column->statName = static_cast<char*>(buffer);
    size_t s = m_statName.size() + 1;
    if ((s % XBL_ALIGN_SIZE) != 0) {
        s = static_cast<size_t>((s + XBL_ALIGN_SIZE - 1) / XBL_ALIGN_SIZE) * XBL_ALIGN_SIZE;
    }
    buffer += s;

    XblLeaderboardStatType statType = XblLeaderboardStatType::Other;
    if (m_statType == legacy::leaderboard_stat_type::stat_boolean) statType = XblLeaderboardStatType::Boolean;
    else if (m_statType == legacy::leaderboard_stat_type::stat_double) statType = XblLeaderboardStatType::Double;
    else if (m_statType == legacy::leaderboard_stat_type::stat_string) statType = XblLeaderboardStatType::String;
    else if (m_statType == legacy::leaderboard_stat_type::stat_uint64) statType = XblLeaderboardStatType::Uint64;
    column->statType = statType;

    return buffer;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END
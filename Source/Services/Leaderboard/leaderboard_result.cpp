// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "leaderboard_internal.h"
#include "social_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

LeaderboardResult::LeaderboardResult(
    _In_ uint32_t totalRowCount,
    _In_ String continuationToken,
    _In_ Vector<LeaderboardColumn> columns,
    _In_ Vector<LeaderboardRow> rows
) :
    m_totalRowCount{ totalRowCount },
    m_continuationToken{ std::move(continuationToken) },
    m_columns{ std::move(columns) },
    m_rows{ std::move(rows) }
{
}

uint32_t 
LeaderboardResult::TotalRowCount() const
{
    return m_totalRowCount;
}

const xsapi_internal_vector<LeaderboardColumn>& 
LeaderboardResult::Columns() const
{
    return m_columns;
}

const xsapi_internal_vector<LeaderboardRow>& 
LeaderboardResult::Rows() const
{
    return m_rows;
}

void 
LeaderboardResult::SetNextQuery(std::shared_ptr<LeaderboardGlobalQuery> query)
{
    m_globalQuery = std::move(query);
}

void 
LeaderboardResult::SetNextQuery(std::shared_ptr<LeaderboardSocialQuery> query)
{
    m_socialQuery = std::move(query);
}

void 
LeaderboardResult::ParseAdditionalColumns(const xsapi_internal_vector<xsapi_internal_string>& additionalColumnNames)
{
    xsapi_internal_vector<LeaderboardColumn> columns;
    if (m_columns.size() == 0)
    {
        return;
    }
    columns.push_back(m_columns[0]);

    std::unordered_map<xsapi_internal_string, legacy::leaderboard_stat_type> stats;

    for (auto& row : m_rows)
    {
        for (uint32_t i = 0; i < additionalColumnNames.size(); ++i)
        {
            const xsapi_internal_string& columnName = additionalColumnNames[i];
            auto stat = stats.find(columnName);
            if (row.m_metadata.IsObject() && row.m_metadata.HasMember(columnName.data()))
            {
                const JsonValue& val = row.m_metadata[columnName.c_str()];
                if (stat == stats.end() || stat->second == legacy::leaderboard_stat_type::stat_other)
                {
                    if (val.IsBool())
                    {
                        stats[columnName] = legacy::leaderboard_stat_type::stat_boolean;
                    }
                    else if (val.IsNumber())
                    {
                        stats[columnName] = legacy::leaderboard_stat_type::stat_double;
                    }
                    else if (val.IsString())
                    {
                        stats[columnName] = legacy::leaderboard_stat_type::stat_string;
                    }
                    else
                    {
                        stats[columnName] = legacy::leaderboard_stat_type::stat_other;
                    }

                }

                auto columnValues = JsonUtils::SerializeJson(val);
                if (i >= row.m_columnValues.size() - 1)
                {
                    row.m_columnValues.push_back(columnValues);
                }
                else
                {
                    row.m_columnValues[i] = columnValues;
                }
            }
        }
    }

    for (const auto& columnName : additionalColumnNames)
    {
        columns.push_back(LeaderboardColumn(columnName, stats[columnName]));
    }
    m_columns = columns;
}

bool 
LeaderboardResult::HasNext() const
{
    return !m_continuationToken.empty();
}

Result<LeaderboardResult> LeaderboardResult::Deserialize(_In_ const JsonValue& json)
{
    if (!json.IsObject())
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    // Paging info
    String continuationToken;
    if (json.HasMember("pagingInfo"))
    {
        const JsonValue& pagingInfo = json["pagingInfo"];
        if (!pagingInfo.IsNull())
        {
            RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonString(pagingInfo, "continuationToken", continuationToken, false));
        }
    }
    else
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    // Leaderboard metadata
    uint32_t totalCount{ 0 };
    Vector<LeaderboardColumn> columns;
    if (json.HasMember("leaderboardInfo"))
    {
        const auto& leaderboardInfoJson{ json["leaderboardInfo"] };
        if (!leaderboardInfoJson.IsObject())
        {
            return WEB_E_INVALID_JSON_STRING;
        }

        RETURN_HR_IF_FAILED(JsonUtils::ExtractJsonInt(leaderboardInfoJson, "totalCount", totalCount));

        if (leaderboardInfoJson.HasMember("columnDefinition"))
        {
            // This response schema is used by Global event based stat backed leaderboard queries
            auto columnResult = LeaderboardColumn::Deserialize(leaderboardInfoJson["columnDefinition"]);
            RETURN_HR_IF_FAILED(columnResult.Hresult());
            columns.push_back(columnResult.ExtractPayload());
        }
        else if (leaderboardInfoJson.HasMember("columns") && leaderboardInfoJson["columns"].IsArray())
        {
            // These response schema is used by all other leaderboard queries
            for (auto& columnJson : leaderboardInfoJson["columns"].GetArray())
            {
                auto columnResult = LeaderboardColumn::Deserialize(columnJson);
                RETURN_HR_IF_FAILED(columnResult.Hresult());
                columns.push_back(columnResult.ExtractPayload());
            }
        }
        else
        {
            return WEB_E_INVALID_JSON_STRING;
        }
    }
    else
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    Vector<LeaderboardRow> rows;
    if (json.HasMember("userList") && json["userList"].IsArray())
    {
        const auto& jsonRows = json["userList"].GetArray();
        for (const auto& row : jsonRows)
        {
            auto rowResult = LeaderboardRow::Deserialize(row);
            RETURN_HR_IF_FAILED(rowResult.Hresult());
            rows.push_back(rowResult.ExtractPayload());
        }
    }
    else if (json.HasMember("leaderboard"))
    {

    }
    else
    {
        return WEB_E_INVALID_JSON_STRING;
    }

    return LeaderboardResult{ totalCount, continuationToken, std::move(columns), std::move(rows) };
}

size_t
LeaderboardResult::SizeOfQuery()
{
    size_t size = m_continuationToken.size() + 1;
    if (m_globalQuery)
    {
        size += m_globalQuery->name.size() + 1;
        size += sizeof(char*) * m_globalQuery->columns.size();
        for (auto column : m_globalQuery->columns)
        {
            size += column.size() + 1;
        }
    }
    else if (m_socialQuery)
    {
        size += m_socialQuery->statName.size() + 1;
    }
    return size;
}

XblSocialGroupType 
LeaderboardResult::ParseSocialGroup(xsapi_internal_string socialGroupStr)
{
    XblSocialGroupType socialGroup = XblSocialGroupType::None;

    if (utils::str_icmp_internal(socialGroupStr, xbox::services::social::legacy::social_group_constants::people()) == 0)
    {
        socialGroup = XblSocialGroupType::People;
    }
    else if (utils::str_icmp_internal(socialGroupStr, xbox::services::social::legacy::social_group_constants::favorite()) == 0)
    {
        socialGroup = XblSocialGroupType::Favorites;
    }

    return socialGroup;
}

char*
LeaderboardResult::SerializeQuery(XblLeaderboardQuery* query, char* buffer)
{
    utils::strcpy(buffer, m_continuationToken.size() + 1, m_continuationToken.c_str());
    query->continuationToken = static_cast<char*>(buffer);
    buffer += m_continuationToken.size() + 1;

    if (m_globalQuery)
    {
        utils::strcpy(query->scid, m_globalQuery->scid.size() + 1, m_globalQuery->scid.c_str());
        query->socialGroup = ParseSocialGroup(m_globalQuery->socialGroup);
        query->xboxUserId = m_globalQuery->xuid.empty() ? 0 : utils::internal_string_to_uint64(m_globalQuery->xuid);

        utils::strcpy(buffer, m_globalQuery->name.size() + 1, m_globalQuery->name.c_str());
        if (m_globalQuery->isTitleManaged)
        {
            query->statName = static_cast<char*>(buffer);
        }
        else
        {
            query->leaderboardName = static_cast<char*>(buffer);
        }
        buffer += m_globalQuery->name.size() + 1;

        m_additionalColumnleaderboardNamesC.resize(m_globalQuery->columns.size());
        query->additionalColumnleaderboardNamesCount = m_globalQuery->columns.size();
        query->additionalColumnleaderboardNames = reinterpret_cast<const char**>(buffer);
        buffer += sizeof(char*) * m_globalQuery->columns.size();
        for (size_t i = 0; i < m_globalQuery->columns.size(); i++)
        {
            m_additionalColumnleaderboardNamesC[i] = m_globalQuery->columns[i].c_str();

            utils::strcpy(buffer, m_globalQuery->columns[i].size() + 1, m_additionalColumnleaderboardNamesC[i]);
            query->additionalColumnleaderboardNames[i] = static_cast<char*>(buffer);
            buffer += m_globalQuery->columns[i].size() + 1;
        }
        query->queryType = m_globalQuery->isTitleManaged ? XblLeaderboardQueryType::TitleManagedStatBackedGlobal : XblLeaderboardQueryType::UserStatBacked;
    }
    else if (m_socialQuery)
    {
        utils::strcpy(query->scid, m_socialQuery->scid.size() + 1, m_socialQuery->scid.c_str());
        query->socialGroup = ParseSocialGroup(m_socialQuery->socialGroup);
        query->xboxUserId = m_socialQuery->xuid.empty() ? 0 : utils::internal_string_to_uint64(m_socialQuery->xuid);

        XblLeaderboardSortOrder sortOrder = XblLeaderboardSortOrder::Descending;
        if (utils::str_icmp_internal(m_socialQuery->sortOrder, "ascending") == 0) sortOrder = XblLeaderboardSortOrder::Ascending;
        query->order = sortOrder;

        utils::strcpy(buffer, m_socialQuery->statName.size() + 1, m_socialQuery->statName.c_str());
        query->statName = static_cast<char*>(buffer);
        buffer += m_socialQuery->statName.size() + 1;
        query->queryType = m_socialQuery->isTitleManaged ? XblLeaderboardQueryType::TitleManagedStatBackedSocial : XblLeaderboardQueryType::UserStatBacked;
    }

    return buffer;
}

size_t
LeaderboardResult::SizeOf()
{
    size_t size = sizeof(XblLeaderboardResult);
    for (auto column : m_columns)
    {
        size += column.SizeOf();
    }
    for (auto row : m_rows)
    {
        size += row.SizeOf();
    }
    
    size += SizeOfQuery();

    return size;
}

char*
LeaderboardResult::Serialize(char* buffer)
{
    XblLeaderboardResult* result = reinterpret_cast<XblLeaderboardResult*>(buffer);
    buffer += sizeof(XblLeaderboardResult);

    result->hasNext = !m_continuationToken.empty();
    result->totalRowCount = m_totalRowCount;

    result->columnsCount = m_columns.size();
    result->columns = reinterpret_cast<XblLeaderboardColumn*>(buffer);
    buffer += sizeof(XblLeaderboardColumn) * m_columns.size();
    for (size_t i = 0; i < m_columns.size(); i++)
    {
        buffer = m_columns[i].Serialize(&result->columns[i], buffer);
    }

    result->rowsCount = m_rows.size();
    result->rows = reinterpret_cast<XblLeaderboardRow*>(buffer);
    buffer += sizeof(XblLeaderboardRow) * m_rows.size();
    for (size_t i = 0; i < m_rows.size(); i++)
    {
        buffer = m_rows[i].Serialize(&result->rows[i], buffer);
    }

    return SerializeQuery(&result->nextQuery, buffer);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END
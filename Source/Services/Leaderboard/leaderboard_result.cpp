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
#include "shared_macros.h"
#include "leaderboard_query.h"
#include "xsapi/leaderboard.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

leaderboard_result::leaderboard_result() :
    m_totalRowCount(0)
{
}

leaderboard_result::leaderboard_result(
    _In_ string_t display_name,
    _In_ uint32_t total_row_count,
    _In_ string_t continuationToken,
    _In_ std::vector<leaderboard_column> columns,
    _In_ std::vector<leaderboard_row> rows,
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_displayName(std::move(display_name)),
    m_totalRowCount(total_row_count),
    m_continuationToken(std::move(continuationToken)),
    m_columns(std::move(columns)),
    m_rows(std::move(rows)),
    m_userContext(std::move(userContext)),
    m_appConfig(std::move(appConfig)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings))
{
}

const string_t& leaderboard_result::display_name() const
{
    return m_displayName;
}

uint32_t leaderboard_result::total_row_count() const
{
    return m_totalRowCount;
}

const std::vector<leaderboard_column>& leaderboard_result::columns() const
{
    return m_columns;
}

const std::vector<leaderboard_row>& leaderboard_result::rows() const
{
    return m_rows;
}

void leaderboard_result::_Set_next_query(std::shared_ptr<leaderboard_global_query> query)
{
    m_globalQuery = std::move(query);
}

void leaderboard_result::_Set_next_query(std::shared_ptr<leaderboard_social_query> query)
{
    m_socialQuery = std::move(query);
}

void leaderboard_result::_Parse_additional_columns(const std::vector<string_t>& additionalColumnNames)
{
    std::vector<leaderboard_column> columns;
    if (m_columns.size() == 0)
    {
        return;
    }
    columns.push_back(m_columns[0]);

    std::unordered_map<string_t, leaderboard_stat_type> stats;

    for (auto& row : m_rows)
    {
        for (uint32_t i = 0; i < additionalColumnNames.size(); ++i)
        {
            const string_t& columnName = additionalColumnNames[i];
            auto stat = stats.find(columnName);
            const web::json::value& val = row.m_metadata[columnName];
            if (stat == stats.end() || stat->second == leaderboard_stat_type::stat_other)
            {
                if(val.is_boolean())
                {
                    stats[columnName] = leaderboard_stat_type::stat_boolean;
                }
                else if (val.is_number())
                {
                    stats[columnName] = leaderboard_stat_type::stat_double;
                }
                else if (val.is_string())
                {
                    stats[columnName] = leaderboard_stat_type::stat_string;
                }
                else
                {
                    stats[columnName] = leaderboard_stat_type::stat_other;
                }

            }
            if (i >= row.m_columnValues.size() - 1)
            {
                row.m_columnValues.push_back(val.serialize());
            }
            else
            {
                row.m_columnValues[i] = val.serialize();
            }
        }
    }

    for (const auto& columnName : additionalColumnNames)
    {
        columns.push_back(leaderboard_column(columnName, columnName, stats[columnName]));
    }
    m_columns = columns;
}

bool leaderboard_result::has_next() const
{
    return !m_continuationToken.empty();
}

pplx::task<xbox_live_result<leaderboard_result>> leaderboard_result::get_next(_In_ uint32_t maxItems) const
{
    if (m_continuationToken.empty())
    {
        return pplx::task_from_result(xbox_live_result<leaderboard_result>(xbox_live_error_code::out_of_range, "leadboard_result does not have a next page"));
    }

    leaderboard_service service(m_userContext, m_xboxLiveContextSettings, m_appConfig);

    if (m_globalQuery != nullptr)
    {
        string_t& scid = m_globalQuery->scid;
        string_t& name = m_globalQuery->name;
        string_t& xuid = m_globalQuery->xuid;
        string_t& socialGroup = m_globalQuery->socialGroup;
        std::vector<string_t>& columns = m_globalQuery->columns;
        return service.get_leaderboard_internal(
            scid,
            name,
            NO_SKIP_RANK,
            NO_SKIP_XUID,
            xuid,
            socialGroup,
            maxItems,
            m_continuationToken,
            columns);
    }
    else if (m_socialQuery != nullptr)
    {
        return service.get_leaderboard_for_social_group_internal(
            m_socialQuery->xuid,
            m_socialQuery->scid,
            m_socialQuery->statName,
            m_socialQuery->socialGroup,
            NO_SKIP_RANK,
            NO_SKIP_XUID,
            m_socialQuery->sortOrder,
            maxItems,
            m_continuationToken);
    }

    // This should never happen
    return pplx::task_from_result(xbox_live_result<leaderboard_result>(xbox_live_error_code::runtime_error, "no query found to continue"));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END
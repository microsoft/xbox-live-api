// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "leaderboard_query.h"
#include "xsapi/leaderboard.h"
#include "leaderboard_service_impl.h"

#define NO_SKIP_XUID_I 0

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_BEGIN

leaderboard_result_internal::leaderboard_result_internal() :
    m_totalRowCount(0)
{
}

leaderboard_result_internal::leaderboard_result_internal(
    _In_ xsapi_internal_string display_name,
    _In_ uint32_t total_row_count,
    _In_ xsapi_internal_string continuationToken,
    _In_ std::vector<leaderboard_column> columns,  // TODO: jasonsa leaderboard_column_internal
    _In_ std::vector<leaderboard_row> rows, // TODO: jasonsa leaderboard_row_internal
    _In_ std::shared_ptr<leaderboard_service_impl> leaderboardService
    ) :
    m_displayName(std::move(display_name)),
    m_totalRowCount(total_row_count),
    m_continuationToken(std::move(continuationToken)),
    m_columns(std::move(columns)),
    m_rows(std::move(rows)),
    m_leaderboardService(std::move(leaderboardService)),
    m_version(leaderboard_version_2013)
{
}

const xsapi_internal_string& leaderboard_result_internal::display_name() const
{
    return m_displayName;
}

uint32_t leaderboard_result_internal::total_row_count() const
{
    return m_totalRowCount;
}

const std::vector<leaderboard_column>& leaderboard_result_internal::columns() const
{
    return m_columns;
}

const std::vector<leaderboard_row>& leaderboard_result_internal::rows() const
{
    return m_rows;
}

void leaderboard_result_internal::_Set_next_query(std::shared_ptr<leaderboard_global_query> query)
{
    m_globalQuery = std::move(query);
}

void leaderboard_result_internal::_Set_next_query(std::shared_ptr<leaderboard_social_query> query)
{
    m_socialQuery = std::move(query);
}

void leaderboard_result_internal::_Set_next_query(const leaderboard_query& query)
{
    m_version = leaderboard_version_2017;
    m_nextQuery = std::move(query);
}

void leaderboard_result_internal::_Parse_additional_columns(const xsapi_internal_vector<xsapi_internal_string>& additionalColumnNames)
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
            const string_t& columnName = utils::string_t_from_internal_string(additionalColumnNames[i]);
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
        auto stdColumnName = utils::string_t_from_internal_string(columnName);
        columns.push_back(leaderboard_column(stdColumnName, stdColumnName, stats[stdColumnName]));
    }
    m_columns = columns;
}

bool leaderboard_result_internal::has_next() const
{
    return !m_continuationToken.empty();
}

HRESULT leaderboard_result_internal::get_next(
    _In_ uint32_t maxItems,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<leaderboard_result_internal>>> callback
    ) const
{
    if (m_version == leaderboard_version_2017)
    {
        // This API is NOT supported for using leaderboards that are configured with stats 2017. 
        // Use get_next_query() instead
        return E_NOINTERFACE;
    }

    if (m_continuationToken.empty())
    {
        // leadboard_result does not have a next page
        return E_BOUNDS;
    }
  
    if (m_globalQuery != nullptr)
    {
        return m_leaderboardService->get_leaderboard_internal(
            m_globalQuery->scid,
            m_globalQuery->name,
            NO_SKIP_RANK,
            NO_SKIP_XUID_I,
            m_globalQuery->xuid,
            m_globalQuery->socialGroup,
            maxItems,
            m_continuationToken,
            m_globalQuery->columns,
            m_version,
            queue,
            callback
            );
    }
    else if (m_socialQuery != nullptr)
    {
        return m_leaderboardService->get_leaderboard_for_social_group_internal(
            m_socialQuery->xuid,
            m_socialQuery->scid,
            m_socialQuery->statName,
            m_socialQuery->socialGroup,
            NO_SKIP_RANK,
            NO_SKIP_XUID_I,
            m_socialQuery->sortOrder,
            maxItems,
            m_continuationToken,
            m_version,
            queue,
            callback
            );
    }

    // This should never happen
    return E_FAIL;
}

xbox_live_result<leaderboard_query> leaderboard_result_internal::get_next_query() const
{
    if (m_version == leaderboard_version_2017)
    {
        return xbox_live_result<leaderboard_query>(m_nextQuery);
    }
    else
    {
        return xbox_live_result<leaderboard_query>(xbox_live_error_code::unsupported, "This API is only supported for using leaderboards that are configured with stats 2017. Use get_next() instead");
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_LEADERBOARD_CPP_END
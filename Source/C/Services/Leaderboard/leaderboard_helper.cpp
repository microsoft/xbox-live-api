// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "leaderboard_helper.h"

using namespace xbox::services;
using namespace xbox::services::leaderboard;

leaderboard_result_get_next_taskargs::leaderboard_result_get_next_taskargs(
    _In_ XSAPI_LEADERBOARD_RESULT* _leaderboard,
    _In_ uint32 _maxItems
) : leaderboard(_leaderboard), maxItems(_maxItems)
{
}

XSAPI_LEADERBOARD_COLUMN_IMPL::XSAPI_LEADERBOARD_COLUMN_IMPL(
    _In_ leaderboard_column cppLeaderboardColumn,
    _In_ XSAPI_LEADERBOARD_COLUMN* cLeaderboardColumn
) : m_cLeaderboardColumn(cLeaderboardColumn), m_cppLeaderboardColumn(cppLeaderboardColumn)
{
    m_statName = utils_c::to_utf8string(m_cppLeaderboardColumn.stat_name());
    m_cLeaderboardColumn->statName = m_statName.c_str();

    m_cLeaderboardColumn->statType = static_cast<XSAPI_LEADERBOARD_STAT_TYPE>(m_cppLeaderboardColumn.stat_type());
}

leaderboard_column XSAPI_LEADERBOARD_COLUMN_IMPL::cppLeaderboardColumn() const
{
    return m_cppLeaderboardColumn;
}

XSAPI_LEADERBOARD_ROW_IMPL::XSAPI_LEADERBOARD_ROW_IMPL(
    _In_ leaderboard_row cppLeaderboardRow,
    _In_ XSAPI_LEADERBOARD_ROW* cLeaderboardRow
) : m_cLeaderboardRow(cLeaderboardRow), m_cppLeaderboardRow(cppLeaderboardRow)
{
    m_gamertag = utils_c::to_utf8string(m_cppLeaderboardRow.gamertag());
    m_cLeaderboardRow->gamertag = m_gamertag.c_str();

    m_xboxUserId = utils_c::to_utf8string(m_cppLeaderboardRow.xbox_user_id());
    m_cLeaderboardRow->xboxUserId = m_xboxUserId.c_str();

    m_cLeaderboardRow->percentile = m_cppLeaderboardRow.percentile();

    m_cLeaderboardRow->rank = m_cppLeaderboardRow.rank();

    for (size_t i = 0; i < m_cppLeaderboardRow.column_values().size(); i++)
    {
        m_columnValuesStrs.push_back(utils_c::to_utf8string(m_cppLeaderboardRow.column_values()[i]));
        m_columnValues.push_back(m_columnValuesStrs[i].c_str());
    }
    m_cLeaderboardRow->columnValues = m_columnValues.data();
    m_cLeaderboardRow->columnValuesCount = (uint32_t)m_columnValues.size();
}

leaderboard_row XSAPI_LEADERBOARD_ROW_IMPL::cppLeaderboardRow() const
{
    return m_cppLeaderboardRow;
}

XSAPI_LEADERBOARD_QUERY_IMPL::XSAPI_LEADERBOARD_QUERY_IMPL(
    _In_ leaderboard_query cppQuery,
    _In_ XSAPI_LEADERBOARD_QUERY* cQuery
) : m_cQuery(cQuery), m_cppQuery(cppQuery)
{
    Refresh();
}

leaderboard_query XSAPI_LEADERBOARD_QUERY_IMPL::cppQuery() const
{
    return m_cppQuery;
}

void XSAPI_LEADERBOARD_QUERY_IMPL::Refresh()
{
    m_cQuery->skipResultToMe = m_cppQuery.skip_result_to_me();

    m_cQuery->skipResultToRank = m_cppQuery.skip_result_to_rank();

    m_cQuery->maxItems = m_cppQuery.max_items();

    m_cQuery->order = static_cast<XSAPI_SORT_ORDER>(m_cppQuery.order());

    m_statName = utils_c::to_utf8string(m_cppQuery.stat_name());
    m_cQuery->statName = m_statName.c_str();

    m_socialGroup = utils_c::to_utf8string(m_cppQuery.social_group());
    m_cQuery->socialGroup = m_socialGroup.c_str();

    m_cQuery->hasNext = m_cppQuery.has_next();
}

void XSAPI_LEADERBOARD_QUERY_IMPL::SetSkipResultToMe(bool skipResultToMe)
{
    m_cQuery->skipResultToMe = skipResultToMe;
    m_cppQuery.set_skip_result_to_me(skipResultToMe);
}

void XSAPI_LEADERBOARD_QUERY_IMPL::SetSkipResultToRank(uint32 skipResultToRank)
{
    m_cQuery->skipResultToRank = skipResultToRank;
    m_cppQuery.set_skip_result_to_rank(skipResultToRank);
}

void XSAPI_LEADERBOARD_QUERY_IMPL::SetMaxItems(uint32 maxItems)
{
    m_cQuery->maxItems = maxItems;
    m_cppQuery.set_max_items(maxItems);
}

void XSAPI_LEADERBOARD_QUERY_IMPL::SetOrder(XSAPI_SORT_ORDER order)
{
    m_cQuery->order = order;
    m_cppQuery.set_order(static_cast<sort_order>(order));
}

XSAPI_LEADERBOARD_RESULT_IMPL::XSAPI_LEADERBOARD_RESULT_IMPL(
    _In_ leaderboard_result cppLeaderboardResult,
    _In_ XSAPI_LEADERBOARD_RESULT* cLeaderboardResult
) : m_cLeaderboardResult(cLeaderboardResult), m_cppLeaderboardResult(cppLeaderboardResult)
{
    m_cLeaderboardResult->totalRowCount = m_cppLeaderboardResult.total_row_count();

    for (auto column : m_cppLeaderboardResult.columns())
    {
        m_columns.push_back(CreateLeaderboardColumnFromCpp(column));
    }
    m_cLeaderboardResult->columns = m_columns.data();
    m_cLeaderboardResult->columnsCount = (uint32_t)m_columns.size();

    for (auto row : m_cppLeaderboardResult.rows())
    {
        m_rows.push_back(CreateLeaderboardRowFromCpp(row));
    }
    m_cLeaderboardResult->rows = m_rows.data();
    m_cLeaderboardResult->rowsCount = (uint32_t)m_rows.size();
}

leaderboard_result XSAPI_LEADERBOARD_RESULT_IMPL::cppLeaderboardResult() const
{
    return m_cppLeaderboardResult;
}

XSAPI_LEADERBOARD_COLUMN* CreateLeaderboardColumnFromCpp(
    _In_ leaderboard_column cppLeaderboardColumn
)
{
    auto leaderboardColumn = new XSAPI_LEADERBOARD_COLUMN();
    leaderboardColumn->pImpl = new XSAPI_LEADERBOARD_COLUMN_IMPL(cppLeaderboardColumn, leaderboardColumn);
    return leaderboardColumn;
}

XSAPI_LEADERBOARD_ROW* CreateLeaderboardRowFromCpp(
    _In_ leaderboard_row cppLeaderboardRow
)
{
    // TODO where is this cleaned up?
    auto leaderboardRow = new XSAPI_LEADERBOARD_ROW();
    leaderboardRow->pImpl = new XSAPI_LEADERBOARD_ROW_IMPL(cppLeaderboardRow, leaderboardRow);
    return leaderboardRow;
}

XSAPI_LEADERBOARD_QUERY* CreateLeaderboardQueryFromCpp(
    _In_ leaderboard_query query
)
{
    auto leaderboardQuery = new XSAPI_LEADERBOARD_QUERY();
    leaderboardQuery->pImpl = new XSAPI_LEADERBOARD_QUERY_IMPL(query, leaderboardQuery);

    return leaderboardQuery;
}

XSAPI_LEADERBOARD_RESULT* CreateLeaderboardResultFromCpp(
    _In_ leaderboard_result cppLeaderboardResult
)
{
    auto leaderboardResult = new XSAPI_LEADERBOARD_RESULT();
    leaderboardResult->pImpl = new XSAPI_LEADERBOARD_RESULT_IMPL(cppLeaderboardResult, leaderboardResult);
    return leaderboardResult;
}
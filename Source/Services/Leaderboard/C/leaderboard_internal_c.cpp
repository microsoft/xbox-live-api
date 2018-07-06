// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/leaderboard.h"
#include "xsapi-c/leaderboard_c.h"
#include "../leaderboard_service_impl.h"
#include "leaderboard_internal_c.h"
#include "utils.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::leaderboard;

void create_xbl_leaderboard_row(
    _In_ const leaderboard_row& rhs,
    _Out_ XblLeaderboardRow& lhs
    )
{
    UNREFERENCED_PARAMETER(rhs);

    memset(&lhs, 0, sizeof(XblLeaderboardRow));
    lhs.gamertag = utils::alloc_and_copy_string(rhs.gamertag());
    lhs.xboxUserId = utils::string_t_to_uint64(rhs.xbox_user_id());
    lhs.percentile = rhs.percentile();
    lhs.rank = rhs.rank();

    lhs.columnValuesCount = static_cast<uint32_t>(rhs.column_values().size());
    lhs.columnValues = (const char**)xsapi_memory::mem_alloc(sizeof(const char*) * lhs.columnValuesCount);
    for (uint32_t j = 0; j < lhs.columnValuesCount; ++j)
    {
        lhs.columnValues[j] = utils::alloc_and_copy_string(rhs.column_values()[j]);
    }
}

XblLeaderboardStatType convert_to_XblLeaderboardStatType(leaderboard_stat_type t)
{
    switch (t)
    {
        case leaderboard_stat_type::stat_uint64: return XblLeaderboardStatType_Uint64;
        case leaderboard_stat_type::stat_boolean: return XblLeaderboardStatType_Boolean;
        case leaderboard_stat_type::stat_double: return XblLeaderboardStatType_Double;
        case leaderboard_stat_type::stat_string: return XblLeaderboardStatType_String;

        case leaderboard_stat_type::stat_other: 
        case leaderboard_stat_type::stat_datetime: 
        default:
            return XblLeaderboardStatType_Other;
    }
}

void create_xbl_leaderboard_column(
    _In_ const leaderboard_column& rhs,
    _Out_ XblLeaderboardColumn& lhs
    )
{
    UNREFERENCED_PARAMETER(rhs);

    memset(&lhs, 0, sizeof(XblLeaderboardColumn));
    lhs.statName = utils::alloc_and_copy_string(rhs.stat_name());
    lhs.statType = convert_to_XblLeaderboardStatType(rhs.stat_type());
}


xbl_leaderboard_result::xbl_leaderboard_result(std::shared_ptr<leaderboard_result_internal> _internalResult)
    : internalResult(std::move(_internalResult)),
    refCount(1)
{
    auto& internalColumns = internalResult->columns();
    columns = xsapi_internal_vector<XblLeaderboardColumn>(internalColumns.size());
    for (uint32_t i = 0; i < columns.size(); ++i)
    {
        create_xbl_leaderboard_column(internalColumns[i], columns[i]);
    }

    auto& internalRows = internalResult->rows();
    rows = xsapi_internal_vector<XblLeaderboardRow>(internalRows.size());
    for (uint32_t i = 0; i < rows.size(); ++i)
    {
        create_xbl_leaderboard_row(internalRows[i], rows[i]);
    }
}

xbl_leaderboard_result::~xbl_leaderboard_result()
{
    for (auto& column : columns)
    {
        xsapi_memory::mem_free((void*)column.statName);
    }

    for (auto& row : rows)
    {
        xsapi_memory::mem_free((void*)row.gamertag);

        for (uint32_t i = 0; i < row.columnValuesCount; ++i)
        {
            xsapi_memory::mem_free((void*)row.columnValues[i]);
        }
        xsapi_memory::mem_free(row.columnValues);
    }
}

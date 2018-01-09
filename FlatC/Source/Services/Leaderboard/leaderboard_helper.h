// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/leaderboard_c.h"
#include "taskargs.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::leaderboard;

struct leaderboard_result_get_next_taskargs : public taskargs_with_payload<XSAPI_GET_NEXT_RESULT>
{
    leaderboard_result_get_next_taskargs(
        _In_ XSAPI_LEADERBOARD_RESULT* leaderboard,
        _In_ uint32 maxItems
    );

    XSAPI_LEADERBOARD_RESULT* leaderboard;
    uint32 maxItems;

    XSAPI_LEADERBOARD_RESULT* nextResult;

    std::string resultErrorMsg;
};

struct XSAPI_LEADERBOARD_COLUMN_IMPL {
public:
    XSAPI_LEADERBOARD_COLUMN_IMPL(
        _In_ leaderboard_column cppLeaderboardColumn,
        _In_ XSAPI_LEADERBOARD_COLUMN* cLeaderboardColumn
    );

    leaderboard_column cppLeaderboardColumn() const;

private:
    std::string m_statName;

    leaderboard_column m_cppLeaderboardColumn;
    XSAPI_LEADERBOARD_COLUMN* m_cLeaderboardColumn;
};

struct XSAPI_LEADERBOARD_ROW_IMPL {
public:
    XSAPI_LEADERBOARD_ROW_IMPL(
        _In_ leaderboard_row cppLeaderboardRow,
        _In_ XSAPI_LEADERBOARD_ROW* cLeaderboardRow
    );

    leaderboard_row cppLeaderboardRow() const;

private:
    std::string m_gamertag;
    std::string m_xboxUserId;
    std::vector<std::string> m_columnValuesStrs;
    std::vector<PCSTR> m_columnValues;

    leaderboard_row m_cppLeaderboardRow;
    XSAPI_LEADERBOARD_ROW* m_cLeaderboardRow;
};

struct XSAPI_LEADERBOARD_QUERY_IMPL
{
public:
    XSAPI_LEADERBOARD_QUERY_IMPL(
        _In_ leaderboard_query cppQuery,
        _In_ XSAPI_LEADERBOARD_QUERY* cQuery
    );

    leaderboard_query cppQuery() const;

    void Refresh();
    void SetSkipResultToMe(bool skipResultToMe);
    void SetSkipResultToRank(uint32 skipResultToRank);
    void SetMaxItems(uint32 maxItems);
    void SetOrder(XSAPI_SORT_ORDER order);

private:
    std::string m_statName;
    std::string m_socialGroup;

    leaderboard_query m_cppQuery;
    XSAPI_LEADERBOARD_QUERY* m_cQuery;
};

struct XSAPI_LEADERBOARD_RESULT_IMPL {
public:
    XSAPI_LEADERBOARD_RESULT_IMPL(
        _In_ leaderboard_result cppLeaderboardResult,
        _In_ XSAPI_LEADERBOARD_RESULT* cLeaderboardResult
    );

    leaderboard_result cppLeaderboardResult() const;

private:
    std::vector<XSAPI_LEADERBOARD_COLUMN*> m_columns;
    std::vector<XSAPI_LEADERBOARD_ROW*> m_rows;

    leaderboard_result m_cppLeaderboardResult;
    XSAPI_LEADERBOARD_RESULT* m_cLeaderboardResult;
};

XSAPI_LEADERBOARD_COLUMN* CreateLeaderboardColumnFromCpp(
    _In_ leaderboard_column cppLeaderboardColumn
);

XSAPI_LEADERBOARD_ROW* CreateLeaderboardRowFromCpp(
    _In_ leaderboard_row cppLeaderboardRow
);

XSAPI_LEADERBOARD_QUERY* CreateLeaderboardQueryFromCpp(
    _In_ leaderboard_query query
);

XSAPI_LEADERBOARD_RESULT* CreateLeaderboardResultFromCpp(
    _In_ leaderboard_result cppLeaderboardResult
);
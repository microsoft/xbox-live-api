// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types_c.h"
#include "xsapi-c/errors_c.h"
#include "xsapi-c/system_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if !XDK_API
struct XSAPI_LEADERBOARD_COLUMN_IMPL;
struct XSAPI_LEADERBOARD_ROW_IMPL;
struct XSAPI_LEADERBOARD_QUERY_IMPL;
struct XSAPI_LEADERBOARD_RESULT_IMPL;

typedef enum XSAPI_LEADERBOARD_STAT_TYPE 
{
    XSAPI_LEADERBOARD_STAT_TYPE_STAT_UINT64,
    XSAPI_LEADERBOARD_STAT_TYPE_STAT_BOOLEAN,
    XSAPI_LEADERBOARD_STAT_TYPE_STAT_DOUBLE,
    XSAPI_LEADERBOARD_STAT_TYPE_STAT_STRING,
    XSAPI_LEADERBOARD_STAT_TYPE_STAT_DATETIME,
    XSAPI_LEADERBOARD_STAT_TYPE_STAT_OTHER
} XSAPI_LEADERBOARD_STAT_TYPE;

typedef enum XSAPI_SORT_ORDER
{
    XSAPI_SORT_ORDER_ASCENDING,
    XSAPI_SORT_ORDER_DESCENDING
} XSAPI_SORT_ORDER;

typedef struct XSAPI_LEADERBOARD_COLUMN
{
    PCSTR statName;
    XSAPI_LEADERBOARD_STAT_TYPE statType;

    XSAPI_LEADERBOARD_COLUMN_IMPL* pImpl;
} XSAPI_LEADERBOARD_COLUMN;

typedef struct XSAPI_LEADERBOARD_ROW
{
    PCSTR gamertag;
    PCSTR xboxUserId;
    double percentile;
    uint32_t rank;
    PCSTR* columnValues;
    uint32_t columnValuesCount;

    XSAPI_LEADERBOARD_ROW_IMPL* pImpl;
} XSAPI_LEADERBOARD_ROW;

typedef struct XSAPI_LEADERBOARD_QUERY
{
    bool skipResultToMe;
    uint32_t skipResultToRank;
    uint32_t maxItems;
    XSAPI_SORT_ORDER order;
    PCSTR statName;
    PCSTR socialGroup;
    bool hasNext;

    XSAPI_LEADERBOARD_QUERY_IMPL* pImpl;
} XSAPI_LEADERBOARD_QUERY;

XBL_API XSAPI_LEADERBOARD_QUERY* XBL_CALLING_CONV
LeaderboardQueryCreate();

XBL_API void XBL_CALLING_CONV
LeaderboardQuerySetSkipResultToMe(
    _In_ XSAPI_LEADERBOARD_QUERY* leaderboardQuery,
    _In_ bool skipResultToMe
    );

XBL_API void XBL_CALLING_CONV
LeaderboardQuerySetSkipResultToRank(
    _In_ XSAPI_LEADERBOARD_QUERY* leaderboardQuery,
    _In_ uint32_t skipResultToRank
    );

XBL_API void XBL_CALLING_CONV
LeaderboardQuerySetMaxItems(
    _In_ XSAPI_LEADERBOARD_QUERY* leaderboardQuery,
    _In_ uint32_t maxItems
    );

XBL_API void XBL_CALLING_CONV
LeaderboardQuerySetOrder(
    _In_ XSAPI_LEADERBOARD_QUERY* leaderboardQuery,
    _In_ XSAPI_SORT_ORDER order
    );

typedef struct XSAPI_LEADERBOARD_RESULT
{
    uint32_t totalRowCount;
    XSAPI_LEADERBOARD_COLUMN** columns;
    uint32_t columnsCount;
    XSAPI_LEADERBOARD_ROW** rows;
    uint32_t rowsCount;

    XSAPI_LEADERBOARD_RESULT_IMPL* pImpl;
} XSAPI_LEADERBOARD_RESULT;

XBL_API bool XBL_CALLING_CONV
LeaderboardResultHasNext(
    _In_ XSAPI_LEADERBOARD_RESULT* leaderboardResult
    );

#if !defined(XBOX_LIVE_CREATORS_SDK)
typedef struct XSAPI_GET_NEXT_RESULT_PAYLOAD
{
    XSAPI_LEADERBOARD_RESULT* nextResult;
} XSAPI_GET_NEXT_RESULT_PAYLOAD;

typedef struct XSAPI_GET_NEXT_RESULT
{
    XBL_RESULT_INFO result;
    XSAPI_GET_NEXT_RESULT_PAYLOAD payload;
} XSAPI_GET_NEXT_RESULT;

typedef void(*GET_NEXT_COMPLETION_ROUTINE)(
    _In_ XBL_RESULT_INFO result,
    _In_ XSAPI_GET_NEXT_RESULT payload,
    _In_opt_ void* context
    );

XBL_API XBL_RESULT XBL_CALLING_CONV
LeaderboardResultGetNext(
    _In_ XSAPI_LEADERBOARD_RESULT* leaderboardResult,
    _In_ uint32_t maxItems,
    _In_ GET_NEXT_COMPLETION_ROUTINE completionRoutine,
    _In_opt_ void* completionRoutineContext,
    _In_ uint64_t taskGroupId
    );
#endif

XBL_API XBL_RESULT XBL_CALLING_CONV
LeaderboardResultGetNextQuery(
    _In_ XSAPI_LEADERBOARD_RESULT* leaderboardResult,
    _Out_ XSAPI_LEADERBOARD_QUERY** nextQuery,
    _Out_ PCSTR* errMessage
    );

typedef struct XSAPI_LEADERBOARD_SERVICE
{
    // TODO: implement
} XSAPI_LEADERBOARD_SERVICE;

#endif //!XDK_API

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)
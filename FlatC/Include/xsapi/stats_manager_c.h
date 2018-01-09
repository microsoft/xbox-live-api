// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "types_c.h"
#include "xsapi/errors_c.h"
#include "xsapi/leaderboard_c.h"
#include "xsapi/system_c.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if !XDK_API
struct XSAPI_STAT_EVENT_IMPL;
struct XSAPI_STAT_VALUE_IMPL;
struct XSAPI_LEADERBOARD_RESULT_EVENT_ARGS_IMPL;

typedef enum XSAPI_STAT_DATA_TYPE 
{
    XSAPI_STAT_DATA_TYPE_UNDEFINED,
    XSAPI_STAT_DATA_TYPE_NUMBER,
    XSAPI_STAT_DATA_TYPE_STRING
} XSAPI_STAT_DATA_TYPE;

typedef enum XSAPI_STAT_EVENT_TYPE 
{
    XSAPI_STAT_EVENT_TYPE_LOCAL_USER_ADDED_STAT,
    XSAPI_STAT_EVENT_TYPE_LOCAL_USER_REMOVED_STAT,
    XSAPI_STAT_EVENT_TYPE_STAT_UPDATE_COMPLETE_STAT,
    XSAPI_STAT_EVENT_TYPE_GET_LEADERBOARD_COMPLETE_STAT
} XSAPI_STAT_EVENT_TYPE;

typedef struct XSAPI_STAT_VALUE
{
    PCSTR name;
    double asNumber;
    int64_t asInteger;
    PCSTR asString;
    XSAPI_STAT_DATA_TYPE dataType;

    XSAPI_STAT_VALUE_IMPL* pImpl;
} XSAPI_STAT_VALUE;

typedef struct XSAPI_STAT_EVENT_ARGS
{
} XSAPI_STAT_EVENT_ARGS;

typedef struct XSAPI_LEADERBOARD_RESULT_EVENT_ARGS : XSAPI_STAT_EVENT_ARGS
{
    XSAPI_LEADERBOARD_RESULT* result;

    XSAPI_LEADERBOARD_RESULT_EVENT_ARGS_IMPL* pImpl;
} XSAPI_LEADERBOARD_RESULT_EVENT_ARGS;

typedef struct XSAPI_STAT_EVENT
{
    XSAPI_STAT_EVENT_TYPE eventType;
    XSAPI_STAT_EVENT_ARGS* eventArgs;
    XSAPI_XBOX_LIVE_USER* localUser;
    int32_t errorCode;
    PCSTR errorMessage;

    XSAPI_STAT_EVENT_IMPL* pImpl;
} XSAPI_STAT_EVENT;

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerAddLocalUser(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerRemoveLocalUser(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerRequestFlushToService(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ bool isHighPriority,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_STAT_EVENT** XBL_CALLING_CONV
StatsManagerDoWork(
    _Out_ uint32_t* statEventsCount
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerSetStatisticNumberData(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _In_ double statValue,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerSetStatisticIntegerData(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _In_ int64_t statValue,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerSetStatisticStringData(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _In_ PCSTR statValue,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerGetStatNames(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _Out_ PCSTR** statNameList,
    _Out_ uint32_t* statNameListCount,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerGetStat(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _Out_ XSAPI_STAT_VALUE** statValue,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerDeleteStat(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerGetLeaderboard(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _In_ XSAPI_LEADERBOARD_QUERY* query,
    _Out_ PCSTR* errMessage
    );

XBL_API XSAPI_RESULT XBL_CALLING_CONV
StatsManagerGetSocialLeaderboard(
    _In_ XSAPI_XBOX_LIVE_USER* user,
    _In_ PCSTR statName,
    _In_ PCSTR socialGroup,
    _In_ XSAPI_LEADERBOARD_QUERY* query,
    _Out_ PCSTR* errMessage
    );

#endif //!XDK_API

#if defined(__cplusplus)
} // end extern "C"
#endif // defined(__cplusplus)
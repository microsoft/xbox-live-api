// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/stats_manager_c.h"
#include "xsapi/stats_manager.h"
#include "leaderboard_helper.h"
#include "user_impl_c.h"

struct XSAPI_STAT_VALUE_IMPL
{
public:
    XSAPI_STAT_VALUE_IMPL(
        _In_ xbox::services::stats::manager::stat_value cppStatValue,
        _In_ XSAPI_STAT_VALUE *cStatValue
    );

    xbox::services::stats::manager::stat_value cppStatValue() const;

private:
    std::string m_name;
    std::string m_asString;

    xbox::services::stats::manager::stat_value m_cppStatValue;
    XSAPI_STAT_VALUE *m_cStatValue;
};

struct XSAPI_LEADERBOARD_RESULT_EVENT_ARGS_IMPL 
{
public:
    XSAPI_LEADERBOARD_RESULT_EVENT_ARGS_IMPL(
        _In_ std::shared_ptr<xbox::services::stats::manager::leaderboard_result_event_args> cppEventArgs,
        _In_ XSAPI_LEADERBOARD_RESULT_EVENT_ARGS* cEventArgs
        );

    std::shared_ptr<xbox::services::stats::manager::leaderboard_result_event_args> cppEventArgs() const;

private:
    XSAPI_LEADERBOARD_RESULT* m_result;

    std::shared_ptr<xbox::services::stats::manager::leaderboard_result_event_args> m_cppEventArgs;
    XSAPI_LEADERBOARD_RESULT_EVENT_ARGS* m_cEventArgs;
};

struct XSAPI_STAT_EVENT_IMPL 
{
public:
    XSAPI_STAT_EVENT_IMPL(
        _In_ xbox::services::stats::manager::stat_event cppStatEvent,
        _In_ XSAPI_STAT_EVENT *cStatEvent
        );

    xbox::services::stats::manager::stat_event cppStatEvent() const;

private:
    XSAPI_STAT_EVENT_TYPE m_eventType;
    XSAPI_STAT_EVENT_ARGS *m_args;
    XBL_XBOX_LIVE_USER *m_localUser;
    xbox::services::xbox_live_result<void> m_errorInfo;
    std::error_code m_errorCode;
    std::string m_errorMessage;

    xbox::services::stats::manager::stat_event m_cppStatEvent;
    XSAPI_STAT_EVENT *m_cStatEvent;
};

XSAPI_STAT_VALUE *CreateStatValueFromCpp(
    _In_ xbox::services::stats::manager::stat_value cppStatValue
    );

XSAPI_LEADERBOARD_RESULT_EVENT_ARGS* CreateLeaderboardResultEventArgs(
    _In_ std::shared_ptr<xbox::services::stats::manager::leaderboard_result_event_args> cppArgs
    );

XSAPI_STAT_EVENT *CreateStatEventFromCpp(
    _In_ xbox::services::stats::manager::stat_event cppEvent
    );
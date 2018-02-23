// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "stats_manager_helper.h"
#include "stats_manager_state.h"

using namespace xbox::services;
using namespace xbox::services::stats::manager;

XSAPI_STAT_VALUE_IMPL::XSAPI_STAT_VALUE_IMPL(
    _In_ stat_value cppStatValue,
    _In_ XSAPI_STAT_VALUE *cStatValue
) : m_cStatValue(cStatValue), m_cppStatValue(cppStatValue)
{
    m_name = utils::utf8_from_utf16(m_cppStatValue.name());
    m_cStatValue->name = m_name.c_str();

    m_cStatValue->asNumber = m_cppStatValue.as_number();

    m_cStatValue->asInteger = m_cppStatValue.as_integer();

    m_asString = utils::utf8_from_utf16(m_cppStatValue.as_string());
    m_cStatValue->asString = m_asString.c_str();

    m_cStatValue->dataType = static_cast<XSAPI_STAT_DATA_TYPE>(m_cppStatValue.data_type());
}

stat_value XSAPI_STAT_VALUE_IMPL::cppStatValue() const
{
    return m_cppStatValue;
}

XSAPI_LEADERBOARD_RESULT_EVENT_ARGS_IMPL::XSAPI_LEADERBOARD_RESULT_EVENT_ARGS_IMPL(
    _In_ std::shared_ptr<leaderboard_result_event_args> cppEventArgs,
    _In_ XSAPI_LEADERBOARD_RESULT_EVENT_ARGS* cEventArgs
    ) :
    m_cEventArgs(cEventArgs),
    m_cppEventArgs(cppEventArgs)
{
    auto result = m_cppEventArgs->result();

    m_result = CreateLeaderboardResultFromCpp(result.payload());
    m_cEventArgs->result = m_result;
}

std::shared_ptr<leaderboard_result_event_args> XSAPI_LEADERBOARD_RESULT_EVENT_ARGS_IMPL::cppEventArgs() const
{
    return m_cppEventArgs;
}

XSAPI_STAT_EVENT_IMPL::XSAPI_STAT_EVENT_IMPL(
    _In_ stat_event cppStatEvent,
    _In_ XSAPI_STAT_EVENT *cStatEvent
) : m_cStatEvent(cStatEvent), m_cppStatEvent(cppStatEvent)
{
    m_eventType = static_cast<XSAPI_STAT_EVENT_TYPE>(m_cppStatEvent.event_type());
    m_cStatEvent->eventType = m_eventType;

    m_args = nullptr;
    if (m_cppStatEvent.event_args()) {
        try
        {
            auto cppEventArgs = std::dynamic_pointer_cast<leaderboard_result_event_args>(m_cppStatEvent.event_args());
            m_args = CreateLeaderboardResultEventArgs(cppEventArgs);
        }
        catch (const std::exception&)
        {
            // not leaderboard_result_event_args
        }
    }
    m_cStatEvent->eventArgs = m_args;

    auto mapping = get_xsapi_singleton()->m_statsVars->cUsersMapping;
    if (mapping.find(m_cppStatEvent.local_user()) != mapping.end())
    {
        m_localUser = mapping[m_cppStatEvent.local_user()];
    }
    else
    {
        throw new std::exception("User doesn't exist. Did you call AddLocalUser?");
    }
    m_cStatEvent->localUser = m_localUser;

    m_errorInfo = m_cppStatEvent.error_info();

    m_errorCode = m_errorInfo.err();
    m_cStatEvent->errorCode = m_errorCode.value();

    m_errorMessage = m_errorInfo.err_message();
    m_cStatEvent->errorMessage = m_errorMessage.c_str();
}

stat_event XSAPI_STAT_EVENT_IMPL::cppStatEvent() const
{
    return m_cppStatEvent;
}

XSAPI_STAT_VALUE *CreateStatValueFromCpp(
    _In_ stat_value cppStatValue
)
{
    auto cStatValue = new XSAPI_STAT_VALUE();
    cStatValue->pImpl = new XSAPI_STAT_VALUE_IMPL(cppStatValue, cStatValue);

    return cStatValue;
}

XSAPI_LEADERBOARD_RESULT_EVENT_ARGS* CreateLeaderboardResultEventArgs(
    _In_ std::shared_ptr<leaderboard_result_event_args> cppArgs
)
{
    auto cppResult = cppArgs->result();
    auto args = new XSAPI_LEADERBOARD_RESULT_EVENT_ARGS();
    args->pImpl = new XSAPI_LEADERBOARD_RESULT_EVENT_ARGS_IMPL(cppArgs, args);
    return args;
}

XSAPI_STAT_EVENT *CreateStatEventFromCpp(
    _In_ stat_event cppEvent
)
{
    auto cEvent = new XSAPI_STAT_EVENT();
    cEvent->pImpl = new XSAPI_STAT_EVENT_IMPL(cppEvent, cEvent);

    return cEvent;
}
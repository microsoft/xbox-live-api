// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/stats_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN

stat_event::stat_event(
    stat_event_type eventType,
    xbox_live_user_t user,
    xbox_live_result<void> errorInfo,
    std::shared_ptr<stat_event_args> args
    ) :
    m_eventType(eventType),
    m_localUser(std::move(user)),
    m_errorInfo(std::move(errorInfo)),
    m_eventArgs(args)
{
}

xbox_live_result<void>&
stat_event::error_info()
{
    return m_errorInfo;
}

stat_event_type
stat_event::event_type() const
{
    return m_eventType;
}

std::shared_ptr<stat_event_args> stat_event::event_args() const
{
    return m_eventArgs;
}

const xbox_live_user_t&
stat_event::local_user() const
{
    return m_localUser;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END
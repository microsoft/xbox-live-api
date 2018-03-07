// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_manager_internal.h"
#include "xsapi/presence.h"

using namespace xbox::services::presence;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

unprocessed_social_event::unprocessed_social_event(
    _In_ unprocessed_social_event_type eventType,
    _In_ xsapi_internal_vector<xbox_social_user> usersAffected
    ) :
    m_socialEventType(eventType),
    m_usersAffected(std::move(usersAffected))
{
    for (auto& user : m_usersAffected)
    {
        m_usersAffectedAsStringVec.push_back(utils::internal_string_from_string_t(user.xbox_user_id()));
    }
}

unprocessed_social_event::unprocessed_social_event(
    _In_ unprocessed_social_event_type eventType, 
    _In_ xsapi_internal_vector<social_manager_presence_record> presenceRecords
    ) :
    m_socialEventType(eventType),
    m_presenceRecords(std::move(presenceRecords))
{
    for (auto& record : m_presenceRecords)
    {
        xsapi_internal_stringstream str;
        str << record._Xbox_user_id();
        m_usersAffectedAsStringVec.push_back(str.str());
    }
}

unprocessed_social_event::unprocessed_social_event(
    _In_ unprocessed_social_event_type eventType,
    _In_ std::shared_ptr<device_presence_change_event_args_internal> devicePresenceArgs
    ) :
    m_socialEventType(eventType),
    m_devicePresenceArgs(std::move(devicePresenceArgs))
{
    m_usersAffectedAsStringVec.push_back(m_devicePresenceArgs->xbox_user_id());
}

unprocessed_social_event::unprocessed_social_event(
    _In_ unprocessed_social_event_type eventType,
    _In_ std::shared_ptr<xbox::services::presence::title_presence_change_event_args_internal> titlePresenceArgs
    ) :
    m_socialEventType(eventType),
    m_titlePresenceArgs(std::move(titlePresenceArgs))
{
    m_usersAffectedAsStringVec.push_back(m_titlePresenceArgs->xbox_user_id());
}

unprocessed_social_event::unprocessed_social_event(
    _In_ unprocessed_social_event_type eventType,
    _In_ xsapi_internal_vector<uint64_t> userList
    ) :
    m_socialEventType(eventType),
    m_userList(std::move(userList))
{
    for (auto& user : m_userList)
    {
        xsapi_internal_stringstream str;
        str << user;
        m_usersAffectedAsStringVec.push_back(str.str());
    }
}

unprocessed_social_event::unprocessed_social_event(
    _In_ unprocessed_social_event_type eventType,
    _In_ xsapi_internal_vector<xsapi_internal_string> userAddList,
    _In_ xbox_live_callback<xbox_live_result<void>> _callback
    ) :
    m_socialEventType(eventType),
    m_usersAffectedAsStringVec(std::move(userAddList)),
    callback(std::move(_callback))
{
}

unprocessed_social_event::unprocessed_social_event(
    _In_ unprocessed_social_event_type socialEventType,
    _In_ xbox_live_result<void> errorInfo,
    _In_ xsapi_internal_vector<xsapi_internal_string> userList
    ) :
    m_socialEventType(socialEventType),
    m_error(std::move(errorInfo)),
    m_usersAffectedAsStringVec(std::move(userList))
{
}

unprocessed_social_event::unprocessed_social_event(
    _In_ unprocessed_social_event_type eventType,
    _In_ xsapi_internal_vector<xsapi_internal_string> userAddList
    ) :
    m_socialEventType(eventType),
    m_usersAffectedAsStringVec(std::move(userAddList))
{
}

unprocessed_social_event_type
unprocessed_social_event::event_type() const
{
    return m_socialEventType;
}

const xsapi_internal_vector<xbox_social_user>&
unprocessed_social_event::users_affected() const
{
    return m_usersAffected;
}

const xsapi_internal_vector<uint64_t>&
unprocessed_social_event::users_to_remove() const
{
    return m_userList;
}

const xsapi_internal_vector<social_manager_presence_record>&
unprocessed_social_event::presence_records() const
{
    return m_presenceRecords;
}

const std::shared_ptr<device_presence_change_event_args_internal>
unprocessed_social_event::device_presence_args() const
{
    return m_devicePresenceArgs;
}

const std::shared_ptr<title_presence_change_event_args_internal>
unprocessed_social_event::title_presence_args() const
{
    return m_titlePresenceArgs;
}

const xsapi_internal_vector<xsapi_internal_string>&
unprocessed_social_event::users_affected_as_string_vec() const
{
    return m_usersAffectedAsStringVec;
}

const xbox_live_result<void>&
unprocessed_social_event::error() const
{
    return m_error;
}

std::shared_ptr<call_buffer_timer_completion_context>
unprocessed_social_event::completion_context() const
{
    return m_completionContext;
}

void
unprocessed_social_event::set_completion_context(
    _In_ std::shared_ptr<call_buffer_timer_completion_context> compleitionContext
    )
{
    m_completionContext = compleitionContext;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
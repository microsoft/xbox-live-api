// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social_manager.h"
#include "user_context.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

social_event::social_event() :
    m_eventType(social_event_type::unknown)
{
}

social_event::social_event(
    _In_ xbox_live_user_t user,
    _In_ social_event_type eventType,
    _In_ std::vector<xbox_user_id_container> usersAffected,
    _In_ std::shared_ptr<social_event_args> socialEventArgs,
    _In_ std::error_code errCode,
    _In_ std::string errMessage
    ) :
    m_user(std::move(user)),
    m_eventType(eventType),
    m_usersAffected(std::move(usersAffected)),
    m_eventArgs(std::move(socialEventArgs)),
    m_errCode(std::move(errCode)),
    m_errMessage(std::move(errMessage))
{
}

xbox_live_user_t
social_event::user() const
{
    return m_user;
}

social_event_type
social_event::event_type() const
{
    return m_eventType;
}

const std::vector<xbox_user_id_container>&
social_event::users_affected() const
{
    return m_usersAffected;
}

const std::error_code&
social_event::err() const
{
    return m_errCode;
}

const std::string&
social_event::err_message() const
{
    return m_errMessage;
}

const std::shared_ptr<social_event_args>&
social_event::event_args() const
{
    return m_eventArgs;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
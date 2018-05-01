// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social_manager.h"
#include "social_manager_internal.h"
#include "user_context.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

social_event::social_event(
    _In_ std::shared_ptr<social_event_internal> internalObj
    ) :
    m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_OBJECT(social_event, xbox_live_user_t, user);
DEFINE_GET_ENUM_TYPE(social_event, social_event_type, event_type);
DEFINE_GET_VECTOR(social_event, xbox_user_id_container, users_affected);

std::shared_ptr<social_event_args> social_event::event_args() const
{
    switch (m_internalObj->event_type())
    {
    case social_event_type::social_user_group_loaded:
        return xsapi_allocate_shared<social_user_group_loaded_event_args>(
            std::dynamic_pointer_cast<social_user_group_loaded_event_args_internal>(m_internalObj->event_args())
            );
    default:
        return nullptr;
    }
}

DEFINE_GET_OBJECT_REF(social_event, std::error_code, err);
DEFINE_GET_STD_STRING(social_event, err_message);

social_event_internal::social_event_internal(
    _In_ xbox_live_user_t user,
    _In_ social_event_type eventType,
    _In_ xsapi_internal_vector<xbox_user_id_container> usersAffected,
    _In_ std::shared_ptr<social_event_args> socialEventArgs,
    _In_ std::error_code errCode,
    _In_ xsapi_internal_string errMessage
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
social_event_internal::user() const
{
    return m_user;
}

social_event_type
social_event_internal::event_type() const
{
    return m_eventType;
}

const xsapi_internal_vector<xbox_user_id_container>&
social_event_internal::users_affected() const
{
    return m_usersAffected;
}

const std::error_code&
social_event_internal::err() const
{
    return m_errCode;
}

const xsapi_internal_string&
social_event_internal::err_message() const
{
    return m_errMessage;
}

const std::shared_ptr<social_event_args>&
social_event_internal::event_args() const
{
    return m_eventArgs;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
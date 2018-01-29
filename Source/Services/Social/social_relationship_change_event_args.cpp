// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"
#include "social_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

social_relationship_change_event_args::social_relationship_change_event_args(
    _In_ std::shared_ptr<social_relationship_change_event_args_internal> internalObj
    )
    : m_internalObj(std::move(internalObj))
{
}

DEFINE_GET_STRING(social_relationship_change_event_args, caller_xbox_user_id);
DEFINE_GET_ENUM_TYPE(social_relationship_change_event_args, social_notification_type, social_notification);
DEFINE_GET_STRING_VECTOR(social_relationship_change_event_args, xbox_user_ids);

social_relationship_change_event_args_internal::social_relationship_change_event_args_internal(
    _In_ xsapi_internal_string callerXboxUserId,
    _In_ social_notification_type notificationType,
    _In_ xsapi_internal_vector<xsapi_internal_string> xboxUserIds
    ) :
    m_callerXboxUserId(std::move(callerXboxUserId)),
    m_notificationType(notificationType),
    m_xboxUserIds(std::move(xboxUserIds))
{
}

const xsapi_internal_string&
social_relationship_change_event_args_internal::caller_xbox_user_id() const
{
    return m_callerXboxUserId;
}

social_notification_type
social_relationship_change_event_args_internal::social_notification() const
{
    return m_notificationType;
}

const xsapi_internal_vector<xsapi_internal_string>&
social_relationship_change_event_args_internal::xbox_user_ids() const
{
    return m_xboxUserIds;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END
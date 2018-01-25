// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

social_relationship_change_event_args::social_relationship_change_event_args()
{
}

social_relationship_change_event_args::social_relationship_change_event_args(
    _In_ xsapi_internal_string callerXboxUserId,
    _In_ social_notification_type notificationType,
    _In_ xsapi_internal_vector<xsapi_internal_string> xboxUserIds
    ) :
    m_callerXboxUserId(std::move(callerXboxUserId)),
    m_notificationType(notificationType),
    m_xboxUserIds(std::move(xboxUserIds))
{
}

string_t
social_relationship_change_event_args::caller_xbox_user_id() const
{
    return utils::external_string_from_internal_string(m_callerXboxUserId);
}

social_notification_type
social_relationship_change_event_args::social_notification() const
{
    return m_notificationType;
}

std::vector<string_t>
social_relationship_change_event_args::xbox_user_ids() const
{
    return utils::std_string_vector_from_internal_string_vector(m_xboxUserIds);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END
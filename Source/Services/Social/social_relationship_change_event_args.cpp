//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/social.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_BEGIN

social_relationship_change_event_args::social_relationship_change_event_args()
{
}

social_relationship_change_event_args::social_relationship_change_event_args(
    _In_ string_t callerXboxUserId,
    _In_ social_notification_type notificationType,
    _In_ std::vector<string_t> xboxUserIds
    ) :
    m_callerXboxUserId(std::move(callerXboxUserId)),
    m_notificationType(notificationType),
    m_xboxUserIds(std::move(xboxUserIds))
{
}

const string_t&
social_relationship_change_event_args::caller_xbox_user_id() const
{
    return m_callerXboxUserId;
}

social_notification_type
social_relationship_change_event_args::social_notification() const
{
    return m_notificationType;
}

const std::vector<string_t>&
social_relationship_change_event_args::xbox_user_ids() const
{
    return m_xboxUserIds;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_CPP_END
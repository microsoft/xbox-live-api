// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/multiplayer_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

join_lobby_completed_event_args::join_lobby_completed_event_args(
    _In_ string_t xboxUserId
    ):
    m_invitedXboxUserid(std::move(xboxUserId))
{
}

const string_t&
join_lobby_completed_event_args::invited_xbox_user_id() const
{
    return m_invitedXboxUserid;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
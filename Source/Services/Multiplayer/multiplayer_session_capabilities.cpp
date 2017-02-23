// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session_capabilities::multiplayer_session_capabilities():
    m_connectivity(false),
    m_suppressPresenceActivityCheck(false),
    m_gameplay(false),
    m_large(false),
    m_connectionRequiredForActiveMembers(false),
    m_userAuthorizationStyle(false),
    m_crossplay(false),
    m_team(false),
    m_arbitration(false),
    m_searchable(false),
    m_hasOwners(false)
{
}

bool
multiplayer_session_capabilities::connectivity() const
{
    return m_connectivity;
}

void 
multiplayer_session_capabilities::set_connectivity(
    _In_ bool connectivity
    )
{
    m_connectivity = connectivity;
}

bool multiplayer_session_capabilities::team() const
{
    return m_team;
}

void 
multiplayer_session_capabilities::set_team(
    _In_ bool team
    )
{
    m_team = team;
}

bool multiplayer_session_capabilities::arbitration() const
{
    return m_arbitration;
}

void 
multiplayer_session_capabilities::set_arbitration(
    _In_ bool arbitration
    )
{
    m_arbitration = arbitration;
}


bool
multiplayer_session_capabilities::suppress_presence_activity_check() const
{
    return m_suppressPresenceActivityCheck;
}


void 
multiplayer_session_capabilities::set_suppress_presence_activity_check(
    _In_ bool suppressPresenceActivityCheck
    )
{
    m_suppressPresenceActivityCheck = suppressPresenceActivityCheck;
}

bool
multiplayer_session_capabilities::gameplay() const
{
    return m_gameplay;
}

void
multiplayer_session_capabilities::set_gameplay(
    _In_ bool gameplay
    )
{
    m_gameplay = gameplay;
}

bool
multiplayer_session_capabilities::large() const
{
    return m_large;
}

void 
multiplayer_session_capabilities::set_large(
    _In_ bool large
    )
{
    m_large = large;
}

bool
multiplayer_session_capabilities::connection_required_for_active_members() const
{
    return m_connectionRequiredForActiveMembers;
}

void
multiplayer_session_capabilities::set_connection_required_for_active_members(
    _In_ bool connectionRequired
    )
{
    m_connectionRequiredForActiveMembers = connectionRequired;
}

bool
multiplayer_session_capabilities::user_authorization_style() const
{
    return m_userAuthorizationStyle;
}

void
multiplayer_session_capabilities::set_user_authorization_style(
    _In_ bool userAuthorizationStyle
    )
{
    m_userAuthorizationStyle = userAuthorizationStyle;
}

bool
multiplayer_session_capabilities::crossplay() const
{
    return m_crossplay;
}

void
multiplayer_session_capabilities::set_crossplay(
    _In_ bool crossplay
    )
{
    m_crossplay = crossplay;
}

bool
multiplayer_session_capabilities::searchable() const
{
    return m_searchable;
}

void
multiplayer_session_capabilities::set_searchable(
    _In_ bool searchable
    )
{
    m_searchable = searchable;
}

bool
multiplayer_session_capabilities::has_owners() const
{
    return m_hasOwners;
}

void
multiplayer_session_capabilities::set_has_owners(
    _In_ bool hasOwners
    )
{
    m_hasOwners = hasOwners;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
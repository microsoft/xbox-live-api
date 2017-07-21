// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

const string_t& club_user_presence_record::xuid() const
{
    return m_xuid;
}

const utility::datetime& club_user_presence_record::last_seen() const
{
    return m_lastSeen;
}

club_user_presence club_user_presence_record::last_seen_state() const
{
    return m_lastSeenState;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END

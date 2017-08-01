// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

const std::vector<club_role_record>& club_roster::moderators() const
{
    return m_moderators;
}

const std::vector<club_role_record>& club_roster::requested_to_join() const
{
    return m_requestedToJoin;
}

const std::vector<club_role_record>& club_roster::recommended() const
{
    return m_recommended;
}

const std::vector<club_role_record>& club_roster::banned() const
{
    return m_banned;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END
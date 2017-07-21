// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

const string_t& club_role_record::xuid() const
{
    return m_xuid;
}

club_role club_role_record::role() const
{
    return m_role;
}

const string_t& club_role_record::actor_xuid() const
{
    return m_actorXuid;
}

const utility::datetime& club_role_record::created_date() const
{
    return m_createdDate;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END
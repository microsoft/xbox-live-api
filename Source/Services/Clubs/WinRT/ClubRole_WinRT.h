// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public enum class ClubRole
{
    /// <summary>Not a member of the club. Used exclusively for permissions/settings</summary>
    Nonmember = xbox::services::clubs::club_role::nonmember,

    /// <summary>Member of a club</summary>
    Member = xbox::services::clubs::club_role::member,

    /// <summary>Moderator of a club</summary>
    Moderator = xbox::services::clubs::club_role::moderator,

    /// <summary>Owner of a club</summary>
    Owner = xbox::services::clubs::club_role::owner,

    /// <summary>User has requested to join a club</summary>
    RequestedToJoin = xbox::services::clubs::club_role::requested_to_join,

    /// <summary>User has been recommended for a club</summary>
    Recommended = xbox::services::clubs::club_role::recommended,

    /// <summary>User has been invited to a club</summary>
    Invited = xbox::services::clubs::club_role::invited,

    /// <summary>
    /// User has been banned from all interaction with a club. A user cannot have any other role
    /// with a club if they are banned from it
    /// </summary>
    Banned = xbox::services::clubs::club_role::banned,

    /// <summary>Follower of a club</summary>
    Follower = xbox::services::clubs::club_role::follower
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END
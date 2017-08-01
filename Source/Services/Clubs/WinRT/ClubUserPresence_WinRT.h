// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public enum class ClubUserPresence
{
    /// <summary>User is no longer on a club page.</summary>
    NotInClub = xbox::services::clubs::club_user_presence::not_in_club,

    /// <summary>User is viewing the club, but not on any specific page.</summary>
    InClub = xbox::services::clubs::club_user_presence::in_club,

    /// <summary>User is on the chat page.</summary>
    Chat = xbox::services::clubs::club_user_presence::chat,

    /// <summary>User is viewing the club feed.</summary>
    Feed = xbox::services::clubs::club_user_presence::feed,

    /// <summary>User is viewing the club roster/presence.</summary>
    Roster = xbox::services::clubs::club_user_presence::roster,

    /// <summary>User is on the play tab in the club (not actually playing anything).</summary>
    Play = xbox::services::clubs::club_user_presence::play,

    /// <summary>User is playing the associated game.</summary>
    InGame = xbox::services::clubs::club_user_presence::in_game
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END
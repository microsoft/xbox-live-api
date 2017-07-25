// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "ClubUserPresence_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubUserPresenceRecord sealed
{
public:
    /// <summary>Xuid of the user who was present at the club.</summary>
    DEFINE_PROP_GET_STR_OBJ(Xuid, xuid);

    /// <summary>Time when the user was last present within the club.</summary>
    DEFINE_PROP_GET_DATETIME_OBJ(LastSeen, last_seen);

    /// <summary>Users state when they were last seen.</summary>
    DEFINE_PROP_GET_ENUM_OBJ(LastSeenState, last_seen_state, ClubUserPresence);

internal:
    ClubUserPresenceRecord(_In_ xbox::services::clubs::club_user_presence_record cppObj);

private:
    xbox::services::clubs::club_user_presence_record m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END
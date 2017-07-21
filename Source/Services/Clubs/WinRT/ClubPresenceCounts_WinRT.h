// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubPresenceCounts sealed
{
public:
    /// <summary>The total number of club members that been tracked by club presence</summary>
    DEFINE_PROP_GET_OBJ(TotalCount, total_count, uint32);
    
    /// <summary>The number of members who are active within the club</summary>
    DEFINE_PROP_GET_OBJ(HereNow, here_now, uint32);

    /// <summary>The number of member who have been active within the last 24 hours</summary>
    DEFINE_PROP_GET_OBJ(HereToday, here_today, uint32);

    /// <summary>The number of members who are currently in game</summary>
    DEFINE_PROP_GET_OBJ(InGameNow, in_game_now, uint32);

internal:
    ClubPresenceCounts(_In_ xbox::services::clubs::club_presence_counts cppObj);

private:
    xbox::services::clubs::club_presence_counts m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public enum class ClubType
{
    /// <summary>Unknown club type</summary>
    Unknown = xbox::services::clubs::club_type::unknown,

    /// <summary>Open club</summary>
    Public = xbox::services::clubs::club_type::public_club,

    /// <summary>Closed club</summary>
    Private = xbox::services::clubs::club_type::private_club,

    /// <summary>Secret club</summary>
    Hidden = xbox::services::clubs::club_type::hidden_club
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END
//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>Enumeration values that indicate the participation type for an achievement.</summary>
public enum class AchievementParticipationType
{
    /// <summary>The participation type is unknown.</summary>
    Unknown = xbox::services::achievements::achievement_participation_type::unknown,

    /// <summary>An achievement that can be earned as an individual participant.</summary>
    Individual = xbox::services::achievements::achievement_participation_type::individual,

    /// <summary>An achievement that can be earned as a group participant.</summary>
    Group = xbox::services::achievements::achievement_participation_type::group
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
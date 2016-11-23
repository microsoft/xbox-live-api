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

/// <summary>Enumeration values that indicate the achievement type.</summary>
public enum class AchievementType
{
    /// <summary>The achievement type is unknown.</summary>
    Unknown = xbox::services::achievements::achievement_type::unknown,

    /// <summary>Used as a request input parameter.
    /// All means to get all achievements regardless of type.</summary>
    All = xbox::services::achievements::achievement_type::all,

    /// <summary>A persistent achievement that may be unlocked at any time.
    /// Persistent achievements can give Gamerscore as a reward.</summary>
    Persistent = xbox::services::achievements::achievement_type::persistent,

    /// <summary>A challenge achievement that can only be unlocked within a certain time period.
    /// Challenge achievements can't give Gamerscore as a reward.</summary>
    Challenge = xbox::services::achievements::achievement_type::challenge
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
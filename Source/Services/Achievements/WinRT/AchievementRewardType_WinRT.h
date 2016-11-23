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

/// <summary>Enumeration values that indicate the reward type for an achievement.</summary>
public enum class AchievementRewardType
{
    /// <summary>The reward type is unknown.</summary>
    Unknown = xbox::services::achievements::achievement_reward_type::unknown,

    /// <summary>A Gamerscore reward.</summary>
    Gamerscore = xbox::services::achievements::achievement_reward_type::gamerscore,

    /// <summary>An in-app reward, defined and delivered by the title.</summary>
    InApp = xbox::services::achievements::achievement_reward_type::in_app,

    /// <summary>A digital art reward.</summary>
    Art = xbox::services::achievements::achievement_reward_type::art
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END

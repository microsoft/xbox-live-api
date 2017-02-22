// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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

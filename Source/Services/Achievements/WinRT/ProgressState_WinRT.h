// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN
    
/// <summary>Enumeration values that indicate the state of a player's progress towards unlocking an achievement.</summary>
public enum class AchievementProgressState
{
    /// <summary>Achievement progress is unknown.</summary>
    Unknown = xbox::services::achievements::achievement_progress_state::unknown, 

    /// <summary>Achievement has been earned.</summary>
    Achieved = xbox::services::achievements::achievement_progress_state::achieved,

    /// <summary>Achievement progress has not been started.</summary>
    NotStarted = xbox::services::achievements::achievement_progress_state::not_started,

    /// <summary>Achievement progress has started.</summary>
    InProgress = xbox::services::achievements::achievement_progress_state::in_progress
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END

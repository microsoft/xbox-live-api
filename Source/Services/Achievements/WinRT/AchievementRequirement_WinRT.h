// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>
/// Represents requirements for unlocking the achievement.
/// </summary>
public ref class AchievementRequirement sealed
{
public:
    /// <summary>
    /// The achievement requirement ID.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Id, id);

    /// <summary>
    /// A value that indicates the current progress of the player towards meeting
    /// the requirement.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(CurrentProgressValue, current_progress_value);

    /// <summary>
    /// The target progress value that the player must reach in order to meet
    /// the requirement.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(TargetProgressValue, target_progress_value);

internal:
    AchievementRequirement(
        _In_ xbox::services::achievements::achievement_requirement cppObj
        ) : 
        m_cppObj(std::move(cppObj))
    {
    }

private:
    xbox::services::achievements::achievement_requirement m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END

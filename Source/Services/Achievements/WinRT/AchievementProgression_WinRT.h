// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi\achievements.h"
#include "AchievementRequirement_winrt.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>
/// Represents progress details about the achievement, including requirements.
/// </summary>
public ref class AchievementProgression sealed
{
public:
    /// <summary>
    /// The actions and conditions that are required to unlock the achievement.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<AchievementRequirement^>^ Requirements { Windows::Foundation::Collections::IVectorView<AchievementRequirement^>^ get(); }

    /// <summary>
    /// The timestamp when the achievement was first unlocked.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(TimeUnlocked, time_unlocked);

internal:
    AchievementProgression(
        _In_ xbox::services::achievements::achievement_progression cppObj
        );

private:
    xbox::services::achievements::achievement_progression m_cppObj;
    Windows::Foundation::Collections::IVector<AchievementRequirement^>^ m_requirements;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
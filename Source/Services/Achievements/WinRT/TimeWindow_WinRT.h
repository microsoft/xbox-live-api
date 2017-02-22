// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/achievements.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>
/// Represents an interval of time during which an achievement can be unlocked. 
/// This class is only used when the AchievementType enumeration is set to Challenge.
/// </summary>
public ref class AchievementTimeWindow sealed
{
public:
    /// <summary>
    /// The start date and time of the achievement time window.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(StartDate, start_date);

    /// <summary>
    /// The end date and time of the achievement time window.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(EndDate, end_date);

internal:
    AchievementTimeWindow(
        _In_ xbox::services::achievements::achievement_time_window cppObj
        );

private:
    xbox::services::achievements::achievement_time_window m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
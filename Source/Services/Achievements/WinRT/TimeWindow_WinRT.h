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
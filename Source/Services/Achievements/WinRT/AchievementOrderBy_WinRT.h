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

/// <summary>Enumeration values that indicate the achievement sort order.</summary>
public enum class AchievementOrderBy
{
    /// <summary>Default order does not guarantee sort order.</summary>
    Default = xbox::services::achievements::achievement_order_by::default_order,

    /// <summary>Sort by title id.</summary>
    TitleId = xbox::services::achievements::achievement_order_by::title_id,

    /// <summary>Sort by achievement unlock time.</summary>
    UnlockTime = xbox::services::achievements::achievement_order_by::unlock_time
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
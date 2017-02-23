// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
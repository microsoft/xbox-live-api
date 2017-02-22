// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>
/// Enumeration values that indicate the media asset type associated with
/// the achievement.
/// </summary>
public enum class AchievementMediaAssetType
{
    /// <summary>The media asset type is unknown.</summary>
    Unknown = xbox::services::achievements::achievement_media_asset_type::unknown,

    /// <summary>An icon media asset.</summary>
    Icon = xbox::services::achievements::achievement_media_asset_type::icon,

    /// <summary>An art media asset.</summary>
    Art = xbox::services::achievements::achievement_media_asset_type::art
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END

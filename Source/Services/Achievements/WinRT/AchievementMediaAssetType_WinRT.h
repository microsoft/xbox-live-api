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

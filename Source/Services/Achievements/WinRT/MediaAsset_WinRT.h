// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/achievements.h"
#include "AchievementMediaAssetType_winrt.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>
/// Represents a media asset for an achievement.
/// </summary>
public ref class AchievementMediaAsset sealed
{
public:
    /// <summary>
    /// The name of the media asset, such as "tile01".
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary>
    /// The type of media asset.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(MediaAssetType, media_asset_type, Microsoft::Xbox::Services::Achievements::AchievementMediaAssetType);

    /// <summary>
    /// The URL of the media asset.
    /// </summary>
    property Platform::String^ Url { Platform::String^ get(); }

internal:
    AchievementMediaAsset(
        _In_ xbox::services::achievements::achievement_media_asset cppObj
        );

private:
    xbox::services::achievements::achievement_media_asset m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
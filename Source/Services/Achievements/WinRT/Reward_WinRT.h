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
#include "AchievementRewardType_winrt.h"
#include "MediaAsset_winrt.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

/// <summary>
/// Represents a reward that is associated with the achievement.
/// </summary>
public ref class AchievementReward sealed
{
public:
    /// <summary>
    /// The localized reward name.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary>
    /// The description of the reward.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Description, description);

    /// <summary>
    /// The title-defined reward value (data type and content varies by reward type).
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Data, value);

    /// <summary>
    /// The reward type.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(RewardType, reward_type, Microsoft::Xbox::Services::Achievements::AchievementRewardType);

    /// <summary>
    /// The property type of the reward value string.
    /// </summary>
    property Windows::Foundation::PropertyType ValuePropertyType { Windows::Foundation::PropertyType get(); }

    /// <summary>
    /// The media asset associated with the reward.
    /// If the RewardType is Gamerscore, this will be nullptr.
    /// If the RewardType is InApp, this will be a media asset.
    /// If the RewardType is Art, this may be a media asset or nullptr.
    /// </summary>
    property AchievementMediaAsset^ MediaAsset { AchievementMediaAsset^ get(); }


internal:
    AchievementReward(
        _In_ xbox::services::achievements::achievement_reward cppObj
        );

private:

    static Windows::Foundation::PropertyType ConvertStringToPropertyType(
        _In_ std::wstring value
        );

private:
    xbox::services::achievements::achievement_reward m_cppObj;
    Windows::Foundation::PropertyType m_valuePropertyType;
    AchievementMediaAsset^ m_mediaAsset;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
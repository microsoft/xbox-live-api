//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "Achievement_winrt.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::achievements;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

Achievement::Achievement(
    _In_ xbox::services::achievements::achievement cppObj
    ):
    m_cppObj(std::move(cppObj))
{
    m_titleAssociations = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<AchievementTitleAssociation, achievement_title_association>(m_cppObj.title_associations());
    m_progression = ref new AchievementProgression(m_cppObj.progression());
    m_mediaAssets = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<AchievementMediaAsset, achievement_media_asset>(m_cppObj.media_assets());
    m_platformsAvailableOn = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.platforms_available_on());
    m_available = ref new AchievementTimeWindow(m_cppObj.available());
    m_rewards = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<AchievementReward, achievement_reward>(m_cppObj.rewards());
    m_estimatedUnlockTime = UtilsWinRT::ConvertSecondsToTimeSpan(m_cppObj.estimated_unlock_time());
}

IVectorView<AchievementTitleAssociation^>^
Achievement::TitleAssociations::get()
{
    return m_titleAssociations->GetView();
}

AchievementProgression^
Achievement::Progression::get()
{
    return m_progression;
}

IVectorView<AchievementMediaAsset^>^
Achievement::MediaAssets::get()
{
    return m_mediaAssets->GetView();
}

IVectorView<String^>^
Achievement::PlatformsAvailableOn::get()
{
    return m_platformsAvailableOn->GetView();
}

AchievementTimeWindow^
Achievement::Available::get()
{
    return m_available;
}

IVectorView<AchievementReward^>^
Achievement::Rewards::get()
{
    return m_rewards->GetView();
}

TimeSpan
Achievement::EstimatedUnlockTime::get()
{
    return m_estimatedUnlockTime;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
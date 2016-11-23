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
#include "Reward_winrt.h"

using namespace Microsoft::Xbox::Services::Achievements;
using namespace Microsoft::Xbox::Services;
using namespace Platform;
using namespace Windows::Foundation;

AchievementReward::AchievementReward(
    _In_ xbox::services::achievements::achievement_reward cppObj
    ) :
    m_cppObj(cppObj)
{
    m_valuePropertyType = ConvertStringToPropertyType(m_cppObj.value_type());
    m_mediaAsset = ref new AchievementMediaAsset(m_cppObj.media_asset());
}

PropertyType
AchievementReward::ValuePropertyType::get()
{
    return m_valuePropertyType;
}

AchievementMediaAsset^
AchievementReward::MediaAsset::get()
{
    return m_mediaAsset;
}

PropertyType
AchievementReward::ConvertStringToPropertyType(
    _In_ std::wstring value
    )
{
    if( value.empty() )
    {
        return PropertyType::String;
    }
    else if( _wcsicmp(value.c_str(), L"Int") == 0 )
    {
        return PropertyType::Int32;
    }
    else if( _wcsicmp(value.c_str(), L"Guid") == 0 )
    {
        return PropertyType::Guid;
    }

    return PropertyType::String;
}

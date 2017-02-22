// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MediaAsset_winrt.h"

using namespace Platform;
using namespace Windows::Foundation;

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_BEGIN

AchievementMediaAsset::AchievementMediaAsset(
    _In_ xbox::services::achievements::achievement_media_asset cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

String^
AchievementMediaAsset::Url::get()
{
    return ref new String(m_cppObj.url().to_string().c_str());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_END
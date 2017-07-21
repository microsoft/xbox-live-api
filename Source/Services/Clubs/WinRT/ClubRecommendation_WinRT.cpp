// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubRecommendation_WinRT.h"
#include "xsapi/clubs.h"
#include "Utils_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

using namespace Microsoft::Xbox::Services::System;

ClubRecommendation::ClubRecommendation(xbox::services::clubs::club_recommendation_t cppObj) 
{
    m_club = ref new Club(cppObj.first);
    m_reasons = UtilsWinRT::CreatePlatformVectorFromStdVectorString(cppObj.second);
}

Club^ ClubRecommendation::RecommendedClub::get()
{
    return m_club;
}

Windows::Foundation::Collections::IVectorView<Platform::String^>^ ClubRecommendation::Reasons::get()
{
    return m_reasons->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END
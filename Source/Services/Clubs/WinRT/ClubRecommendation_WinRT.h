// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "Club_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubRecommendation sealed
{
public:
    /// <summary>The club being recommended</summary>
    property Club^ RecommendedClub
    {
        Club^ get();
    }
    
    /// <summary>Localized string giving the reason the club is recommended</summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Reasons
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

internal:
    ClubRecommendation(_In_ xbox::services::clubs::club_recommendation_t cppObj);

private:
    Club^ m_club;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_reasons;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubSearchAutoComplete sealed
{
public:
    /// <summary>Club Id of suggested club</summary>
    DEFINE_PROP_GET_STR_OBJ(Id, id);

    /// <summary>Name of suggested club</summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary>Club description</summary>
    DEFINE_PROP_GET_STR_OBJ(Description, description);

    /// <summary>URL for the club display image</summary>
    DEFINE_PROP_GET_STR_OBJ(DisplayImageUrl, display_image_url);

    /// <summary>Score of the suggestion result relative to others. Larger scores are more relevant results.</summary>
    DEFINE_PROP_GET_OBJ(Score, score, double);

    /// <summary>Suggested query text for this club</summary>
    DEFINE_PROP_GET_STR_OBJ(SuggestedQueryText, suggested_query_text);

    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Tags
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ AssociatedTitles
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

internal:
    ClubSearchAutoComplete(_In_ xbox::services::clubs::club_search_auto_complete cppObj);

private:
    xbox::services::clubs::club_search_auto_complete m_cppObj;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_tags;
    Windows::Foundation::Collections::IVector<Platform::String^>^ m_associatedTitles;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END
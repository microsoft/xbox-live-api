// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"
#include "ClubSearchFacetResult_WinRT.h"
#include "Club_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

typedef Windows::Foundation::Collections::IMapView<Platform::String^, Windows::Foundation::Collections::IVectorView<ClubSearchFacetResult^>^>^ SearchFacetResults_t;

public ref class ClubsSearchResult sealed
{
public:
    /// <summary>List of clubs that match the search query</summary>
    property Windows::Foundation::Collections::IVectorView<Club^>^ Clubs
    {
        Windows::Foundation::Collections::IVectorView<Club^>^ get();
    }

    /// <summary>
    /// Facets can be used to further narrow down search results. The return map maps a facet (ie. tag or title) to
    /// to a collection of search search facet result objects.  A search facet result object describes how often a particular 
    /// value of that facet occurred.
    /// </summary>
    property SearchFacetResults_t SearchFacetResults
    {
        SearchFacetResults_t get();
    }

internal:
    ClubsSearchResult(_In_ xbox::services::clubs::clubs_search_result cppObj);

private:
    xbox::services::clubs::clubs_search_result m_cppObj;
    Windows::Foundation::Collections::IVector<Club^>^ m_clubs;
    Windows::Foundation::Collections::IMap<Platform::String^, Windows::Foundation::Collections::IVectorView<ClubSearchFacetResult^>^>^ m_searchFacetResults;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ClubsSearchResult_WinRT.h"
#include "xsapi/clubs.h"
#include "Utils_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Xbox::Services::System;
using namespace xbox::services::clubs;

ClubsSearchResult::ClubsSearchResult(_In_ xbox::services::clubs::clubs_search_result cppObj)
    : m_cppObj(std::move(cppObj))
{
    m_clubs = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<Club, club>(m_cppObj.clubs());
    
    m_searchFacetResults = ref new Map<String^, IVectorView<ClubSearchFacetResult^>^>();

    for (const auto& facet : m_cppObj.search_facet_results())
    {
        auto facetName = ref new String(facet.first.data());
        auto facetResults = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ClubSearchFacetResult, club_search_facet_result>(facet.second);
        m_searchFacetResults->Insert(facetName, facetResults->GetView());
    }
}

IVectorView<Club^>^ ClubsSearchResult::Clubs::get()
{
    return m_clubs->GetView();
}

SearchFacetResults_t ClubsSearchResult::SearchFacetResults::get()
{
    return m_searchFacetResults->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END
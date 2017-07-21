// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

using namespace std;

const vector<club>& clubs_search_result::clubs() const
{
    return m_clubs;
}

const unordered_map<string_t, std::vector<club_search_facet_result>>& clubs_search_result::search_facet_results() const
{
    return m_searchFacetResults;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END

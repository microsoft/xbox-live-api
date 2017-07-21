// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/clubs.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_BEGIN

public ref class ClubSearchFacetResult sealed
{
public:
    /// <summary>
    /// The particular value of the facet. For example, if the facet was "tags", this value might be 
    /// something like "cooperative".
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Value, value);

    /// <summary>The number of times the value occurred in the returned search results. </summary>
    DEFINE_PROP_GET_OBJ(Count, count, uint32);

internal:
    ClubSearchFacetResult(_In_ xbox::services::clubs::club_search_facet_result cppObj);

private:
    xbox::services::clubs::club_search_facet_result m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_END
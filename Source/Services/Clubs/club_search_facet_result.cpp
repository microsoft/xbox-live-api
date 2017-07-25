// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

uint32_t club_search_facet_result::count() const
{
    return m_count;
}

const string_t& club_search_facet_result::value() const
{
    return m_value;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END

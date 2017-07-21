// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/clubs.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_BEGIN

const string_t& club_search_auto_complete::id() const
{
    return m_id;
}

const string_t& club_search_auto_complete::name() const
{
    return m_name;
}

const string_t& club_search_auto_complete::description() const
{
    return m_description;
}

const string_t& club_search_auto_complete::display_image_url() const
{
    return m_displayImageUrl;
}

double club_search_auto_complete::score() const
{
    return m_score;
}

const string_t& club_search_auto_complete::suggested_query_text() const
{
    return m_suggestedQueryText;
}

const std::vector<string_t>& club_search_auto_complete::tags() const
{
    return m_tags;
}

const std::vector<string_t>& club_search_auto_complete::associated_titles() const
{
    return m_associatedTitles;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CLUBS_CPP_END
//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN

contextual_search_configured_stat::contextual_search_configured_stat() :
    m_visibility(contextual_search_stat_visibility::public_visibility),
    m_canBeFiltered(false),
    m_canBeSorted(false),
    m_displayType(contextual_search_stat_display_type::defined_range),
    m_rangeMin(0),
    m_rangeMax(0)
{
}

contextual_search_configured_stat::contextual_search_configured_stat(
    _In_ string_t name,
    _In_ string_t dataType,
    _In_ contextual_search_stat_visibility visibility,
    _In_ string_t displayName,
    _In_ bool canBeFiltered,
    _In_ bool canBeSorted,
    _In_ contextual_search_stat_display_type displayType,
    _In_ std::unordered_map<string_t, string_t> valueToDisplayNameMap,
    _In_ uint64_t rangeMin,
    _In_ uint64_t rangeMax
    ) :
    m_name(std::move(name)),
    m_dataType(std::move(dataType)),
    m_visibility(visibility),
    m_displayName(std::move(displayName)),
    m_canBeFiltered(canBeFiltered),
    m_canBeSorted(canBeSorted),
    m_displayType(displayType),
    m_valueToDisplayNameMap(std::move(valueToDisplayNameMap)),
    m_rangeMin(rangeMin),
    m_rangeMax(rangeMax)
{
}

const string_t& contextual_search_configured_stat::name() const
{
    return m_name;
}

const string_t& contextual_search_configured_stat::data_type() const
{
    return m_dataType;
}

contextual_search_stat_visibility contextual_search_configured_stat::visibility() const
{
    return m_visibility;
}

const string_t& contextual_search_configured_stat::display_name() const
{
    return m_displayName;
}

bool contextual_search_configured_stat::can_be_filtered() const
{
    return m_canBeFiltered;
}

bool contextual_search_configured_stat::can_be_sorted() const
{
    return m_canBeSorted;
}

contextual_search_stat_display_type contextual_search_configured_stat::display_type() const
{
    return m_displayType;
}

const std::unordered_map<string_t, string_t>& contextual_search_configured_stat::value_to_display_name_map() const
{
    return m_valueToDisplayNameMap;
}

uint64_t contextual_search_configured_stat::range_min() const
{
    return m_rangeMin;
}

uint64_t contextual_search_configured_stat::range_max() const
{
    return m_rangeMax;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END

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
#include "contextual_config_result.h"
#include "xbox_system_factory.h"

using namespace pplx;
using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN

contextual_config_result_stat::contextual_config_result_stat() :
    m_visibility(contextual_search_stat_visibility::unknown)
{
}

contextual_config_result_stat::contextual_config_result_stat(
    _In_ string_t name,
    _In_ string_t type,
    _In_ contextual_search_stat_visibility visibility
    ) :
    m_name(std::move(name)),
    m_type(std::move(type)),
    m_visibility(visibility)
{
}

xbox_live_result<contextual_config_result_stat> 
contextual_config_result_stat::_Deserialize(
    _In_ const web::json::value& inputJson
    )
{
    std::error_code errc = xbox_live_error_code::no_error;
    contextual_config_result_stat val(
        utils::extract_json_string(inputJson, _T("name"), errc, false),
        utils::extract_json_string(inputJson, _T("type"), errc, false),
        convert_string_to_visibility(utils::extract_json_string(inputJson, _T("visibility"), errc, false))
        );

    return xbox_live_result<contextual_config_result_stat>(val, errc);
}

contextual_search_stat_visibility
contextual_config_result_stat::convert_string_to_visibility(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("Private")) == 0)
    {
        return contextual_search_stat_visibility::private_visibility;
    }
    else if (utils::str_icmp(value, _T("Public")) == 0)
    {
        return contextual_search_stat_visibility::public_visibility;
    }

    return contextual_search_stat_visibility::unknown;
}

const string_t& contextual_config_result_stat::name() const
{
    return m_name;
}

const string_t& contextual_config_result_stat::type() const
{
    return m_type;
}

contextual_search_stat_visibility contextual_config_result_stat::visibility() const
{
    return m_visibility;
}

contextual_config_result_metadata::contextual_config_result_metadata() :
    m_filter(false),
    m_sort(false),
    m_type(contextual_search_stat_display_type::unknown),
    m_rangeMin(0),
    m_rangeMax(0)
{
}

contextual_config_result_metadata::contextual_config_result_metadata(
    _In_ string_t name,
    _In_ string_t displayName,
    _In_ bool filter,
    _In_ bool sort,
    _In_ contextual_search_stat_display_type type,
    _In_ string_t setName,
    _In_ uint64_t rangeMin,
    _In_ uint64_t rangeMax
    ) :
    m_name(std::move(name)),
    m_displayName(std::move(displayName)),
    m_filter(filter),
    m_sort(sort),
    m_type(type),
    m_setName(std::move(setName)),
    m_rangeMin(rangeMin),
    m_rangeMax(rangeMax)
{
}

xbox_live_result<contextual_config_result_metadata> 
contextual_config_result_metadata::_Deserialize(_In_ const web::json::value& inputJson)
{
    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value rangeValues(utils::extract_json_field(inputJson, _T("rangeValues"), errc, false));

    contextual_config_result_metadata val(
        utils::extract_json_string(inputJson, _T("name"), errc, false),
        utils::extract_json_string(inputJson, _T("displayName"), errc, false),
        utils::extract_json_bool(inputJson, _T("filter"), errc, false),
        utils::extract_json_bool(inputJson, _T("sort"), errc, false),
        convert_string_to_display_type(utils::extract_json_string(inputJson, _T("type"), errc, false)),
        utils::extract_json_string(inputJson, _T("setName"), errc, false),
        utils::extract_json_int(rangeValues, _T("min"), errc, false, 0),
        utils::extract_json_int(rangeValues, _T("max"), errc, false, 0)
        );
    return xbox_live_result<contextual_config_result_metadata>(val, errc);
}

contextual_search_stat_display_type
contextual_config_result_metadata::convert_string_to_display_type(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("Range")) == 0)
    {
        return contextual_search_stat_display_type::defined_range;
    }
    else if (utils::str_icmp(value, _T("Set")) == 0)
    {
        return contextual_search_stat_display_type::set;
    }
    else if (utils::str_icmp(value, _T("UndefinedRange")) == 0)
    {
        return contextual_search_stat_display_type::undefined_range;
    }

    return contextual_search_stat_display_type::unknown;
}

const string_t& contextual_config_result_metadata::name() const
{
    return m_name;
}

const string_t& contextual_config_result_metadata::display_name() const
{
    return m_displayName;
}

bool contextual_config_result_metadata::filter() const
{
    return m_filter;
}

bool contextual_config_result_metadata::sort() const
{
    return m_sort;
}

contextual_search_stat_display_type contextual_config_result_metadata::type() const
{
    return m_type;
}

const string_t& contextual_config_result_metadata::setname() const
{
    return m_setName;
}

uint64_t contextual_config_result_metadata::range_min() const
{
    return m_rangeMin;
}

uint64_t contextual_config_result_metadata::range_max() const
{
    return m_rangeMax;
}

contextual_config_result_set_pair::contextual_config_result_set_pair()
{
}

contextual_config_result_set_pair::contextual_config_result_set_pair(
    _In_ string_t statValue,
    _In_ string_t displayName
    ) :
    m_statValue(std::move(statValue)),
    m_displayName(std::move(displayName))
{
}

xbox_live_result<contextual_config_result_set_pair> contextual_config_result_set_pair::_Deserialize(
    _In_ const web::json::value& inputJson
    )
{
    std::error_code errc = xbox_live_error_code::no_error;

    contextual_config_result_set_pair val(
        utils::extract_json_string(inputJson, _T("memberStatValue"), errc, false),
        utils::extract_json_string(inputJson, _T("memberDisplayName"), errc, false)
        );
    return xbox_live_result<contextual_config_result_set_pair>(val, errc);
}

const string_t& contextual_config_result_set_pair::stat_value() const
{
    return m_statValue;
}

const string_t& contextual_config_result_set_pair::display_name() const
{
    return m_displayName;
}

contextual_config_result_set::contextual_config_result_set()
{
}

contextual_config_result_set::contextual_config_result_set(
    _In_ string_t name,
    _In_ std::unordered_map<string_t, string_t> values
    ) :
    m_name(std::move(name)),
    m_values(std::move(values))
{
}

const string_t& contextual_config_result_set::name() const
{
    return m_name;
}

const std::unordered_map<string_t, string_t>& contextual_config_result_set::values() const
{
    return m_values;
}

xbox_live_result<contextual_config_result_set> contextual_config_result_set::_Deserialize(_In_ const web::json::value& inputJson)
{
    std::error_code errc = xbox_live_error_code::no_error;
    auto pairs = utils::extract_json_vector<contextual_config_result_set_pair>(
            contextual_config_result_set_pair::_Deserialize, 
            inputJson, 
            _T("values"), 
            errc, 
            true
            );

    std::unordered_map<string_t, string_t> valueMap;
    for (const auto& pair : pairs)
    {
        if (!pair.display_name().empty() && !pair.stat_value().empty())
        {
            valueMap[pair.stat_value()] = pair.display_name();
        }
    }

    contextual_config_result_set val(
        utils::extract_json_string(inputJson, _T("name"), errc, false),
        valueMap
        );
    
    return xbox_live_result<contextual_config_result_set>(val, errc);
}

contextual_config_result::contextual_config_result()
{
}

contextual_config_result::contextual_config_result(
    _In_ std::vector<contextual_config_result_stat> stats,
    _In_ std::vector<contextual_config_result_metadata> metadataSet,
    _In_ std::vector<contextual_config_result_set> sets
    ) :
    m_stats(std::move(stats)),
    m_metadataSet(std::move(metadataSet)),
    m_sets(std::move(sets))
{
}

xbox_live_result<contextual_config_result> contextual_config_result::_Deserialize(
    _In_ const web::json::value& inputJson
    )
{
    string_t jsTxt = inputJson.serialize();

    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value metadataStats(utils::extract_json_field(inputJson, _T("metadata"), errc, false));

    auto vecStats = utils::extract_json_vector<contextual_config_result_stat>(contextual_config_result_stat::_Deserialize, inputJson, _T("stats"), errc, false);

    contextual_config_result val(
        vecStats,
        utils::extract_json_vector<contextual_config_result_metadata>(contextual_config_result_metadata::_Deserialize, metadataStats, _T("stats"), errc, false),
        utils::extract_json_vector<contextual_config_result_set>(contextual_config_result_set::_Deserialize, inputJson, _T("sets"), errc, false)
        );

    return xbox_live_result<contextual_config_result>(val, errc);
}

const std::vector<contextual_config_result_stat>& contextual_config_result::stats() const
{
    return m_stats;
}

const std::vector<contextual_config_result_metadata>& contextual_config_result::metadata_set() const
{
    return m_metadataSet;
}

const std::vector<contextual_config_result_set>& contextual_config_result::sets() const
{
    return m_sets;
}

std::vector<contextual_search_configured_stat> contextual_config_result::get_contextual_search_configured_stats()
{
    std::vector<contextual_search_configured_stat> configuredStats;

    for (const auto& stat : m_stats)
    {
        contextual_config_result_metadata matchingMetaData;
        for (const auto& metadata : m_metadataSet)
        {
            if (metadata.name() == stat.name())
            {
                matchingMetaData = metadata;
                break;
            }
        }

        contextual_config_result_set matchingSet;
        for (const auto& set : m_sets)
        {
            if (set.name() == stat.name())
            {
                matchingSet = set;
                break;
            }
        }

        contextual_search_configured_stat configuredStat(
            stat.name(),
            stat.type(),
            stat.visibility(),
            matchingMetaData.display_name(),
            matchingMetaData.filter(),
            matchingMetaData.sort(),
            matchingMetaData.type(),
            matchingSet.values(),
            matchingMetaData.range_min(),
            matchingMetaData.range_max()
            );

        configuredStats.push_back(configuredStat);
    }

    return configuredStats;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END

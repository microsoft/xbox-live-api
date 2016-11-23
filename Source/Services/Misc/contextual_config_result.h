//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN

class contextual_config_result_stat
{
public:
    contextual_config_result_stat();
    contextual_config_result_stat(
        _In_ string_t name,
        _In_ string_t type,
        _In_ xbox::services::contextual_search::contextual_search_stat_visibility visibility
        );

    static xbox_live_result<contextual_config_result_stat> _Deserialize(_In_ const web::json::value& inputJson);

    const string_t& name() const;
    const string_t& type() const;
    xbox::services::contextual_search::contextual_search_stat_visibility visibility() const;

private:
    static xbox::services::contextual_search::contextual_search_stat_visibility convert_string_to_visibility(
        _In_ const string_t& value
        );

    string_t m_name;
    string_t m_type;
    xbox::services::contextual_search::contextual_search_stat_visibility m_visibility;
};

class contextual_config_result_metadata
{
public:
    contextual_config_result_metadata();
    contextual_config_result_metadata(
        _In_ string_t name,
        _In_ string_t displayName,
        _In_ bool filter,
        _In_ bool sort,
        _In_ contextual_search_stat_display_type type,
        _In_ string_t setName,
        _In_ uint64_t rangeMin,
        _In_ uint64_t rangeMax
        );

    static xbox_live_result<contextual_config_result_metadata> _Deserialize(_In_ const web::json::value& inputJson);

    const string_t& name() const;
    const string_t& display_name() const;
    bool filter() const;
    bool sort() const;
    contextual_search_stat_display_type type() const;
    const string_t& setname() const;
    uint64_t range_min() const;
    uint64_t range_max() const;

private:
    static contextual_search_stat_display_type convert_string_to_display_type(
        _In_ const string_t& value
        );

    string_t m_name;
    string_t m_displayName;
    bool m_filter;
    bool m_sort;
    contextual_search_stat_display_type m_type;
    string_t m_setName;
    uint64_t m_rangeMin;
    uint64_t m_rangeMax;
};

class contextual_config_result_set_pair
{
public:
    contextual_config_result_set_pair();
    contextual_config_result_set_pair(
        _In_ string_t statValue,
        _In_ string_t displayName
        );

    static xbox_live_result<contextual_config_result_set_pair> _Deserialize(_In_ const web::json::value& inputJson);

    const string_t& stat_value() const;
    const string_t& display_name() const;

private:
    string_t m_statValue;
    string_t m_displayName;
};

class contextual_config_result_set
{
public:
    contextual_config_result_set();
    contextual_config_result_set(
        _In_ string_t name,
        _In_ std::unordered_map<string_t, string_t> values
        );

    static xbox_live_result<contextual_config_result_set> _Deserialize(_In_ const web::json::value& inputJson);

    const string_t& name() const;
    const std::unordered_map<string_t, string_t>& values() const;

private:
    string_t m_name;
    std::unordered_map<string_t, string_t> m_values;
};

class contextual_config_result
{
public:
    contextual_config_result();
    contextual_config_result(
        _In_ std::vector<contextual_config_result_stat> stats,
        _In_ std::vector<contextual_config_result_metadata> metadataSet,
        _In_ std::vector<contextual_config_result_set> sets
        );

    static xbox_live_result<contextual_config_result> _Deserialize(_In_ const web::json::value& inputJson);

    std::vector<contextual_search_configured_stat> get_contextual_search_configured_stats();

    const std::vector<contextual_config_result_stat>& stats() const;
    const std::vector<contextual_config_result_metadata>& metadata_set() const;
    const std::vector<contextual_config_result_set>& sets() const;

private:
    std::vector<contextual_config_result_stat> m_stats;
    std::vector<contextual_config_result_metadata> m_metadataSet;
    std::vector<contextual_config_result_set> m_sets;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END

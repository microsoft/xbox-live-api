// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/contextual_search_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_BEGIN

contextual_search_game_clip_stat::contextual_search_game_clip_stat()
{
}

contextual_search_game_clip_stat::contextual_search_game_clip_stat(
    _In_ string_t statName,
    _In_ string_t value,
    _In_ string_t minValue,
    _In_ string_t maxValue,
    _In_ string_t deltaValue
    ) :
    m_statName(std::move(statName)),
    m_value(std::move(value)),
    m_minValue(std::move(minValue)),
    m_maxValue(std::move(maxValue)),
    m_deltaValue(std::move(deltaValue))
{
}

xbox_live_result<contextual_search_game_clip_stat>
contextual_search_game_clip_stat::_Deserialize(_In_ const web::json::value& inputJson)
{
    if (inputJson.is_null()) return xbox_live_result<contextual_search_game_clip_stat>();

    std::error_code errc = xbox_live_error_code::no_error;

    contextual_search_game_clip_stat result(
        utils::extract_json_string(inputJson, _T("statName"), errc, false),
        utils::extract_json_string(inputJson, _T("value"), errc, false),
        utils::extract_json_string(inputJson, _T("min"), errc, false),
        utils::extract_json_string(inputJson, _T("max"), errc, false),
        utils::extract_json_string(inputJson, _T("delta"), errc, false)
        );

    return xbox_live_result<contextual_search_game_clip_stat>(result, errc);
}

const string_t& contextual_search_game_clip_stat::stat_name() const
{
    return m_statName;
}

const string_t& contextual_search_game_clip_stat::value() const
{
    return m_value;
}

const string_t& contextual_search_game_clip_stat::min_value() const
{
    return m_minValue;
}

const string_t& contextual_search_game_clip_stat::max_value() const
{
    return m_maxValue;
}

const string_t& contextual_search_game_clip_stat::delta_value() const
{
    return m_deltaValue;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CONTEXTUAL_SEARCH_CPP_END

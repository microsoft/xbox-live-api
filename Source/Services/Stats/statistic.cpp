// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/user_statistics.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

statistic::statistic()
{
}

// class statistic
statistic::statistic(
    _In_ string_t name,
    _In_ string_t type,
    _In_ string_t value
    ) :
    m_statName(std::move(name)),
    m_statType(std::move(type)),
    m_value(std::move(value))
{
}

const string_t&
statistic::statistic_name() const
{
    return m_statName;
}

const string_t&
statistic::statistic_type() const
{
    return m_statType;
}

const string_t&
statistic::value() const
{
    return m_value;
}

void
statistic::_Set_statistic_name(
    _In_ string_t name
    )
{
    m_statName = std::move(name);
}

void 
statistic::_Set_statistic_type(
    _In_ string_t type
    )
{
    m_statType = std::move(type);
}

void
statistic::_Set_statistic_value(
    _In_ string_t value
    )
{
    m_value = std::move(value);
}

xbox_live_result<statistic>
statistic::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<statistic>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto result = statistic(
        utils::extract_json_string(json, _T("statname"), errc, true),
        utils::extract_json_string(json, _T("type"), errc, true),
        utils::extract_json_string(json, _T("value"), errc, true)
        );

    return xbox_live_result<statistic>(
        result,
        errc,
        ""
        );
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/stats_manager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN

stat_value::stat_value() :
    m_dataType(stat_data_type::undefined)
{
}

const string_t
stat_value::name() const
{
    return m_name;
}

void
stat_value::set_name(
    _In_ const string_t& name
    )
{
    utils::char_t_copy(m_name, ARRAYSIZE(m_name), name.c_str());
}

double
stat_value::as_number() const
{
    return m_statData.numberType;
}

int64_t
stat_value::as_integer() const
{
    return static_cast<int64_t>(as_number());
}

const string_t
stat_value::as_string() const
{
    return m_statData.stringType;
}

stat_data_type
stat_value::data_type() const
{
    return m_dataType;
}

void
stat_value::set_stat(
    _In_ double value
    )
{
    m_statData.numberType = value;
    m_dataType = stat_data_type::number;
}

void
stat_value::set_stat(
    _In_ const char_t* value
    )
{
    m_dataType = stat_data_type::string;
    utils::char_t_copy(m_statData.stringType, ARRAYSIZE(m_statData.stringType), value);
}

web::json::value
stat_value::serialize() const
{
    web::json::value returnJSON;
    if (m_dataType == stat_data_type::number)
    {
        returnJSON[_T("value")] = web::json::value::number(m_statData.numberType);
    }
    else if(m_dataType == stat_data_type::string)
    {
        returnJSON[_T("value")] = web::json::value::string(m_statData.stringType);
    }

    return returnJSON;
}

xbox_live_result<stat_value>
stat_value::_Deserialize(
    _In_ const web::json::value& data
    )
{
    stat_value statValue;
    if (data.is_null()) { return statValue; }

    auto value = data.at(_T("value"));
    auto valueType = value.type();

    switch (valueType)
    {
        case web::json::value::value_type::Number:
            statValue.m_statData.numberType = value.as_double();
            statValue.m_dataType = stat_data_type::number;
            break;

        case web::json::value::value_type::String:
            utils::char_t_copy(statValue.m_statData.stringType, ARRAYSIZE(statValue.m_statData.stringType), value.as_string().c_str());
            statValue.m_dataType = stat_data_type::string;
            break;

        default:
            // error
            break;
    }

    return statValue;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END
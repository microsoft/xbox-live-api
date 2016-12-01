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
#include "xsapi/simple_stats.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_BEGIN

const string_t
stat_value::name() const
{
    return m_name;
}

double
stat_value::as_number() const
{
    return m_statData.numberType;
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

stat_compare_type
stat_value::compare_type() const
{
    return m_statCompareType;
}

void
stat_value::set_stat(
    _In_ double value
    )
{
    m_dataType = stat_data_type::number;
    m_statData.numberType = value;

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
        returnJSON[_T("value")] = web::json::value(m_statData.numberType);
    }
    else if(m_dataType == stat_data_type::string)
    {
        returnJSON[_T("value")] = web::json::value::string(m_statData.stringType);
    }
    switch (m_statCompareType)
    {
        case stat_compare_type::always:
            returnJSON[_T("op")] = web::json::value::string(_T("add"));
            break;
        case stat_compare_type::max:
            returnJSON[_T("op")] = web::json::value::string(_T("max"));
            break;
        case stat_compare_type::min:
            returnJSON[_T("op")] = web::json::value::string(_T("min"));
            break;
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
    auto statOp = utils::extract_json_string(data, _T("op"), false);

    switch (valueType)
    {
        case web::json::value::value_type::Number:
            statValue.m_statData.numberType = value.as_double();
            statValue.m_dataType = stat_data_type::number;

            if (utils::str_icmp(statOp, _T("min")) == 0)
            {
                statValue.m_statCompareType = stat_compare_type::min;
            }
            else if(utils::str_icmp(statOp, _T("max")) == 0)
            {
                statValue.m_statCompareType = stat_compare_type::max;
            }
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
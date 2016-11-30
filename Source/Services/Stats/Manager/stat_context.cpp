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

stat_context::stat_context()
{
    initialize_char_arr(m_name);
    initialize_char_arr(m_value);
}

stat_context::stat_context(
    _In_ const char_t* name,
    _In_ const char_t* value
    )
{
    utils::char_t_copy(m_name, ARRAYSIZE(m_name), name);
    utils::char_t_copy(m_value, ARRAYSIZE(m_value), value);
}

const char_t*
stat_context::name() const
{
    return m_name;
}

const char_t*
stat_context::value() const
{
    return m_value;
}


xbox_live_result<stat_context>
stat_context::_Deserialize(
    _In_ const web::json::value& data
    )
{
    stat_context statContext;
    if (data.is_null()) { return statContext; }

    std::error_code errc = xbox_live_error_code::no_error;
    web::json::object asObject = data.as_object();
    for (auto pair : asObject)
    {
        utils::char_t_copy(statContext.m_name, ARRAYSIZE(statContext.m_name), pair.first.c_str());
        utils::char_t_copy(statContext.m_value, ARRAYSIZE(statContext.m_value), pair.second.as_string().c_str());
    }
    return statContext;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_STAT_MANAGER_CPP_END
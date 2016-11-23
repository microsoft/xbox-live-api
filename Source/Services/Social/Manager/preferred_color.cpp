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
#include "xsapi/social_manager.h"

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

preferred_color::preferred_color()
{
    initialize_char_arr(m_primaryColor);
    initialize_char_arr(m_secondaryColor);
    initialize_char_arr(m_tertiaryColor);
}

const char_t*
preferred_color::primary_color() const
{
    return m_primaryColor;
}

const char_t*
preferred_color::secondary_color() const
{
    return m_secondaryColor;
}

const char_t*
preferred_color::tertiary_color() const
{
    return m_tertiaryColor;
}

bool
preferred_color::operator!=(const preferred_color& rhs) const
{
    return (
        utils::str_icmp(m_primaryColor, rhs.m_primaryColor) != 0 ||
        utils::str_icmp(m_secondaryColor, rhs.m_secondaryColor) != 0 ||
        utils::str_icmp(m_tertiaryColor, rhs.m_tertiaryColor) != 0
        );
}

xbox_live_result<preferred_color>
preferred_color::_Deserialize(
    _In_ const web::json::value& json,
    _In_ std::error_code& errcOut
    )
{
    preferred_color returnObject;
    if (json.is_null()) return xbox_live_result<preferred_color>();

    std::error_code errc = xbox_live_error_code::no_error;
    utils::extract_json_string_to_char_t_array(json, _T("primaryColor"), errc, returnObject.m_primaryColor, ARRAYSIZE(returnObject.m_primaryColor));
    utils::extract_json_string_to_char_t_array(json, _T("secondaryColor"), errc, returnObject.m_secondaryColor, ARRAYSIZE(returnObject.m_secondaryColor));
    utils::extract_json_string_to_char_t_array(json, _T("tertiaryColor"), errc, returnObject.m_tertiaryColor, ARRAYSIZE(returnObject.m_tertiaryColor));

    if (errcOut)
    {
        errcOut = errc;
    }
    return xbox_live_result<preferred_color>(
        returnObject,
        errc
        );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
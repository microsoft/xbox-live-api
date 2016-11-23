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
#include "xsapi/privacy.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_BEGIN

multiple_permissions_check_result::multiple_permissions_check_result()
{
}

const string_t& 
multiple_permissions_check_result::xbox_user_id() const
{
    return m_xboxUserId;
}

const std::vector<permission_check_result>&
multiple_permissions_check_result::items() const
{
    return m_items;
}

void multiple_permissions_check_result::initialize(
    _In_ uint32_t index,
    _In_ const string_t& permissionIdRequested
    )
{
    if( index < m_items.size() )
    {
        m_items[index].initialize(permissionIdRequested);
    }
}

xbox_live_result<multiple_permissions_check_result>
multiple_permissions_check_result::_Deserializer(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<multiple_permissions_check_result>();

    multiple_permissions_check_result result;

    std::error_code errc = xbox_live_error_code::no_error;
    web::json::value userObj = utils::extract_json_field(json, _T("user"), errc, true);
    result.m_xboxUserId = utils::extract_json_string(userObj, _T("xuid"), errc, true);
    result.m_items = utils::extract_json_vector<permission_check_result>(
        permission_check_result::_Deserializer, 
        std::move(json), 
        _T("permissions"), 
        errc, 
        true
        );

    return xbox_live_result<multiple_permissions_check_result>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRIVACY_CPP_END
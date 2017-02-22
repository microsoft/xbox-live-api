// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_role_type::multiplayer_role_type() :
    m_ownerManaged(false)
{
}

const std::unordered_map<string_t, multiplayer_role_info>&
multiplayer_role_type::roles() const
{
    return m_roles;
}

void
multiplayer_role_type::set_roles(
    _In_ const std::unordered_map<string_t, multiplayer_role_info>& roles
    )
{
    m_roles = roles;
}

bool
multiplayer_role_type::owner_managed() const
{
    return m_ownerManaged;
}

const std::vector<mutable_role_setting>&
multiplayer_role_type::mutable_role_settings() const
{
    return m_mutableRoleSettings;
}

std::vector<mutable_role_setting>
multiplayer_role_type::_Convert_string_vector_to_mutable_role_settings(
    std::vector<string_t> roleSettings
    )
{
    std::vector<mutable_role_setting> mutableRoleSettings;
    for (auto& setting : roleSettings)
    {
        if (utils::str_icmp(setting, _T("max")) == 0)
        {
            mutableRoleSettings.push_back(mutable_role_setting::max);
        }
        else if (utils::str_icmp(setting, _T("target")) == 0)
        {
            mutableRoleSettings.push_back(mutable_role_setting::target);
        }
    }

    return mutableRoleSettings;
}

xbox_live_result<multiplayer_role_type>
multiplayer_role_type::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<multiplayer_role_type>();
 
    multiplayer_role_type returnResult;
    std::error_code errc = xbox_live_error_code::no_error;

    returnResult.m_ownerManaged = utils::extract_json_bool(json, _T("ownerManaged"), errc, false);
    returnResult.m_mutableRoleSettings = _Convert_string_vector_to_mutable_role_settings(
        utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("mutableRoleSettings"), errc, false)
        );

    auto rolesJson = utils::extract_json_field(json, _T("roles"), errc, false);
    if (!rolesJson.is_null() && rolesJson.is_object())
    {
        web::json::object rolesObj = rolesJson.as_object();
        for (const auto& role : rolesObj)
        {
            auto roleInfoResult = multiplayer_role_info::_Deserialize(role.second);
            if (roleInfoResult.err())
            {
                errc = roleInfoResult.err();
            }
            returnResult.m_roles[role.first] = roleInfoResult.payload();
        }
    }

    return xbox_live_result<multiplayer_role_type>(returnResult, errc);

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
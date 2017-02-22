// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_role_info::multiplayer_role_info():
    m_maxMembersCount(0),
    m_membersCount(0),
    m_targetCount(0)
{
}

const std::vector<string_t>&
multiplayer_role_info::member_xbox_user_ids() const
{
    return m_memberXuids;
}

uint32_t
multiplayer_role_info::max_members_count() const
{
    return m_maxMembersCount;
}

uint32_t
multiplayer_role_info::members_count() const
{
    return m_membersCount;
}

uint32_t
multiplayer_role_info::target_count() const
{
    return m_targetCount;
}

void
multiplayer_role_info::set_max_members_count(
    _In_ uint32_t maxCount
    )
{
    m_maxMembersCount = maxCount;
}

void
multiplayer_role_info::set_target_count(
    _In_ uint32_t targetCount
    )
{
    m_targetCount = targetCount;
}

xbox_live_result<multiplayer_role_info>
multiplayer_role_info::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<multiplayer_role_info>();

    multiplayer_role_info returnResult;
    std::error_code errc = xbox_live_error_code::no_error;
    
    returnResult.m_membersCount = utils::extract_json_int(json, _T("count"), errc, false);
    returnResult.m_maxMembersCount = utils::extract_json_int(json, _T("max"), errc, false);
    returnResult.m_targetCount = utils::extract_json_int(json, _T("target"), errc, false);
    if (returnResult.m_membersCount > 0)
    {
        returnResult.m_memberXuids = utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("memberXuids"), errc, true);
    }

    return xbox_live_result<multiplayer_role_info>(returnResult, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
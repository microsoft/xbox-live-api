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
#include "utils.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session_role_types::multiplayer_session_role_types()
{
}

const std::unordered_map<string_t, multiplayer_role_type>&
multiplayer_session_role_types::role_types() const
{
    return m_roleTypes;
}

xbox_live_result<multiplayer_session_role_types>
multiplayer_session_role_types::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<multiplayer_session_role_types>();

    multiplayer_session_role_types returnResult;
    std::error_code errc = xbox_live_error_code::no_error;

    web::json::object roleTypesObj = json.as_object();
    for (const auto& roleType : roleTypesObj)
    {
        auto roleTypeResult = multiplayer_role_type::_Deserialize(roleType.second);
        if (roleTypeResult.err())
        {
            errc = roleTypeResult.err();
        }
        returnResult.m_roleTypes[roleType.first] = roleTypeResult.payload();
    }

    return xbox_live_result<multiplayer_session_role_types>(returnResult, errc);

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
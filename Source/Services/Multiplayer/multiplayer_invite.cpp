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
#include "xsapi/multiplayer.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_invite::multiplayer_invite()
{
}

const string_t&
multiplayer_invite::handle_id() const
{
    return m_handleId;
}

xbox_live_result<multiplayer_invite>
multiplayer_invite::deserialize(
    _In_ const web::json::value& inputJson
    )
{
    multiplayer_invite returnResult;
    if (inputJson.is_null()) return xbox_live_result<multiplayer_invite>(returnResult);

    std::error_code errc = xbox_live_error_code::no_error;
    returnResult.m_handleId = utils::extract_json_string(inputJson, _T("id"), errc);

    return xbox_live_result<multiplayer_invite>(returnResult, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
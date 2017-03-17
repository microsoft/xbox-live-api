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
#include "xsapi/tournaments.h"

using namespace pplx;
using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

current_match_metadata::current_match_metadata()
{
}

const xbox::services::multiplayer::multiplayer_session_reference&
current_match_metadata::game_session_reference() const
{
    return m_gameSessionReference;
}

const match_metadata&
current_match_metadata::match_details() const
{
    return m_matchDetails;
}

xbox_live_result<current_match_metadata>
current_match_metadata::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<current_match_metadata>();

    current_match_metadata result;
    std::error_code errCode = xbox_live_error_code::no_error;
    
    auto sessionReference = multiplayer_session_reference::_Deserialize(utils::extract_json_field(json, _T("gameSessionRef"), errCode, false));
    if (sessionReference.err())
    {
        errCode = sessionReference.err();
    }
    result.m_gameSessionReference = sessionReference.payload();

    auto matchDetailsResult = match_metadata::_Deserialize(json);
    if (matchDetailsResult.err())
    {
        errCode = matchDetailsResult.err();
    }
    result.m_matchDetails = matchDetailsResult.payload();

    return result;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END
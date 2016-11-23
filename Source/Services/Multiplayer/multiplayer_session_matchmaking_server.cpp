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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session_matchmaking_server::multiplayer_session_matchmaking_server() :
    m_status(matchmaking_status::unknown),
    m_isNull(true)
{
}

matchmaking_status
multiplayer_session_matchmaking_server::status() const
{
    return m_status;
}

const string_t&
multiplayer_session_matchmaking_server::status_details() const
{
    return m_statusDetails;
}

const std::chrono::seconds&
multiplayer_session_matchmaking_server::typical_wait() const
{
    return m_typicalWait;
}

const multiplayer_session_reference& 
multiplayer_session_matchmaking_server::target_session_ref() const
{
    return m_targetSessionRef;
}

bool
multiplayer_session_matchmaking_server::is_null() const
{
    return m_isNull;
}

xbox_live_result<multiplayer_session_matchmaking_server>
multiplayer_session_matchmaking_server::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_session_matchmaking_server returnObject;
    if (json.is_null()) return xbox_live_result<multiplayer_session_matchmaking_server>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;

    auto matchmakingStatusResult = multiplayer_session::_Convert_string_to_matchmaking_status(
        utils::extract_json_string(json, _T("status"), errc)
        );

    if (matchmakingStatusResult.err())
    {
        return xbox_live_result<multiplayer_session_matchmaking_server>(matchmakingStatusResult.err(), matchmakingStatusResult.err_message());
    }
    returnObject.m_status = matchmakingStatusResult.payload();

    returnObject.m_statusDetails = utils::extract_json_string(
        json, 
        _T("statusDetails"),
        errc
        );

    returnObject.m_typicalWait = std::chrono::seconds(
        utils::extract_json_int(
            json, 
            _T("typicalWait"),
            errc
            )
        );
    
    auto multiplayerSessionReference = multiplayer_session_reference::_Deserialize(
        utils::extract_json_field(
            json, 
            _T("targetSessionRef"), 
            errc,
            false
            )
        );

    if (multiplayerSessionReference.err())
    {
        errc = multiplayerSessionReference.err();
    }
    returnObject.m_targetSessionRef = multiplayerSessionReference.payload();

    returnObject.m_isNull = false;
    return returnObject;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
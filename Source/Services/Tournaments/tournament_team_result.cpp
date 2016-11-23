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
using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

tournament_team_result::tournament_team_result() :
    m_state(tournament_game_result_state::no_contest),
    m_ranking(0)
{

}

tournament_team_result::tournament_team_result(
    _In_ tournament_game_result_state state,
    _In_ uint64_t ranking
    ) :
    m_state(state),
    m_ranking(ranking)
{

}

tournament_game_result_state tournament_team_result::state() const
{
    return m_state;
}

uint64_t tournament_team_result::ranking() const
{
    return m_ranking;
}

void tournament_team_result::set_state(_In_ tournament_game_result_state state)
{
    m_state = state;
}

void tournament_team_result::set_ranking(_In_ uint64_t ranking)
{
    m_state = tournament_game_result_state::rank;
    m_ranking = ranking;
}

xbox_live_result<tournament_team_result> tournament_team_result::_Deserialize(
    _In_ const web::json::value& json
    )
{
    tournament_team_result returnObject;
    if (json.is_null()) return xbox_live_result<tournament_team_result>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;

    tournament_game_result_state state = multiplayer_service::_Convert_string_to_game_result_state(
        utils::extract_json_string(json, _T("outcome"), errc)
        );
    if (state == tournament_game_result_state::rank)
    {
        returnObject.set_ranking(static_cast<uint64_t>(utils::extract_json_int(json, _T("ranking"), errc)));
    }
    else
    {
        returnObject.set_state(state);
    }

    return xbox_live_result<tournament_team_result>(returnObject, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END
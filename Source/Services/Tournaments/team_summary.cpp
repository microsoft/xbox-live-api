// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"
#include "utils.h"

using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

team_summary::team_summary() :
    m_ranking(0),
    m_state(team_state::unknown)
{
}

const string_t& 
team_summary::id() const
{
    return m_id;
}

team_state 
team_summary::state() const
{
    return m_state;
}

uint64_t 
team_summary::ranking() const
{
    return m_ranking;
}

bool
team_summary::is_null() const
{
    return m_id.empty();
}

xbox::services::xbox_live_result<team_summary>
team_summary::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<team_summary>();

    team_summary result;
    std::error_code errCode = xbox_live_error_code::no_error;

    result.m_id = utils::extract_json_string(json, _T("id"), errCode);
    result.m_ranking = utils::extract_json_int(json, _T("ranking"), errCode);
    result.m_state = tournament_service::_Convert_string_to_team_state(utils::extract_json_string(json, _T("state"), errCode));

    return xbox_live_result<team_summary>(result, errCode);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END
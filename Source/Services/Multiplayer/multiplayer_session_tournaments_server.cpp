// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/multiplayer.h"

using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_session_tournaments_server::multiplayer_session_tournaments_server() :
    m_registrationState(tournament_registration_state::unknown),
    m_registrationReason(tournament_registration_reason::unknown),
    m_lastGameResultSource(tournament_game_result_source::none),
    m_isNull(true)
{
}

const xbox::services::tournaments::tournament_reference&
multiplayer_session_tournaments_server::tournament_reference() const
{
    return m_tournamentRef;
}

const std::unordered_map<string_t, multiplayer_session_reference>& 
multiplayer_session_tournaments_server::teams() const
{
    return m_teams;
}

tournament_registration_state
multiplayer_session_tournaments_server::registration_state() const
{
    return m_registrationState;
}

tournament_registration_reason
multiplayer_session_tournaments_server::registration_reason() const
{
    return m_registrationReason;
}

const utility::datetime&
multiplayer_session_tournaments_server::next_game_start_time() const
{
    return m_nextGameStartTime;
}

const multiplayer_session_reference&
multiplayer_session_tournaments_server::next_game_session_reference() const
{
    return m_nextGameSessionReference;
}

const utility::datetime& 
multiplayer_session_tournaments_server::last_game_end_time() const
{
    return m_lastGameEndTime;
}

tournament_team_result
multiplayer_session_tournaments_server::last_team_result() const
{
    return m_lastTeamResult;
}

tournament_game_result_source
multiplayer_session_tournaments_server::last_game_result_source() const
{
    return m_lastGameResultSource;
}

bool
multiplayer_session_tournaments_server::_Is_null() const
{
    return m_isNull;
}

bool 
multiplayer_session_tournaments_server::_Is_same(const multiplayer_session_tournaments_server& other) const
{
    if (m_lastGameEndTime != other.last_game_end_time() ||
        m_lastGameResultSource != other.last_game_result_source() ||
        m_lastTeamResult.state() != other.last_team_result().state() ||
        m_lastTeamResult.ranking() != other.last_team_result().ranking() ||
        m_nextGameSessionReference._Serialize() != other.next_game_session_reference()._Serialize() ||
        m_nextGameStartTime != other.next_game_start_time() ||
        m_registrationReason != other.registration_reason() ||
        m_registrationState != other.registration_state())
    {
        return false;
    }
    return true;
}

tournament_registration_state
multiplayer_session_tournaments_server::_Convert_string_to_registration_result(_In_ const string_t& value)
{
    if (utils::str_icmp(value, _T("pending")) == 0)
    {
        return tournament_registration_state::pending;
    }
    else if (utils::str_icmp(value, _T("rejected")) == 0)
    {
        return tournament_registration_state::rejected;
    }
    else if (utils::str_icmp(value, _T("withdrawn")) == 0)
    {
        return tournament_registration_state::withdrawn;
    }
    else if (utils::str_icmp(value, _T("registered")) == 0)
    {
        return tournament_registration_state::registered;
    }
    else if (utils::str_icmp(value, _T("completed")) == 0)
    {
        return tournament_registration_state::completed;
    }

    return tournament_registration_state::unknown;
}

tournament_registration_reason
multiplayer_session_tournaments_server::_Convert_string_to_registration_reason(_In_ const string_t& value)
{
    if (utils::str_icmp(value, _T("RegistrationClosed")) == 0)
    {
        return tournament_registration_reason::registration_closed;
    }
    else if (utils::str_icmp(value, _T("MemberAlreadyRegistered")) == 0)
    {
        return tournament_registration_reason::member_already_registered;
    }
    else if (utils::str_icmp(value, _T("TournamentFull")) == 0)
    {
        return tournament_registration_reason::tournament_full;
    }
    else if (utils::str_icmp(value, _T("TeamEliminated")) == 0)
    {
        return tournament_registration_reason::team_eliminated;
    }
    else if (utils::str_icmp(value, _T("TournamentCompleted")) == 0)
    {
        return tournament_registration_reason::tournament_completed;
    }

    return tournament_registration_reason::unknown;
}

xbox_live_result<multiplayer_session_tournaments_server>
multiplayer_session_tournaments_server::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_session_tournaments_server returnObject;
    if (json.is_null()) return xbox_live_result<multiplayer_session_tournaments_server>(returnObject);

    std::error_code errc = xbox_live_error_code::no_error;

    auto constantsJson = utils::extract_json_field(json, _T("constants"), errc, true);
    auto systemConstantsJson = utils::extract_json_field(constantsJson, _T("system"), errc, true);

    auto tournamentRefJson = utils::extract_json_field(systemConstantsJson, _T("tournamentRef"), errc, true);
    if (!tournamentRefJson.is_null())
    {
        auto tournamentResult = xbox::services::tournaments::tournament_reference::_Deserialize(tournamentRefJson);
        if (tournamentResult.err())
        {
            errc = tournamentResult.err();
        }
        returnObject.m_tournamentRef = tournamentResult.payload();
    }
    
    auto teamsJson = utils::extract_json_field(systemConstantsJson, _T("teams"), errc, false);
    if (!teamsJson.is_null() && teamsJson.is_object())
    {
        web::json::object teamsObj = teamsJson.as_object();
        for (const auto& team : teamsObj)
        {
            auto sessionRefJson = utils::extract_json_field(team.second, _T("teamSessionRef"), true);
            auto sessionResult = multiplayer_session_reference::_Deserialize(sessionRefJson);
            if (!sessionResult.err())
            {
                returnObject.m_teams[team.first] = sessionResult.payload();
            }
            else
            {
                errc = sessionResult.err();
            }
        }
    }

    auto propertyJson = utils::extract_json_field(json, _T("properties"), errc, false);
    auto systemPropertyJson = utils::extract_json_field(propertyJson, _T("system"), errc, false);

    auto registrationJson = utils::extract_json_field(systemPropertyJson, _T("registration"), errc, false);
    if (!registrationJson.is_null())
    {
        returnObject.m_registrationState = multiplayer_session_tournaments_server::_Convert_string_to_registration_result(
            utils::extract_json_string(registrationJson, _T("state"), errc)
            );
        returnObject.m_registrationReason = multiplayer_session_tournaments_server::_Convert_string_to_registration_reason(
            utils::extract_json_string(registrationJson, _T("reason"), errc)
            );
    }

    auto rendezvousJson = utils::extract_json_field(systemPropertyJson, _T("rendezvous"), errc, false);
    if (!rendezvousJson.is_null())
    {
        returnObject.m_nextGameStartTime = utils::extract_json_time(rendezvousJson, _T("startTime"), errc, false);

        auto nextGameJson = utils::extract_json_field(rendezvousJson, _T("gameSessionRef"), errc, false);
        if (!nextGameJson.is_null())
        {
            auto gameSessionResult = multiplayer_session_reference::_Deserialize(nextGameJson);
            if (gameSessionResult.err())
            {
                errc = gameSessionResult.err();
            }
            returnObject.m_nextGameSessionReference = gameSessionResult.payload();
        }
    }

    auto lastGameJson = utils::extract_json_field(systemPropertyJson, _T("lastGame"), errc, false);
    if (!lastGameJson.is_null())
    {
        returnObject.m_lastGameEndTime = utils::extract_json_time(lastGameJson, _T("endTime"), errc, false);
        auto lastGameResultJson = utils::extract_json_field(lastGameJson, _T("result"), errc, false);
        if (!lastGameResultJson.is_null())
        {
             auto tournamentTeamResult = tournament_team_result::_Deserialize(lastGameResultJson);
             if (tournamentTeamResult.err())
             {
                 errc = tournamentTeamResult.err();
             }
            returnObject.m_lastTeamResult = tournamentTeamResult.payload();
            returnObject.m_lastGameResultSource = multiplayer_service::_Convert_string_to_game_result_source(
                utils::extract_json_string(lastGameResultJson, _T("source"), errc)
                );
        }
    }

    returnObject.m_isNull = false;
    return xbox_live_result<multiplayer_session_tournaments_server>(returnObject, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
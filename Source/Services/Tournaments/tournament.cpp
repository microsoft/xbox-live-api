// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"
#include "xbox_system_factory.h"
#include "utils.h"
#include "user_context.h"

using namespace pplx;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

tournament::tournament() :
    m_minTeamSize(0),
    m_maxTeamSize(0),
    m_numTeamsRegistered(0),
    m_minTeamsRegistered(0),
    m_maxTeamsRegistered(0),
    m_isRegistrationOpen(false),
    m_isCheckinOpen(false),
    m_isPlayingOpen(false),
    m_hasPrize(false),
    m_isPaused(false),
    m_tournamentState(xbox::services::tournaments::tournament_state::unknown)
{
}

const string_t&
tournament::id() const
{
    return m_tournamentId;
}

const string_t&
tournament::organizer_id() const
{
    return m_organizerId;
}

const string_t&
tournament::organizer_name() const
{
    return m_organizerName;
}

const string_t&
tournament::name() const
{
    return m_name;
}

const string_t&
tournament::description() const
{
    return m_description;
}

const string_t&
tournament::game_mode() const
{
    return m_gameMode;
}

const string_t&
tournament::tournament_style() const
{
    return m_tournamentStyle;
}

bool
tournament::is_registration_open() const
{
    return m_isRegistrationOpen;
}

bool
tournament::is_checkin_open() const
{
    return m_isCheckinOpen;
}

bool
tournament::is_playing_open() const
{
    return m_isPlayingOpen;
}

bool
tournament::has_prize() const
{
    return m_hasPrize;
}

bool
tournament::is_paused() const
{
    return m_isPaused;
}

uint32_t
tournament::teams_registered_count() const
{
    return m_numTeamsRegistered;
}

uint32_t
tournament::min_teams_registered() const
{
    return m_minTeamsRegistered;
}

uint32_t
tournament::max_teams_registered() const
{
    return m_maxTeamsRegistered;
}

uint32_t
tournament::min_team_size() const
{
    return m_minTeamSize;
}

uint32_t
tournament::max_team_size() const
{
    return m_maxTeamSize;
}

tournament_state
tournament::tournament_state() const
{
    return m_tournamentState;
}

const utility::datetime&
tournament::registration_start_time() const
{
    return m_registrationStartTime;
}

const utility::datetime&
tournament::registration_end_time() const
{
    return m_registrationEndTime;
}

const utility::datetime&
tournament::checkin_start_time() const
{
    return m_checkinStartTime;
}

const utility::datetime&
tournament::checkin_end_time() const
{
    return m_checkinEndTime;
}

const utility::datetime&
tournament::playing_start_time() const
{
    return m_playingStartTime;
}

const utility::datetime&
tournament::playing_end_time() const
{
    return m_playingEndTime;
}

const utility::datetime&
tournament::end_time() const
{
    return m_tournamentEndTime;
}

const xbox::services::tournaments::team_summary&
tournament::team_summary() const
{
    return m_teamSummary;
}

xbox_live_result<tournament>
tournament::_Deserialize(
    _In_ const web::json::value& tournamentJson,
    _In_ const web::json::value& teamJson
    )
{
    if (tournamentJson.is_null()) return xbox_live_result<tournament>();

    tournament result;
    std::error_code errCode = xbox_live_error_code::no_error;

    result.m_tournamentId = utils::extract_json_string(tournamentJson, _T("id"), errCode, true);
    result.m_organizerId = utils::extract_json_string(tournamentJson, _T("organizerId"), errCode);
    result.m_organizerName = utils::extract_json_string(tournamentJson, _T("organizerName"), errCode);
    result.m_name = utils::extract_json_string(tournamentJson, _T("name"), errCode);
    result.m_description = utils::extract_json_string(tournamentJson, _T("description"), errCode);
    result.m_gameMode = utils::extract_json_string(tournamentJson, _T("gameMode"), errCode);
    result.m_tournamentStyle = utils::extract_json_string(tournamentJson, _T("tournamentStyle"), errCode);
    result.m_tournamentState = _Convert_string_to_tournament_state(
        utils::extract_json_string(tournamentJson, _T("state"), errCode)
        );

    result.m_isRegistrationOpen = utils::extract_json_bool(tournamentJson, _T("isRegistrationOpen"), errCode);
    result.m_isPlayingOpen = utils::extract_json_bool(tournamentJson, _T("isPlayingOpen"), errCode);
    result.m_isCheckinOpen = utils::extract_json_bool(tournamentJson, _T("isCheckinOpen"), errCode);
    result.m_hasPrize = utils::extract_json_bool(tournamentJson, _T("hasPrize"), errCode);
    result.m_isPaused = utils::extract_json_bool(tournamentJson, _T("paused"), errCode);
    result.m_minTeamSize = utils::extract_json_int(tournamentJson, _T("minTeamSize"), errCode);
    result.m_maxTeamSize = utils::extract_json_int(tournamentJson, _T("maxTeamSize"), errCode);
    result.m_numTeamsRegistered = utils::extract_json_int(tournamentJson, _T("numTeamsRegistered"), errCode);
    result.m_minTeamsRegistered = utils::extract_json_int(tournamentJson, _T("minRegistrationCount"), errCode);
    result.m_maxTeamsRegistered = utils::extract_json_int(tournamentJson, _T("maxRegistrationCount"), errCode);
    result.m_tournamentEndTime = utils::extract_json_time(tournamentJson, _T("endTime"), errCode), false;

    web::json::value schedulesJson = utils::extract_json_field(tournamentJson, _T("schedule"), false);
    if (!schedulesJson.is_null())
    {
        result.m_registrationStartTime = utils::extract_json_time(schedulesJson, _T("registrationStart"), errCode);
        result.m_registrationEndTime = utils::extract_json_time(schedulesJson, _T("registrationEnd"), errCode);
        result.m_checkinStartTime = utils::extract_json_time(schedulesJson, _T("checkinStart"), errCode);
        result.m_checkinEndTime = utils::extract_json_time(schedulesJson, _T("checkinEnd"), errCode);
        result.m_playingStartTime = utils::extract_json_time(schedulesJson, _T("playingStart"), errCode);
        result.m_playingEndTime = utils::extract_json_time(schedulesJson, _T("playingEnd"), errCode);
    }

    if (!teamJson.is_null())
    {
        auto teamResult = team_summary::_Deserialize(teamJson);
        if (teamResult.err())
        {
            errCode = teamResult.err();
        }
        result.m_teamSummary = teamResult.payload();
    }

    return xbox_live_result<tournament>(result, errCode);
}

xbox::services::tournaments::tournament_state
tournament::_Convert_string_to_tournament_state(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("active")) == 0)
    {
        return xbox::services::tournaments::tournament_state::active;
    }
    else if (utils::str_icmp(value, _T("canceled")) == 0)
    {
        return xbox::services::tournaments::tournament_state::canceled;
    }
    else if (utils::str_icmp(value, _T("completed")) == 0)
    {
        return xbox::services::tournaments::tournament_state::completed;
    }

    return xbox::services::tournaments::tournament_state::unknown;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GameLogic\Multiplayer.h"

using namespace Concurrency;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::tournaments;

void Sample::GetTournaments()
{
    static bool filterUsersForMe = true;
    tournament_request request = tournament_request(filterUsersForMe);
    std::vector<tournament_state> states;
    states.push_back(tournament_state::active);
    states.push_back(tournament_state::canceled);
    request.set_state_filter(states);

    auto context = m_liveResources->GetLiveContext();
    auto result = context->tournament_service().get_tournaments(request)
    .then([this](xbox_live_result<tournament_request_result> result)
    {
        if (!result.err())
        {
            auto& tournamentResult = result.payload();

            // For testing, assign the first tournament as the locally cached tournament.
            for (const auto& tournament : tournamentResult.tournaments())
            {
                if (!tournament.team_summary().is_null())
                {
                    m_organizerId = tournament.organizer_id();
                    m_tournamentId = tournament.id();
                    break;
                }
            }

            // Get next page until the end
            xbox_live_result<tournament_request_result> lastResult;
            lastResult.set_payload(tournamentResult);
            while (!lastResult.err() && lastResult.payload().has_next())
            {
                lastResult.payload().get_next()
                .then([this, &lastResult](xbox_live_result<tournament_request_result> newTournamentResult)
                {
                    lastResult = newTournamentResult;
                }).wait();
            }

            // Get Tournament Details
            auto context = m_liveResources->GetLiveContext();
            auto getTournamentResult = context->tournament_service().get_tournament_details(m_organizerId, m_tournamentId)
            .then([this](xbox_live_result<tournament> result)
            {
                if (result.err())
                {
                    LogErrorFormat(L"get_tournament_details failed: %S\n", result.err_message().c_str());
                }
            });
        }
        else
        {
            if (result.err() == xbox_live_error_code::out_of_range)
            {
                // we hit the end of the tournaments
            }
            else
            {
                LogErrorFormat(L"get_tournaments failed: %S\n", result.err_message().c_str());
            }
        }
    });
}

void Sample::GetTeams()
{
    static bool filterUsersForMe = false;
    team_request request = team_request(m_organizerId, m_tournamentId, filterUsersForMe);
    std::vector<team_state> states;
    states.push_back(team_state::registered);
    states.push_back(team_state::checked_in);
    request.set_state_filter(states);
    request.set_max_items(1);

    auto context = m_liveResources->GetLiveContext();
    auto result = context->tournament_service().get_teams(request)
    .then([this](xbox_live_result<team_request_result> result)
    {
        if (!result.err())
        {
            auto& teamResult = result.payload();

            // For testing, assign the first team as the locally cached team.
            for (const auto& team : teamResult.teams())
            {
                m_teamId = team.id();
                break;
            }

            // Get next page until the end
            xbox_live_result<team_request_result> lastResult;
            lastResult.set_payload(teamResult);
            while (!lastResult.err() && lastResult.payload().has_next())
            {
                lastResult.payload().get_next()
                .then([this, &lastResult](xbox_live_result<team_request_result> newTeamResult)
                {
                    lastResult = newTeamResult;
                }).wait();
            }

            // Get Team Details
            auto context = m_liveResources->GetLiveContext();
            auto getTeamResult = context->tournament_service().get_team_details(m_organizerId, m_tournamentId, m_teamId)
            .then([this](xbox_live_result<team_info> result)
            {
                if (result.err())
                {
                    LogErrorFormat(L"get_team_details failed: %S\n", result.err_message().c_str());
                }
            });
        }
        else
        {
            if (result.err() == xbox_live_error_code::out_of_range)
            {
                // we hit the end of the tournaments
            }
            else
            {
                LogErrorFormat(L"get_teams failed: %S\n", result.err_message().c_str());
            }
        }
    });
}
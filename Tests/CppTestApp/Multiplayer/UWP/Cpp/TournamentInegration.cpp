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
    auto context = m_liveResources->GetLiveContext();

    if (m_organizerId.empty() && m_tournamentId.empty())
    {
        // If coming for the first time, subscribe for RTA.
        context->real_time_activity_service()->activate();
        context->tournament_service().add_tournament_changed_handler(
        [this](tournament_change_event_args args)
        {
            GetTournamentDetails(args.organizer_id(), args.tournament_id());
        });

        context->tournament_service().add_team_changed_handler(
        [this, context](team_change_event_args args)
        {
            GetTeamDetails(args.organizer_id(), args.tournament_id(), args.team_id());
        });
    }

    static bool filterUsersForMe = true;
    tournament_request request = tournament_request(filterUsersForMe);
    std::vector<tournament_state> states;
    states.push_back(tournament_state::active);
    states.push_back(tournament_state::canceled);
    request.set_state_filter(states);

    auto result = context->tournament_service().get_tournaments(request)
    .then([this](xbox_live_result<tournament_request_result> result)
    {
        if (!result.err())
        {
            auto& tournamentResult = result.payload();

            // For testing, assign the first tournament as the locally cached tournament.
            for (const auto& tournament : tournamentResult.tournaments())
            {
                const string_t& organizerId = tournament.organizer_id();
                const string_t& tournamentId = tournament.id();
                if (!tournament.team_summary().is_null() && 
                    m_organizerId.empty() &&
                    m_tournamentId.empty())
                {
                    m_organizerId = organizerId;
                    m_tournamentId = tournamentId;
                    GetTournamentDetails(organizerId, tournamentId);
                    SubscribeForTournamentRTASubscription(organizerId, tournamentId);
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

void Sample::GetTournamentDetails(
    const string_t& organizerId,
    const string_t& tournamentId
    )
{
    auto context = m_liveResources->GetLiveContext();
    context->tournament_service().get_tournament_details(organizerId, tournamentId)
    .then([this](xbox_live_result<tournament> result)
    {
        if (result.err())
        {
            LogErrorFormat(L"get_tournament_details failed: %S\n", result.err_message().c_str());
        }
    });
}

void Sample::SubscribeForTournamentRTASubscription(
    const string_t& organizerId,
    const string_t& tournamentId
    )
{
    auto context = m_liveResources->GetLiveContext();
    auto tournamentResults = context->tournament_service().subscribe_to_tournament_change(
        organizerId,
        tournamentId
        );

    if (tournamentResults.err())
    {
        LogErrorFormat(L"Error calling subscribe_to_tournament_change:: %S\n", tournamentResults.err_message().c_str());
    }
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
            GetTeamDetailsAndSubscribeForRTA(result);

            // Get next page until the end
            xbox_live_result<team_request_result> lastResult;
            lastResult.set_payload(result.payload());
            while (!lastResult.err() && lastResult.payload().has_next())
            {
                lastResult.payload().get_next()
                .then([this, &lastResult](xbox_live_result<team_request_result> newTeamResult)
                {
                    lastResult = newTeamResult;
                    GetTeamDetailsAndSubscribeForRTA(lastResult);
                }).wait();
            }
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

void Sample::GetTeamDetailsAndSubscribeForRTA(xbox_live_result<team_request_result> &result)
{
    auto& teamResult = result.payload();
    for (const auto& team : teamResult.teams())
    {
        const string_t& teamId = team.id();
        GetTeamDetails(m_organizerId, m_tournamentId, teamId);
        SubscribeForTeamRTASubscription(m_organizerId, m_tournamentId, teamId);
    }
}

void Sample::GetTeamDetails(
    const string_t& organizerId,
    const string_t& tournamentId,
    const string_t& teamId
    )
{
    // Get Team Details
    auto context = m_liveResources->GetLiveContext();
    context->tournament_service().get_team_details(organizerId, tournamentId, teamId)
    .then([this](xbox_live_result<team_info> result)
    {
        if (result.err())
        {
            LogErrorFormat(L"get_team_details failed: %S\n", result.err_message().c_str());
        }
    });
}

void Sample::SubscribeForTeamRTASubscription(
    const string_t& organizerId,
    const string_t& tournamentId,
    const string_t& teamId
    )
{
    auto context = m_liveResources->GetLiveContext();
    auto teamResults = context->tournament_service().subscribe_to_team_change(
        organizerId,
        tournamentId,
        teamId
        );

    if (!teamResults.err())
    {
        m_teamSubscription = teamResults.payload();
    }
    else
    {
        LogErrorFormat(L"Error calling subscribe_to_team_change:: %S\n", teamResults.err_message().c_str());
    }
}
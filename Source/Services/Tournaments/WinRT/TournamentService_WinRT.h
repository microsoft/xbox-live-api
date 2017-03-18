// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/tournaments.h"
#include "Tournament_WinRT.h"
#include "TournamentRequest_WinRT.h"
#include "TournamentRequestResult_WinRT.h"
#include "TeamInfo_WinRT.h"
#include "TeamRequest_WinRT.h"
#include "TeamRequestResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents an endpoint that you can use to access the Tournament service.
/// </summary>
public ref class TournamentService sealed
{
public:
    /// <summary>
    /// Returns a list of tournaments for the current title.
    /// </summary>
    /// <param name="request">A tournament request object that retrieves tournaments based on the configuration of the request.</param>
    /// <returns>A list of tournaments if they exist.</returns>
    /// <remarks>
    /// Returns a concurrency::task<T> object that represents the state of the asynchronous operation.
    /// This method calls GET /tournaments
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TournamentRequestResult^>^  GetTournamentsAsync(
        _In_ TournamentRequest^ request
        );

    /// <summary>
    /// Returns a specific tournament object.
    /// The tournament ID and the organizer ID together uniquely identify a tournament.
    /// </summary>
    /// <param name="organizerId">The ID of the tournament organizer.</param>
    /// <param name="tournamentId">The ID of the tournament.</param>
    /// <returns>The specific tournament if they exist.</returns>
    /// <remarks>
    /// Returns a concurrency::task<T> object that represents the state of the asynchronous operation.
    /// This method calls GET /tournaments/{organizer}/{id}.
    /// </remarks>
    Windows::Foundation::IAsyncOperation<Tournament^>^ GetTournamentDetailsAsync(
        _In_ Platform::String^ organizerId,
        _In_ Platform::String^ tournamentId
        );

    /// <summary>
    /// Returns a list of tournaments for the current title.
    /// </summary>
    /// <param name="request">A team request object that retrieves team based on the configuration of the request.</param>
    /// <returns>A list of teams if they exist.</returns>
    /// <remarks>
    /// Returns a concurrency::task<T> object that represents the state of the asynchronous operation.
    /// This method calls GET /tournaments/{organizer}/{id}/teams
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TeamRequestResult^>^ GetTeamsAsync(
        _In_ TeamRequest^ request
        );

    /// <summary>
    /// Returns a specific tournament object.
    /// The tournament ID and the organizer ID together uniquely identify a tournament.
    /// </summary>
    /// <param name="organizerId">The ID of the tournament organizer. This is case sensitive.</param>
    /// <param name="tournamentId">The ID of the tournament.</param>
    /// <param name="teamId">The ID of the team.</param>
    /// <returns>A specific tournament if they exist.</returns>
    /// <remarks>
    /// Returns a concurrency::task<T> object that represents the state of the asynchronous operation.
    /// This method calls GET /tournaments/{organizer}/{id}/teams/{teamId}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TeamInfo^>^ GetTeamDetailsAsync(
        _In_ Platform::String^ organizerId,
        _In_ Platform::String^ tournamentId,
        _In_ Platform::String^ teamId
        );

internal:
    TournamentService(
        _In_ xbox::services::tournaments::tournament_service cppObj
        );

    const xbox::services::tournaments::tournament_service& GetCppObj() const;
private:
    xbox::services::tournaments::tournament_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END

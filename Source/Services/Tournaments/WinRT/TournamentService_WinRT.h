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
#include "TeamChangeEventArgs_WinRT.h"
#include "TeamChangeSubscription_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

class TeamEventBind : public std::enable_shared_from_this<TeamEventBind>
{
public:
    TeamEventBind(
        _In_ Platform::WeakReference setting,
        _In_ xbox::services::tournaments::tournament_service& cppObj
        );

    void AddTeamEvent();

    void RemoveTeamEvent(
        _In_ xbox::services::tournaments::tournament_service& cppObj
        );

    void TeamChangeRouter(_In_ const xbox::services::tournaments::team_change_event_args& teamChangeEventArgs);

private:
    function_context m_functionContext;
    Platform::WeakReference m_setting;
    xbox::services::tournaments::tournament_service m_cppObj;
};

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
    /// Returns a concurrency::task{T} object that represents the state of the asynchronous operation.
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
    /// Returns a concurrency::task{T} object that represents the state of the asynchronous operation.
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
    /// Returns a concurrency::task{T} object that represents the state of the asynchronous operation.
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
    /// Returns a concurrency::task{T} object that represents the state of the asynchronous operation.
    /// This method calls GET /tournaments/{organizer}/{id}/teams/{teamId}
    /// </remarks>
    Windows::Foundation::IAsyncOperation<TeamInfo^>^ GetTeamDetailsAsync(
        _In_ Platform::String^ organizerId,
        _In_ Platform::String^ tournamentId,
        _In_ Platform::String^ teamId
        );

    /// <summary>
    /// Registers for team change notifications.  Event handlers receive a TeamChangeEventArgs^ object.
    /// </summary>
    event Windows::Foundation::EventHandler<TeamChangeEventArgs^>^ TeamChanged;

    /// <summary>
    /// Subscribes to team update notifications via the TeamChanged event.
    /// </summary>
    /// <param name="organizerId">The ID of the tournament organizer. This is case sensitive.</param>
    /// <param name="tournamentId">The ID of the tournament.</param>
    /// <param name="teamId">The ID of the team.</param>
    /// <returns>A subscription object that contains the state of the subscription. 
    /// You can register an event handler for team changes by using the TeamChanged event.</returns>
    TeamChangeSubscription^ SubscribeToTeamChange(
        _In_ Platform::String^ organizerId,
        _In_ Platform::String^ tournamentId,
        _In_ Platform::String^ teamId
        );

    /// <summary>
    /// Unsubscribe a previously created team change subscription.
    /// </summary>
    /// <param name="subscription">The subscription object to unsubscribe.</param>
    void UnsubscribeFromTeamChange(
        _In_ TeamChangeSubscription^ subscription
        );

internal:
    TournamentService(
        _In_ xbox::services::tournaments::tournament_service cppObj
        );

    const xbox::services::tournaments::tournament_service& GetCppObj() const;

    void RaiseTeamChange(_In_ TeamChangeEventArgs^ args);
private:
    xbox::services::tournaments::tournament_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END

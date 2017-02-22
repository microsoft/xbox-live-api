// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Defines values used to indicate the reason why the team in under selected tournament registration state.
/// </summary>
public enum class TournamentRegistrationReason
{
    /// <summary>
    /// The team registration reason is unknown.
    /// </summary>
    Unknown = xbox::services::tournaments::tournament_registration_reason::unknown,

    /// <summary>
    /// The registration for this tournament has been closed.
    /// </summary>
    RegistrationClosed = xbox::services::tournaments::tournament_registration_reason::registration_closed,

    /// <summary>
    /// One of the team members have already been registered for this tournament.
    /// </summary>
    MemberAlreadyRegistered = xbox::services::tournaments::tournament_registration_reason::member_already_registered,

    /// <summary>
    /// The tournament has reached its max team registration limit and is now full.
    /// </summary>
    TournamentFull = xbox::services::tournaments::tournament_registration_reason::tournament_full,

    /// <summary>
    /// The team has been eliminated from the tournament.
    /// </summary>
    TeamEliminated = xbox::services::tournaments::tournament_registration_reason::team_eliminated,

    /// <summary>
    /// The tournament has been completed.
    /// </summary>
    TournamentCompleted = xbox::services::tournaments::tournament_registration_reason::tournament_completed
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Defines values used to indicate the state of a tournament game.
/// </summary>
public enum class TournamentArbitrationStatus
{
    /// <summary>
    /// Occurs before arbitrationStartTime.
    /// </summary>
    Waiting = xbox::services::tournaments::tournament_arbitration_status::waiting,

    /// <summary>
    /// Occurs after arbitrationStartTime and at least one user has gone active.
    /// </summary>
    InProgress = xbox::services::tournaments::tournament_arbitration_status::in_progress,

    /// <summary>
    /// The user has reported results and his role in the arbitration process is now done.
    /// Occurs either after (arbitrationForfeitTime and no players have joined), (arbitration succeeded), or (arbitrationFinishTime).
    /// </summary>
    Complete = xbox::services::tournaments::tournament_arbitration_status::complete,

    /// <summary>
    /// The user has gone active at least once and is now participating in the match.
    /// </summary>
    Playing = xbox::services::tournaments::tournament_arbitration_status::playing,

    /// <summary>
    /// The user was not able to upload results before the arbitrationTimeout deadline.
    /// </summary>
    InComplete = xbox::services::tournaments::tournament_arbitration_status::incomplete,

    /// <summary>
    /// The status is "joining" until both the TournamentArbitrationStatus is "InProgress" and the member is active.
    /// </summary>
    Joining = xbox::services::tournaments::tournament_arbitration_status::joining
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
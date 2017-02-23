// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Defines values used to indicate the state of a tournament game.
/// </summary>
public enum class TournamentArbitrationState
{
    /// <summary>
    /// No arbitration state set.
    /// </summary>
    None = xbox::services::tournaments::tournament_arbitration_state::none,

    /// <summary>
    /// Results were fully uploaded and complete
    /// </summary>
    Completed = xbox::services::tournaments::tournament_arbitration_state::completed,

    /// <summary>
    /// The match was canceled, for example in the case of forfeiting
    /// </summary>
    Canceled = xbox::services::tournaments::tournament_arbitration_state::canceled,

    /// <summary>
    /// The match begun, but no players or servers reported results in time before the arbitration deadline
    /// </summary>
    NoResults = xbox::services::tournaments::tournament_arbitration_state::no_results,

    /// <summary>
    /// Some results were received, and results were compiled based on this incomplete data
    /// </summary>
    PartialResults = xbox::services::tournaments::tournament_arbitration_state::partial_results,

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
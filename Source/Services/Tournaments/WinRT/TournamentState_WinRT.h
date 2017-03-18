// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Defines values used to indicate the state for a tournament.
/// </summary>
public enum class TournamentState
{
    /// <summary>
    /// The Tournament state is unknown.
    /// </summary>
    Unknown = xbox::services::tournaments::tournament_state::unknown,

    /// <summary>
    /// Default. The Tournament is currently active.
    /// </summary>
    Active = xbox::services::tournaments::tournament_state::active,

    /// <summary>
    /// The Tournament was canceled.
    /// </summary>
    Canceled = xbox::services::tournaments::tournament_state::canceled,

    /// <summary>
    ///  The Tournament has been completed.
    /// </summary>
    Completed = xbox::services::tournaments::tournament_state::completed

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
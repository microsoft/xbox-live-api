// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Defines values used to indicate the source for a tournament game state.
/// </summary>
public enum class TournamentGameResultSource
{
    /// <summary>
    /// No game result source .
    /// </summary>
    None = xbox::services::tournaments::tournament_game_result_source::none,

    /// <summary>
    /// Game result source is from client arbitration submit.
    /// </summary>
    Arbitration = xbox::services::tournaments::tournament_game_result_source::arbitration,

    /// <summary>
    /// Game result is determined by game servers.
    /// </summary>
    Server = xbox::services::tournaments::tournament_game_result_source::server,

    /// <summary>
    /// Game result is adjusted by tournament administrator.
    /// </summary>
    Adjusted = xbox::services::tournaments::tournament_game_result_source::adjusted,

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
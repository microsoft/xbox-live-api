// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Defines values used to indicate the state of a tournament game.
/// </summary>
public enum class TournamentGameResultState
{
    /// <summary>
    /// No game
    /// </summary>
    NoContest = xbox::services::tournaments::tournament_game_result_state::no_contest,

    /// <summary>
    /// Win
    /// </summary>
    Win = xbox::services::tournaments::tournament_game_result_state::win,

    /// <summary>
    /// Loss
    /// </summary>
    Loss = xbox::services::tournaments::tournament_game_result_state::loss,

    /// <summary>
    /// Draw
    /// </summary>
    Draw = xbox::services::tournaments::tournament_game_result_state::draw,

    /// <summary>
    /// Rank
    /// </summary>
    Rank = xbox::services::tournaments::tournament_game_result_state::rank,

    /// <summary>
    /// Didn't show up
    /// </summary>
    NoShow = xbox::services::tournaments::tournament_game_result_state::no_show,

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
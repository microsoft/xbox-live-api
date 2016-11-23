//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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
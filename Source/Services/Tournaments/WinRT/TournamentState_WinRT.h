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
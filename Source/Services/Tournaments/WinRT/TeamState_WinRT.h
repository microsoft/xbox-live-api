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
/// 
/// </summary>
public enum class TeamState
{
    /// <summary>
    /// The team state is unknown.
    /// </summary>
    Unknown = xbox::services::tournaments::team_state::unknown,

    /// <summary>
    ///
    /// </summary>
    Registered = xbox::services::tournaments::team_state::registered,

    /// <summary>
    ///
    /// </summary>
    Waitlisted = xbox::services::tournaments::team_state::waitlisted,

    /// <summary>
    ///
    /// </summary>
    StandBy = xbox::services::tournaments::team_state::stand_by,

    /// <summary>
    ///
    /// </summary>
    CheckedIn = xbox::services::tournaments::team_state::checked_in,

    /// <summary>
    ///
    /// </summary>
    Playing = xbox::services::tournaments::team_state::playing,

    /// <summary>
    ////
    /// </summary>
    Completed = xbox::services::tournaments::team_state::completed

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
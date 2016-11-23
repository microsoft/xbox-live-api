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
/// Defines values used to indicate the team session registration state for a tournament.
/// </summary>
public enum class TournamentRegistrationState
{
    /// <summary>
    /// The team registration state is unknown.
    /// </summary>
    Unknown = xbox::services::tournaments::tournament_registration_state::unknown,

    /// <summary>
    /// Registration was successfully received by the Tournament service and will be eventually processed.
    /// </summary>
    Pending = xbox::services::tournaments::tournament_registration_state::pending,

    /// <summary>
    /// Registration for the team was withdrawn.
    /// </summary>
    Withdrawn = xbox::services::tournaments::tournament_registration_state::withdrawn,
    
    /// <summary>
    /// Registration could not be performed for the team.
    /// </summary>
    Rejected = xbox::services::tournaments::tournament_registration_state::rejected,

    /// <summary>
    /// Registration has been confirmed by the Tournament service.
    /// </summary>
    Registered = xbox::services::tournaments::tournament_registration_state::registered,

    /// <summary>
    /// The team has completed its participation in the Tournament.
    /// </summary>
    Completed = xbox::services::tournaments::tournament_registration_state::completed
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
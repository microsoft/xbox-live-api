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
public enum class TeamCompletedReason
{
    /// <summary>
    ///
    /// </summary>
    Rejected = xbox::services::tournaments::team_completed_reason::rejected,

    /// <summary>
    ///
    /// </summary>
    Eliminated = xbox::services::tournaments::team_completed_reason::eliminated,

    /// <summary>
    ///
    /// </summary>
    Evicted = xbox::services::tournaments::team_completed_reason::evicted,

    /// <summary>
    ///
    /// </summary>
    Completed = xbox::services::tournaments::team_completed_reason::completed

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
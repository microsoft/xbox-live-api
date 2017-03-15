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
public enum class TournamentOrderBy
{

    /// <summary>
    /// No order by provided.
    /// </summary>
    None = xbox::services::tournaments::tournament_order_by::none,

    /// <summary>
    /// The only valid sort order for playing_start is tournament_sort_order::ascending. 
    /// TODO: clarify what this is used for
    /// </summary>
    StartTime = xbox::services::tournaments::tournament_order_by::start_time,

    /// <summary>
    /// end_time is only set on completed tournaments so it is only valid to sort by end_time when 
    /// the state query parameter is set to tournament_state::completed.
    /// </summary>
    EndTime = xbox::services::tournaments::tournament_order_by::end_time
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
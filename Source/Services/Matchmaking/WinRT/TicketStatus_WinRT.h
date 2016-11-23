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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_BEGIN

/// <summary>
/// Defines values that indicate the status of the match request.
/// request.
/// </summary>
public enum class TicketStatus
{
    /// <summary>
    /// The status of the match request has not been returned by the server yet
    /// or the server returned an unrecognized response.
    /// </summary>
    Unknown = xbox::services::matchmaking::ticket_status::unknown,

    /// <summary>
    /// Matchmaking has not found a match and the search
    /// request has expired according to its give up duration.
    /// </summary>
    Expired = xbox::services::matchmaking::ticket_status::expired,

    /// <summary>
    /// Matchmaking has not found a match yet and it is
    /// still searching.
    /// </summary>
    Searching = xbox::services::matchmaking::ticket_status::searching,

    /// <summary>
    /// Matchmaking has found a match and the ticket contains a
    /// reference to the session that is to be created.
    /// </summary>
    Found = xbox::services::matchmaking::ticket_status::found,

    /// <summary>
    /// Matchmaking has been canceled for this ticket.
    /// </summary>
    Canceled = xbox::services::matchmaking::ticket_status::canceled
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_END
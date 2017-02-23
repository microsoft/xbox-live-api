// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "PreserveSessionMode_WinRT.h"
#include "TicketStatus_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_BEGIN

/// <summary>
/// Represents a server response to a request for match ticket details.
/// </summary>
public ref class MatchTicketDetailsResponse sealed
{
public:
    /// <summary>
    /// Status of a match request.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(MatchStatus, match_status, TicketStatus);

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(EstimatedWaitTime, estimated_wait_time);

    /// <summary>
    /// An enum value to specify whether the match should preserve the session on which the match has been requested.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(PreserveSession, preserve_session, PreserveSessionMode);

    /// <summary>
    /// The session on which the match was requested.
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ TicketSession
    {
        Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ get();
    }

    /// <summary>
    /// The session on which a match request has been found.
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ TargetSession
    {
        Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ get();
    }

    /// <summary>
    /// The attributes of a match request.
    /// </summary>
    property Platform::String^ TicketAttributes
    {
        Platform::String^ get();
    }

internal:
    MatchTicketDetailsResponse(
        _In_ xbox::services::matchmaking::match_ticket_details_response cppObj
        );

private:
    Multiplayer::MultiplayerSessionReference^ m_ticketSession;
    Multiplayer::MultiplayerSessionReference^ m_targetSession;
    Platform::String^ m_ticketAttributes;
    xbox::services::matchmaking::match_ticket_details_response m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_END
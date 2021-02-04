// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#if !defined(__cplusplus)
#error C++11 required
#endif

#pragma once
#include "multiplayer_c.h"

extern "C"
{
/// <summary>
/// Defines values used to indicate whether a match ticket is for a new
/// game session or an existing session.
/// </summary>
enum class XblPreserveSessionMode : uint32_t
{
    /// <summary>
    /// The server returned an unrecognized response.
    /// </summary>
    Unknown,

    /// <summary>
    /// Always use an existing game session.  
    /// This is for matching more players for a game session 
    /// that is already created or in progress.
    /// </summary>
    Always,

    /// <summary>
    /// Never use an existing game session.  
    /// This is for matching players for a new game session.
    /// </summary>
    Never
};

/// <summary>
/// Defines values used to indicate the status of the match request.
/// </summary>
enum class XblTicketStatus : uint32_t
{
    /// <summary>
    /// The status of the match request has not been returned by the server yet
    /// or the server returned an unrecognized response.
    /// </summary>
    Unknown,

    /// <summary>
    /// Matchmaking has not found a match and the search
    /// request has expired according to its give up duration.
    /// </summary>
    Expired,

    /// <summary>
    /// Matchmaking has not found a match yet and it is
    /// still searching.
    /// </summary>
    Searching,

    /// <summary>
    /// Matchmaking has found a match and the ticket contains a
    /// reference to the session that is to be created.
    /// </summary>
    Found,

    /// <summary>
    /// Matchmaking has been canceled for this ticket.
    /// </summary>
    Canceled
};

/// <summary>
/// Contains information about server response to a create match ticket request.
/// </summary>
typedef struct XblCreateMatchTicketResponse 
{
    /// <summary>
    /// Ticket ID of a match request.
    /// </summary>
    _Field_z_ char matchTicketId[XBL_SCID_LENGTH];

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    int64_t estimatedWaitTime;

} XblCreateMatchTicketResponse;

/// <summary>
/// Represents a server response to a request for match ticket details.
/// </summary>
typedef struct XblMatchTicketDetailsResponse 
{
    /// <summary>
    /// Status of a match request.
    /// </summary>
    XblTicketStatus matchStatus;

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    int64_t estimatedWaitTime;

    /// <summary>
    /// An enum value to specify whether the match should preserve the session on which the match has been requested.
    /// </summary>
    XblPreserveSessionMode preserveSession;

    /// <summary>
    /// The session on which the match was requested.
    /// </summary>
    XblMultiplayerSessionReference ticketSession;
        
    /// <summary>
    /// The session on which a match request has been found.
    /// </summary>
    XblMultiplayerSessionReference targetSession;

    /// <summary>
    /// The attributes of a match request.
    /// </summary>
    char* ticketAttributes;

} XblMatchTicketDetailsResponse;


/// <summary>
/// Represents a server response to a hopper statistics request.
/// </summary>
typedef struct XblHopperStatisticsResponse 
{
    /// <summary>
    /// Name of the hopper in which a match was requested.
    /// </summary>
    _Field_z_ char* hopperName;

    /// <summary>
    /// Estimated wait time for a match request to be matched with other players.
    /// </summary>
    int64_t estimatedWaitTime;

    /// <summary>
    /// The number of players in the hopper waiting to be matched.
    /// </summary>
    uint32_t playersWaitingToMatch;

} XblHopperStatisticsResponse;

/// <summary>
/// Sends a matchmaking request to the server and returns the match ticket with a ticket id.  
/// Call XblMatchmakingCreateMatchTicketResult upon completion to get the result.
/// </summary>
/// <param name="xboxLiveContext">Xbox live context for the local user.</param>
/// <param name="ticketSessionReference">The multiplayer session to use for the match.</param>
/// <param name="matchmakingServiceConfigurationId">The service configuration ID for the match.</param>
/// <param name="hopperName">The name of the hopper.</param>
/// <param name="ticketTimeout">The maximum time to wait for players to join the session.</param>
/// <param name="preserveSession">Indicates if the session should be preserved.</param>
/// <param name="ticketAttributesJson">The ticket attributes for the session. (Optional)</param>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <rest>Calls V103 POST /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}</rest>
/// <remarks>
/// The match ticket object contains server returned information such as ticket id and wait
/// time, and also contains copies of the title specified data from the ticket data object.
/// </remarks>
STDAPI XblMatchmakingCreateMatchTicketAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ XblMultiplayerSessionReference ticketSessionReference,
    _In_ const char* matchmakingServiceConfigurationId,
    _In_ const char* hopperName,
    _In_ const uint64_t ticketTimeout,
    _In_ XblPreserveSessionMode preserveSession,
    _In_ const char* ticketAttributesJson,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblMatchmakingCreateMatchTicketAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="resultPtr">A caller allocated struct to be filled with the match ticket results.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMatchmakingCreateMatchTicketResult(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ XblCreateMatchTicketResponse* resultPtr
) XBL_NOEXCEPT;

/// <summary>
/// Deletes a the match ticket on the server.
/// </summary>
/// <param name="xboxLiveContext">Xbox live context for the local user.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="hopperName">The name of the hopper where the match ticket is located.</param>
/// <param name="ticketId">The id of the ticket to delete on the server.</param>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <rest>Calls V103 DELETE /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/tickets/{ticketId}</rest>
STDAPI XblMatchmakingDeleteMatchTicketAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ const char* serviceConfigurationId,
    _In_ const char* hopperName,
    _In_ const char* ticketId,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves the properties of a match ticket from the server.
/// </summary>
/// <param name="xboxLiveContext">Xbox live context for the local user.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="hopperName">The name of the hopper where the match ticket is located.</param>
/// <param name="ticketId">The ticket id of the match ticket to retrieve.</param>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <rest>Calls V103 GET /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/tickets/{ticketId}</rest>
STDAPI XblMatchmakingGetMatchTicketDetailsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ const char* serviceConfigurationId,
    _In_ const char* hopperName,
    _In_ const char* ticketId,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblMatchmakingGetMatchTicketDetailsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes required to store the Create Match Ticket result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMatchmakingGetMatchTicketDetailsResultSize(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblMatchmakingGetMatchTicketDetailsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.
/// Use <see cref="XblMatchmakingGetMatchTicketDetailsResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer to write result into.</param>
/// <param name="ptrToBuffer">Strongly typed pointer that points into buffer.  
/// This is a pointer within buffer and should not be freed separately.</param>
/// <param name="bufferUsed">Number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMatchmakingGetMatchTicketDetailsResult(
    _In_ XAsyncBlock* asyncBlock,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblMatchTicketDetailsResponse** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;

/// <summary>
/// Gets statistics about a hopper such as how many players are in it.
/// </summary>
/// <param name="xboxLiveContext">Xbox live context for the local user.</param>
/// <param name="serviceConfigurationId">The Service Configuration ID (SCID) for the title. The SCID is considered case sensitive so paste it directly from the Partner Center.</param>
/// <param name="hopperName">The name of the hopper to query stats for.</param>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <rest>Calls V103 GET /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/stats</rest>
STDAPI XblMatchmakingGetHopperStatisticsAsync(
    _In_ XblContextHandle xboxLiveContext,
    _In_ const char* serviceConfigurationId,
    _In_ const char* hopperName,
    _In_ XAsyncBlock* asyncBlock
) XBL_NOEXCEPT;

/// <summary>
/// Get the result size for an XblMatchmakingGetHopperStatisticsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="resultSizeInBytes">The size in bytes required to store the Create Match Ticket result.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMatchmakingGetHopperStatisticsResultSize(
    _In_ XAsyncBlock* asyncBlock,
    _Out_ size_t* resultSizeInBytes
) XBL_NOEXCEPT;

/// <summary>
/// Get the result for an XblMatchmakingGetHopperStatisticsAsync call.
/// </summary>
/// <param name="asyncBlock">The AsyncBlock for this operation.</param>
/// <param name="bufferSize">The size of the provided buffer.
/// Use <see cref="XblMatchmakingGetHopperStatisticsResultSize"/> to get the size required.</param>
/// <param name="buffer">A caller allocated byte buffer to write result into.</param>
/// <param name="ptrToBuffer">Strongly typed pointer that points into buffer.  
/// This is a pointer within buffer and should not be freed separately.</param>
/// <param name="bufferUsed">Number of bytes written to the buffer.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMatchmakingGetHopperStatisticsResult(
    _In_ XAsyncBlock* asyncBlock,
    _In_ size_t bufferSize,
    _Out_writes_bytes_to_(bufferSize, *bufferUsed) void* buffer,
    _Outptr_ XblHopperStatisticsResponse** ptrToBuffer,
    _Out_opt_ size_t* bufferUsed
) XBL_NOEXCEPT;
} // end extern c
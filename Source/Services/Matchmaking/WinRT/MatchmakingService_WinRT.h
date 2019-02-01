// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "CreateMatchTicketResponse_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"
#include "MatchTicketDetailsResponse_WinRT.h"
#include "PreserveSessionMode_WinRT.h"
#include "HopperStatisticsResponse_WinRT.h"
#include "xsapi/matchmaking.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_BEGIN
/// <summary>
/// Represents a service for matchmaking. 
/// </summary>

public ref class MatchmakingService sealed
{
public:
    /// <summary>
    /// Sends a matchmaking request to the server and returns the match ticket with a ticket id.
    /// </summary>
    /// <param name="ticketSessionReference">The multiplayer session to use for the match.</param>
    /// <param name="matchmakingServiceConfigurationId">The service configuration ID for the match.</param>
    /// <param name="hopperName">The name of the hopper.</param>
    /// <param name="ticketTimeout">The maximum time to wait for players to join the session.</param>
    /// <param name="preserveSession">Indicates if the session should be preserved.</param>
    /// <param name="ticketAttributesJson">The ticket attributes for the session. (Optional)</param>
    /// <param name="isSymmetric">Use symmetric evaluation (Bi-directional) of matchmaking tickets (Optional)</param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, a new match ticket
    /// object is returned. The match ticket object contains server returned information such as ticket id and wait
    /// time, and also contains copies of the title specified data from the ticket data object.</returns>
    /// <remarks>Calls V103 POST /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}</remarks>
    Windows::Foundation::IAsyncOperation<CreateMatchTicketResponse^>^ CreateMatchTicketAsync(
        _In_ Multiplayer::MultiplayerSessionReference^ ticketSessionReference,
        _In_ Platform::String^ matchmakingServiceConfigurationId,
        _In_ Platform::String^ hopperName,
        _In_ Windows::Foundation::TimeSpan ticketTimeout,
        _In_ PreserveSessionMode preserveSession,
        _In_opt_ Platform::String^ ticketAttributesJson,
        _In_ Platform::Boolean isSymmetric
        );

    /// <summary>
    /// Deletes a the match ticket on the server.
    /// </summary>
    /// <param name="serviceConfigurationId">The service config id that is specific for the title.</param>
    /// <param name="hopperName">The name of the hopper where the match ticket is located.</param>
    /// <param name="ticketId">The id of the ticket to delete on the server.</param>
    /// <returns>The async object for notifying when the operation has been completed.</returns>
    /// <remarks>Calls V103 DELETE /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/tickets/{ticketId}</remarks>
    Windows::Foundation::IAsyncAction^ DeleteMatchTicketAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ hopperName,
        _In_ Platform::String^ ticketId
        );

    /// <summary>
    /// Retrieves the properties of a match ticket from the server.
    /// </summary>
    /// <param name="serviceConfigurationId">The service config id that is specific for the title.</param>
    /// <param name="hopperName">The name of the hopper where the match ticket is located.</param>
    /// <param name="ticketId">The ticket id of the match ticket to retrieve.</param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, the match
    /// ticket object with the data for the ticket, including ticket id and wait time information, is returned
    /// returned from the server.</returns>
    /// <remarks>Calls V103 GET /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/tickets/{ticketId}</remarks>
    Windows::Foundation::IAsyncOperation<MatchTicketDetailsResponse^>^ GetMatchTicketDetailsAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ hopperName,
        _In_ Platform::String^ ticketId
        );

    /// <summary>
    /// Gets statistics about a hopper such as how many players are in it.
    /// </summary>
    /// <param name="serviceConfigurationId">The service config id that is specific for the title.</param>
    /// <param name="hopperName">The name of the hopper to query stats for.</param>
    /// <returns>The async object for notifying when the operation is completed. With the handler, an object
    /// containing statistics about the hopper is returned.</returns>
    /// <remarks>Calls V103 GET /serviceconfigs/{serviceConfigId}/hoppers/{hopperName}/stats</remarks>
    Windows::Foundation::IAsyncOperation<HopperStatisticsResponse^>^ GetHopperStatisticsAsync(
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ hopperName
        );

internal:
    MatchmakingService(
        _In_ xbox::services::matchmaking::matchmaking_service cppObj
        );

private:
    xbox::services::matchmaking::matchmaking_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MATCHMAKING_END

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
#include "xsapi/game_server_platform.h"
#include "ClusterResult_WinRT.h"
#include "AllocationResult_WinRT.h"
#include "GameServerTicketStatus_WinRT.h"
#include "GameServerMetadataResult_WinRT.h"
#include "QualityOfServiceServer_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN
/// <summary>
/// Represents the game server platform service for allocating game server clusters and retrieving game server metadata.
/// </summary>

public ref class GameServerPlatformService sealed
{
public:

    /// <summary>
    /// Allocates a new Game Server Cluster using an existing Game Server Image.
    /// </summary>
    /// <param name="gameServerTitleId">Title ID of the game server.</param>
    /// <param name="serviceConfigurationId">Service configuration ID (SCID).  This can come from the MatchTicket.</param>
    /// <param name="sessionTemplateName">Session template name.  This can come from the MatchTicket.</param>
    /// <param name="sessionName">Session name.  This can come from the MatchTicket.</param>
    /// <param name="abortIfQueued">Indicates if the allocate should aborted if the operation is queued.</param>
    /// <returns>ClusterResult object</returns>
    /// <remarks>Calls V1 POST /titles/{gameServerTitleId}/clusters</remarks>
    Windows::Foundation::IAsyncOperation<ClusterResult^>^ AllocateClusterAsync(
        _In_ uint32 gameServerTitleId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ sessionTemplateName,
        _In_ Platform::String^ sessionName,
        _In_ bool abortIfQueued
        );

    /// <summary>
    /// Allocates a new Game Server Cluster using an existing Game Server Image, without interacting with MPSD.
    /// </summary>
    /// <param name="gameServerTitleId">Title ID of the game server.</param>
    /// <param name="serviceConfigurationId">Service configuration ID (SCID).</param>
    /// <param name="ticketId">Unique id for this request.  The ticket id should be the same for all members of the group.</param>
    /// <param name="gsiSetId">The GSI Set id to use for this request.  Comes from GetGameServerMetadataAsync.</param>
    /// <param name="gameVariantId">Which game variant to use.  Comes from GetGameServerMetadataAsync.</param>
    /// <param name="maxAllowedPlayers">The maximum players allowed by this variant.</param>
    /// <param name="location">Which datacenter to use for allocation.  This information should come from QoS probes.</param>
    /// <param name="abortIfQueued">Indicates if the allocate should aborted if the operation is queued.</param>
    /// <returns>ClusterResult object</returns>
    /// <remarks>Calls V2 POST /titles/{gameServerTitleId}/inlineclusters</remarks>
    Windows::Foundation::IAsyncOperation<ClusterResult^>^ AllocateClusterInlineAsync(
        _In_ uint32 gameServerTitleId,
        _In_ Platform::String^ serviceConfigurationId,
        _In_ Platform::String^ sandboxId,
        _In_ Platform::String^ ticketId,
        _In_ Platform::String^ gsiSetId,
        _In_ Platform::String^ gameVariantId,
        _In_ uint64 maxAllowedPlayers,
        _In_ Platform::String^ location,
        _In_ bool abortIfQueued
        );

    /// <summary>
    /// Allocates a new Game Server Cluster using an existing Game Server Image, without interacting with MPSD.
    /// </summary>
    /// <param name="gameServerTitleId">Title ID of the game server.</param>
    /// <param name="ticketId">Unique id for this request.  The ticket id should be the same for all members of the group.</param>
    /// <returns>GameServerTicketStatus object</returns>
    /// <remarks>Calls V6 GET titles/{titleId}/tickets/{ticketId}/status</remarks>
    Windows::Foundation::IAsyncOperation<GameServerTicketStatus^>^ GetTicketStatusAsync(
        _In_ uint32 gameServerTitleId,
        _In_ Platform::String^ ticketId
        );

    /// <summary>
    /// Retrieves a list of game variants for the specified title Id. 
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="maxAllowedPlayers">The maximum players the variant allows.</param>
    /// <param name="publisherOnly">Indicates whether to filter on publisher-created variants; otherwise user-created variants.</param>
    /// <param name="maxVariants">The maximum number of variants to return.</param>
    /// <param name="locale">Deprecated. This value is ignored and no longer required. (Optional)</param>
    /// <returns>GameServerMetadataResult object</returns>
    /// <remarks>Calls V1 POST /titles/{titleId}/variants</remarks>
    Windows::Foundation::IAsyncOperation<GameServerMetadataResult^>^ GetGameServerMetadataAsync(
        _In_ uint32 titleId,
        _In_ uint32 maxAllowedPlayers,
        _In_ bool publisherOnly,
        _In_ uint32 maxVariants,
        _In_opt_ Platform::String^ locale
        );

    /// <summary>
    /// Retrieves a list of game variants for the specified title Id. 
    /// </summary>
    /// <param name="titleId">The title ID.</param>
    /// <param name="maxAllowedPlayers">The maximum players the variant allows.</param>
    /// <param name="publisherOnly">Indicates whether to filter on publisher-created variants; otherwise user-created variants.</param>
    /// <param name="maxVariants">The maximum number of variants to return.</param>
    /// <param name="locale">Deprecated. This value is ignored and no longer required. (Optional)</param>
    /// <param name="filterTags">Tags are key value metadata on game variant, that a game developer sets at the time of ingestion and then use it as filters or 
    /// additional metadata.  The developer can use this value to filter which game variants they want to see. (Optional)</param>
    /// <returns>GameServerMetadataResult object</returns>
    /// <remarks>Calls V1 POST /titles/{titleId}/variants</remarks>
    Windows::Foundation::IAsyncOperation<GameServerMetadataResult^>^ GetGameServerMetadataAsync(
        _In_ uint32 titleId,
        _In_ uint32 maxAllowedPlayers,
        _In_ bool publisherOnly,
        _In_ uint32 maxVariants,
        _In_opt_ Platform::String^ locale,
        _In_opt_ Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ filterTags
        );

    /// <summary>
    /// Retrieves a collection of Quality of Service (QoS) servers. 
    /// </summary>
    /// <returns>A collection of QualityOfServiceServer objects.</returns>
    /// <remarks>Calls V1 GET /qosservers/</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<QualityOfServiceServer^>^>^ GetQualityOfServiceServersAsync();

    /// <summary>
    /// Allocates a new session host
    /// </summary>
    /// <param name="gameServerTitleId">The title ID of the game server.</param>
    /// <param name="locations">The ordered list of preferred location you wish the session host to be allocated from.</param>
    /// <param name="sessionId">This is the caller specified identifier.  It is assigned to the session host that is allocated and returned.  Later on you can reference the specific sessionhost by this identifier.  It must be globally unique(i.e.GUID).</param>
    /// <param name="cloudGameId">The cloud game identifier (GUID), otherwise known as the GSI Set ID.</param>
    /// <param name="gameModeId">The game mode identifier otherwise known as game variant IDs.</param>
    /// <param name="sessionCookie">This is a caller specified opaque string.  It too is assign to the sessionhost and can be referenced in your game code.  Use this to pass a small amount of information from the client to the server.  Max size is 32KB.</param>
    /// <returns>AllocationResult object</returns>
    /// <remarks>Calls V8 POST /titles/{gameServerTitleId}/allocationStatus</remarks>
    Windows::Foundation::IAsyncOperation<AllocationResult^>^ AllocateSessionHost(
        _In_ uint32 gameServerTitleId,
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ locations,
        _In_ Platform::String^ sessionId,
        _In_ Platform::String^ cloudGameId,
        _In_ Platform::String^ gameModeId,
        _In_ Platform::String^ sessionCookie
        );

    /// <summary>
    /// Get allocation status for a previous allocate session host request
    /// </summary>
    /// <param name="gameServerTitleId">Title ID of the game server</param>
    /// <param name="sessionId">This is the caller specified identifier. It is assigned to the session host that is allocated and returned. Later on you can reference the specific sessionhost by this identifier. It must be globally unique(i.e.GUID).</param>
    /// <returns>AllocationResult object</returns>
    /// <remarks>Calls V8 POST /titles/{gameServerTitleId}/sessionhosts</remarks>
    Windows::Foundation::IAsyncOperation<AllocationResult^>^ GetSessionHostAllocationStatus(
        _In_ uint32 gameServerTitleId,
        _In_ Platform::String^ sessionId
        );

internal:
    GameServerPlatformService(
        _In_ xbox::services::game_server_platform::game_server_platform_service cppObj
        );

private:
    xbox::services::game_server_platform::game_server_platform_service m_cppObj;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END

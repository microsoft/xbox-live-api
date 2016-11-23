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
#include "GameServerHostStatus_WinRT.h"
#include "GameServerPortMapping_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

/// <summary>
/// Represents status information for the Allocation request previously sent to the service.
/// </summary>
public ref class GameServerTicketStatus sealed
{
    // Example:
    // {
    //    "ticketId": "12341234-1234-1234-1234-123412341234",
    //    "clusterId": "12341234-1234-1234-1234-123412341234",
    //    "titleId": "1234",
    //    "hostName": "cloudapp.net",
    //    "status": "Active",
    //    "description": "",
    //    "secureContext": "",
    //    "portMappings": {},
    //    "gameHostId": "abc.Cloud.Gaming.Bootstrap_IN_0.0"
    //    "region": "Central US"
    // }

public:
    /// <summary>
    /// The ID of the ticket whose status is being checked.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(TicketId, ticket_id);

    /// <summary>
    /// The cluster ID of the server allocated for this ticket.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ClusterId, cluster_id);

    /// <summary>
    /// The title ID of the requesting title.
    /// </summary>
    DEFINE_PROP_GET_OBJ(TitleId, title_id, uint32);

    /// <summary>
    /// The host name of the server which was allocated to support this request.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(HostName, host_name);

    /// <summary>
    /// The status of the server.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(Status, status, GameServerHostStatus);

    /// <summary>
    /// Description of the allocation request.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Description, description);

    /// <summary>
    /// The secure device address information for the assigned server.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SecureContext, secure_context);

    /// <summary>
    /// The list of GameServerPortMapping for this server.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<GameServerPortMapping^>^ PortMappings { Windows::Foundation::Collections::IVectorView<GameServerPortMapping^>^ get(); }

    /// <summary>
    /// The ID of the game host.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(GameHostId, game_host_id);

    /// <summary>
    /// The region of the game host.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Region, region);

internal:
    GameServerTicketStatus(
        _In_ xbox::services::game_server_platform::game_server_ticket_status cppObj
        );

private:
    xbox::services::game_server_platform::game_server_ticket_status m_cppObj;
    Windows::Foundation::Collections::IVector<GameServerPortMapping^>^ m_portMappings;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END

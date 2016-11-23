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

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

/// <summary>
/// Represents a mapping between the port numbers requested for the server to use, and the port numbers exposed via load balancing hardware.
/// </summary>
public ref class GameServerPortMapping sealed
{
    // Example:
    //    "portMappings": {
    //        "GameData1": {
    //            "internal": 8347,
    //            "external": 30002
    //        },
    //        "GameData2": {
    //            "internal": 8348,
    //            "external": 31002
    //        },
    //        "Microsoft.WindowsAzure.Plugins.RemoteAccess.Rdp": {
    //            "internal": 3389,
    //            "external": 3389
    //        },
    //        "Microsoft.WindowsAzure.Plugins.RemoteForwarder.RdpInput": {
    //            "internal": 20000,
    //            "external": 3389
    //        }
    //    }

public:
    /// <summary>
    /// The name of the port, which is specified in your Azure project cscfg.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(PortName, port_name);

    /// <summary>
    /// The internal port number for the named port.  This is the port that the server will open using the socket API.
    /// </summary>
    DEFINE_PROP_GET_OBJ(InternalPortNumber, internal_port_number, uint32);

    /// <summary>
    /// The external port number clients should use when connecting.  This port is mapped by networking hardware to the internal port number.
    /// </summary>
    DEFINE_PROP_GET_OBJ(ExternalPortNumber, external_port_number, uint32);

internal:
    GameServerPortMapping(_In_ xbox::services::game_server_platform::game_server_port_mapping cppObj);

private:
    xbox::services::game_server_platform::game_server_port_mapping m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END

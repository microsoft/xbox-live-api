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
#include "GameServerFulfillmentState_WinRT.h"
#include "GameServerPortMapping_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN
/// <summary>
/// Represents a game server cluster result.
/// </summary>

public ref class ClusterResult sealed
{
public:
    /// <summary>
    /// Recommended interval to poll for completion. Note that this is not an estimate of when 
    /// the cluster will be ready, but rather a recommendation for how frequently the caller should 
    /// poll for a session status update. 
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(PollInterval, poll_interval)

    /// <summary>
    /// Indicates if the cluster is available if or the caller should wait or if the request was aborted.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(FulfillmentState, fulfillment_state, GameServerFulfillmentState);

    /// <summary>
    /// The host name of the cluster.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(HostName, host_name);

    /// <summary>
    /// The region of the cluster.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Region, region);

    /// <summary>
    /// The port mappings of the cluster.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<GameServerPortMapping^>^ PortMappings { Windows::Foundation::Collections::IVectorView<GameServerPortMapping^>^ get(); }

    /// <summary>
    /// The secure context which is the secure device address of the cluster.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SecureDeviceAddress, secure_device_address);

internal:
    ClusterResult(
        _In_ xbox::services::game_server_platform::cluster_result cppObj
        );

private:
    xbox::services::game_server_platform::cluster_result m_cppObj;
    Windows::Foundation::Collections::IVector<GameServerPortMapping^>^ m_portMappings;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END

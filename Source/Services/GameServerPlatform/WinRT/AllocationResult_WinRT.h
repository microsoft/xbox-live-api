// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/game_server_platform.h"
#include "GameServerFulfillmentState_WinRT.h"
#include "GameServerPortMapping_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN
/// <summary>
/// Represents a game server cluster result.
/// </summary>

public ref class AllocationResult sealed
{
public:
    /// <summary>
    /// Indicates if the allocation is available if or the caller should wait or if the request was aborted.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(FulfillmentState, fulfillment_state, GameServerFulfillmentState);

    /// <summary>
    /// The host name of the session host.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(HostName, host_name);

    /// <summary>
    /// The ID of the session host.  It is responsible for hosting the session. 
    /// A session host can only host one session at a time, but over its life time, will host several sessions.   
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SessionHostId, session_host_id);

    /// <summary>
    /// The region of the session host.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Region, region);

    /// <summary>
    /// The port mappings of the session host.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<GameServerPortMapping^>^ PortMappings { Windows::Foundation::Collections::IVectorView<GameServerPortMapping^>^ get(); }

    /// <summary>
    /// The secure device address of the session host.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SecureDeviceAddress, secure_device_address);

internal:
    AllocationResult(
        _In_ xbox::services::game_server_platform::allocation_result cppObj
        );

private:
    xbox::services::game_server_platform::allocation_result m_cppObj;
    Windows::Foundation::Collections::IVector<GameServerPortMapping^>^ m_portMappings;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END

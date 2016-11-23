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
/// Represents the server used to measure network quality of service (QoS) for the session. 
/// </summary>

public ref class QualityOfServiceServer sealed
{
    // Example:
    //
    // {
    //      "qosServers" :
    //      [
    //          {
    //              "serverFqdn":"microsoft.com",
    //              "serverSecureDeviceAddress":"r5Y=",
    //              "targetLocation":"North Central US"
    //          },
    //          {
    //              "serverFqdn":"microsoft.com",
    //              "serverSecureDeviceAddress":"r5Y=",
    //              "targetLocation":"West US"
    //          }
    //      ]
    //  }

public:
    /// <summary>
    /// The full qualified domain name of the server.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServerFullQualifiedDomainName, server_full_qualified_domain_name);

    /// <summary>
    /// The secure device address in base64 of the server.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SecureDeviceAddressBase64, secure_device_address_base64);

    /// <summary>
    /// The target location.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(TargetLocation, target_location);

internal:
    QualityOfServiceServer(
        _In_ xbox::services::game_server_platform::quality_of_service_server cppObj
        );

    const xbox::services::game_server_platform::quality_of_service_server& GetCppObj() const;
internal:
    xbox::services::game_server_platform::quality_of_service_server m_cppObj;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END

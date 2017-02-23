// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GameServerPortMapping_WinRT.h"

using namespace xbox::services::game_server_platform;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

GameServerPortMapping::GameServerPortMapping(
    _In_ game_server_port_mapping cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "AllocationResult_WinRT.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::game_server_platform;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

AllocationResult::AllocationResult(
    _In_ allocation_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_portMappings = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<GameServerPortMapping, game_server_port_mapping>(m_cppObj.port_mappings());
}

IVectorView<GameServerPortMapping^>^
AllocationResult::PortMappings::get()
{
    return m_portMappings->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END

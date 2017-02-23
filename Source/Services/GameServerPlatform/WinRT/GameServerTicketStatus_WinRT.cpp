// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GameServerTicketStatus_WinRT.h"

using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace xbox::services::game_server_platform;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

GameServerTicketStatus::GameServerTicketStatus(
    _In_ game_server_ticket_status cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_portMappings = ref new Vector<GameServerPortMapping^>();
    for (const auto& i : m_cppObj.port_mappings())
    {
        m_portMappings->Append(ref new GameServerPortMapping(i));
    }
}

IVectorView<GameServerPortMapping^>^ 
GameServerTicketStatus::PortMappings::get()
{ 
    return m_portMappings->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END

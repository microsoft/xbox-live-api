//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "ClusterResult_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace xbox::services::game_server_platform;

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_BEGIN

ClusterResult::ClusterResult(
    _In_ cluster_result cppObj
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
ClusterResult::PortMappings::get()
{
    return m_portMappings->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_GAMESERVERPLATFORM_END

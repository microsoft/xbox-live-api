//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#pragma once
#include "MeshPacketStructs.h"
#include "MeshPacketStatisticsForPacketType.h"
#include "MeshHeartbeatStatisticsForConnection.h"
#include "MeshPacketStructs.h"

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

public ref class MeshPacketStatistics sealed
{
public:
    MeshPacketStatistics();
    MeshPacketStatisticsForPacketType^ GetStatForPacketType(uint8 messageType);
    MeshHeartbeatStatisticsForConnection^ GetStatForConnection(uint8 consoleId);

    void ClearAllStatistics();
    
internal:
    void InspectPacket( Microsoft::Xbox::Samples::NetworkMesh::MeshPacketHeader& packet, bool sending );
    void PacketDropped( Microsoft::Xbox::Samples::NetworkMesh::MeshPacketHeader& packet, int packetsDropped );
    void PacketSkipped( Microsoft::Xbox::Samples::NetworkMesh::MeshPacketHeader& packet, int packetsSkipped );

private:
    Concurrency::critical_section m_stateLock;
    std::map<uint8, MeshPacketStatisticsForPacketType^> m_messageTypeMap;
    std::map<uint8, MeshHeartbeatStatisticsForConnection^> m_consoleIdMap;
};

}}}}

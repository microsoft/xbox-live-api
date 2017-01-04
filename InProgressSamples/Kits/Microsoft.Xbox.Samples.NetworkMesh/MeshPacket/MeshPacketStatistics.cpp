//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
#include "pch.h"
#include "MeshPacketStatistics.h"
#include "MeshPacketStructs.h"
#include "Utils.h"

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

MeshPacketStatistics::MeshPacketStatistics()
{
}

void MeshPacketStatistics::InspectPacket( Microsoft::Xbox::Samples::NetworkMesh::MeshPacketHeader& packet, bool sending )
{
    MeshPacketStatisticsForPacketType^ stat = GetStatForPacketType(packet.messageType);
    if( stat == nullptr )
    {
        stat = ref new MeshPacketStatisticsForPacketType();
        {
            Concurrency::critical_section::scoped_lock lock(m_stateLock);    
            m_messageTypeMap[packet.messageType] = stat;
        }
    }

    if( sending )
    {
        stat->SetLargestPacketSent( max(packet.messageSize, stat->LargestPacketSent) );
        stat->SetNumberPacketsSent( stat->NumberPacketsSent + 1 );
    }
    else
    {
        stat->SetLargestPacketReceived( max(packet.messageSize, stat->LargestPacketReceived) );
        stat->SetNumberPacketsReceived( stat->NumberPacketsReceived + 1 );
    }
    
    if( (MessageTypeEnum) packet.messageType == MessageTypeEnum::GAME_HEARTBEAT_DATA )
    {
        MeshHeartbeatStatisticsForConnection^ hbstat = GetStatForConnection(packet.consoleId);

        if( !sending )
        {
            hbstat->SetLastHeartbeatReceived( Utils::GetCurrentTime() );
        }
    }
}

void MeshPacketStatistics::PacketSkipped( Microsoft::Xbox::Samples::NetworkMesh::MeshPacketHeader& packet, int packetsSkipped )
{
    MeshPacketStatisticsForPacketType^ stat = GetStatForPacketType(packet.messageType);
    if( stat == nullptr )
    {
        stat = ref new MeshPacketStatisticsForPacketType();
        {
            Concurrency::critical_section::scoped_lock lock(m_stateLock);    
            m_messageTypeMap[packet.messageType] = stat;
        }
    }

    stat->SetNumberPacketsSkipped( stat->NumberPacketsSkipped + packetsSkipped );
}

void MeshPacketStatistics::PacketDropped( Microsoft::Xbox::Samples::NetworkMesh::MeshPacketHeader& packet, int packetsDropped )
{
    MeshPacketStatisticsForPacketType^ stat = GetStatForPacketType(packet.messageType);
    if( stat == nullptr )
    {
        stat = ref new MeshPacketStatisticsForPacketType();
        {
            Concurrency::critical_section::scoped_lock lock(m_stateLock);    
            m_messageTypeMap[packet.messageType] = stat;
        }
    }

    stat->SetNumberPacketsDropped( stat->NumberPacketsDropped + packetsDropped );
}

MeshPacketStatisticsForPacketType^ MeshPacketStatistics::GetStatForPacketType(uint8 messageType)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);    
    return m_messageTypeMap[messageType];
}

MeshHeartbeatStatisticsForConnection^ MeshPacketStatistics::GetStatForConnection(uint8 consoleId)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);

    if( m_consoleIdMap.find(consoleId) == m_consoleIdMap.end() )
    {
        m_consoleIdMap[consoleId] = ref new MeshHeartbeatStatisticsForConnection();
        m_consoleIdMap[consoleId]->SetLastHeartbeatReceived(Utils::GetCurrentTime());
        m_consoleIdMap[consoleId]->SetLastHeartbeatSent(Utils::GetCurrentTime());
    }

    return m_consoleIdMap[consoleId];
}

void MeshPacketStatistics::ClearAllStatistics()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    for(std::map<uint8, MeshPacketStatisticsForPacketType^>::iterator iter = m_messageTypeMap.begin(); iter != m_messageTypeMap.end(); ++iter)
    {
        MeshPacketStatisticsForPacketType^ stat =  iter->second;
        if( stat != nullptr )
        {
            stat->ClearAllStatisticsForPacketType();
        }
    }
}

}}}}

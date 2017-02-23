// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MeshPacketManager.h"
#include "Utils.h"
#include "MeshManager.h"

using namespace Concurrency;

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

MeshPacketManager::MeshPacketManager( 
    uint8 localConsoleId, 
    unsigned short transportLevelPortNumber,
    MeshManager^ meshManager,
    bool dropOutOfOrderPackets ) : 
    m_localConsoleId( localConsoleId ),
    m_debugTimeSincePacketReceive( 0.0f ),
    m_debugTimeSincePacketSend( 0.0f ),
    m_debugInsideWSAReceive( false ),
    m_debugInsideWSASend( false ),
    m_packetMessageId ( 0 ),
    m_previousPacketMessageId(0),
    m_dropOutOfOrderPackets(dropOutOfOrderPackets),
    m_heartbeatMessageSize(DEFAULT_HEARTBEAT_SIZE)
{
    // Note: this library requires the NetworkConnectivityLevel to be one of the following:
    //   XboxLiveAccess
    //   InternetAccess
    //   LocalAccess
    // Depending on what the parent application is doing, only certain connectivty levels will work. 
    // This is clearly a matter for the parent application, so enforcement or lack thereof is best
    // left to the parent, rather than down here in the library. 

    m_meshManager = Platform::WeakReference(meshManager);
    m_meshPacketStatistics = ref new MeshPacketStatistics();

    memset(m_bufferForWSARecv, 0, sizeof(WSARECV_BUFFER_SIZE));

    WSADATA wsadata;
    int result = WSAStartup( MAKEWORD( 2, 2 ), &wsadata );
    if( result != 0 )
    {
        LogMeshPacketManagerComment( L"InitializeNetworkLayer failed with WSAError" );
        throw ref new Platform::COMException( HRESULT_FROM_WIN32(result) );
    }

    m_localSocket = WSASocket(
        AF_INET6,
        SOCK_DGRAM,
        IPPROTO_UDP,
        NULL,
        0,
        WSA_FLAG_OVERLAPPED
        );

    if ( m_localSocket == INVALID_SOCKET )
    {
        result = WSAGetLastError();
        LogMeshPacketManagerComment( L"Error: Failed creating a socket" );
        throw ref new Platform::COMException( HRESULT_FROM_WIN32((unsigned int)result) );
    }

    // set sockets options for exclusive IPv6.
    int v6only = 0;
    result = setsockopt(
        m_localSocket,
        IPPROTO_IPV6,
        IPV6_V6ONLY,
        (char*) &v6only,
        sizeof( v6only )
        );

    if ( result != 0 )
    {
        result = WSAGetLastError();
        LogMeshPacketManagerComment( L"Error: setsockopt() failed" );
        throw ref new Platform::COMException( HRESULT_FROM_WIN32(result) );
    }

    // set sockets to non-blocking
    unsigned long nonBlockingValue = 1;
    result = ioctlsocket(m_localSocket, FIONBIO, &nonBlockingValue);
    if ( result != 0 )
    {
        result = WSAGetLastError();
        LogMeshPacketManagerComment( L"Error: ioctlsocket() failed" );
        throw ref new Platform::COMException( HRESULT_FROM_WIN32(result) );
    }

    ZeroMemory( &m_localSockAddress, sizeof( m_localSockAddress ) );
    m_localSockAddress.sin6_family = AF_INET6;
    m_localSockAddress.sin6_port = transportLevelPortNumber;
    LogMeshPacketManagerComment( L"Binding to port " + transportLevelPortNumber.ToString() );

    // bind IPv6 socket.
    result = bind(
        m_localSocket,
        (SOCKADDR*) &m_localSockAddress,
        sizeof( m_localSockAddress )
        );
    if ( result != 0 )
    {
        result = WSAGetLastError();
        LogMeshPacketManagerComment( L"Error: bind() failed" );
        throw ref new Platform::COMException( HRESULT_FROM_WIN32(result) );
    }

    LogMeshPacketManagerComment( L"Starting thread to listening for network traffic" );
    int32 threadAffinityMask = ~0x04; // Means to this thread can run all everything except core 3 (which is reserved for graphics for example).
    m_socketReceiveThread = ref new MeshThread(0, threadAffinityMask, NORMAL_PRIORITY_CLASS); 
    m_socketReceiveThread->OnDoWork += ref new Windows::Foundation::EventHandler<ProcessThreadsEventArgs^>( [this]( Platform::Object^, ProcessThreadsEventArgs^ args )
    {
        SocketReceiveWorkerThreadDoWork(args);
    });

    LogMeshPacketManagerComment( L"Starting thread to send network traffic" );
    threadAffinityMask = ~0x04; // Means to this thread can run all everything except core 3 (which is reserved for graphics for example).
    m_socketSendThread = ref new MeshThread(INFINITE, threadAffinityMask, NORMAL_PRIORITY_CLASS); // 0xFFFFFFFF == INFINITE.  This thread will only wake up when the code tells it to, or upon shutdown
    m_socketSendThread->OnDoWork += ref new Windows::Foundation::EventHandler<ProcessThreadsEventArgs^>( [this]( Platform::Object^, ProcessThreadsEventArgs^ args )
    {
        SocketSendWorkerThreadDoWork(args);
    });

    m_timerLastSendReliablePacketsUntilACK.QuadPart = 0;
    if (!QueryPerformanceFrequency(&m_timerFrequency))
    {
        THROW_HR( E_UNEXPECTED );
    }
}

uint8 MeshPacketManager::GetLocalConsoleId()
{
    return m_localConsoleId;
}

void MeshPacketManager::Shutdown()
{
    if (m_socketSendThread != nullptr)
    {
        m_socketSendThread->Shutdown();
        m_socketSendThread = nullptr;
    }

    if (m_socketReceiveThread != nullptr)
    {
        m_socketReceiveThread->Shutdown();
        m_socketReceiveThread = nullptr;
    }
    
    // close socket after the send and receive threads are shutdown
    // otherwise the threads will attempt to use an invalid socket and throw exceptions
    if (m_localSocket != INVALID_SOCKET )
    {
        shutdown( m_localSocket, SD_BOTH );
        closesocket( m_localSocket );
        m_localSocket = INVALID_SOCKET;
    }

    WSACleanup();
}

void MeshPacketManager::SendHeartbeatMessageAsync( 
#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAssociation^ association,
#else
    Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
#endif
    uint8 consoleId
    )
{
    size_t packetSize = sizeof(MeshPacketHeader) + m_heartbeatMessageSize;

    std::shared_ptr<MESH_PACKET_INFO> packetInfo( new MESH_PACKET_INFO );
    packetInfo->association = association;

    GetPacketWithHeader(packetSize, (uint8)MessageTypeEnum::GAME_HEARTBEAT_DATA, packetInfo->packetBuffer, false);
    QueuePacketToSend( packetInfo );

    MeshHeartbeatStatisticsForConnection^ stats;

    stats = m_meshPacketStatistics->GetStatForConnection(consoleId);
    stats->SetLastHeartbeatSent(Utils::GetCurrentTime());
}

void MeshPacketManager::SendHelloMessage( 
#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAssociation^ association,
#else
    Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
#endif
    Platform::String^ consoleName,
    bool respondingToHello 
    )
{
    size_t consoleNameSizeInChars = consoleName->Length(); // WCHAR, one element already there so handy null terminator
    size_t consoleNameSizeInBytes = consoleNameSizeInChars * 2;
    size_t packetSize = sizeof(MeshPacketHeader) + sizeof(MeshPacketHelloMessageHeader) + consoleNameSizeInBytes;

    std::shared_ptr<MESH_PACKET_INFO> packetInfo( new MESH_PACKET_INFO );
    packetInfo->association = association;

    GetPacketWithHeader(packetSize, (uint8)MessageTypeEnum::GAME_HELLO_DATA, packetInfo->packetBuffer, false);

    // Fill out a MeshPacketHelloMessageHeader struct, which appears after the MeshPacketHeader
    BYTE* meshPacketHelloMessageDataPtr = packetInfo->packetBuffer.data() + sizeof(MeshPacketHeader);
    MeshPacketHelloMessageHeader& meshPacketHelloMessageData = (MeshPacketHelloMessageHeader&)*meshPacketHelloMessageDataPtr;
    meshPacketHelloMessageData.respondingToHello = respondingToHello;
    meshPacketHelloMessageData.consoleNameLength = (uint16)(consoleNameSizeInChars);

    // Fill out a console name string, which appears after the MeshPacketHelloMessageHeader
    BYTE* consoleNamePtr = packetInfo->packetBuffer.data() + sizeof(MeshPacketHeader) + sizeof(MeshPacketHelloMessageHeader);
    memcpy_s(consoleNamePtr, packetSize - sizeof(MeshPacketHelloMessageHeader) - sizeof(MeshPacketHeader), consoleName->Data(), consoleNameSizeInBytes);

    QueuePacketToSend( packetInfo );
}

void MeshPacketManager::SendChatMessage( 
#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAssociation^ association,
#else
    Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
#endif
    Windows::Storage::Streams::IBuffer^ buffer,
    bool sendReliable
    )
{
    size_t packetSize = sizeof(MeshPacketHeader) + buffer->Length;

    std::shared_ptr<MESH_PACKET_INFO> packetInfo( new MESH_PACKET_INFO );
    packetInfo->association = association;

    GetPacketWithHeader(packetSize, (uint8)MessageTypeEnum::GAME_CHAT_DATA, packetInfo->packetBuffer, sendReliable);

    BYTE* byteBufferPointer;
    Utils::GetBufferBytes(buffer, &byteBufferPointer);
    BYTE* bufferPacketPointer = packetInfo->packetBuffer.data() + sizeof(MeshPacketHeader);
    memcpy_s(bufferPacketPointer, packetSize - sizeof(MeshPacketHeader), byteBufferPointer, buffer->Length);

    QueuePacketToSend( packetInfo );
}

void MeshPacketManager::SendAckMessage( 
#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAssociation^ association,
#else
    Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
#endif
    uint16 messageIdToAck
    )
{
    size_t packetSize = sizeof(MeshPacketHeader);
    std::shared_ptr<MESH_PACKET_INFO> packetInfo( new MESH_PACKET_INFO );
    packetInfo->association = association;    

    GetPacketWithHeader(packetSize, (uint8)MessageTypeEnum::GAME_ACK, packetInfo->packetBuffer, false);

    // The ACK packet treats MeshPacketHeader's messageId as the message that's being ACK'd
    BYTE* packetBufferPtr = packetInfo->packetBuffer.data();
    MeshPacketHeader& packet = (MeshPacketHeader&)*packetBufferPtr;
    packet.messageId = messageIdToAck;

    QueuePacketToSend( packetInfo );
}

void MeshPacketManager::SendCustomMessage( 
#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAssociation^ association,
#else
    Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
#endif
    uint8 messageType,
    Windows::Storage::Streams::IBuffer^ buffer,
    bool sendReliable
    )
{
    size_t packetSize = sizeof(MeshPacketHeader) + buffer->Length;

    uint8 baseIndexOfGameCustomData = (uint8)MessageTypeEnum::GAME_CUSTOM_DATA; // eg. 64
    uint16 maxIndex = 256 - baseIndexOfGameCustomData;
    if( messageType >= maxIndex ) // eg. 192 = maxIndex
    {
        LogMeshPacketManagerComment( L"Can not send custom message type that is greater or equal to " + maxIndex.ToString() );
        throw ref new Platform::InvalidArgumentException();
    }
    messageType += baseIndexOfGameCustomData;

    std::shared_ptr<MESH_PACKET_INFO> packetInfo( new MESH_PACKET_INFO );
    packetInfo->association = association;

    GetPacketWithHeader(packetSize, messageType, packetInfo->packetBuffer, sendReliable);

    BYTE* byteBufferPointer;
    Utils::GetBufferBytes(buffer, &byteBufferPointer);
    BYTE* bufferPacketPointer = packetInfo->packetBuffer.data() + sizeof(MeshPacketHeader);
    memcpy_s(bufferPacketPointer, packetSize - sizeof(MeshPacketHeader), byteBufferPointer, buffer->Length);

    QueuePacketToSend( packetInfo );
}

void
MeshPacketManager::RecordMessageIfSendingReliable( 
    std::shared_ptr<MESH_PACKET_INFO> packetInfo
    )
{
    // Check if the top bit of the messageId is set
    BYTE* messageBufferPtr = packetInfo->packetBuffer.data();
    MeshPacketHeader& meshPacketHeader = reinterpret_cast<MeshPacketHeader&>(messageBufferPtr);
    uint16 sendReliableBit = 1 << 15;
    uint16 sendReliableBitSet = (meshPacketHeader.messageId & sendReliableBit);
    bool wasSendReliableBitSet = (sendReliableBitSet != 0);
    uint16 packetMessageId = meshPacketHeader.messageId & ~sendReliableBit; // remove the sendReliable bit from the message ID

    if( wasSendReliableBitSet )
    {
        Concurrency::critical_section::scoped_lock lock(m_meshPacketsThatNeedAckLock);
        bool matchFound = false;
        for each (std::shared_ptr<MESH_PACKET_THAT_NEEDS_ACK> meshPacketThatNeedAck in m_meshPacketsThatNeedAck)
        {
            if( meshPacketThatNeedAck->messageId == packetMessageId )
            {
                matchFound = true;
                break;
            }
        }

        if( !matchFound )
        {
            std::shared_ptr<MESH_PACKET_THAT_NEEDS_ACK> meshPacketThatNeedAck(new MESH_PACKET_THAT_NEEDS_ACK());
            meshPacketThatNeedAck->messageId = packetMessageId;
            meshPacketThatNeedAck->packetInfo = packetInfo;
            m_meshPacketsThatNeedAck.push_back( meshPacketThatNeedAck );
        }
    }
}

void
MeshPacketManager::QueuePacketToSend( 
    std::shared_ptr<MESH_PACKET_INFO> packetInfo
    )
{
    RecordMessageIfSendingReliable( packetInfo );

    {
        Concurrency::critical_section::scoped_lock lock(m_sendLock);
        m_packetsToSend.push( packetInfo );
    }
    m_socketSendThread->WakeupThread();
}

MeshPacketStatistics^ MeshPacketManager::GetMeshPacketStatistics()
{
    return m_meshPacketStatistics;
}

void MeshPacketManager::GetPacketWithHeader( 
    size_t packetSize, 
    uint8 messageType, 
    std::vector<BYTE>& packetBuffer,
    bool sendReliable
    )
{
    // Create a packet buffer which std::vector will clean up automatically  
    packetBuffer.resize(packetSize, 0x33); // 0x33 for debug testing
    BYTE* messageBufferPtr = packetBuffer.data();

    // Fill out MeshPacketHeader
    MeshPacketHeader& packet = (MeshPacketHeader&)*messageBufferPtr;
    packet.messageType = messageType;
    packet.consoleId = m_localConsoleId;

    packet.messageId = IncrementPacketMessageId();

    if( sendReliable )
    {
        // The top bit of the messageId indicates if the remote machine should send back a GAME_ACK message with this messageId 
        uint16 sendReliableBit = 1 << 15;
        packet.messageId |= sendReliableBit;
    }
    packet.messageSize = (uint16)(packetSize);
}

void MeshPacketManager::SocketSendWorkerThreadDoWork( Microsoft::Xbox::Samples::NetworkMesh::ProcessThreadsEventArgs^ args )
{
    std::shared_ptr<MESH_PACKET_INFO> packetInfo;

    for(;;)
    {
        packetInfo = nullptr;
        {
            Concurrency::critical_section::scoped_lock lock(m_sendLock);
            if( !m_packetsToSend.empty() )
            {
                packetInfo = m_packetsToSend.front();
                m_packetsToSend.pop();
            }
        }

        if( packetInfo == nullptr )
        {
            // Nothing to do, so ignore
            break;
        }

        ProcessSendPacket(packetInfo);
    }
}

void MeshPacketManager::ProcessSendPacket( std::shared_ptr<MESH_PACKET_INFO> packetInfo )
{
    if (packetInfo->association == nullptr)
    {
        LogMeshPacketManagerComment( L"Invalid association to SendPacket" );
        return;
    }

    if (packetInfo->packetBuffer.size() == 0)
    {
        LogMeshPacketManagerComment( L"No data for SendPacket" );
        return;
    }

    if (INVALID_SOCKET == m_localSocket)
    {
        LogMeshPacketManagerComment( L"Can't send data if the socket has not been initialized" );
        return;
    }

    const BYTE* messageBufferPtr = packetInfo->packetBuffer.data();
    MeshPacketHeader& meshPacketHeader = (MeshPacketHeader&)*messageBufferPtr;

    static bool logFirstTimeOnly = true;
    if( logFirstTimeOnly )
    {
        logFirstTimeOnly = false;
        LogMeshPacketManagerComment( Utils::GetThreadDescription(L"THREAD: WSASendTo") );
    }

    // Get the remote IPv6 socket addresses from the peerDeviceAssociation
    SOCKADDR_STORAGE remoteSocketAddress = {0};
    Platform::ArrayReference<BYTE> remoteSocketAddressBytes(
        (BYTE*) &remoteSocketAddress,
        sizeof(remoteSocketAddress)
        );
    packetInfo->association->GetRemoteSocketAddressBytes(remoteSocketAddressBytes);

    // Collect stats on it before sending it out
    m_meshPacketStatistics->InspectPacket(meshPacketHeader, true);

    WSABUF wsabuf;
    wsabuf.len = meshPacketHeader.messageSize;
    wsabuf.buf = (CHAR*)&meshPacketHeader;
    DWORD numBytesSent = 0;

    SetDebugInsideWSASend(true); // for debugging purposes only

    int result = WSASendTo(
        m_localSocket,
        &wsabuf,
        1,
        &numBytesSent,
        0,
        (SOCKADDR*) &remoteSocketAddress,
        sizeof(remoteSocketAddress),
        nullptr, 
        nullptr
        );

    INT lastError = WSAGetLastError();

    SetDebugInsideWSASend(false); // for debugging purposes only
    SetDebugTimeSincePacketSend( 0.0f ); // for debugging purposes only

    if(result != 0 || numBytesSent != meshPacketHeader.messageSize)
    {
        // Ignore and log failure
        LogMeshPacketManagerComment( 
            Utils::FormatString(L"WSASendTo.  ErrorCode: %d. BytesSent: %d. DesiredBytesSent: %d", lastError, numBytesSent,meshPacketHeader.messageSize )
            );
    }
}

void MeshPacketManager::SocketReceiveWorkerThreadDoWork( Microsoft::Xbox::Samples::NetworkMesh::ProcessThreadsEventArgs^ args )
{
    // This gets called by the MeshThead class over and over again
    DWORD flags = 0;
    DWORD numberBytesReceived = 0;
    SOCKADDR_STORAGE senderSocketAddress;
    int senderSocketAddressSize = sizeof(senderSocketAddress);

    WSABUF wsabuf;
    wsabuf.len = WSARECV_BUFFER_SIZE;
    wsabuf.buf = (char*) m_bufferForWSARecv;

    SetDebugInsideWSAReceive(true); // for debugging purposes only

    static bool logFirstTimeOnly = true;
    if( logFirstTimeOnly )
    {
        logFirstTimeOnly = false;
        LogMeshPacketManagerComment( Utils::GetThreadDescription(L"THREAD: WSARecvFrom") );
    }

    int result = WSARecvFrom(
        m_localSocket,
        &wsabuf,
        1,
        &numberBytesReceived,
        &flags,
        (SOCKADDR*) &senderSocketAddress,
        &senderSocketAddressSize,
        NULL,
        NULL
        );

    SetDebugInsideWSAReceive(false); // for debugging purposes only
    SetDebugTimeSincePacketReceive( 0.0f );

    if (result != 0 || numberBytesReceived == 0 )
    {
        INT lastError = WSAGetLastError();
        if (lastError != ERROR_SUCCESS)
        {
            // Ignore but log receive errors
            LogMeshPacketManagerComment( 
                Utils::FormatString(L"WSARecvFrom.  ErrorCode: %d. BytesReceived: %d", result, numberBytesReceived )
                );
        }
        return;
    }

    MeshConnection^ meshConnection = GetMeshConnection( senderSocketAddress );
    if( meshConnection != nullptr )
    {
        if( meshConnection->GetConnectionStatus() == ConnectionStatus::Disconnected || 
            meshConnection->GetConnectionStatus() == ConnectionStatus::Pending )
        {
            LogMeshPacketManagerComment( L"ERROR: Receiving data from console who isn't connected. " + meshConnection->GetConsoleName() );
            LogMeshPacketManagerComment( L"This can happen when the OnAssociationIncoming() event fires after the first hello packet" );

            return;
        }

        DWORD offset = 0;
        while( offset < numberBytesReceived )
        {
            BYTE* packetBuffer = m_bufferForWSARecv + offset;
            MeshPacketHeader& meshPacketHeader = reinterpret_cast<MeshPacketHeader&>(*packetBuffer);
            if( offset + meshPacketHeader.messageSize > numberBytesReceived)
            {
                // Invalid packet, so skip it
                LogMeshPacketManagerComment( L"ERROR: Invalid packet sent to us" );
                break;
            }

            ProcessPacket(meshConnection, packetBuffer);
            offset += meshPacketHeader.messageSize;
        }
    }
    else
    {
        LogMeshPacketManagerComment( L"ERROR: Receiving data from console who isn't known" );
        LogMeshPacketManagerComment( L"This can happen when the OnAssociationIncoming() event fires after the first hello packet" );
    }
}

MeshConnection^ MeshPacketManager::GetMeshConnection( SOCKADDR_STORAGE senderSocketAddress )
{
    MeshManager^ meshManager = m_meshManager.Resolve<MeshManager>();
    if(meshManager == nullptr)
    {
        return nullptr;
    }

    // Check if we already know about this connection. If the console id is not 255, use it for
    // a "fast" lookup. If console id is 255, then intentionally skip the use of console id for 
    // lookups and go with the "slow" lookup based on GetAssociationBySocketAddressBytes(). 255
    // was chosen since it outside the expected range of 0...63.
    MeshPacketHeader& meshPacketHeader = reinterpret_cast<MeshPacketHeader&>(*m_bufferForWSARecv);
    if (meshPacketHeader.consoleId != 0xFF)
    {
        MeshConnection^ meshConnection = meshManager->GetConnectionFromConsoleId(meshPacketHeader.consoleId);
        if (meshConnection != nullptr)
        {
            return meshConnection;
        }
    }

    // Do a lookup of the association based on the socket address
    Platform::ArrayReference<BYTE> localSocketAddressBytes(
        (BYTE*) &m_localSockAddress,
        sizeof(m_localSockAddress));

    Platform::ArrayReference<BYTE> senderSocketAddressBytes(
        (BYTE*) &senderSocketAddress,
        sizeof(senderSocketAddress));

#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAddress^ secureDeviceAddress = nullptr;
#else
    Windows::Networking::XboxLive::XboxLiveDeviceAddress^ secureDeviceAddress = nullptr;
#endif
    try
    {
#ifdef _XBOX_ONE
        auto receivedSecureDeviceAssociation = Windows::Xbox::Networking::SecureDeviceAssociation::GetAssociationBySocketAddressBytes(
            senderSocketAddressBytes,
            localSocketAddressBytes);
        secureDeviceAddress = receivedSecureDeviceAssociation->RemoteSecureDeviceAddress;
#else
        auto receivedSecureDeviceAssociation = Windows::Networking::XboxLive::XboxLiveEndpointPair::FindEndpointPairBySocketAddressBytes(
            localSocketAddressBytes,
            senderSocketAddressBytes);

        if (receivedSecureDeviceAssociation != nullptr)
        {
            secureDeviceAddress = receivedSecureDeviceAssociation->RemoteDeviceAddress;
        }
#endif
    }
    catch (...)
    {
        LogMeshPacketManagerComment( L"Failed getting GetAssociationBySocketAddressBytes" );
        try
        {
#ifdef _XBOX_ONE
            secureDeviceAddress = Windows::Xbox::Networking::SecureDeviceAddress::FromBytes(senderSocketAddressBytes);
#else
            secureDeviceAddress = Windows::Networking::XboxLive::XboxLiveDeviceAddress::CreateFromSnapshotBytes(senderSocketAddressBytes);
#endif
        }
        catch (...)
        {
#ifdef _XBOX_ONE
            LogMeshPacketManagerComment( L"ERROR: Failed getting SecureDeviceAddress::FromBytes. Ignoring WSARecvFrom packet" );
#else
            LogMeshPacketManagerComment(L"ERROR: Failed getting XboxLiveDeviceAddress::CreateFromSnapshotBytes. Ignoring WSARecvFrom packet");
#endif
            return nullptr;
        }
    }

    return meshManager->GetConnectionFromSecureDeviceAddress(secureDeviceAddress);
}


UINT16 MeshPacketManager::IncrementPacketMessageId()
{
    UINT16 result = (UINT16)InterlockedIncrement(&m_packetMessageId);
    return result & 0xffff;
}

bool MeshPacketManager::ShouldPacketBeDropped( Microsoft::Xbox::Samples::NetworkMesh::MeshPacketHeader& packet )
{
    if( !m_dropOutOfOrderPackets )
    {
        return false;
    }
    uint16 incomingPacketMessageId = packet.messageId;
    uint16 previousPacketMessageId = GetPreviousPacketMessageId();

    uint16 skippedPercentage = ( SKIPPED_PERCENT * 0xffff) / 100;
    // If the incoming is greater than the previous and incoming is less than Prev + 10%, then we should keep it.
    // For e.g. Max = 100; Prev = 90; Incoming = 95; then anything from 90-100, we keep.
    if (incomingPacketMessageId > previousPacketMessageId && incomingPacketMessageId < (previousPacketMessageId + skippedPercentage))
    {
        return false;
    }

    // If the incoming is less than the previous and less than the max wrapped packet (Prev + 10% wrapped around), then we should keep it.
    // For e.g. Max = 100; Prev = 95; Incoming = 4; then maxWrapped = (95+10)%100 = 5; Anything from 0-4, we keep.
    uint16 maxPacketNumberToDropWrapped = (previousPacketMessageId + skippedPercentage) % 0xffff;
    if (maxPacketNumberToDropWrapped > 0xffff)
    {
        // This is the wrap round check
        if(incomingPacketMessageId < previousPacketMessageId && incomingPacketMessageId < maxPacketNumberToDropWrapped)
        {
            return false;
        }
    }

    // everything else is dropped.
    return true;
}

void MeshPacketManager::ProcessPacket( MeshConnection^ sender, BYTE* packetBuffer )
{
    MeshPacketHeader& meshPacketHeader = reinterpret_cast<MeshPacketHeader&>(*packetBuffer);

    // The top bit of the messageId indicates if the remote machine should send back a GAME_ACK message with this messageId 
    uint16 sendReliableBit = 1 << 15;
    uint16 sendReliableBitSet = (meshPacketHeader.messageId & sendReliableBit);
    bool wasSendReliableBitSet = (sendReliableBitSet != 0);
    meshPacketHeader.messageId &= ~sendReliableBit; // remove the sendReliable bit from the message ID

    if( wasSendReliableBitSet )
    {
        // If this packet had the bit set, then send back an ACK packet to this sender
        SendAckMessage(sender->GetAssociation(), meshPacketHeader.messageId);
    }

    // MessageTypeEnum::GAME_ACK is unique because the meshPacketHeader.messageId 
    // is the message of the ID packet that's being ACK'd.
    // So ignore it for the drop packet feature
    if( meshPacketHeader.messageType != (uint8)MessageTypeEnum::GAME_ACK )
    {
        if(ShouldPacketBeDropped(meshPacketHeader))
        {
            m_meshPacketStatistics->InspectPacket(meshPacketHeader, false);
            m_meshPacketStatistics->PacketDropped(meshPacketHeader, 1);
            //LogMeshPacketManagerComment("Packets dropped. Curr: " + meshPacketHeader.messageId.ToString() + L" Prev: " + GetPreviousPacketMessageId().ToString());
            return;
        }

        int packetsSkipped = 0;
        uint16 incomingPacketMessageId = meshPacketHeader.messageId;
        uint16 previousPacketMessageId = GetPreviousPacketMessageId();

        if(incomingPacketMessageId > previousPacketMessageId)
        {
            packetsSkipped = (incomingPacketMessageId - previousPacketMessageId) - 1;
        }
        else
        {
            // If we haven't dropped him, that means we must have wrapped.
            // For e.g. Max = 100; Prev = 95; incoming = 4;
            packetsSkipped = (incomingPacketMessageId + 0xffff) - previousPacketMessageId - 1;
        }

        SetPreviousPacketMessageId(incomingPacketMessageId);
        m_meshPacketStatistics->InspectPacket(meshPacketHeader, false);
        if(packetsSkipped > 0)
        {
            //LogMeshPacketManagerComment("Packets skipped " + packetsSkipped.ToString());
            m_meshPacketStatistics->PacketSkipped(meshPacketHeader, packetsSkipped);
        }
    }

    switch(meshPacketHeader.messageType)
    {
    case MessageTypeEnum::GAME_HEARTBEAT_DATA:
        {
            // Logging done in MeshManager::OnHeartbeatReceived 
            auto args = ref new MeshHeartbeatReceivedEvent(meshPacketHeader.consoleId, sender);
            OnHeartbeatReceived(this, args);
        }
        break;

    case MessageTypeEnum::GAME_HELLO_DATA:
        {
            // Logging done in MeshManager::OnHelloReceived 
            BYTE* meshPacketHelloMessageDataPtr = packetBuffer + sizeof(MeshPacketHeader);
            MeshPacketHelloMessageHeader& meshPacketHelloMessageData = (MeshPacketHelloMessageHeader&)*meshPacketHelloMessageDataPtr;

            BYTE* consoleNamePtr = packetBuffer + sizeof(MeshPacketHeader) + sizeof(MeshPacketHelloMessageHeader);
            Platform::String^ consoleName = ref new Platform::String((WCHAR*)consoleNamePtr, meshPacketHelloMessageData.consoleNameLength);

            auto args = ref new MeshHelloReceivedEvent(
                meshPacketHeader.consoleId,
                sender,
                consoleName,
                meshPacketHelloMessageData.respondingToHello != 0
                );

            OnHelloReceived(this, args);
        }
        break;

    case MessageTypeEnum::GAME_CHAT_DATA:
        {
            BYTE* srcBufferPtr = packetBuffer + sizeof(MeshPacketHeader);
            uint32 srcBufferSizeInBytes = meshPacketHeader.messageSize - sizeof(MeshPacketHeader);

            Windows::Storage::Streams::Buffer^ destBuffer = ref new Windows::Storage::Streams::Buffer( srcBufferSizeInBytes );
            destBuffer->Length = srcBufferSizeInBytes;
            BYTE* destBufferBytes = nullptr;
            Utils::GetBufferBytes( destBuffer, &destBufferBytes );
            memcpy_s(destBufferBytes, destBuffer->Length, srcBufferPtr, srcBufferSizeInBytes);

            auto args = ref new MeshChatMessageReceivedEvent(
                meshPacketHeader.consoleId,
                sender,
                destBuffer
                );

            OnChatMessageReceived(this, args);
        }
        break;

    case MessageTypeEnum::GAME_ACK:
        {
            DeleteMeshPacketWhenGotAck( meshPacketHeader.messageId );

            auto args = ref new MeshAckReceivedEvent(
                meshPacketHeader.consoleId,
                sender,
                meshPacketHeader.messageId
                );

            OnAckReceived(this, args);
        }
        break;

    case MessageTypeEnum::GAME_CUSTOM_DATA:
    default:
        {
            if( meshPacketHeader.messageType < (uint8)MessageTypeEnum::GAME_CUSTOM_DATA )
            {
                // Ignore invalid packets
                LogMeshPacketManagerComment( L"Invalid packet header: " + meshPacketHeader.messageType.ToString() );
                break;
            }

            BYTE* srcBufferPtr = packetBuffer + sizeof(MeshPacketHeader);
            uint32 srcBufferSizeInBytes = meshPacketHeader.messageSize - sizeof(MeshPacketHeader);

            Windows::Storage::Streams::Buffer^ destBuffer = ref new Windows::Storage::Streams::Buffer( srcBufferSizeInBytes );
            destBuffer->Length = srcBufferSizeInBytes;
            BYTE* destBufferBytes = nullptr;
            Utils::GetBufferBytes( destBuffer, &destBufferBytes );
            memcpy_s(destBufferBytes, destBuffer->Length, srcBufferPtr, srcBufferSizeInBytes);

            auto args = ref new GameCustomMessageReceivedEvent(
                meshPacketHeader.consoleId,
                sender,
                meshPacketHeader.messageType - (uint8)MessageTypeEnum::GAME_CUSTOM_DATA,
                destBuffer
                );

            OnGameCustomMessageReceived(this, args);
        }
        break;
    }
}

void MeshPacketManager::LogMeshPacketManagerComment( Platform::String^ message )
{
    DebugMessageEventArgs^ args = ref new DebugMessageEventArgs( message, S_OK );
    OnDebugMessage(this, args);
}

void MeshPacketManager::LogMeshPacketManagerCommentWithError( Platform::String^ message, HRESULT hr )
{
    DebugMessageEventArgs^ args = ref new DebugMessageEventArgs( message + Utils::GetErrorString(hr), hr );
    OnDebugMessage(this, args);
}

void MeshPacketManager::UpdateDebugTimers( float timeDelta )
{
    SetDebugTimeSincePacketReceive( GetDebugTimeSincePacketReceive() + timeDelta );
    SetDebugTimeSincePacketSend( GetDebugTimeSincePacketSend() + timeDelta );
}

float MeshPacketManager::GetDebugTimeSincePacketReceive()
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    return m_debugTimeSincePacketReceive;
}

void MeshPacketManager::SetDebugTimeSincePacketReceive( float val )
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    m_debugTimeSincePacketReceive = val;
}

float MeshPacketManager::GetDebugTimeSincePacketSend()
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    return m_debugTimeSincePacketSend;
}

void MeshPacketManager::SetDebugTimeSincePacketSend( float val )
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    m_debugTimeSincePacketSend = val;
}

bool MeshPacketManager::GetDebugInsideWSAReceive()
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    return m_debugInsideWSAReceive;
}

void MeshPacketManager::SetDebugInsideWSAReceive( bool val )
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    m_debugInsideWSAReceive = val;
}

bool MeshPacketManager::GetDebugInsideWSASend()
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    return m_debugInsideWSASend;
}

void MeshPacketManager::SetDebugInsideWSASend( bool val )
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    m_debugInsideWSASend = val;
}

uint16 MeshPacketManager::GetPreviousPacketMessageId()
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    return m_previousPacketMessageId;
}

void MeshPacketManager::SetPreviousPacketMessageId( uint16 val )
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    m_previousPacketMessageId = val;
}

void MeshPacketManager::SetDropOutOfOrderPackets( bool val )
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    m_dropOutOfOrderPackets = val;
}

void MeshPacketManager::SetHeartbeatSize(UINT size)
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    m_heartbeatMessageSize = size;
}

UINT MeshPacketManager::GetHeartbeatSize()
{
    Concurrency::critical_section::scoped_lock lock(m_debugStatsLock);
    return m_heartbeatMessageSize;
}

void MeshPacketManager::SendReliablePacketsUntilACK()
{
    LARGE_INTEGER timeNow;
    if (!QueryPerformanceCounter(&timeNow))
    {
        assert(false); 
    }

    LONGLONG timeDelta = m_timerLastSendReliablePacketsUntilACK.QuadPart - timeNow.QuadPart;
    LONGLONG numberOfMillisecondsSinceLast = 1000 * timeDelta / m_timerFrequency.QuadPart;

    if( numberOfMillisecondsSinceLast > 1000 )
    {
        m_timerLastSendReliablePacketsUntilACK = timeNow;
        SendReliablePacketsToConsolesWhoHaveNotAcked();
    }
}


std::vector< std::shared_ptr<MESH_PACKET_THAT_NEEDS_ACK> > MeshPacketManager::GetMeshPacketsThatNeedAckCopy()
{
    Concurrency::critical_section::scoped_lock lock(m_meshPacketsThatNeedAckLock);
    std::vector< std::shared_ptr<MESH_PACKET_THAT_NEEDS_ACK> > meshPacketsThatNeedAckCopy( m_meshPacketsThatNeedAck );
    return meshPacketsThatNeedAckCopy;
}

void MeshPacketManager::SendReliablePacketsToConsolesWhoHaveNotAcked()
{
    std::vector< std::shared_ptr<MESH_PACKET_THAT_NEEDS_ACK> > meshPacketsThatNeedAckCopy = GetMeshPacketsThatNeedAckCopy();
    for each (std::shared_ptr<MESH_PACKET_THAT_NEEDS_ACK> meshPacketThatNeedAck in meshPacketsThatNeedAckCopy)
    {
        QueuePacketToSend( meshPacketThatNeedAck->packetInfo );
    }
}

void MeshPacketManager::DeleteMeshPacketWhenGotAck(uint16 messageId)
{
    Concurrency::critical_section::scoped_lock lock(m_meshPacketsThatNeedAckLock);

    bool found = false;
    auto iter = m_meshPacketsThatNeedAck.begin();
    for( ; iter != m_meshPacketsThatNeedAck.end(); iter++ )
    {
        std::shared_ptr<MESH_PACKET_THAT_NEEDS_ACK> iterMeshPacketThatNeedAck = *iter;
        if (iterMeshPacketThatNeedAck->messageId == messageId)
        {
            found = true;
            break;
        }
    }

    if (found)
    {
        m_meshPacketsThatNeedAck.erase(iter);
    }
}

void MeshPacketManager::DeleteAllPendingAckMeshPackets()
{
    Concurrency::critical_section::scoped_lock lock(m_meshPacketsThatNeedAckLock);
    m_meshPacketsThatNeedAck.clear();
}


}}}}

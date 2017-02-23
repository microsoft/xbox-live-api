// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "MeshPacketStructs.h"
#include "MeshPacketStatistics.h"
#include "MeshConnection.h"
#include "MeshEvents.h"
#include "MeshThread.h"

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

static const int WSARECV_BUFFER_SIZE = 10000;    
#define SKIPPED_PERCENT 5 // how much ahead do you want to skip to.

#define DEFAULT_HEARTBEAT_SIZE 0

struct MESH_PACKET_INFO
{
#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAssociation^ association;
#else
    Windows::Networking::XboxLive::XboxLiveEndpointPair^ association;
#endif
    std::vector<BYTE> packetBuffer;
};

struct MESH_PACKET_THAT_NEEDS_ACK
{
    std::shared_ptr<MESH_PACKET_INFO> packetInfo;
    uint16 messageId;
};


public ref class MeshPacketManager sealed
{
internal:
    /// <summary>
    /// To get MeshPacketManager, call MeshManger::GetMeshPacketManager()
    /// </summary>
    /// <param name="localConsoleId">Local console ID</param>
    /// <param name="portNumberToBindTo">This is the sin6_port for the localSockAddress which is used to bind to the socket</param>
    /// <param name="meshManager">Instance of the mesh manager</param>
    MeshPacketManager( 
        uint8 localConsoleId, 
        unsigned short portNumberToBindTo, 
        MeshManager^ meshManager, 
        bool dropOutOfOrderPackets 
        );

public:
    /// <summary>
    /// Get local consoleId
    /// </summary>
    uint8 GetLocalConsoleId();

    MeshPacketStatistics^ GetMeshPacketStatistics();

    void UpdateDebugTimers( float timeDelta );

#ifdef _XBOX_ONE
    void SendChatMessage( 
        Windows::Xbox::Networking::SecureDeviceAssociation^ association, 
        Windows::Storage::Streams::IBuffer^ buffer,
        bool sendReliable
        );
#else
    void SendChatMessage(
        Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
        Windows::Storage::Streams::IBuffer^ buffer,
        bool sendReliable
        );
#endif

    /// <summary>
    /// gameDefinedMessageType is a 0 indexed number that the game can use to identify packet types
    /// gameDefinedMessageType can be no greater than 192
    /// </summary>
#ifdef _XBOX_ONE
    void SendCustomMessage( 
        Windows::Xbox::Networking::SecureDeviceAssociation^ association, 
        uint8 gameDefinedMessageType,
        Windows::Storage::Streams::IBuffer^ buffer,
        bool sendReliable
        );
#else
    void SendCustomMessage(
        Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
        uint8 gameDefinedMessageType,
        Windows::Storage::Streams::IBuffer^ buffer,
        bool sendReliable
        );
#endif

    /// <summary>
    /// Resend packets until ack
    /// </summary>
    void SendReliablePacketsUntilACK();

    float GetDebugTimeSincePacketReceive();
    float GetDebugTimeSincePacketSend();
    bool GetDebugInsideWSAReceive();
    bool GetDebugInsideWSASend();
    uint16 GetPreviousPacketMessageId();

    event Windows::Foundation::EventHandler<Microsoft::Xbox::Samples::NetworkMesh::MeshChatMessageReceivedEvent^>^ OnChatMessageReceived;
    event Windows::Foundation::EventHandler<Microsoft::Xbox::Samples::NetworkMesh::GameCustomMessageReceivedEvent^>^ OnGameCustomMessageReceived;

internal:  
#ifdef _XBOX_ONE
    /// <summary>
    /// The heartbeat is handled internally and shouldn't be called by the game
    /// </summary>
    void SendHeartbeatMessageAsync( 
        Windows::Xbox::Networking::SecureDeviceAssociation^ association,
        uint8 consoleId
        );

    /// <summary>
    /// The hello handshake is handled internally and shouldn't be called by the game
    /// </summary>
    void SendHelloMessage( 
        Windows::Xbox::Networking::SecureDeviceAssociation^ association, 
        Platform::String^ consoleName, 
        bool respondingToHello 
        );

    /// <summary>
    /// Sends an ACK for a message ID
    /// </summary>
    void SendAckMessage( 
        Windows::Xbox::Networking::SecureDeviceAssociation^ association, 
        uint16 messageIdToAck
        );

#else
    /// <summary>
    /// The heartbeat is handled internally and shouldn't be called by the game
    /// </summary>
    void SendHeartbeatMessageAsync(
        Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
        uint8 consoleId
        );

    /// <summary>
    /// The hello handshake is handled internally and shouldn't be called by the game
    /// </summary>
    void SendHelloMessage(
        Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
        Platform::String^ consoleName,
        bool respondingToHello
        );

    /// <summary>
    /// Sends an ACK for a message ID
    /// </summary>
    void SendAckMessage(
        Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
        uint16 messageIdToAck
        );
#endif

    void DeleteAllPendingAckMeshPackets();

    void SetDropOutOfOrderPackets(bool val);
    void SetHeartbeatSize(UINT size);
    UINT GetHeartbeatSize();
    UINT16 IncrementPacketMessageId();

    event Windows::Foundation::EventHandler<Microsoft::Xbox::Samples::NetworkMesh::MeshHeartbeatReceivedEvent^>^ OnHeartbeatReceived;
    event Windows::Foundation::EventHandler<Microsoft::Xbox::Samples::NetworkMesh::MeshHelloReceivedEvent^>^ OnHelloReceived;
    event Windows::Foundation::EventHandler<Microsoft::Xbox::Samples::NetworkMesh::MeshAckReceivedEvent^>^ OnAckReceived;

internal:
    /// <summary>
    /// The MeshManager ripples the debug event to its own OnDebugMessage so the caller can see debug events in the MeshPacketManager
    /// </summary>
    event Windows::Foundation::EventHandler<DebugMessageEventArgs^>^ OnDebugMessage;

    /// <summary>
    /// Called by the MeshManager when shutting down
    /// </summary>
    void Shutdown();

private:
    void GetPacketWithHeader( 
        size_t packetSize, 
        uint8 messageType, 
        std::vector<BYTE>& packetBuffer,
        bool sendReliable
        );

    void QueuePacketToSend( 
        std::shared_ptr<MESH_PACKET_INFO> packetInfo
        );

    void ProcessPacket( 
        Microsoft::Xbox::Samples::NetworkMesh::MeshConnection^ meshConnection, 
        BYTE* packetBuffer 
        );

    Platform::Array<BYTE>^ ConvertSenderSocketAddressToArray(
        BYTE* buffer, 
        int bufferSize
        );

    void LogMeshPacketManagerComment( 
        Platform::String^ message 
        );

    void LogMeshPacketManagerCommentWithError( 
        Platform::String^ message, 
        HRESULT hr 
        );

    bool ShouldPacketBeDropped( Microsoft::Xbox::Samples::NetworkMesh::MeshPacketHeader& packet );

    void RecordMessageIfSendingReliable( 
        std::shared_ptr<MESH_PACKET_INFO> packetInfo
        );

    MeshConnection^ GetMeshConnection( SOCKADDR_STORAGE senderSocketAddress );

    void SendReliablePacketsToConsolesWhoHaveNotAcked();
    std::vector< std::shared_ptr<MESH_PACKET_THAT_NEEDS_ACK> > GetMeshPacketsThatNeedAckCopy();
    void DeleteMeshPacketWhenGotAck(uint16 messageId);

    void SocketReceiveWorkerThreadDoWork( Microsoft::Xbox::Samples::NetworkMesh::ProcessThreadsEventArgs^ args );
    void SocketSendWorkerThreadDoWork( Microsoft::Xbox::Samples::NetworkMesh::ProcessThreadsEventArgs^ args );
    void ProcessSendPacket( std::shared_ptr<MESH_PACKET_INFO> packetInfo );

    void SetDebugTimeSincePacketReceive(float val);
    void SetDebugTimeSincePacketSend(float val);
    void SetDebugInsideWSAReceive(bool val);
    void SetDebugInsideWSASend(bool val);
    void SetPreviousPacketMessageId(uint16 val);

private:
    SOCKET m_localSocket;
    SOCKADDR_IN6 m_localSockAddress;
    uint8 m_localConsoleId;
    LONG m_packetMessageId;
    uint16 m_previousPacketMessageId;
    MeshPacketStatistics^ m_meshPacketStatistics;
    Platform::WeakReference m_meshManager;
    bool m_dropOutOfOrderPackets;

    MeshThread^ m_socketReceiveThread;
    BYTE m_bufferForWSARecv[WSARECV_BUFFER_SIZE];

    MeshThread^ m_socketSendThread;
    Concurrency::critical_section m_sendLock;
    std::queue< std::shared_ptr<MESH_PACKET_INFO> > m_packetsToSend;
    HANDLE m_sendWakeUpEventHandle;

    Concurrency::critical_section m_debugStatsLock;
    Concurrency::critical_section m_stateLock;
    float m_debugTimeSincePacketReceive;
    float m_debugTimeSincePacketSend;
    bool m_debugInsideWSAReceive;
    bool m_debugInsideWSASend;

    UINT m_heartbeatMessageSize;

    Concurrency::critical_section m_meshPacketsThatNeedAckLock;
    std::vector< std::shared_ptr<MESH_PACKET_THAT_NEEDS_ACK> > m_meshPacketsThatNeedAck;
    LARGE_INTEGER m_timerFrequency;
    LARGE_INTEGER m_timerLastSendReliablePacketsUntilACK;
};

}}}}

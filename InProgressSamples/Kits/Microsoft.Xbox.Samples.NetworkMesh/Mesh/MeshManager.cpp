// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MeshManager.h"
#include "Utils.h"

using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Xbox::Networking;
using namespace Microsoft::Xbox::Samples::NetworkMesh;

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

MeshManager::MeshManager(
    uint8 localConsoleId, 
    Platform::String^ secureDeviceAssociationTemplateName, 
    Platform::String^ localConsoleName,
    bool dropOutOfOrderPackets ) :
    m_localConsoleName(localConsoleName),
    m_dropOutOfOrderPackets(dropOutOfOrderPackets)
{
    // load template for Secure Device Association.
    m_associationTemplate = Windows::Xbox::Networking::SecureDeviceAssociationTemplate::GetTemplateByName( secureDeviceAssociationTemplateName );

    Initialize(localConsoleId);
    RegisterMeshPacketEventHandlers();
}

void MeshManager::Initialize(uint8 localConsoleId)
{
    m_connections.clear();
    
    if(m_associationTemplate != nullptr)
    {
        DestroyAllTemplateAssociations();

        // Listen to AssociationIncoming event
        TypedEventHandler<Windows::Xbox::Networking::SecureDeviceAssociationTemplate^, Windows::Xbox::Networking::SecureDeviceAssociationIncomingEventArgs^>^ associationIncomingEvent = 
            ref new TypedEventHandler<Windows::Xbox::Networking::SecureDeviceAssociationTemplate^, Windows::Xbox::Networking::SecureDeviceAssociationIncomingEventArgs^>(
            [this] (Windows::Xbox::Networking::SecureDeviceAssociationTemplate^ associationTemplate, Windows::Xbox::Networking::SecureDeviceAssociationIncomingEventArgs^ args)
        {
            OnAssociationIncoming( associationTemplate, args );
        });
        m_associationIncomingToken = m_associationTemplate->AssociationIncoming += associationIncomingEvent;
    }
    else
    {
        LogComment("Association template is NULL!");
        return;
    }

    unsigned short sin6_port = htons(m_associationTemplate->AcceptorSocketDescription->BoundPortRangeLower);

    m_meshPacketManager = ref new MeshPacketManager(localConsoleId, sin6_port, this, m_dropOutOfOrderPackets);
    
    // Try to connect to anything we need to every second 
    int32 threadAffinityMask = ~0x04; // Means to this thread can run all everything except core 3 (which is reserved for graphics for example).
    m_autoConnectThread = ref new MeshThread(1000, threadAffinityMask, NORMAL_PRIORITY_CLASS);
    m_autoConnectThread->OnDoWork += ref new Windows::Foundation::EventHandler<ProcessThreadsEventArgs^>( [this]( Platform::Object^, ProcessThreadsEventArgs^ args )
    {
        OnAutoConnectWorkerThreadDoWork(args);
    });

    // Send heartbeats and hellos every 2 seconds
    threadAffinityMask = ~0x04; // Means to this thread can run all everything except core 3 (which is reserved for graphics for example).
    m_heartbeatThread = ref new MeshThread(DEFAULT_HEARTBEAT_PERIOD_MILLISECONDS, threadAffinityMask, NORMAL_PRIORITY_CLASS);
    m_heartbeatThread->OnDoWork += ref new Windows::Foundation::EventHandler<ProcessThreadsEventArgs^>( [this]( Platform::Object^, ProcessThreadsEventArgs^ args )
    {
        OnHeartbeatWorkerThreadDoWork(args);
    });
}

SecureDeviceAssociationTemplate^ MeshManager::GetSecureDeviceAssociationTemplate()
{
    return m_associationTemplate;
}

// This should only be called from ConnectTo or OnIncomingAssociation.
MeshConnection^ MeshManager::AddConnection( 
    Windows::Xbox::Networking::SecureDeviceAddress^ secureDeviceAddress,
    Windows::Xbox::Networking::SecureDeviceAssociation^ secureDeviceAssociation,
    bool inComingAssociation,
    ConnectionStatus connectionStatus    
    )
{
    if(secureDeviceAddress == nullptr)
    {
        return nullptr;
    }

    if(GetConnectionFromSecureDeviceAddress(secureDeviceAddress) != nullptr)
    {
        // The secure device address already exists.
        return nullptr;
    }

    MeshConnection^ meshConnection = ref new MeshConnection(secureDeviceAddress, this);
    meshConnection->SetInComingAssociation(inComingAssociation);
    meshConnection->SetConnectionStatus(connectionStatus);
    meshConnection->SetAssociation(secureDeviceAssociation);

    // Add him to the list of mesh connections.
    {
        Concurrency::critical_section::scoped_lock lock(m_connectionsLock);
        m_connections.push_back(meshConnection);
    }

    return meshConnection;
}

void MeshManager::ConnectToAddress(
    Windows::Xbox::Networking::SecureDeviceAddress^ secureDeviceAddress,
    Platform::String^ debugName
    )
{
    if (secureDeviceAddress == nullptr)
    {
        LogComment(L"Cannot pass a nullptr for the address to MeshManager::ConnectTo");
        throw ref new InvalidArgumentException(L"Cannot pass a nullptr for the address to MeshManager::ConnectTo");
    }

    if(AreSecureDeviceAddressesEqual(SecureDeviceAddress::GetLocal(), secureDeviceAddress))
    {
        // Don't try to connect to local console.
        return;
    }

    LogComment( "ConnectToAddress: Attempting to connect to remote console: " + debugName);
    MeshConnection^ newMeshConnected = AddConnection(secureDeviceAddress, nullptr, false, ConnectionStatus::Disconnected);
    if( newMeshConnected != nullptr )
    {
        // If this connection was new, then set the console name to be debug name.  It will change once the handshake is done
        newMeshConnected->SetConsoleName(debugName);
    }
}

void MeshManager::OnAssociationIncoming( 
    Windows::Xbox::Networking::SecureDeviceAssociationTemplate^ associationTemplate, 
    Windows::Xbox::Networking::SecureDeviceAssociationIncomingEventArgs^ args 
    )
{
    SecureDeviceAssociation^ association = args->Association;
    if(association != nullptr)
    {
        LogCommentFormat( L"OnAssociationIncoming: Incoming connection %s", Utils::PrintSecureDeviceAssociation(association, false, true)->Data() );
        MeshConnection^ newMeshConnected = AddConnection(association->RemoteSecureDeviceAddress, association, true, ConnectionStatus::Connected);
        if(newMeshConnected != nullptr)
        {
            RefreshConnections();
        }
    }
}

void MeshManager::OnAutoConnectWorkerThreadDoWork( Microsoft::Xbox::Samples::NetworkMesh::ProcessThreadsEventArgs^ args )
{
    // Try to connect to every outgoing connection that we are disconnected from.
    SecureDeviceAssociationTemplate^ secureDeviceAssociationTemplate = GetSecureDeviceAssociationTemplate();
    Windows::Foundation::Collections::IVectorView<MeshConnection^>^ allDisconnectedConnections = GetConnectionsByType(ConnectionStatus::Disconnected);
    for each (MeshConnection^ meshConnection in allDisconnectedConnections)
    {
        if(meshConnection->IsInComingAssociation())
        {
            // Don't bother connecting to someone who connected to you 
            // because that would create bi-directional connection which cause failures.
            continue;
        }

        // Ignore connections that have a CreateAssociationAsync in progress on them
        if(meshConnection->IsConnectionInProgress())
        {
            continue;
        }

        // Ignore anyone we have now a connection with.  Just in case anything changed since we got the list of disconnected connections above
        if( meshConnection->GetAssociation() != nullptr )
        {
            continue;
        }

        LogComment( Utils::GetThreadDescription(L"THREAD: Calling CreateAssociationAsync") );

        meshConnection->SetConnectionInProgress(true);
        auto asyncOp = secureDeviceAssociationTemplate->CreateAssociationAsync(
            meshConnection->GetSecureDeviceAddress(),
            Windows::Xbox::Networking::CreateSecureDeviceAssociationBehavior::Default
            );

        create_task( asyncOp )
        .then([this, meshConnection](task<SecureDeviceAssociation^> t)
        {
            LogComment( Utils::GetThreadDescription(L"THREAD: CreateAssociationAsync result") );

            try
            {
                SecureDeviceAssociation^ association = t.get();
                LogCommentFormat( L"CreateAssociationAsync success: %s %s", meshConnection->GetConsoleName()->Data(), Utils::PrintSecureDeviceAssociation(association, false, true)->Data() );
                MeshConnection^ newMeshConnection = GetConnectionFromSecureDeviceAddress(association->RemoteSecureDeviceAddress);
                if(newMeshConnection != nullptr)
                {
                    LogCommentFormat( L"Created new connection for %s", meshConnection->GetConsoleName()->Data() );
                    newMeshConnection->SetAssociation(association);
                    newMeshConnection->SetConnectionStatus(ConnectionStatus::Connected);
                }
            }
            catch(Platform::Exception^ ex)
            {
                //We will auto-retry this again on next update.
                LogCommentFormat(L"Connecting to remote machine failed %s. %s", meshConnection->GetConsoleName()->Data(), Utils::GetErrorString(ex->HResult)->Data() );
            }

            meshConnection->SetConnectionInProgress(false);
        });

        // This thread is waiting for CreateAssociationAsync to complete since this is happening 
        // inside a worker thread that does nothing but try to connect to other consoles
    }
}

void MeshManager::OnHeartbeatWorkerThreadDoWork( Microsoft::Xbox::Samples::NetworkMesh::ProcessThreadsEventArgs^ args )
{
    // This function is called every so often (eg. every 2 sec) by the MeshThread class

    // First, refresh the connection list
    RefreshConnections();

    // Send hello to all connections that need it
    Windows::Foundation::Collections::IVectorView<MeshConnection^>^ allconnected = GetConnectionsByType(ConnectionStatus::Connected);
    for each (MeshConnection^ meshConnection in allconnected)
    {
        //Send hello to retry handshake.
        Windows::Xbox::Networking::SecureDeviceAssociation^ association = meshConnection->GetAssociation();
        if(association != nullptr)
        {
            bool isRespondingToHello = false;
            Platform::String^ remoteName = Utils::PrintSecureDeviceAssociation(association, false, true);
            LogCommentFormat( L"Sending hello to remote console: %s", remoteName->Data());
            m_meshPacketManager->SendHelloMessage(association, GetLocalConsoleDisplayName(), isRespondingToHello);
        }
    }

    // Send heartbeat to all connections that we have completed handshake with
    Windows::Foundation::Collections::IVectorView<MeshConnection^>^ initializedConnections = GetConnectionsByType(ConnectionStatus::PostHandshake);
    for each (MeshConnection^ meshConnection in initializedConnections)
    {
        Windows::Xbox::Networking::SecureDeviceAssociation^ association = meshConnection->GetAssociation();
        if( association != nullptr )
        {
            m_meshPacketManager->SendHeartbeatMessageAsync( association, meshConnection->GetConsoleId() );
        }
    }
}

Platform::String^ MeshManager::GetLocalConsoleName()
{
    Platform::String^ localConsoleName = L"Console";
    {
        Concurrency::critical_section::scoped_lock lock(m_connectionsLock);
        localConsoleName = m_localConsoleName;
    }

    return localConsoleName;
}

void MeshManager::SetLocalConsoleName(Platform::String^ localConsoleName)
{
    {
        Concurrency::critical_section::scoped_lock lock(m_connectionsLock);
        m_localConsoleName = localConsoleName;
    }
}

Platform::String^ MeshManager::GetLocalConsoleDisplayName()
{
    return Utils::FormatString(L"%s [%d]", GetLocalConsoleName()->Data(), m_meshPacketManager->GetLocalConsoleId() );
}

/// <summary>
/// </summary>
Windows::Foundation::Collections::IVectorView<MeshConnection^>^ MeshManager::GetConnections()
{
    Concurrency::critical_section::scoped_lock lock(m_connectionsLock);
    auto v = ref new Platform::Collections::Vector<MeshConnection^>(m_connections);
    return v->GetView();
}

Windows::Foundation::Collections::IVectorView<MeshConnection^>^ MeshManager::GetConnectionsByType(ConnectionStatus type)
{
    auto associationsByType = ref new Platform::Collections::Vector<MeshConnection^>();
    
    Windows::Foundation::Collections::IVectorView<MeshConnection^>^ allConnections = GetConnections();
    for each (MeshConnection^ meshConnection in allConnections)
    {
        if( meshConnection->GetConnectionStatus() == type )
        {
            associationsByType->Append(meshConnection);
        }
    }
    return associationsByType->GetView();
}

void MeshManager::RefreshConnections()
{
    // First, loop through all connections and mark all connections as not found in the template
    Windows::Foundation::Collections::IVectorView<MeshConnection^>^ allConnections = GetConnections();
    for each (MeshConnection^ meshConnection in allConnections)
    {
        meshConnection->SetAssocationFoundInTemplate(false);
    }

    // Now, Loop thought template and find the each connection and mark the ones that we found
    // And take note of anything in the template that is not yet in our internal list of connections.
    Windows::Foundation::Collections::IVectorView<Windows::Xbox::Networking::SecureDeviceAssociation^>^ associations = GetSecureDeviceAssociationTemplate()->Associations;
    for each (Windows::Xbox::Networking::SecureDeviceAssociation^ associationInTemplate in associations)
    {
        MeshConnection^ meshConnection = GetConnectionFromSecureDeviceAddress(associationInTemplate->RemoteSecureDeviceAddress);
        if(meshConnection != nullptr)
        {
            meshConnection->SetAssocationFoundInTemplate(true);
            ConnectionStatus status = meshConnection->GetConnectionStatus();

            // If the status is incorrect, then fix it log the error and fix it
            if( status != ConnectionStatus::Connected && 
                status != ConnectionStatus::PostHandshake )
            {
                LogCommentFormat( L"ERROR: Found association in template that we didn't CreateAssociationAsync or get an OnAssociationIncoming event for" );
                LogCommentFormat( L"ERROR: Address: %s", Utils::PrintSecureDeviceAssociation(associationInTemplate, false, true)->Data() );

                meshConnection->SetConnectionStatus(ConnectionStatus::Connected);
                meshConnection->SetAssociation(associationInTemplate);
            }
        }
        else
        {
            LogCommentFormat( L"ERROR: Mismatch occurred: Association in template not found in our internal list of connections" );
        }
    }

    // First, loop through all connections again and for anything not found in the template, disconnect from it and tell the game.
    // Get the latest list again as connections could have been deleted.
    allConnections = GetConnections();
    for each (MeshConnection^ meshConnection in allConnections)
    {
        if ( meshConnection != nullptr && 
            false == meshConnection->GetAssocationFoundInTemplate() && 
            meshConnection->GetConnectionStatus() != ConnectionStatus::Pending &&
            meshConnection->GetConnectionStatus() != ConnectionStatus::Disconnected
            )
        {
            // Do not delete him. We want the raise an event to the game and have the game call DestroyConnection explicitly.
            LogCommentFormat( L"Disconnecting remote console %s who was not in template", meshConnection->GetConsoleName()->Data() );
            LogCommentFormat( L"Address: %s", Utils::PrintSecureDeviceAssociation(meshConnection->GetAssociation(), false, true)->Data() );
            meshConnection->SetConnectionStatus(ConnectionStatus::Disconnected);
            OnDisconnected(this, meshConnection);
        }
    }
}

bool MeshManager::DoesConnectionExistInList(Windows::Foundation::Collections::IVectorView<MeshConnection^>^ list, MeshConnection^ connection)
{
    if(connection == nullptr)
    {
        return false;
    }

    bool bfound = false;
    for (unsigned int i = 0; i < list->Size; i++ )
    {
        if (list->GetAt(i) == connection)
        {
            bfound = true;
            break;
        }
    }

    return bfound;
}

void MeshManager::OnAssociationChange( Windows::Xbox::Networking::SecureDeviceAssociationStateChangedEventArgs^ args, Windows::Xbox::Networking::SecureDeviceAssociation^ association )
{
    // Update this mesh member with the latest information
    switch(args->NewState)
    {
    case Windows::Xbox::Networking::SecureDeviceAssociationState::DestroyingLocal:
    case Windows::Xbox::Networking::SecureDeviceAssociationState::DestroyingRemote:
    case Windows::Xbox::Networking::SecureDeviceAssociationState::Invalid:
        {
            bool bFound = false;
            {
                MeshConnection^ meshConnection = GetConnectionFromAssociation(association);
                if(meshConnection != nullptr)
                {
                    bFound = true;
                    // let the user know that this connection is being disconnected.
                    LogCommentFormat( L"Remote console is disconnecting %s", meshConnection->GetConsoleName()->Data() );
                    LogComment( Utils::GetThreadDescription(L"THREAD: Remote console disconnecting") );

                    meshConnection->SetConnectionDestroying(true);
                    meshConnection->SetConnectionStatus(ConnectionStatus::Disconnected);
                    OnDisconnected(this, meshConnection);
                }
            }

            if (!bFound)
            {
                LogComment(L"Association Change event fired for the wrong association.");
            }
        }
    default:
        break;
    }
}

MeshConnection^ MeshManager::GetConnectionFromAssociation(Windows::Xbox::Networking::SecureDeviceAssociation^ association)
{
    if(association == nullptr)
    {
        return nullptr;
    }

    SecureDeviceAddress^ remoteSecureDeviceAddress = association->RemoteSecureDeviceAddress;
    return GetConnectionFromSecureDeviceAddress(remoteSecureDeviceAddress);
}

MeshConnection^ MeshManager::GetConnectionFromSecureDeviceAddress(Windows::Xbox::Networking::SecureDeviceAddress^ address)
{
    if(address == nullptr)
    {
        return nullptr;
    }

    Windows::Foundation::Collections::IVectorView<MeshConnection^>^ allConnections = GetConnections();
    for each (MeshConnection^ meshConnection in allConnections)
    {
        SecureDeviceAddress^ remoteSecureDeviceAddress = meshConnection->GetSecureDeviceAddress();
        if(AreSecureDeviceAddressesEqual(remoteSecureDeviceAddress, address))
        {
            return meshConnection;
        }
    }
    return nullptr;
}

MeshConnection^ MeshManager::GetConnectionFromConsoleId(uint8 consoleId)
{
    Windows::Foundation::Collections::IVectorView<MeshConnection^>^ allConnections = GetConnections();
    for each (MeshConnection^ meshConnection in allConnections)
    {
        uint8 meshConsoleId = meshConnection->GetConsoleId();
        if( meshConsoleId == consoleId )
        {
            return meshConnection;
        }
    }
    return nullptr;
}

void MeshManager::DeleteConnection(MeshConnection^ connection)
{
    if (connection == nullptr)
    {
        LogComment(L"Cannot pass a nullptr for the member to DeleteConnection");
        throw ref new InvalidArgumentException(L"Cannot pass a nullptr for the member to DeleteConnection");
    }

    bool found = false;
    {
        Concurrency::critical_section::scoped_lock lock(m_connectionsLock);

        auto iter = m_connections.begin();
        for( ; iter != m_connections.end(); iter++ )
        {
            if ((*iter) == connection)
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            m_connections.erase(iter);
        }
    }
}

void MeshManager::DestroyConnection( MeshConnection^ connection )
{
    if(connection == nullptr)
    {
        return;
    }

    SecureDeviceAssociation^ meshAssociation = connection->GetAssociation();
    if(meshAssociation != nullptr)
    {
        if(false == connection->IsConnectionDestroying())
        {
            connection->SetConnectionDestroying(true);
            IAsyncAction^ asyncOp = meshAssociation->DestroyAsync();
            create_task(asyncOp)
                .then([this] (task<void> t)
            {
                try
                {
                    t.get(); // if t.get fails, it will throw.
                }
                catch (Platform::COMException^ ex)
                {
                    LogCommentFormat( L"MeshManager::DestroyConnection - DestroyAsync failed %s", Utils::GetErrorString(ex->HResult)->Data());
                }
            }).wait();
        }
        connection->SetAssociation(nullptr);
    }

    DeleteConnection(connection);
}

void MeshManager::DisconectFromAddress( Windows::Xbox::Networking::SecureDeviceAddress^ address )
{
    MeshConnection^ connection = GetConnectionFromSecureDeviceAddress(address);
    DestroyConnection(connection);
}

/// <summary>
/// </summary>
void MeshManager::DestroyAndDisconnectAll()
{
    Windows::Foundation::Collections::IVectorView<MeshConnection^>^ allConnections = GetConnections();
    for each (MeshConnection^ meshConnection in allConnections)
    {
        DestroyConnection(meshConnection);
    }

    {
        Concurrency::critical_section::scoped_lock lock(m_connectionsLock);
        m_connections.clear();
    }
    
    GetMeshPacketManager()->DeleteAllPendingAckMeshPackets();
}

void MeshManager::DestroyAllTemplateAssociations()
{
    if(GetSecureDeviceAssociationTemplate() != nullptr)
    {
        Windows::Foundation::Collections::IVectorView<Windows::Xbox::Networking::SecureDeviceAssociation^>^ associations = GetSecureDeviceAssociationTemplate()->Associations;
        if(associations == nullptr || associations->Size == 0)
        {
            return;
        }

        LogCommentFormat(L"We have %d associations on MeshManager::Initialize", associations->Size );
        for each (Windows::Xbox::Networking::SecureDeviceAssociation^ associationInTemplate in associations)
        {
            IAsyncAction^ asyncOp = associationInTemplate->DestroyAsync();
            create_task(asyncOp)
                .then([this] (task<void> t)
            {
                try
                {
                    t.get(); // if t.get fails, it will throw.
                }
                catch (Platform::COMException^ ex)
                {
                    LogCommentFormat( L"MeshManager::DestroyAllTemplateAssociations - DestroyAsync failed %s", Utils::GetErrorString(ex->HResult)->Data());
                }
            }).wait();
        }
    }
}

void MeshManager::Shutdown()
{
    LogComment( L"MeshManager::Shutdown");

    if (m_autoConnectThread != nullptr)
    {
        m_autoConnectThread->Shutdown();
        m_autoConnectThread = nullptr;
    }

    if (m_heartbeatThread != nullptr)
    {
        m_heartbeatThread->Shutdown();
        m_heartbeatThread = nullptr;
    }

    if( m_associationTemplate != nullptr )
    {
        m_associationTemplate->AssociationIncoming -= m_associationIncomingToken;
        m_associationTemplate = nullptr;
    }

    if( m_meshPacketManager != nullptr )
    {
        m_meshPacketManager->OnHelloReceived -= m_onHelloReceivedToken;
        m_meshPacketManager->OnHeartbeatReceived -= m_onHeartbeatReceivedToken;
        m_meshPacketManager->OnDebugMessage -= m_onDebugMessageReceivedToken;

        m_meshPacketManager->Shutdown();
        m_meshPacketManager = nullptr;
    }
}

bool MeshManager::AreSecureDeviceAddressesEqual( Windows::Xbox::Networking::SecureDeviceAddress^ secureDeviceAddress1, Windows::Xbox::Networking::SecureDeviceAddress^ secureDeviceAddress2 )
{
    if( secureDeviceAddress1 != nullptr && 
        secureDeviceAddress2 != nullptr &&
        secureDeviceAddress1->Compare(secureDeviceAddress2) == 0 )
    {
        return true;
    }
    return false;
}

Microsoft::Xbox::Samples::NetworkMesh::MeshPacketManager^ MeshManager::GetMeshPacketManager()
{
    return m_meshPacketManager;
}

UINT MeshManager::GetHeartbeatPeriod()
{
    if (m_heartbeatThread == nullptr)
        return 0;

    return m_heartbeatThread->GetSendPeriod();
}

void MeshManager::SetHeartbeatPeriod(UINT periodInMilliseconds)
{
    if (m_heartbeatThread != nullptr)
    m_heartbeatThread->SetSendPeriod(periodInMilliseconds);
}

void MeshManager::OnHeartbeatReceived( Microsoft::Xbox::Samples::NetworkMesh::MeshHeartbeatReceivedEvent^ args )
{
    if(args != nullptr)
    {
        MeshConnection^ meshConnection = args->Sender;
        if(meshConnection != nullptr)
        {
            OnHeartbeat( this, args->Sender );
        }
    }
}

void MeshManager::OnHelloReceived( Microsoft::Xbox::Samples::NetworkMesh::MeshHelloReceivedEvent^ args )
{
    if(args != nullptr)
    {
        LogCommentFormat( L"OnHelloReceived: Remote console: %s [%d]. RespondingToHello: %d", args->ConsoleName->Data(), args->ConsoleId, (int)args->RespondingToHello );

        MeshConnection^ meshConnection = args->Sender;
        if(meshConnection != nullptr )
        {
            Windows::Xbox::Networking::SecureDeviceAssociation^ secureDeviceAssociation = meshConnection->GetAssociation();
            if( secureDeviceAssociation == nullptr )
            {
                return;
            }

            if (args->ConsoleId != 0)
            {
                meshConnection->SetConsoleId(args->ConsoleId);
            }

            if (!args->ConsoleName->IsEmpty())
            {
                meshConnection->SetConsoleName(args->ConsoleName);
            }

            // When the remote console sends us a Hello that wasn't a response from us, respond back with Hello
            if( args->RespondingToHello == false )
            {
                LogCommentFormat( L"OnHelloReceived: Responding to hello to %s", Utils::PrintSecureDeviceAssociation(secureDeviceAssociation, false, true)->Data()  );
                m_meshPacketManager->SendHelloMessage( secureDeviceAssociation, GetLocalConsoleDisplayName(), true );
            }

            // If the connection to them was not Initialized, mark it and fire OnPostHandshake
            ConnectionStatus connectionStatus = meshConnection->GetConnectionStatus();
            if( connectionStatus != ConnectionStatus::PostHandshake )
            {
                meshConnection->SetConnectionStatus(ConnectionStatus::PostHandshake);
                LogCommentFormat( L"Remote console %s", Utils::PrintSecureDeviceAssociation(secureDeviceAssociation, false, true)->Data() );
                LogCommentFormat( L"is now known as %s", meshConnection->GetConsoleName()->Data() );
                OnPostHandshake( this, meshConnection );
            }
        }
    }
}

void MeshManager::OnDebugMessageReceived( Microsoft::Xbox::Samples::NetworkMesh::DebugMessageEventArgs^ args )
{
    DebugMessageEventArgs^ eventArgs = ref new DebugMessageEventArgs( args->Message, args->HResult);
    OnDebugMessage(this,eventArgs);
}

void MeshManager::RegisterMeshPacketEventHandlers()
{
    if(m_meshPacketManager == nullptr)
    {
        LogComment(L"Could not register for events as MeshPacketManager may have failed initializing.");
        return;
    }

    EventHandler<MeshHelloReceivedEvent^>^ onHelloReceivedEvent = ref new EventHandler<MeshHelloReceivedEvent^>(
        [this] (Platform::Object^, MeshHelloReceivedEvent^ eventArgs)
    {
        OnHelloReceived(eventArgs);
    });
    m_onHelloReceivedToken = m_meshPacketManager->OnHelloReceived += onHelloReceivedEvent;

    EventHandler<MeshHeartbeatReceivedEvent^>^ onHeartbeatReceivedEvent = ref new EventHandler<MeshHeartbeatReceivedEvent^>(
        [this] (Platform::Object^, MeshHeartbeatReceivedEvent^ eventArgs)
    {
        OnHeartbeatReceived(eventArgs);
    });
    m_onHeartbeatReceivedToken = m_meshPacketManager->OnHeartbeatReceived += onHeartbeatReceivedEvent;

    EventHandler<DebugMessageEventArgs^>^ onDebugMessageEvent = ref new EventHandler<DebugMessageEventArgs^>(
        [this] (Platform::Object^, DebugMessageEventArgs^ eventArgs)
    {
        OnDebugMessageReceived(eventArgs);
    });
    m_onDebugMessageReceivedToken = m_meshPacketManager->OnDebugMessage += onDebugMessageEvent;
}

void MeshManager::LogCommentFormat( LPCWSTR strMsg, ... )
{
    WCHAR strBuffer[2048];

    va_list args;
    va_start(args, strMsg);
    _vsnwprintf_s( strBuffer, 2048, _TRUNCATE, strMsg, args );
    strBuffer[2047] = L'\0';

    va_end(args);

    LogComment(ref new Platform::String(strBuffer));
}

void MeshManager::LogComment( Platform::String^ strText )
{
    DebugMessageEventArgs^ eventArgs = ref new DebugMessageEventArgs( strText, S_OK);
    OnDebugMessageReceived(eventArgs);
}

}}}}



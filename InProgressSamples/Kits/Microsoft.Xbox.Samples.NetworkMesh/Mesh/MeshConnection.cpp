// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MeshConnection.h"
#include "MeshManager.h"

using namespace Microsoft::Xbox::Samples::NetworkMesh;
using namespace Windows::Foundation;

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

#ifdef _XBOX_ONE
MeshConnection::MeshConnection(Windows::Xbox::Networking::SecureDeviceAddress^ secureDeviceAddress, MeshManager^ manager) :
#else
MeshConnection::MeshConnection(Windows::Networking::XboxLive::XboxLiveDeviceAddress^ secureDeviceAddress, MeshManager^ manager) :
#endif
    m_secureDeviceAddress(secureDeviceAddress),
    m_customProperty(nullptr),
    m_assocationFoundInTemplate(false),
    m_isInComingAssociation(false),
    m_isConnectionDestroying(false),
    m_isConnectionInProgress(false),
    m_retryAttempts(0),
    m_timerSinceLastAttempt(0.0f),
    m_connectionStatus(ConnectionStatus::Disconnected),
    m_consoleId(0xFF),
    m_heartTimer(0.0f)
{
    m_meshManager = Platform::WeakReference(manager);
    m_userIdsToUserData = std::map<Platform::String^, UserMeshConnectionPropertyBag^>();

    if(secureDeviceAddress == nullptr || manager == nullptr)
    {
        throw ref new Platform::InvalidArgumentException();
    }
}

uint8 MeshConnection::GetConsoleId()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_consoleId;
}

void MeshConnection::SetConsoleId(uint8 consoleId)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_consoleId = consoleId;
}

Platform::String^ MeshConnection::GetConsoleName()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);

    Platform::String^ consoleName = L"n/a";
    if( !m_consoleName->IsEmpty() )
    {
        consoleName = m_consoleName;
    }

    return consoleName;
}

void MeshConnection::SetConsoleName(Platform::String^ consoleName)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_consoleName = consoleName;
}

Platform::Object^ MeshConnection::GetCustomProperty()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_customProperty;
}

void MeshConnection::SetCustomProperty(Platform::Object^ object)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_customProperty = object;
}

int MeshConnection::GetNumberOfRetryAttempts()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_retryAttempts;
}

void MeshConnection::SetNumberOfRetryAttempts(int retryAttempt)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_retryAttempts = retryAttempt;
}

ConnectionStatus MeshConnection::GetConnectionStatus()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_connectionStatus;
}

void MeshConnection::SetConnectionStatus(ConnectionStatus status)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_connectionStatus = status;
}

bool MeshConnection::GetAssocationFoundInTemplate()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_assocationFoundInTemplate;
}

void MeshConnection::SetAssocationFoundInTemplate(bool val)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_assocationFoundInTemplate = val;
}

#ifdef _XBOX_ONE
Windows::Xbox::Networking::SecureDeviceAssociation^ MeshConnection::GetAssociation()
#else
Windows::Networking::XboxLive::XboxLiveEndpointPair^ MeshConnection::GetAssociation()
#endif
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_association;
}

#ifdef _XBOX_ONE
void MeshConnection::SetAssociation(Windows::Xbox::Networking::SecureDeviceAssociation^ association)
#else
void MeshConnection::SetAssociation(Windows::Networking::XboxLive::XboxLiveEndpointPair^ association)
#endif
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);

    if (m_association != nullptr)
    {
        m_association->StateChanged -= m_associationStateChangeToken;
    }

    m_association = association;

    if(association != nullptr)
    {
        // This is needed to know if the association is ever dropped.
#ifdef _XBOX_ONE
        TypedEventHandler<Windows::Xbox::Networking::SecureDeviceAssociation^, Windows::Xbox::Networking::SecureDeviceAssociationStateChangedEventArgs^>^ stateChangeEvent = 
            ref new TypedEventHandler<Windows::Xbox::Networking::SecureDeviceAssociation^, Windows::Xbox::Networking::SecureDeviceAssociationStateChangedEventArgs^>(
            [this] (Windows::Xbox::Networking::SecureDeviceAssociation^ association, Windows::Xbox::Networking::SecureDeviceAssociationStateChangedEventArgs^ args)
#else
        TypedEventHandler<Windows::Networking::XboxLive::XboxLiveEndpointPair^, Windows::Networking::XboxLive::XboxLiveEndpointPairStateChangedEventArgs^>^ stateChangeEvent =
            ref new TypedEventHandler<Windows::Networking::XboxLive::XboxLiveEndpointPair^, Windows::Networking::XboxLive::XboxLiveEndpointPairStateChangedEventArgs^>(
                [this](Windows::Networking::XboxLive::XboxLiveEndpointPair^ association, Windows::Networking::XboxLive::XboxLiveEndpointPairStateChangedEventArgs^ args)
#endif
        {
            HandleAssociationChangedEvent(association, args);
        });

        m_associationStateChangeToken = association->StateChanged += stateChangeEvent;
    }
}

#ifdef _XBOX_ONE
void MeshConnection::HandleAssociationChangedEvent(
    Windows::Xbox::Networking::SecureDeviceAssociation^ association, 
    Windows::Xbox::Networking::SecureDeviceAssociationStateChangedEventArgs^ args)
#else
void MeshConnection::HandleAssociationChangedEvent(
    Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
    Windows::Networking::XboxLive::XboxLiveEndpointPairStateChangedEventArgs^ args)
#endif
{
    MeshManager^ meshManager = m_meshManager.Resolve<MeshManager>();
    meshManager->OnAssociationChange(args, association);
}

#ifdef _XBOX_ONE
Windows::Xbox::Networking::SecureDeviceAddress^ MeshConnection::GetSecureDeviceAddress()
#else
Windows::Networking::XboxLive::XboxLiveDeviceAddress^ MeshConnection::GetSecureDeviceAddress()
#endif
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_secureDeviceAddress;
}

bool MeshConnection::IsInComingAssociation()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_isInComingAssociation;
}

void MeshConnection::SetInComingAssociation(bool val)
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_isInComingAssociation = val;
}

bool MeshConnection::IsConnectionDestroying()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_isConnectionDestroying;
}

void MeshConnection::SetConnectionDestroying( bool val )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_isConnectionDestroying = val;
}

bool MeshConnection::IsConnectionInProgress()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_isConnectionInProgress;
}

void MeshConnection::SetConnectionInProgress( bool val )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_isConnectionInProgress = val;
}

void MeshConnection::SetHeartTimer( float timer )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    m_heartTimer = timer;
}

float MeshConnection::GetHeartTimer()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_heartTimer;
}


UserMeshConnectionPropertyBag^ MeshConnection::GetUserPropertyBag( Platform::String^ xboxUserId )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    return m_userIdsToUserData[xboxUserId];
}

UserMeshConnectionPropertyBag^ MeshConnection::AddUserPropertyBag( Platform::String^ xboxUserId )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    UserMeshConnectionPropertyBag^ userMeshConnectionPropertyBag = ref new UserMeshConnectionPropertyBag(xboxUserId);
    m_userIdsToUserData[xboxUserId] = userMeshConnectionPropertyBag;
    return userMeshConnectionPropertyBag;
}

}}}}
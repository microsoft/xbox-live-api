// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "UserMeshConnectionPropertyBag.h"
#include <map>
#include <concrt.h>

namespace Microsoft {
namespace Xbox {
namespace Samples {
namespace NetworkMesh {

ref class MeshManager;

public enum class ConnectionStatus
{
    Disconnected,
    Pending,
    Connected,
    PostHandshake
};

public ref class MeshConnection sealed
{
internal:
#ifdef _XBOX_ONE
    MeshConnection(Windows::Xbox::Networking::SecureDeviceAddress^ secureDeviceAddress, MeshManager^ manager);
#else
    MeshConnection(Windows::Networking::XboxLive::XboxLiveDeviceAddress^ secureDeviceAddress, MeshManager^ manager);
#endif

public:
    uint8 GetConsoleId();
    void SetConsoleId(uint8 consoleId);

    Platform::String^ GetConsoleName();
    void SetConsoleName(Platform::String^ consoleName);

    Platform::Object^ GetCustomProperty();
    void SetCustomProperty(Platform::Object^ object);

    int GetNumberOfRetryAttempts();
    void SetNumberOfRetryAttempts(int retryAttempt);

    ConnectionStatus GetConnectionStatus();
    void SetConnectionStatus(ConnectionStatus status);

    bool GetAssocationFoundInTemplate();
    void SetAssocationFoundInTemplate(bool bFound);
    
#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAssociation^ GetAssociation();
    void SetAssociation(Windows::Xbox::Networking::SecureDeviceAssociation^ association);
#else
    Windows::Networking::XboxLive::XboxLiveEndpointPair^ GetAssociation();
    void SetAssociation(Windows::Networking::XboxLive::XboxLiveEndpointPair^ association);
#endif

    bool IsInComingAssociation();
    void SetInComingAssociation(bool val);

    bool IsConnectionDestroying();
    void SetConnectionDestroying(bool val);

    bool IsConnectionInProgress();
    void SetConnectionInProgress(bool val);

    void SetHeartTimer( float timer );
    float GetHeartTimer();

#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAddress^ MeshConnection::GetSecureDeviceAddress();
#else
    Windows::Networking::XboxLive::XboxLiveDeviceAddress^ MeshConnection::GetSecureDeviceAddress();
#endif

   UserMeshConnectionPropertyBag^ GetUserPropertyBag(Platform::String^ xboxUserId);

   UserMeshConnectionPropertyBag^ AddUserPropertyBag(Platform::String^ xboxUserId);

private:
    Concurrency::critical_section m_stateLock;

#ifdef _XBOX_ONE
    Windows::Xbox::Networking::SecureDeviceAddress^ m_secureDeviceAddress;
    Windows::Xbox::Networking::SecureDeviceAssociation^ m_association;
#else
    Windows::Networking::XboxLive::XboxLiveDeviceAddress^ m_secureDeviceAddress;
    Windows::Networking::XboxLive::XboxLiveEndpointPair^ m_association;
#endif

    Windows::Foundation::EventRegistrationToken m_associationStateChangeToken;

    Platform::WeakReference m_meshManager; // weak ref to MeshManager^ 
    std::map<Platform::String^, UserMeshConnectionPropertyBag^> m_userIdsToUserData;

    uint8 m_consoleId;
    Platform::String^ m_consoleName;
    Platform::Object^ m_customProperty;
    ConnectionStatus m_connectionStatus;
    bool m_isInComingAssociation;
    bool m_isConnectionInProgress;
    bool m_isConnectionDestroying;
    bool m_assocationFoundInTemplate;
    int m_retryAttempts;
    float m_timerSinceLastAttempt;
    float m_heartTimer;

#ifdef _XBOX_ONE
    void HandleAssociationChangedEvent(
        Windows::Xbox::Networking::SecureDeviceAssociation^ association, 
        Windows::Xbox::Networking::SecureDeviceAssociationStateChangedEventArgs^ args
        );
#else
    void HandleAssociationChangedEvent(
        Windows::Networking::XboxLive::XboxLiveEndpointPair^ association,
        Windows::Networking::XboxLive::XboxLiveEndpointPairStateChangedEventArgs^ args
        );
#endif
};

}}}}

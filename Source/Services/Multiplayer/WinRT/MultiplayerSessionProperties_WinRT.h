// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/multiplayer.h"
#include "MultiplayerSessionRestriction_WinRT.h"
#include "MultiplayerSessionMember_WinRT.h"
#include "MatchmakingStatus_WinRT.h"    
#include "MultiplayerSessionReference_WinRT.h"
#include "Utils_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents multiplayer session properties.
/// </summary>
/// <example>
/// The following example shows the format of the JSON object that represents
/// this class:
/// <code>
///"properties": {
///        "system": {
///            "keywords": [
///                "hello"
///            ],
///            "turn": [
///                0
///            ],
///            "host": "99e4c701",
///            "initializationSucceeded": true,
///            "joinRestriction": "None",
///            "readRestriction": "None",
///            "serverConnectionStringCandidates": [
///                "west.azure.com",
///                "east.azure.com"
///            ],
///            "matchmaking": {
///                "clientResult": {
///                    "status": "Searching",
///                    "statusDetails": "Description",
///                    "typicalWait": 30,
///                    "targetSessionRef": {
///                        "scid": "1ECFDB89-36EB-4E59-8901-11F7393689AE",
///                        "templateName": "capture-the-flag",
///                        "name": "2D58F65F-0E3C-4F1F-8277-2BC9873FDB23"
///                    }
///                },
///                "targetSessionConstants": {},
///                "serverConnectionString": "west.azure.com"
///            },
///            "matchmakingResubmit": true
///        },
///        "custom": {}
///    },
/// </code>
/// </example>
public ref class MultiplayerSessionProperties sealed
{
public:
    /// <summary>
    /// (Optional) A collection of case-insensitive keywords associated with the session. This value can be empty.
    /// When changing keywords, call MultiplayerService::WriteSessionAsync to write the changes to the service.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Keywords
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
        void set(_In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ value);
    }

    /// <summary>
    /// Gets or sets the restriction that is applied to members when joining an open session.
    /// Defaults to "none".
    /// </summary>
    /// <remarks>
    /// The join restriction has no effect on reservations, which means that it has no impact on "private" and "visible" sessions.
    ///
    /// If "local", only users whose token's DeviceId matches someone else already in the session and "active": true.
    /// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can join without a reservation.
    ///
    /// The join restriction must be at least as closed as the ReadRestriction restriction.
    /// For example, ReadRestriction cannot be set to MultiplayerSessionReadRestriction.Local without also setting
    /// JoinRestriction to MultiplayerSessionJoinRestriction.Local. 
    /// </remarks>
    DEFINE_PTR_PROP_GETSET_ENUM_OBJ(JoinRestriction, join_restriction, Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionRestriction, xbox::services::multiplayer::multiplayer_session_restriction);
    
    /// <summary>
    /// Gets or sets the restriction that is applied to members when reading an open session. 
    /// Defaults to "none".
    /// </summary>
    /// <remarks>
    /// The read restriction has no effect on reservations, which means that it has no impact on "private" and "visible" sessions.
    ///
    /// If "local", only users whose token's DeviceId matches someone else already in the session and "active": true.
    /// If "followed", only local users (as defined above) and users who are followed by an existing (not reserved) member of the session can read without a reservation.
    ///
    /// The read restriction must be at least as open as the JoinRestriction restriction.
    /// For example, JoinRestriction cannot be set to MultiplayerSessionJoinRestriction.Followed without also setting
    /// ReadRestriction to MultiplayerSessionReadRestriction.Followed. 
    /// </remarks>
    DEFINE_PTR_PROP_GETSET_ENUM_OBJ(ReadRestriction, read_restriction, Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionRestriction, xbox::services::multiplayer::multiplayer_session_restriction);

    /// <summary>
    /// A collection of MultiplayerSessionMember objects indicating who has played turns in the sessions.
    /// When changing, call MultiplayerService::WriteSessionAsync to write the changes to the service.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<MultiplayerSessionMember^>^ TurnCollection
    {
        Windows::Foundation::Collections::IVectorView<MultiplayerSessionMember^>^ get();
        void set(_In_  Windows::Foundation::Collections::IVectorView<MultiplayerSessionMember^>^ value);
    }

    /// <summary>
    /// A JSON string of title-defined constants for the target session to use in matchmaking. 
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(MatchmakingTargetSessionConstantsJson, matchmaking_target_session_constants_json);

    /// <summary>
    /// A JSON string that specifies the custom properties for the session.  These can be changed anytime.
    /// When changing, call MultiplayerService::WriteSessionAsync to write the changes to the service.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(SessionCustomPropertiesJson, session_custom_properties_json);

    /// <summary>
    /// Force a specific connection string to be used, which is useful for join in progress scenarios.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(MatchmakingServerConnectionString, matchmaking_server_connection_string);

    /// <summary>
    /// The ordered list of connection strings that the session can use to connect to a game server. Generally titles should use the first on
    /// the list, but sophisticated titles could use a custom mechanism for choosing one of the others (e.g. based on load).
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ ServerConnectionStringCandidates
    {
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

    /// <summary>
    /// Member index of owners of the session.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<uint32>^ SessionOwnerIndices
    {
        Windows::Foundation::Collections::IVectorView<uint32>^ get();
    }

    /// <summary>
    /// A token representing the host device. 
    /// Must match the "deviceToken" of at least one member, otherwise this field is deleted.
    /// If "peerToHostRequirements" is set and "host" is set, the measurement stage assumes that 
    /// the given host is the correct host and only measures metrics to that host.
    /// </summary>        
    DEFINE_PTR_PROP_GET_STR_OBJ(HostDeviceToken, host_device_token);

    /// <summary>
    /// A value that indicates if the session is closed. 
    /// Thsi value controls whether a session is joinable, independent of visibility, joinrestriction, and available space in the session.
    /// Does not affect reservations.  Defaults to false.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(Closed, closed, bool);

    /// <summary>
    /// If true, it would allow the members of the session to be locked, such that if a user leaves they are able to 
    /// come back into the session but no other user could take that spot. Defaults to false.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(Locked, locked, bool);

    /// <summary>
    /// A value that indicates if there is an outstanding request to allocate.
    /// Set to true to indicate that the client would like an allocation. Set to false to cancel the allocation request.
    /// MPSD will remove this property entirely if the allocation has failed.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(AllocateCloudCompute, allocate_cloud_compute, bool);

internal:
    MultiplayerSessionProperties(
        std::shared_ptr<xbox::services::multiplayer::multiplayer_session_properties> cppObj
        );

private:
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session_properties> m_cppObj;
    Windows::Foundation::Collections::IVectorView<MultiplayerSessionMember^>^ m_turnCollection;
    Windows::Foundation::Collections::IVectorView<uint32>^ m_sessionOwnerIndices;
    MultiplayerSessionReference^ m_matchmakingTargetSessionReference;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/multiplayer.h"
#include "MultiplayerPeerToPeerRequirements_WinRT.h"
#include "MultiplayerPeerToHostRequirements_WinRT.h"
#include "MultiplayerManagedInitialization_WinRT.h"
#include "MultiplayerMemberInitialization_WinRT.h"
#include "MultiplayerSessionVisibility_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Represents constant values for a multiplayer session.
/// </summary>
/// <example>
/// The following example shows the format of the JSON object that represents
/// this class:
/// <code>
/// "constants": {
///     "system": {
///         "capabilities": {
///             "clientMatchmaking": true,
///             "connectivity": true,
///             "suppressPresenceActivityCheck": true,
///             "gameplay": true,
///             "large": false
///         },
///         "version": 1,
///         "maxMembersCount": 100,
///         "visibility": "Open",
///         "initiators": [
///             "3456"
///         ],
///         "inviteProtocol": "party",
///         "reservedRemovalTimeout": 30000,
///         "inactiveRemovalTimeout": 7200000,
///         "readyRemovalTimeout": 180000,
///         "sessionEmptyTimeout": 0,
///         "metrics": {
///             "latency": true,
///             "bandwidthDown": true,
///             "bandwidthUp": true,
///             "custom": true
///         },
///         "memberInitialization": {
///             "joinTimeout": 4000,
///             "measurementTimeout": 5000,
///             "evaluationTimeout": 5000,
///             "externalEvaluation": false,
///             "membersNeededToStart": 2
///         },
///         "peerToPeerRequirements": {
///             "latencyMaximum": 250,
///             "bandwidthMinimum": 10000
///         },
///         "peerToHostRequirements": {
///             "latencyMaximum": 250,
///             "bandwidthDownMinimum": 100000,
///             "bandwidthUpMinimum": 1000,
///             "hostSelectionMetric": "bandwidthUp"
///         },
///         "measurementServerAddresses": {
///             "east.azure.com": {
///                 "secureDeviceAddress": "r5Y="
///             },
///             "west.azure.com": {
///                 "secureDeviceAddress": "rwY="
///             }
///         }
///     },
///     "custom": {}
/// }
/// </code>
/// </example>

public ref class MultiplayerSessionConstants sealed
{
public:

    /// <summary>
    /// The maximum number of members in this session.
    /// </summary>
    /// <remarks>
    /// To accommodate a maximum number of members that is over 100, the session must be considered large
    /// and have the MultiplayerSessionConstants.CapabilitiesLarge Property set to true.
    /// </remarks>
    DEFINE_PTR_PROP_GETSET_OBJ(MaxMembersInSession, max_members_in_session, uint32);

    /// <summary>
    /// The visibility of this session.
    /// </summary>
    DEFINE_PTR_PROP_GETSET_ENUM_OBJ(MultiplayerSessionVisibility, visibility, Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionVisibility, xbox::services::multiplayer::multiplayer_session_visibility);

    /// <summary>
    /// A collection of Xbox User IDs indicating who initiated the session (Optional).
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ InitiatorXboxUserIds { Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); }

    /// <summary>
    /// A JSON string that specifies the custom constants for the session.  These can not be changed after the session is created (Optional).
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(CustomConstantsJson, session_custom_constants_json);

    /// <summary>
    /// The period of time, in milliseconds, that the session holds a member reservation, after which the member is removed.
    /// The default value is 10 seconds. A value of 0 is allowed and indicates an immediate timeout.
    /// If the timeout is not specified, it is considered infinite.
    /// </summary>
    DEFINE_PTR_PROP_GET_TIMESPAN_OBJ(MemberReservationTimeout, member_reserved_time_out);

    /// <summary>
    /// The period of time, in milliseconds, that the session waits for an inactive member reservation
    /// to become active, after which the session removes the member.
    /// The default value is 2 hours. A value of 0 is allowed and indicates an immediate timeout.
    /// If the timeout is not specified, it is considered infinite.
    /// </summary>
    DEFINE_PTR_PROP_GET_TIMESPAN_OBJ(MemberInactiveTimeout, member_inactive_timeout);

    /// <summary>
    /// The period of time, in milliseconds, that the session waits for a member marked as ready by the
    /// shell to be set as active. 
    /// When the shell launches the title to start a multiplayer game, the member is marked as ready.
    /// The default value is 1 minute. A value of 0 is allowed and indicates an immediate timeout.
    /// If the timeout is not specified, it is considered infinite.
    /// </summary>
    DEFINE_PTR_PROP_GET_TIMESPAN_OBJ(MemberReadyTimeout, member_ready_timeout);

    /// <summary>
    /// The period of time, in milliseconds, that the session is empty before the session is deleted.
    /// If the session is empty for this timeout, then the session is deleted.
    /// The default value is 0, which indicates an immediate timeout.
    /// If the timeout is not specified, it is considered infinite.
    /// </summary>
    DEFINE_PTR_PROP_GET_TIMESPAN_OBJ(SessionEmptyTimeout, session_empty_timeout);

    /// <summary>
    /// Delta from start time representing the point at which results are finalized. 
    /// If no one has reported (client or server) at this point, we declare the match results incomplete.
    /// </summary>
    DEFINE_PTR_PROP_GET_TIMESPAN_OBJ(ArbitrationTimeout, arbitration_timeout);

    /// <summary>
    /// Delta from start time representing the point at which, if the session has no active users, the match is canceled.
    /// </summary>
    DEFINE_PTR_PROP_GET_TIMESPAN_OBJ(ForfeitTimeout, forfeit_timeout);

    /// <summary>
    /// A value that indicates if the session has connectivity capabilities.
    /// True if the session has connectivity capabilities, and false otherwise.
    /// If false, the session can't enable any metrics and the session members can not set their SecureDeviceAddress.  
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(CapabilitiesConnectivity, capabilities_connectivity, bool);

    /// <summary>
    /// A value that indicates if active users of the session are demoted to inactive status. 
    /// By default (if false), active users are required to remain online playing the title, otherwise they get demoted to 
    /// inactive status. Setting this flag to true disables this check so that members stay active indefinitely.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(CapabilitiesSuppressPresenceActivityCheck, capabilities_suppress_presence_activity_check, bool);

    /// <summary>
    /// A value that indicates whether the session represents actual gameplay, as opposed to setup/menu time
    /// such as a lobby or matchmaking.
    /// If true, then the session is in gameplay mode.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(CapabilitiesGameplay, capabilities_gameplay, bool);

    /// <summary>
    /// A value that indicates if the session can host a large number of users, which has impact on other session properties.
    /// If true, this session can host a large number of users.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(CapabilitiesLarge, capabilities_large, bool);
    
    /// <summary>
    /// A Boolean value that indicates whether a connection is required for active members in a multiplayer session.
    /// If true, this connection is required to have a member be active.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(CapabilitiesConnectionRequiredForActiveMember, capabilities_connection_required_for_active_member, bool);

    /// <summary>
    /// A Boolean value that indicates whether a multiplayer session supports crossplay between Xbox One and Windows 10. 
    /// True if the multiplayer session supports crossplay.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(CapabilitiesCrossplay, capabilities_crossplay, bool);

    /// <summary>
    /// Aa Boolean value that indicates whether a multiplayer session uses user authorization style, which indicates
    /// that the session supports calls from platforms without strong title identity. This capability can't be set on large sessions.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(CapabilitiesUserAuthorizationStyle, capabilities_user_authorization_style, bool);

    /// <summary>
    /// True if team capability is set on the session for a tournament.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(CapabilitiesTeam, capabilities_team, bool);

    /// <summary>
    /// True, if the session can be linked to a search handle for searching.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(CapabilitiesSearchable, capabilities_searchable, bool);

    /// <summary>
    /// True if arbitration capability is set on the session for a tournament.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(CapabilitiesArbitration, capabilities_arbitration, bool);

    /// <summary>
    /// Indicates if the title wants latency measured for determining connectivity.
    /// Requires CapabilitiesConnectivity capability.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(EnableMetricsLatency, enable_metrics_latency, bool);

    /// <summary>
    /// Indicates if the title wants bandwidth down (host to session members) measured for determining connectivity.
    /// Requires CapabilitiesConnectivity capability.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(EnableMetricsBandwidthDown, enable_metrics_bandwidth_down, bool);

    /// <summary>
    /// Indicates if the title wants bandwidth up (session members to host) measured for determining connectivity.
    /// Requires CapabilitiesConnectivity capability.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(EnableMetricsBandwidthUp, enable_metrics_bandwidth_up, bool);

    /// <summary>
    /// Indicates if the title wants a custom measurement measured for determining connectivity.
    /// Requires CapabilitiesConnectivity capability.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(EnableMetricsCustom, enable_metrics_custom, bool);

#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Call MemberInitialization instead", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    /// <summary>
    /// Information about managed initialization for the session.
    /// If a ManagedInitialization object is set, the session expects the client system or title to perform
    /// initialization following session creation. The timeouts and initialization stages are automatically tracked by
    /// the session, including initial QoS if any metrics are set.
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::MultiplayerManagedInitialization^ ManagedInitialization 
    { 
        Microsoft::Xbox::Services::Multiplayer::MultiplayerManagedInitialization^ get(); 
    }

    /// <summary>
    /// Information about managed initialization for the session.
    /// If a MemberInitialization object is set, the session expects the client system or title to perform
    /// initialization following session creation. The timeouts and initialization stages are automatically tracked by
    /// the session, including initial QoS if any metrics are set.
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::MultiplayerMemberInitialization^ MemberInitialization
    {
        Microsoft::Xbox::Services::Multiplayer::MultiplayerMemberInitialization^ get();
    }
             
    /// <summary>
    /// An object that contains QoS requirements for the connection between session members. 
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::MultiplayerPeerToPeerRequirements^ PeerToPeerRequirements 
    { 
        Microsoft::Xbox::Services::Multiplayer::MultiplayerPeerToPeerRequirements^ get(); 
    }
      
    /// <summary>
    /// An object that contains QoS requirements for the connection between the host and session members. 
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::MultiplayerPeerToHostRequirements^ PeerToHostRequirements 
    { 
        Microsoft::Xbox::Services::Multiplayer::MultiplayerPeerToHostRequirements^ get();
    }

    /// <summary>
    /// A JSON string specifying the servers that can be used for QoS measurements.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(MeasurementServerAddressesJson, measurement_server_addresses_json);

internal:
    MultiplayerSessionConstants(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session_constants> cppObj
        );

private:
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session_constants> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
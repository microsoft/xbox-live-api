// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "Macros_WinRT.h"
#include "MultiplayerSessionConstants_WinRT.h"
#include "MultiplayerSessionMember_WinRT.h"
#include "MultiplayerInitializationStage_WinRT.h"
#include "MultiplayerSessionProperties_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"
#include "MatchmakingStatus_WinRT.h"
#include "MultiplayerSessionCapabilities_WinRT.h"
#include "MultiplayerSessionChangeTypes.h"
#include "QualityOfServiceServer_WinRT.h"
#include "MultiplayerServiceMatchmakingServer_WinRT.h"
#include "WriteSessionStatus_WinRT.h"
#include "TournamentArbitrationStatus_WinRT.h"
#include "MultiplayerSessionTournamentsServer_WinRT.h"
#include "MultiplayerSessionArbitrationServer_WinRT.h"
#include "MultiplayerSessionRoleTypes_WinRT.h"
#include "MultiplayerRoleType_WinRT.h"
#include "TournamentTeamResult_WinRT.h"

namespace Microsoft{ namespace Xbox{ namespace Services {
    ref class XboxLiveContext;
}}}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Represents a multiplayer session. 
/// </summary>

// Response Structure 
//
//    {
//        // The contract version of this rendering of the session. A function of the contract version of the request and constants/system/version.
//        "contractVersion": 105,
//
//        // Use for tracing.
//        "correlationId": "0FE81338-EE96-46E3-A3B5-2DBBD6C41C3B",
//
//        // The time that the session began.
//        // If an empty session is deleted and then restarted, this time will be the time of the restart.
//        "startTime": "2009-06-15T13:45:30.0900000Z",
//
//        // If any timeouts are in progress, this is the time that the next one will fire.
//        "nextTimer": "2009-06-15T13:45:30.0900000Z",
//
//        // Present during managed initialization.
//        // The "stage" goes from "joining" to "measuring" to "evaluating".
//        // If episode 1 fails, then "stage" is set to "failed" and the session cannot be initialized.
//        // Otherwise, when an initialization episode completes, the "initialization" object is removed.
//        // If "externalEvaluation" is not set, "evaluating" is skipped. If "metrics" isn't set, "measuring" is skipped.
//        "initializing": 
//        {
//            "stage": "measuring",
//            "stageStartTime": "2009-06-15T13:45:30.0900000Z",
//            "episode": 0
//        },
//
//        // Set after the "measuring" stage of initialization episode 1, if "peerToHostRequirements" is set and no /properties/system/host is set.
//        // Cleared once a /properties/system/host is set.
//        // Host candidates are device tokens listed in order, order of preference.
//        "hostCandidates": [ "ab90a362", "99582e67" ],
//        
//        "constants": { /* Property Bag */ },
//        "properties": { /* Property Bag */ },
//    
//        "members": 
//        {
//            "1": 
//            {
//                "constants": { /* Property Bag */ },
//                "properties": { /* Property Bag */ },
//
//                // Only if the member has accepted. Optional (not set if no gamertag claim was found).
//                "gamertag": "stacy",
//
//                // This is set when the member uploads a secure device address. It's a case-insensitive string that can be used for equality comparisons.
//                "deviceToken": "9f4032ba7",
//
//                // This value is removed once the user does their first PUT to the session.
//                "reserved": true,
//
//                // If the member is active, this is the title in which they are active, in decimal.
//                "activeTitleId": "8397267",
//
//                // The time the user joined the session. If "reserved" is true, the time the reservation was made.
//                "joinTime": "2009-06-15T13:45:30.0900000Z",
//
//                // Present if this member is in the properties/system/turn array, otherwise not.
//                "turn": true,
//
//                // Set when transitioning out of the "joining" or "measuring" stage if this member doesn't pass.
//                // In order of precedence, could be set to "timeout", "latency", "bandwidthDown", "bandwidthUp", "network", "group", or "quorum".
//                // The "network" value means the network configuration and/or conditions (such as conflicting NAT) prevented the QoS metrics from being measured.
//                // The only possible value at the end of "joining" is "group". (On timeout from "joining", the reservation is removed.)
//                // The "quorum" value is set after a failed "evaluation" stage on all members of the initialization episode that didn't fail during "joining" or "measuring".
//                "failedMetric": "latency",
//
//                // If "memberInitialization" is set and the member was added with "initialize": true, this is set to the initialization episode that the member will participate in.
//                // Zero is a special value used for the members added to a new session at create time.
//                "initializationEpisode": 0,
//
//                // The next member's index, which is the same as 'next' below if there's no more.
//                "next": 4
//            },
//            "4": { "next": 5 /* etc */ }
//        },
//    
//        "membersInfo": 
//        {
//            "first": 1,  // The first member's index.
//            "next": 5,  // The index that the next member added will get.
//            "count": 2,  // The number of members.
//            "accepted": 1  // The number of members that are no longer 'pending'.
//        },
//    
//        "servers": 
//        {
//            "name": 
//            {
//                "constants": { /* Property Bag */ },
//                "properties": { /* Property Bag */ }
//            }
//        }
//    }
//
//
//    // Property Bags Each of the above objects marked as a property bag consists of two optional inner objects, system and custom. 
//    // The custom objects can contain any JSON. 
//
//    "custom": 
//    {
//        "myField1": true,
//        "myField2": "string",
//        "myField3": 5.5,
//        "myField4": { "myObject": null },
//        "myField5": [ "my", "array" ]
//    }
//
//    // System Objects Each of the system objects in the session document has a fixed schema that is enforced and interpreted by MPSD. 
//    // Within the body of PUT requests, the system objects are validated and merged just like the custom objects. 
//    // But unlike custom objects, once merged the system objects are further validated and acted upon based on these schemas. 
//
//    /constants/system 
//    {
//        "version": 1,
//        "maxMembersCount": 100,  // Defaults to 100 if not set on creation. Must be between 1 and 100.
//        "visibility": "private",  // Or "visible" or "open", defaults to "open" if not set on creation.
//        "initiators": [ "1234" ],  // If specified on a new session, the creator's xuid must be in the list (or the creator must be a server).
//        "inviteProtocol": "party",  // Optional URI scheme of the launch URI for invite toasts.
//
//        // Timeouts are specified in Milliseconds. Zero is allowed and means to immediately time out. If a given timeout isn't specified, it's considered infinite.
//        // Specified as a group for usability, to allow 'unspecified' to be meaningful, and because timeouts are often related.
//
//       "reservedRemovalTimeout": 10000,  // Default is 10 seconds. Member is removed from the session.
//       "inactiveRemovalTimeout": 7200000,  // Default is two hours. Member is removed from the session.
//       "readyRemovalTimeout": 60000,  // Default is one minute. Member becomes inactive.
//       "sessionEmptyTimeout": 0,  // Default is zero. Session is deleted.
//
//        // Capabilities are boolean values that are optionally set in the session template. If no capabilities
//        // are needed, an empty "capabilities" object should be in the template in order to prevent capabilities from
//        // being specified on session creation, unless the title desires dynamic session capabilities.
//        "capabilities": 
//        {
//            "clientMatchmaking": true,
//            "connectivity": true,
//            "suppressPresenceActivityCheck": false,
//            "gameplay" : true,
//            "large" : false
//        },
//
//        // Requires "connectivity" capability.
//        // If no "metrics" are specified, they default to what are needed to satisfy the QoS requirements.
//        // If they are specified, they must be sufficient to satisfy the QoS requirements.
//        "metrics": 
//        {
//            "latency": true,
//            "bandwidthDown": true,
//            "bandwidthUp": true,
//            "custom": true
//        },
//
//        // If a "memberInitialization" object is set, the session expects the client system or title to perform initialization following session creation and/or as new members join the session.
//        // The timeouts and initialization stages are automatically tracked by the session, including QoS measurements if any metrics are set.
//        "memberInitialization": 
//        {
//            "joinTimeout": 4000,  // Milliseconds. Default is 4 seconds. Overrides "reservationTimeout" for the initial members.
//            "measurementTimeout": 5000,  // Milliseconds. Default is based on which of the metrics and/or server ping addresses are set.
//            "evaluationTimeout": 5000,  // Milliseconds. Can only be set if 'autoEvaluate' is false. Default is 5 seconds.
//            "externalEvaluation": false,
//            "membersNeededToStart": 2  // Defaults to 2. Must be between 1 and maxMemberCount. Only applies to initialization episode zero.
//        },
//
//        // Thresholds to apply to each pairwise connection for all members in a session.
//        "peerToPeerRequirements": 
//        {
//            "latencyMaximum": 250,  // Milliseconds
//            "bandwidthMinimum": 10000  // Kilobits per second
//        },
//
//        // Thresholds to apply to each connection from a host candidate.
//        "peerToHostRequirements": 
//        {
//            "latencyMaximum": 250,  // Milliseconds
//            "bandwidthDownMinimum": 100000,  // Kilobits per second
//            "bandwidthUpMinimum": 1000,  // Kilobits per second
//            "hostSelectionMetric": "bandwidthUp"  // Or "bandwidthDown" or "latency". Not specified is the same as "latency".
//        },
//
//        // The set of potential server connection strings that should be evaluated.
//        "measurementServerAddresses": 
//        {
//            "east.azure.com": 
//            {
//                "secureDeviceAddress": "r5Y="  // Base-64 encoded secure-device-address
//            },
//            "west.azure.com": 
//            {
//                "secureDeviceAddress": "rwY="
//            }
//        }
//    }
//
//    /properties/system 
//    {
//        "keywords": [ "hello" ],  // Optional array of case-insensitive strings. Cannot be set if the session's visibility is "private"
//        "turn": [ 0 ],  // Array of integer member indices whose turn it is. Defaults to empty.
//
//        // Device token of the host.
//        // Must match the "deviceToken" of at least one member, otherwise this field is deleted.
//        // If "peerToHostRequirements" is set and "host" is set, the measurement stage assumes the given host is the correct host and only measures metrics to that host.
//        "host": "99e4c701",
//        
//        // Can only be set while "initialization/stage" is "evaluating". True indicates success, and false indicates failure.
//        // Once set, "initialization/stage" is immediately updated, and this field is removed.
//        "initializationSucceeded": true,
//
//        // The ordered list of connection strings that the session could use to connect to a game server. Generally titles should use the first on
//        // the list, but sophisticated titles could use a custom mechanism for choosing one of the others (e.g. based on load).
//        "serverConnectionStrings": [ "west.azure.com", "east.azure.com" ],
//
//        "matchmaking": 
//        {
//            "clientResult": 
//            {  
//                // Requires the clientMatchmaking property.
//                "status": "searching",  // Or "expired", "found", "failed", or "canceled". 
//                "statusDetails": "Description",  // Default is empty string.
//                "typicalWait": 30,  // The expected number of seconds waiting as a non-negative integer.
//                "targetSessionRef": 
//                {
//                    "scid": "1ECFDB89-36EB-4E59-8901-11F7393689AE",
//                    "templateName": "capture-the-flag",
//                    "name": "2D58F65F-0E3C-4F1F-8277-2BC9873FDB23"
//                }
//            },
//
//            // Default is empty object.
//            "targetSessionConstants": { },
//
//            // Force a specific connection string to be used (useful in preserveSession=always cases).
//            "serverConnectionString": "west.azure.com"
//        },
//        // True if the match that was found didn't work out and needs to be resubmitted. Set to false
//        // to signal that the match did work, and the matchmaking service can release the session.
//        "matchmakingResubmit": true
//    }
//
//
//    /members/{index}/constants/system 
//    {
//        "xuid": "12345678",
//
//        // Run initialization for this user. Defaults to false.
//        // Ignored if no "memberInitialization" section is set for the session.
//        "initialize": true,
//
//        // When match adds a user to a session, it can provide some context around how and why they were matched into the session.
//        "matchmakingResult": 
//        {
//
//            // This is a copy of the user's serverMeasurements from the matchmaking session.
//            "serverMeasurements": 
//            {
//                "east.azure.com": 
//                {
//                    "latency": 233  // Milliseconds
//                }
//            }
//        }
//    }
//
//
//    /members/{index}/properties/system 
//    {
//        // These flags control the member status and "activeTitle", and are mutually exclusive (it's an error to set both to true).
//        // For each, false is the same as not present. The default status is "inactive", i.e. neither present.
//        "ready": true,
//        "active": false,
//
//        // Base64 blob, or not present. Empty-string is the same as not present.
//        "secureDeviceAddress": "ryY=",
//
//        // List of members in my group, by index.
//        // If a "initializationGroup" list is set, the member's own index will always be added if it isn't already present.
//        // During managed initialization, if any members in the list fail, this member will also fail.
//        "initializationGroup": [ 5 ],
//
//        // QoS measurements by lower-case device token.
//        // Like all fields, "measurements" must be updated as a whole. It should be set once when measurement is complete, not incrementally.
//        // Metrics can me omitted if they weren't successfully measured, i.e. the peer is unreachable.
//        // If a "measurements" object is set, it can't contain an entry for the member's own address.
//        "measurements": 
//        {
//            "e69c43a8":
//            {
//                "latency": 5953,  // Milliseconds
//                "bandwidthDown": 19342,  // Kilobits per second
//                "bandwidthUp": 944,  // Kilobits per second
//                "custom": { }
//            }
//        },
//
//        // QoS measurements by game-server connection string. Like all fields, "serverMeasurements" must be updated as a whole, so it should be set once when measurement is complete.
//        // If empty, it means that none of the measurements completed within the "serverMeasurementTimeout".
//        "serverMeasurements": 
//        {
//            "east.azure.com": 
//            {
//                "latency": 233  // Milliseconds
//            }
//        }
//    }
//
//    /servers/{server-name}/constants/system 
//    {
//    }
//
//    /servers/{server-name}/properties/system 
//    {
//        "lockId": "opaque56789",  // If set, a matchmaking service is servicing this session.
//        "status": "searching",  // Or "expired", "found", "failed", or "canceled". Optional.
//        "statusDetails": "Description",  // Optional free-form text. Default is empty string.
//        "typicalWait": 30,  // Optional. The expected number of seconds waiting as a non-negative integer.
//        "targetSessionRef": 
//        {  
//            // Optional.
//            "scid": "1ECFDB89-36EB-4E59-8901-11F7393689AE",
//            "templateName": "capture-the-flag",
//            "name": "2D58F65F-0E3C-4F1F-8277-2BC9873FDB23"
//        }
//    }

public ref class MultiplayerSession sealed
{
public:
    /// <summary>
    /// Creates a new MultiplayerSession Class object from the supplied Xbox Live context, without any constants or session reference. 
    /// </summary>
    /// <param name="xboxLiveContext">The XboxLiveContext containing the user who is creating this session.</param>
    /// <remarks>
    /// This override is intended to be used when the session serviceconfigid, template, and name are not known.  A MultiplayerSession created
    /// using this constructor must be retrieved or written to by using the "ByHandle" overrides.  
    /// (e.g. WriteSessionByHandleAsync and GetCurrentSessionByHandleAsync)
    ///
    /// After calling this constructor, the client must use the MultiplayerService::WriteSessionAsync method to write batched local changes to the service. 
    /// If this constructor is called without calling MultiplayerService::WriteSessionAsync, it only changes the local session 
    /// object but does not commit it to the service.
    ///
    /// This constructor is intended for use with template contract 104/105 or later.
    /// </remarks>
    MultiplayerSession(
        _In_ XboxLiveContext^ xboxLiveContext
        );

#if TV_API
    /// <summary>
    /// Creates a new MultiplayerSession using the specified session constants.
    /// </summary>
    /// <param name="xboxLiveContext">The XboxLiveContext containing the user who is creating this session.</param>
    /// <param name="multiplayerSessionReference">A reference that uniquely identifies the session.</param>
    /// <param name="maxMembersInSession">The maximum number of members in this session. This value can only be set if the maximum
    /// is not specified in the title's multiplayer session template. If the maximum is specified in the title's multiplayer
    /// session template, then set to 0 to ignore this parameter.</param>
    /// <param name="reserved">DEPRECATED. This param is ignored</param>
    /// <param name="multiplayerSessionVisibility">An enumeration value indicating the visibility of this session.</param>
    /// <param name="initiatorXboxUserIds">(Optional) A collection of Xbox User IDs indicating who initiated the session.</param>
    /// <param name="sessionCustomConstantsJson">(Optional) A JSON string that specify the custom constants for the session.
    /// These constants cannot be changed after the session is created.</param>
    /// <remarks>
    /// After calling this constructor, the client must use the MultiplayerService::WriteSessionAsync method to write batched local changes to the service. 
    /// If this constructor is called without calling MultiplayerService::WriteSessionAsync, it only changes the local session 
    /// </remarks>
    MultiplayerSession(
        _In_ XboxLiveContext^ xboxLiveContext,
        _In_ MultiplayerSessionReference^ multiplayerSessionReference,
        _In_ uint32 maxMembersInSession,
        _In_ bool reserved,
        _In_ MultiplayerSessionVisibility multiplayerSessionVisibility,
        _In_opt_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ initiatorXboxUserIds,
        _In_opt_ Platform::String^ sessionCustomConstantsJson
        );
#endif

    /// <summary>
    /// Creates a new MultiplayerSession using the specified session constants.
    /// </summary>
    /// <param name="xboxLiveContext">The XboxLiveContext containing the user who is creating this session.</param>
    /// <param name="multiplayerSessionReference">A reference that uniquely identifies the session.</param>
    /// <param name="maxMembersInSession">The maximum number of members in this session. This value can only be set if the maximum
    /// is not specified in the title's multiplayer session template. If the maximum is specified in the title's multiplayer
    /// session template, then set to 0 to ignore this parameter.</param>
    /// <param name="multiplayerSessionVisibility">An enumeration value indicating the visibility of this session.</param>
    /// <param name="initiatorXboxUserIds">(Optional) A collection of Xbox User IDs indicating who initiated the session.</param>
    /// <param name="sessionCustomConstantsJson">(Optional) A JSON string that specify the custom constants for the session.
    /// These constants cannot be changed after the session is created.</param>
    /// <remarks>
    /// After calling this constructor, the client must use the MultiplayerService::WriteSessionAsync method to write batched local changes to the service. 
    /// If this constructor is called without calling MultiplayerService::WriteSessionAsync, it only changes the local session 
    /// </remarks>
    MultiplayerSession(
        _In_ XboxLiveContext^ xboxLiveContext,
        _In_ MultiplayerSessionReference^ multiplayerSessionReference,
        _In_ uint32 maxMembersInSession,
        _In_ MultiplayerSessionVisibility multiplayerSessionVisibility,
        _In_opt_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ initiatorXboxUserIds,
        _In_opt_ Platform::String^ sessionCustomConstantsJson
        );

    /// <summary>
    /// Creates a new MultiplayerSession without any constants, which allows the request to simply use whatever constants
    /// are already specified in the session template on the service.  Those constants are returned in the response session data.
    /// </summary>
    /// <param name="xboxLiveContext">The XboxLiveContext containing the user who is creating this session.</param>
    /// <param name="multiplayerSessionReference">A reference that uniquely identifies the session.</param>
    /// <remarks>
    /// After calling this constructor, the client must use the MultiplayerService::WriteSessionAsync method to write batched local changes to the service. 
    /// If this constructor is called without calling MultiplayerService::WriteSessionAsync, it only changes the local session 
    /// </remarks>
    MultiplayerSession(
        _In_ XboxLiveContext^ xboxLiveContext,
        _In_ MultiplayerSessionReference^ multiplayerSessionReference
        );

    /// <summary>
    /// A unique ID to the session used to query trace logs for entries that relate to the session.  
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(MultiplayerCorrelationId, multiplayer_correlation_id);

    /// <summary>
    /// A unique search handle ID to the session.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(SearchHandleId, search_handle_id);

    /// <summary>
    /// Arbitration Status of a tournament
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(ArbitrationStatus, arbitration_status, Microsoft::Xbox::Services::Tournaments::TournamentArbitrationStatus);

    /// <summary>
    /// The date and time that the session began.
    /// </summary>
    DEFINE_PTR_PROP_GET_DATETIME_OBJ(StartTime, start_time);

    /// <summary>
    /// If any timeouts are in progress, this is the date and time when the the next timer fires.
    /// </summary>
    DEFINE_PTR_PROP_GET_DATETIME_OBJ(DateOfNextTimer, date_of_next_timer);

    /// <summary>
    /// The date and time when the server returned the session.
    /// </summary>
    DEFINE_PTR_PROP_GET_DATETIME_OBJ(DateOfSession, date_of_session);

    /// <summary>
    /// Indicates the managed initialization stage, if the session is using managed initialization.
    /// The "stage" goes from "joining" to "measuring" to "evaluating".
    /// If episode 1 fails, then "stage" is set to "failed" and the session cannot be initialized.
    /// Otherwise, when an initialization episode completes, the "initialization" object is removed.
    /// If "externalEvaluation" is not set, "evaluating" is skipped. If "metrics" isn't set, "measuring" is skipped.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(InitializationStage, initialization_stage, MultiplayerInitializationStage);

    /// <summary>
    /// The date and time when the initialization stage started during managed initialization.
    /// </summary>
    DEFINE_PTR_PROP_GET_DATETIME_OBJ(InitializingStageStartTime, initializing_stage_start_time);
    
    /// <summary>
    /// The episode number associated with a set of session members requiring QoS metrics to be applied
    /// during managed initialization.
    /// </summary>
    /// <remarks>
    /// If MemberInitialization is set and Initialize is true on the member, then the member is assigned to an InitializingEpisode.  
    /// An episode is a set of users that need to have QoS metrics applied to them.
    /// This value is only useful when manually managing QoS.
    /// This value is 0 when the InitializingEpisode is not set.
    /// </remarks>
    DEFINE_PTR_PROP_GET_OBJ(InitializingEpisode, intializing_episode, uint32);

    /// <summary>
    /// The status of a write operation when writing a session.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(WriteStatus, write_status, WriteSessionStatus);

    /// <summary>
    /// An ordered list of device tokens for host names that can be used for the session,
    /// ordered by the preference specified by MultiplayerSessionConstants::PeerToHostRequirements::HostSelectionMetric.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ HostCandidates
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

    /// <summary>
    /// The unique identifying information for the session.
    /// </summary>
    property MultiplayerSessionReference^ SessionReference { MultiplayerSessionReference^ get(); }

    /// <summary>
    /// A set of constants associated with this session.  These can only be set when creating the session.
    /// </summary>
    property MultiplayerSessionConstants^ SessionConstants { MultiplayerSessionConstants^ get(); }

    /// <summary>
    /// A set of properties associated with this session.  Any player can modify these.
    /// </summary>
    property MultiplayerSessionProperties^ SessionProperties { MultiplayerSessionProperties^ get(); }

    /// <summary>
    /// A set of role types associated with this session.
    /// </summary>
    property MultiplayerSessionRoleTypes^ SessionRoleTypes { MultiplayerSessionRoleTypes^ get(); }

    /// <summary>
    /// A list of all session members, including those who have been accepted and those who are waiting to join the session.
    /// </summary>
    /// <remarks>
    /// Call MultiplayerSession::Join or MultiplayerSession::Leave to add or remove yourself from this list.  
    /// Call MultiplayerSession::AddMemberReservation to add a reservation for another user on this list.
    /// Call MultiplayerService::WriteSessionAsync to write these changes to the service.
    /// </remarks>
    property Windows::Foundation::Collections::IVectorView<MultiplayerSessionMember^>^ Members { Windows::Foundation::Collections::IVectorView<MultiplayerSessionMember^>^ get(); }

    /// <summary>
    /// A multiplayer session server that contains properties associated with a target session reference.
    /// </summary>
    property MultiplayerSessionMatchmakingServer^ MatchmakingServer{ MultiplayerSessionMatchmakingServer^ get(); }

    /// <summary>
    /// A multiplayer tournaments server that contains properties associated with a tournament reference.
    /// </summary>
    property MultiplayerSessionTournamentsServer^ TournamentsServer { MultiplayerSessionTournamentsServer^ get(); }

    /// <summary>
    /// An arbitration server that contains properties associated with a tournament games results.
    /// </summary>
    property MultiplayerSessionArbitrationServer^ ArbitrationServer { MultiplayerSessionArbitrationServer^ get(); }

    /// <summary>
    // The number of members that have accepted and were added to the session. These member joins are no longer pending.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(MembersAccepted, members_accepted, uint32);

    /// <summary>
    /// A JSON string containing a collection of servers for this multiplayer session.
    /// </summary>
    /// <remarks>
    /// The most common server involved in the session is used when the session is submitted to matchmaking using the
    /// MatchmakingService.CreateMatchTicketAsync Method. Once the title has made this call, the JSON object for ServersJson
    /// has one server object child, named "matchmaking". The server object contains information about the matchmaking for the session.
    ///
    /// After setting ServersJson, the title must call MultiplayerService.WriteSessionAsync to write batched local changes to the service.
    /// If the method is called without calling WriteSessionAsync, the method only changes the local session object but does not commit it to the server. 
    /// </remarks>
    DEFINE_PTR_PROP_GETSET_STR_FROM_JSON_OBJ(ServersJson, servers_json);

    /// <summary>
    /// The ETag returned with this session, used to determine if the session has been updated since it was last retrieved from the server.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(ETag, e_tag);

    /// <summary>
    /// Returns an OR'd set of MultiplayerSessionChangeTypes values representing the aspects of 
    /// the session that the current xboxlivecontext is subscribed to, of None if there is none.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(SubscribedChangeTypes, subscribed_change_types, MultiplayerSessionChangeTypes);

    /// <summary>
    /// The current user in the session.  A nullptr is returned if there is no current user in the session.
    /// </summary>
    property MultiplayerSessionMember^ CurrentUser { MultiplayerSessionMember^ get(); }
   
    /// <summary>
    /// The branch of the session, used to scope change numbers. 
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(Branch, branch);

    /// <summary>
    /// The change number of the session.
    /// </summary>
    /// <remarks>
    /// Branch indicates if two change numbers are from the same series and therefore can be compared. If a session is deleted and
    /// then recreated with the same session name, it receives a different branch. The branch also changes if the session is
    /// reverted due to a service issue that forced a restore from backup or other data synchronization operation. 
    /// </remarks>
    DEFINE_PTR_PROP_GET_OBJ(ChangeNumber, change_number, uint64);

    /// <summary>
    /// Adds a new member reservation on the session for the specified Xbox user ID and member constants.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the member to add a reservation for.</param>
    /// <param name="memberCustomConstantsJson">(Optional) A JSON string with custom constants to set for the session member. This is the only time the member's constants can be set.</param>
    /// <remarks>
    /// After calling AddMemberReservation, the Xbox system or title must call MultiplayerService.WriteSessionAsync
    /// to write batched local changes to the service. If AddMemberReservation is called without calling WriteSessionAsync,
    /// it only changes the local session object but does not commit it to the service. 
    /// </remarks>
    void AddMemberReservation(
        _In_ Platform::String^ xboxUserId,
        _In_opt_ Platform::String^ memberCustomConstantsJson
        );

    /// <summary>
    /// Adds a new member reservation on the session for the specified Xbox user ID, member constants, and initialization request.
    /// </summary>
    /// <param name="xboxUserId">The Xbox User ID of the member to add a reservation for.</param>
    /// <param name="memberCustomConstantsJson">(Optional) A JSON string with custom constants to set for the session member. This is the only time the member's constants can be set.</param>
    /// <param name="initializeRequested">True if the system should perform managed initialization, and false otherwise.</param>
    /// <remarks>
    /// After calling AddMemberReservation, the system or title must call MultiplayerService.WriteSessionAsync
    /// to write batched local changes to the service. If AddMemberReservation is called without calling WriteSessionAsync,
    /// it only changes the local session object but does not commit it to the service. 
    /// </remarks>
    void AddMemberReservation(
        _In_ Platform::String^ xboxUserId,
        _In_opt_ Platform::String^ memberCustomConstantsJson,
        _In_ bool initializeRequested
        );

    /// <summary>
    /// Joins the user that either created or got the session to the session.  
    /// It does not set an initialization request and does not set specified member constants
    /// </summary>
    /// <returns>The MultiplayerSessionMember object that represents the user.</returns>
    /// <remarks>
    /// After calling Join, the system or title must call MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service. If Join is called without calling WriteSessionAsync, it only changes the local session object but does not
    /// commit it to the service. 
    /// </remarks>
    MultiplayerSessionMember^ Join();

    /// <summary>
    /// Joins the user that either created or got the session to the session, using the specified member constants.
    /// It does not set an initialization request 
    /// </summary>
    /// <param name="memberCustomConstantsJson">(Optional) A JSON string with custom constants for the new session member. This is the only time the member's constants can be set.</param>
    /// <returns>The MultiplayerSessionMember object that represents the user.</returns>
    /// <remarks>
    /// After calling Join, the Xbox system or title must call MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service. If Join is called without calling WriteSessionAsync, it only changes the local session object but does not
    /// commit it to the service. 
    /// </remarks>
    MultiplayerSessionMember^ Join(
        _In_opt_ Platform::String^ memberCustomConstantsJson
        );

    /// <summary>
    /// Joins the user that either created or got the session to the session, using the specified member constants and an initialization request.
    /// </summary>
    /// <param name="memberCustomConstantsJson">(Optional) A JSON string with custom constants for the new session member. This is the only time the member's constants can be set.</param>
    /// <param name="initializeRequested">True if the caller would like to enroll in automatic member initialization, and false otherwise.  Usually used for QoS.</param>
    /// <returns>The MultiplayerSessionMember object that represents the user.</returns>
    /// <remarks>
    /// After calling Join, the system or title must call MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service. If Join is called without calling WriteSessionAsync, it only changes the local session object but does not
    /// commit it to the service. 
    /// </remarks>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Call Join(String^) or Join(String^, bool, bool) instead", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    MultiplayerSessionMember^ Join(
        _In_opt_ Platform::String^ memberCustomConstantsJson,
        _In_ bool initializeRequested
        );

    /// <summary>
    /// Joins the user that either created or got the session to the session, using the specified member constants, an initialization request,
    /// and a join condition.
    /// </summary>
    /// <param name="memberCustomConstantsJson">(Optional) A JSON string with custom constants for the new session member. This is the only time the member's constants can be set.</param>
    /// <param name="initializeRequested">True if the caller would like to enroll in automatic member initialization, and false otherwise.  Usually used for QoS.</param>
    /// <param name="joinWithActiveStatus">True if the player should join with an active status.</param>
    /// <returns>The MultiplayerSessionMember object that represents the user.</returns>
    /// <remarks>
    /// After calling Join, the system or title must call MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service. If Join is called without calling WriteSessionAsync, it only changes the local session object but does not
    /// commit it to the service. 
    /// </remarks>
    MultiplayerSessionMember^ Join(
        _In_opt_ Platform::String^ memberCustomConstantsJson,
        _In_ bool initializeRequested,
        _In_ bool joinWithActiveStatus
        );

    /// <summary>
    /// Sets the visibility of the session.
    /// </summary>
    /// <param name="visibility">An enumeration value indicating the visibility for the session.</param>
    /// <remarks>
    /// This can only be set when creating a new session.
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
    void SetVisibility(
        _In_ Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionVisibility visibility
        );

    /// <summary>
    /// Sets the maximum number of members allowed in the session.
    /// </summary>
    /// <param name="maxMembersInSession">The maximum number of members.</param>
    /// <remarks>
    /// This can only be set when creating a new session.
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
    void SetMaxMembersInSession(
        _In_ uint32 maxMembersInSession
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Sets the max member count per role. 
    /// Note: Only the session owner can modify role settings and only those that are multiplayer_role_type::mutable_role_settings()
    /// In your session template, you also need to set 'hasOwners' capability and 'ownerManaged' to true for the specific role type 
    /// that you want to modify the mutable_role_setting off.
    /// </summary>
    /// <param name="roleTypes">A map of role type names to multiplayer role type</param>
    void SetMutableRoleSettings(
        _In_ Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerRoleType^>^ roleTypes
        );

    /// <summary>
    /// Sets the timeout values for the session.
    /// </summary>
    /// <param name="memberReservedTimeout">The timeout for a member reservation, in milliseconds. A value of 0 is allowed and
    /// indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
    /// <param name="memberInactiveTimeout">The timeout for a member to be considered inactive, in milliseconds. A value of 0
    /// is allowed and indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
    /// <param name="memberReadyTimeout">The timeout for a member to be considered ready, in milliseconds. A value of 0 is
    /// allowed and indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
    /// <param name="sessionEmptyTimeout">The timeout for an empty session, in milliseconds. A value of 0 is allowed and
    /// indicates an immediate timeout. If the timeout is not specified, it is considered infinite.</param>
    /// <remarks>
    /// This can only be set when creating a new session.
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
    void SetTimeouts(
        _In_ Windows::Foundation::TimeSpan memberReservedTimeout,
        _In_ Windows::Foundation::TimeSpan memberInactiveTimeout,
        _In_ Windows::Foundation::TimeSpan memberReadyTimeout,
        _In_ Windows::Foundation::TimeSpan sessionEmptyTimeout
        );

    /// <summary>
    /// Sets the arbitration timeouts for the session.
    /// </summary>
    /// <param name="arbitrationTimeout">The timeout for arbitration, in milliseconds representing the point at which results are finalized.</param>
    /// <param name="forfeitTimeout">The timeout for forfeit, in milliseconds representing the point at which, if the session has no active users, the match is canceled.</param>
    /// <remarks>
    /// This can only be set when creating a new session.
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
    void SetArbitrationTimeouts(
        _In_ Windows::Foundation::TimeSpan arbitrationTimeout,
        _In_ Windows::Foundation::TimeSpan forfeitTimeout
        );

    /// <summary>
    /// Enables or disables the connectivity metrics used to satisfy QoS requirements on the network for the session.
    /// </summary>
    /// <param name="enableLatencyMetric">True to enable the measuring of latency, and false to disable latency measurement.</param>
    /// <param name="enableBandwidthDownMetric">True to enable the measuring of bandwidth down, and false to disable bandwidth down measurement.</param>
    /// <param name="enableBandwidthUpMetric">True to enable the measuring of bandwidth up, and false to disable bandwidth up measurement.</param>
    /// <param name="enableCustomMetric">True to enable custom metrics, and false to disable them.</param>
    /// <remarks>
    /// This can only be set when creating a new session.
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
    void SetQualityOfServiceConnectivityMetrics(
        _In_ bool enableLatencyMetric,
        _In_ bool enableBandwidthDownMetric,
        _In_ bool enableBandwidthUpMetric,
        _In_ bool enableCustomMetric
        );

    /// <summary>
    /// Configures managed initialization parameters for a session.
    /// </summary>
    /// <param name="joinTimeout">The period of time, in milliseconds, that the system waits for a member to join the session.
    /// This value overrides the value of the memberReservedTimeout parameter for the MultiplayerSession.SetTimeouts Method
    /// for the initial session members.</param>
    /// <param name="measurementTimeout">The period of time, in milliseconds, that the system waits for a measuring operation
    /// during managed initialization.
    /// Note: During matchmaking, a 45-second timeout for QoS measurements is enforced. Therefore we recommend the use of a
    /// measurement timeout that is less than or equal to 30 seconds during matchmaking. </param>
    /// <param name="evaluationTimeout">The period of time, in milliseconds, that the system waits for an evaluation.</param>
    /// <param name="autoEvalute">True if the system should auto-evaluate the session service, and false if the title performs the evaluation.</param>
    /// <param name="membersNeededToStart">The number of members needed to start the session, for initialization episode zero only.</param>
    /// <remarks>
    /// This can only be set when creating a new session.
    ///
    /// If a 'managedInitialization' object is set, the session expects the client system or title to perform 
    /// initialization following session creation and/or as new members join the session.
    ///
    /// The timeouts and initialization stages are automatically tracked by the session, including QoS 
    /// measurements if any metrics are set. These timeouts override the session's reservation and ready 
    /// timeouts for members that have 'initializationEpisode' set.
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
#if _MSC_VER >= 1800
    [Windows::Foundation::Metadata::Deprecated("Call SetMemberInitialization instead", Windows::Foundation::Metadata::DeprecationType::Deprecate, 0x0)]
#endif
    void SetManagedInitialization(
        _In_ Windows::Foundation::TimeSpan joinTimeout,
        _In_ Windows::Foundation::TimeSpan measurementTimeout,
        _In_ Windows::Foundation::TimeSpan evaluationTimeout,
        _In_ bool autoEvalute,
        _In_ uint32 membersNeededToStart
        );

    /// <summary>
    /// Configures managed initialization parameters for a session.
    /// </summary>
    /// <param name="joinTimeout">The period of time, in milliseconds, that the system waits for a member to join the session.
    /// This value overrides the value of the memberReservedTimeout parameter for the MultiplayerSession.SetTimeouts Method
    /// for the initial session members.</param>
    /// <param name="measurementTimeout">The period of time, in milliseconds, that the system waits for a measuring operation
    /// during managed initialization.
    /// Note: During matchmaking, a 45-second timeout for QoS measurements is enforced. Therefore we recommend the use of a
    /// measurement timeout that is less than or equal to 30 seconds during matchmaking. </param>
    /// <param name="evaluationTimeout">The period of time, in milliseconds, that the system waits for an evaluation.</param>
    /// <param name="autoEvalute">True if the system should auto-evaluate the session service, and false if the title performs the evaluation.</param>
    /// <param name="membersNeededToStart">The number of members needed to start the session, for initialization episode zero only.</param>
    /// <remarks>
    /// This can only be set when creating a new session.
    ///
    /// If a 'managedInitialization' object is set, the session expects the client system or title to perform 
    /// initialization following session creation and/or as new members join the session.
    ///
    /// The timeouts and initialization stages are automatically tracked by the session, including QoS 
    /// measurements if any metrics are set. These timeouts override the session's reservation and ready 
    /// timeouts for members that have 'initializationEpisode' set.
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
    void SetMemberInitialization(
        _In_ Windows::Foundation::TimeSpan joinTimeout,
        _In_ Windows::Foundation::TimeSpan measurementTimeout,
        _In_ Windows::Foundation::TimeSpan evaluationTimeout,
        _In_ bool autoEvalute,
        _In_ uint32 membersNeededToStart
        );

    /// <summary>
    /// Sets thresholds that apply to each pairwise connection for all members in a session.
    /// </summary>
    /// <param name="latencyMaximum">The maximum latency, in milliseconds, between session members.</param>
    /// <param name="bandwidthMinimumInKilobitsPerSecond">The minimum bandwidth, in kilobits per second, between members.</param>
    /// <remarks>
    /// The system or title calls SetPeerToPeerRequirements only when creating a new session. It accesses the
    /// MultiplayerPeerToPeerRequirements Class. 
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
    void SetPeerToPeerRequirements(
        _In_ Windows::Foundation::TimeSpan latencyMaximum,
        _In_ uint32 bandwidthMinimumInKilobitsPerSecond
        );

    /// <summary>
    /// Sets thresholds that apply to each connection between a host candidate and a session member.
    /// </summary>
    /// <param name="latencyMaximum">The maximum latency time, in milliseconds.</param>
    /// <param name="bandwidthDownMinimumInKilobitsPerSecond">The minimum bandwidth, in kilobits per second, for information sent from the host to the session member.</param>
    /// <param name="bandwidthUpMinimumInKilobitsPerSecond">The minimum bandwidth, in kilobits per second, for information sent from the session member to the host.</param>
    /// <param name="hostSelectionMetric">An enumeration value indicating the metric for the Xbox system to use in selecting a host.</param>
    /// <remarks>
    /// The system or title calls SetPeerToHostRequirements only when creating a new session. It accesses the
    /// MultiplayerPeerToHostRequirements Class. 
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
    void SetPeerToHostRequirements(
        _In_ Windows::Foundation::TimeSpan latencyMaximum,
        _In_ uint32 bandwidthDownMinimumInKilobitsPerSecond,
        _In_ uint32 bandwidthUpMinimumInKilobitsPerSecond,
        _In_ MultiplayMetrics hostSelectionMetric
        );

    /// <summary>
    /// Sets the connection paths and information for the servers that can be used for QoS measurements. 
    /// </summary>
    /// <param name="measurementServerAddresses">A collection of QualityOfServiceServer objects, returned from
    /// GameServerPlatformService::GetQualityOfServiceServersAsync(), that represent the servers.</param>
    /// <remarks>
    /// The system or title calls SetMeasurementServerAddresses only when creating a new session.
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
    void SetMeasurementServerAddresses(
        _In_ Windows::Foundation::Collections::IVectorView<Microsoft::Xbox::Services::GameServerPlatform::QualityOfServiceServer^>^ measurementServerAddresses
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// This can only be set when creating a new session.
    /// Can only be specified if the 'cloudCompute' capability is set. Enables clients to request that a cloud compute instance be allocated on behalf of the session.
    /// </summary>
    /// <param name="sessionCloudComputePackageConstantsJson">Cloud compute instance be allocated on behalf of the session.</param>
    void SetCloudComputePackageJson(
        _In_ Platform::String^ sessionCloudComputePackageConstantsJson
        );

    /// <summary>
    /// Sets the capabilities of the session.
    /// </summary>
    /// <param name="capabilities">A collection of MultiplayerSessionCapabilities flags that apply to
    /// the MultiplayerSessionConstant's capabilities JSON object.</param>
    /// <remarks>
    /// This can only be set when creating a new session.    
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write
    /// batched local changes to the service.
    /// </remarks>
    void SetSessionCapabilities(
        _In_ MultiplayerSessionCapabilities^ capabilities
        );

    /// <summary>
    /// Sets a flag that indicates if the session initialization succeeded,
    /// </summary>
    /// <param name="initializationSucceeded">True if initialization succeeded, and false otherwise.</param>
    /// <remarks>
    /// This can only be set when creating a new session.
    /// 
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.
    /// </remarks>
    void SetInitializationStatus(
        _In_ bool initializationSucceeded
        );

    /// <summary>
    /// Sets the device token of the host to use in multiplayer transactions.
    /// </summary>
    /// <param name="hostDeviceToken">The host device token.</param>
    /// <remarks>
    /// The host device token is set in the local cache of the session and written to the multiplayer session directory.
    ///
    /// If the system or title calls SetHostDeviceToken and sets peer to host requirements with a call to
    /// MultiplayerSession.SetPeerToHostRequirements, the measuring stage of managed initialization assumes that the
    /// specified host is the correct host and only measures metrics to that host.
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetHostDeviceToken(
        _In_ Platform::String^ hostDeviceToken
        );

    /// <summary>
    /// Forces a specific server connection string to be used. This is useful in preserveSession=always cases.
    /// </summary>
    /// <param name="serverConnectionPath">The server connection path. Setting this path can be useful when the session is preserved.</param>
    /// <remarks>
    /// After calling SetMatchmakingServerConnectionPath, the system or title must call
    /// MultiplayerService.WriteSessionAsync to write batched local changes to the service.
    /// If SetMatchmakingServerConnectionPath is called without calling WriteSessionAsync,
    /// it only changes the local session object but does not commit it to the service. 
    /// </remarks>
    void SetMatchmakingServerConnectionPath(
        _In_ Platform::String^ serverConnectionPath
        );

    /// <summary>
    /// Sets the session status to closed, meaning that new users will not be able to join the session
    /// unless they already have a reservation. 
    /// </summary>
    /// <param name="closed">Whether new users will not be able to join the session unless they already have a reservation</param>
    /// <remarks>
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetClosed(
        _In_ bool closed
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// If set to true, it would allow the members of the session to be locked, such that if a user leaves they are able to come back into the session but
    /// no other user could take that spot. If the session is locked, it must also be set to closed.
    /// </summary>
    void SetLocked(
        _In_ bool locked
        );

    /// <summary>
    /// Sets the session properties/system/allocateCloudCompute field
    /// </summary>
    /// <param name="closed">This triggers a Thunderhead allocation attempt by MPSD</param>
    /// <remarks>
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetAllocateCloudCompute(
        _In_ bool allocateCloudCompute
        );

    /// <summary>
    /// Sets a flag that indicates if a match is not successful and needs to be resubmitted, or if the match is successful
    /// and the matchmaking service can release the session.
    /// </summary>
    /// <param name="matchResubmit ">True if the match that was found was not successful and needs to be resubmitted.
    /// Set this value to false to indicate that the match was successful and the matchmaking service can release the session.</param>
    /// <remarks>
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetMatchmakingResubmit(
        _In_ bool matchResubmit
        );

    /// <summary>
    /// Sets an ordered list of case-insensitive connection path strings that the session can use to connect to 
    /// a game server. Generally titles should use the first on the list, but sophisticated titles could use 
    /// a custom mechanism for choosing one of the others (e.g. based on load).
    /// </summary>
    /// <param name="serverConnectionStringCandidates">The collection of connection paths.</param>
    /// <remarks>
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetServerConnectionStringCandidates(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ serverConnectionStringCandidates
        );

    /// <summary>
    /// Sets the set of session changes that this client will be subscribed to.
    /// Set to "MultiplayerSessionChangeTypes::None" to clear the subscription.
    /// </summary>
    /// <param name="changeTypes">An Or'd set of MultiplayerSessionChangeType enum values representing the change types to subscribe to.</param>
    /// <remarks>
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetSessionChangeSubscription(
        _In_ MultiplayerSessionChangeTypes changeTypes
        );

    /// <summary>
    /// Removes the current member from a session.
    /// </summary>
    /// <remarks>
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void Leave();

    /// <summary>
    /// Set the multiplayer status of the current user to active or inactive.  

    /// </summary>
    /// <param name="status">Indicates the current user status</param>
    /// <remarks>
    /// The member must first be joined to the session before the system or the title can set the status.
    /// Use AddMemberReservation() to add a member reservation.
    /// You cannot set the the user's status to reserved or ready by using this method.
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetCurrentUserStatus(
        _In_ MultiplayerSessionMemberStatus status
        );

    /// <summary>
    /// Sets the base64 encoded secure device address of the member.
    /// </summary>
    /// <param name="value">Indicates the value of the current user's secure device address encoded in base64.</param>
    /// <remarks>
    /// The member must first be joined to the session before the system or the title can set the secure device address.
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetCurrentUserSecureDeviceAddressBase64(
        _In_ Platform::String^ value
        );

    /// <summary>
    /// Call multiplayer_service::write_session after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Set the role info of the member.
    /// The member must first be joined to the session.
    /// </summary>
    /// <param name="roles">Indicates a collection of role types to role names for the current user.</param>
    void SetCurrentUserRoleInfo(
        _In_ Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ roles
        );

    /// <summary>
    /// Sets a collection of members that make up a group.
    /// </summary>
    /// <param name="membersInGroup">A collection of MultiplayerSessionMember objects that represent the session members to make up the group.</param>
    /// <remarks>
    /// The members must first be joined to the session before the system or the title can call this method.
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetCurrentUserMembersInGroup(
        _In_ Windows::Foundation::Collections::IVectorView<MultiplayerSessionMember^>^ membersInGroup
        );

    /// <summary>
    /// Sets the measurements to use to determine the quality of service (QoS) available for the current session member.
    /// </summary>
    /// <param name="measurements">A collection of objects representing the QoS measurements.</param>
    /// <remarks>
    /// This method is only useful when the title manually manages QoS. If the platform is automatically performing QoS,
    /// the title does not need to call this method. 
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetCurrentUserQualityOfServiceMeasurements(
        _In_ Windows::Foundation::Collections::IVectorView<MultiplayerQualityOfServiceMeasurements^>^ measurements
        );

    /// <summary>
    /// Sets a JSON string with QoS measurements for the current session member.
    /// </summary>
    /// <param name="valueJson">The JSON string representing the server measurements.</param>
    /// <remarks>
    /// This method is only useful when the title manually manages QoS. If the platform is automatically performing QoS,
    /// the title does not need to call this method. 
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetCurrentUserQualityOfServiceServerMeasurementsJson(
        _In_ Platform::String^ valueJson
        );

    /// <summary>
    /// Call WriteSession() after this to write batched local changes to the service. 
    /// If this is called without multiplayer_service::write_session, this will only change the local session object but does not commit it to the service.
    /// Set the arbitration results for a tournament game session.
    /// </summary>
    /// <param name="results">A map of team names to team results.</param>
    void SetCurrentUserArbitrationResults(
        _In_ Windows::Foundation::Collections::IMapView<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ results
        );

    /// <summary>
    /// Set a custom property on the current user to the specified JSON string
    /// The member must first be joined to the session.
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">(Optional) The JSON string value to assign to the property.</param>
    /// <remarks>
    /// This method is used to store per-member attributes, for example, during creation of a match ticket session.
    /// The client calls the method for each session member. For a match ticket session, the method copies attributes
    /// from the /members/{index}/properties/custom/matchAttrs field to 
    /// the /members/{index}/constants/custom/matchmakingResult/playerAttrs field. They are later combined and processed
    /// by the matchmaking service when the session is passed to matchmaking in the call to the
    /// MatchmakingService.CreateMatchTicketAsync Method during match ticket creation. 
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetCurrentUserMemberCustomPropertyJson(
        _In_ Platform::String^ name,
        _In_opt_ Platform::String^ valueJson
        );

    /// <summary>
    /// Delete a custom property on the current session member.
    /// </summary>
    /// <param name="name">The name of the property to delete.</param>
    /// <remarks>
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void DeleteCurrentUserMemberCustomPropertyJson(
        _In_ Platform::String^ name
        );

    /// <summary>
    /// Sets matchmaking constants for the session.
    /// </summary>
    /// <param name="matchmakingTargetSessionConstants">A JSON string representing the target session constants</param>
    /// <remarks>
    /// This method is optional. Only a client acting as a matchmaking service should call it. Its use
    /// requires the MultiplayerSessionCapabilities.ClientMatchmaking Property to be set. 
    ///
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetMatchmakingTargetSessionConstantsJson(
        _In_ Platform::String^ matchmakingTargetSessionConstants
        );

    /// <summary>
    /// Set a session custom property to the specified JSON string.
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="valueJson">(Optional) The JSON string value to assign to the property.</param>
    /// <remarks>
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void SetSessionCustomPropertyJson(
        _In_ Platform::String^ name,
        _In_opt_ Platform::String^ valueJson
        );

    /// <summary>
    /// Deletes a session custom property.
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <remarks>
    /// After calling this method, the caller must use MultiplayerService.WriteSessionAsync to write batched local changes
    /// to the service.If SetHostDeviceToken is called without calling WriteSessionAsync, it only changes the local session
    /// object but does not commit it to the service.
    /// </remarks>
    void DeleteSessionCustomPropertyJson(
        _In_ Platform::String^ name
        );

    /// <summary>
    /// Static compare method that compares two sessions and returns an Or'ed MultiplayerSessionChangeType.
    /// </summary>
    /// <param name="currentSession">The current session to compare to an older session.</param>
    /// <param name="oldSession">The older session to compare to the current session.</param>
    /// <returns>An OR'ed MultiplayerSessionChangeType that contains all of the differences.</returns>
    static MultiplayerSessionChangeTypes CompareMultiplayerSessions(
        _In_ MultiplayerSession^ currentSession,
        _In_ MultiplayerSession^ oldSession
        );

    /// <summary>
    /// Static method that converts an HTTP Status code to a WriteSessionStatus.
    /// </summary>
    /// <param name="httpStatusCode">Status code of a http result</param>
    /// <returns>A WriteSessionStatus which gives more specific information about the status code in regards to the Write Session call.</returns>
    static WriteSessionStatus ConvertHttpStatusToWriteSessionStatus(
        _In_ int32 httpStatusCode
        );

    /// <summary>
    /// Internal function
    /// </summary>
    void _Init(
        _In_ XboxLiveContext^ xboxLiveContext,
        _In_ MultiplayerSessionReference^ multiplayerSessionReference,
        _In_ uint32 maxMembersInSession,
        _In_ MultiplayerSessionVisibility multiplayerSessionVisibility,
        _In_opt_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ initiatorXboxUserIds,
        _In_opt_ Platform::String^ sessionCustomConstantsJson
        );

internal:
    MultiplayerSession(
        _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session> cppObj
        );

    MultiplayerSessionMember^ JoinHelper(
        _In_opt_ Platform::String^ memberCustomConstantsJson,
        _In_ bool initializeRequested,
        _In_ bool joinWithActiveStatus,
        _In_ bool addInitializePropertyToRequest
        );

    std::shared_ptr<xbox::services::multiplayer::multiplayer_session> GetCppObj() const;
private:
    std::shared_ptr<xbox::services::multiplayer::multiplayer_session> m_cppObj;
    MultiplayerSessionReference^ m_sessionReference;
    MultiplayerSessionConstants^ m_sessionConstants;
    MultiplayerSessionProperties^ m_sessionProperties;
    MultiplayerSessionRoleTypes^ m_sessionRoleTypes;
    MultiplayerSessionMember^ m_currentUser;
    MultiplayerSessionMatchmakingServer^ m_matchmakingServer;
    MultiplayerSessionTournamentsServer^ m_tournamentsServer;
    MultiplayerSessionArbitrationServer^ m_arbitrationServer;
    Windows::Foundation::Collections::IMapView<Platform::String^, Platform::String^>^ m_roles;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
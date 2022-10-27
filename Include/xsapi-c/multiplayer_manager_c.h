// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
    #error C++11 required
#endif

#pragma once

#include "multiplayer_c.h"

extern "C"
{

/// <summary>
/// Defines values used to indicate who can join your lobby.
/// </summary>
enum class XblMultiplayerJoinability : uint32_t
{
    /// <summary>
    /// Joinability not set or no lobby exists yet.
    /// </summary>
    None,

    /// <summary>
    /// Default value. The lobby is joinable by users who are followed by an existing member of the session.
    /// </summary>
    JoinableByFriends,

    /// <summary>
    /// The lobby is joinable only via an invite.
    /// </summary>
    InviteOnly,

    /// <summary>
    /// This option will close the lobby only when a game is in progress.  
    /// All other times, it will keep the lobby open for InviteOnly so invitees can join when no game is in progress.
    /// </summary>
    DisableWhileGameInProgress,

    /// <summary>
    /// This option will close the lobby immediately.
    /// </summary>
    Closed
};

/// <summary>
/// Defines values used to indicate status for the matchmaking stages.
/// </summary>
enum class XblMultiplayerMatchStatus : uint32_t
{
    /// <summary>
    /// Indicates no matchmaking search has been started.
    /// </summary>
    None,

    /// <summary>
    /// Indicates that a match ticket was submitted for matchmaking.
    /// </summary>
    SubmittingMatchTicket,

    /// <summary>
    /// Indicates that matchmaking is still searching.
    /// </summary>
    Searching,

    /// <summary>
    /// Indicates that matchmaking search has found a match.
    /// </summary>
    Found,

    /// <summary> 
    /// Joining initialization stage.  
    /// Matchmaking creates the game session and adds users to it.  
    /// The client has up to the joining timeout to join the session during this phase.
    /// </summary>
    Joining,

    /// <summary> 
    /// Waiting for remote clients to join the game session.  
    /// The client has up to the joining timeout to join the session during this phase.
    /// </summary>
    WaitingForRemoteClientsToJoin,

    /// <summary>
    /// Measuring initialization stage.  
    /// Stage where QoS measurement happens.  
    /// The client has up to the measurement timeout to upload qos measurements to the service during this phase.
    /// </summary>
    Measuring,

    /// <summary>
    /// Uploading QoS measurement results to the service.  
    /// The client has up to the measurement timeout to upload qos measurements to the service during this phase.
    /// </summary>
    UploadingQosMeasurements,

    /// <summary>
    /// Waiting for remote clients to upload QoS measurement results to the service.  
    /// The client has up to the measurement timeout to upload qos measurements to the service during this phase.
    /// </summary>
    WaitingForRemoteClientsToUploadQos,

    /// <summary>
    /// Evaluating initialization stage.  
    /// If auto evaluate is true, then this stage is skipped.  
    /// Otherwise the title will do its own evaluation.
    /// </summary>
    Evaluating,

    /// <summary>
    /// Match was found and QoS measurement was successful.
    /// </summary>
    Completed,

    /// <summary>
    /// If the match that was found was not successful and is resubmitting.
    /// </summary>
    Resubmitting,

    /// <summary>
    /// Indicates that matchmaking search has expired.
    /// </summary>
    Expired,

    /// <summary>
    /// Indicates that matchmaking is in the process of canceling the search.
    /// </summary>
    Canceling,

    /// <summary>
    /// Indicates that matchmaking search has been canceled.
    /// </summary>
    Canceled,

    /// <summary>
    /// Failed initialization stage.  
    /// The initialization failed.
    /// </summary>
    Failed,
};

/// <summary>
/// Defines values used to indicate event types for a multiplayer lobby or game.  
/// The XblMultiplayerEventArgsHandle can be used to get additional information 
/// about the event depending on the event type.
/// </summary>
enum class XblMultiplayerEventType : uint32_t
{
    /// <summary>
    /// Indicates the user was added.  
    /// You can call XblMultiplayerEventArgsXuid to get the xuid of the added user.
    /// </summary>
    UserAdded,

    /// <summary>
    /// Indicates the user was removed.  
    /// You can call XblMultiplayerEventArgsXuid to get the xuid of the added user.
    /// </summary>
    UserRemoved,

    /// <summary>
    /// Indicates a new member has joined the session.  
    /// You can call XblMultiplayerEventArgsMembersCount, XblMultiplayerEventArgsMembers
    /// to get the members who joined.
    /// </summary>
    MemberJoined,

    /// <summary>
    /// Indicates a member has left the session.  
    /// You can call XblMultiplayerEventArgsMembersCount, XblMultiplayerEventArgsMembers
    /// to get the members who left.
    /// </summary>
    MemberLeft,

    /// <summary>
    /// Indicates a member property has changed.  
    /// You can call XblMultiplayerEventArgsMember, XblMultiplayerEventArgsPropertiesJson
    /// to get the properties that changed and the member.
    /// </summary>
    MemberPropertyChanged,

    /// <summary>
    /// Indicates that the XblMultiplayerManagerLobbySessionSetLocalMemberProperties() or 
    /// XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties() operation has completed.  
    /// Upon completion, the game can view the XblMultiplayerEvent::Result to see if the write succeeded.  
    /// A game can write local member properties by calling the XblMultiplayerManagerLobbySessionSetLocalMemberProperties() operation.
    /// </summary>
    LocalMemberPropertyWriteCompleted,

    /// <summary>
    /// Indicates that the XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress() operation has completed.  
    /// Upon completion, the game can view the XblMultiplayerEvent::Result to see if the write succeeded.  
    /// A game can write local member properties by calling the XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress() operation.
    /// </summary>
    LocalMemberConnectionAddressWriteCompleted,

    /// <summary>
    /// Indicates a session (lobby or game) property has changed.  
    /// You can call XblMultiplayerEventArgsPropertiesJson for the changed properties.
    /// </summary>
    SessionPropertyChanged,

    /// <summary>
    /// Indicates that the set property operation has completed.  
    /// Upon completion, the game can view the XblMultiplayerEvent::Result to see if the write succeeded.  
    /// A game can write synchronized properties by calling the XblMultiplayerManagerLobbySessionSetProperties() or 
    /// XblMultiplayerManagerGameSessionSetProperties() operation.
    /// </summary>
    SessionPropertyWriteCompleted,

    /// <summary>
    /// Indicates that the set synchronized property operation has completed.  
    /// Upon completion, the game can view the XblMultiplayerEvent::Result to see if the write succeeded.  
    /// A game can write synchronized properties by calling the XblMultiplayerManagerGameSessionSetSynchronizedProperties() operation.
    /// </summary>
    SessionSynchronizedPropertyWriteCompleted,

    /// <summary>
    /// Indicates host has changed.  
    /// You can call XblMultiplayerEventArgsMember for the new host.
    /// </summary>
    HostChanged,

    /// <summary>
    /// Indicates that the XblMultiplayerManagerLobbySessionSetSynchronizedHost() operation has completed.  
    /// Upon completion, the game can view the XblMultiplayerEvent::Result to see if the write succeeded.  
    /// A game can write synchronized host by calling the XblMultiplayerManagerLobbySessionSetSynchronizedHost() operation.
    /// </summary>
    SynchronizedHostWriteCompleted,

    /// <summary>
    /// Indicates that the XblMultiplayerJoinability value has changed.  
    /// A game can change the state by calling the XblMultiplayerManagerSetJoinability() operation.
    /// </summary>
    JoinabilityStateChanged,

    /// <summary>
    /// Fired when a match has been found, and the client has joined the target game session.  
    /// When this event occurs, title should provide QOS measurement results (via XblMultiplayerSessionCurrentUserSetQosMeasurements) 
    /// between itself and a list of remote clients.  
    /// Note: If your title does not require QoS (based on the session template), this event will not be triggered.  
    /// You can call XblMultiplayerEventArgsPerformQoSMeasurements for the measurements.
    /// </summary>
    PerformQosMeasurements,

    /// <summary>
    /// Indicates that the XblMultiplayerManagerFindMatch() operation has completed.  
    /// You can call XblMultiplayerEventArgsFindMatchCompleted for more information.
    /// </summary>
    FindMatchCompleted,

    /// <summary>
    /// Indicates that the XblMultiplayerManagerJoinGame() operation has completed.  
    /// Once the join succeeds, the member is now part of the game session, and can use 
    /// data in the session to connect to other game members.
    /// </summary>
    JoinGameCompleted,

    /// <summary>
    /// Indicates that the XblMultiplayerManagerLeaveGame() operation has completed.  
    /// After receiving this event, the game session object will be set to null.  
    /// You can join another game by calling XblMultiplayerManagerJoinGame() or XblMultiplayerManagerJoinGameFromLobby().
    /// </summary>
    LeaveGameCompleted,

    /// <summary>
    /// Indicates that the <see cref="XblMultiplayerManagerJoinLobby"/> operation has completed.  
    /// Once the join succeeds, the member is now part of the lobby session, 
    /// and can use data in the session to connect to other lobby members.  
    /// You can call XblMultiplayerEventArgsXuid for the xuid.
    /// </summary>
    JoinLobbyCompleted,

    /// <summary>
    /// Fired when the title's connection to MPSD using the real-time activity service is lost.  
    /// When this event occurs, the title should shut down the multiplayer.
    /// </summary>
    ClientDisconnectedFromMultiplayerService,

    /// <summary>
    /// Indicates that the invite API operation has been completed.
    /// </summary>
    /// <remarks>
    /// On Xbox, receiving this event does not necessarily mean an invite has been sent successfully. 
    /// You will receive this event as long as the game invite UI flow was successfully invoked, 
    /// even if the user cancelled the UI flow without sending an invite, or the invite failed to submit to the service for various reasons. 
    /// </remarks>
    InviteSent,

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.  
    /// Triggered when the tournament's team registration state changes.  
    /// You can call XblMultiplayerEventArgsTournamentRegistrationStateChanged for more information.
    /// DEPRECATED. It will be removed in a future release
    /// </summary>
    TournamentRegistrationStateChanged,

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.  
    /// Triggered when a new game has been scheduled.  
    /// You can call XblMultiplayerEventArgsTournamentGameSessionReady for more information.
    /// DEPRECATED. It will be removed in a future release
    /// </summary>
    TournamentGameSessionReady,

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.  
    /// Triggered when arbitration is complete and game results have been written to the game session.
    /// DEPRECATED. It will be removed in a future release
    /// </summary>
    ArbitrationComplete
};

/// <summary>
/// Defines values used to indicate types for multiplayer sessions.
/// </summary>
enum class XblMultiplayerSessionType : uint32_t
{
    /// <summary>
    /// The session type is unknown.
    /// </summary>
    Unknown,

    /// <summary>
    /// Indicates multiplayer lobby session.
    /// </summary>
    LobbySession,

    /// <summary>
    /// Indicates multiplayer game session.
    /// </summary>
    GameSession,

    /// <summary>
    /// Indicates multiplayer match session.
    /// </summary>
    MatchSession
};


/// <summary>
/// Represents a reference to a member in a multiplayer game.
/// </summary>
/// <remarks>
/// The member objects are created and owned by MultiplayerManager.  
/// The fields of returned XblMultiplayerManagerMember objects are only valid until XblMultiplayerManagerDoWork is called again.
/// </remarks>
typedef struct XblMultiplayerManagerMember
{
    /// <summary>
    /// Id for the member.
    /// </summary>
    uint32_t MemberId;

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.  
    /// Id of this members' team in a tournament.
    /// DEPRECATED. It will be removed in a future release
    /// </summary>
    XBL_DEPRECATED _Field_z_ const char* TeamId;

    /// <summary> 
    /// Only applicable if you are using Team rules with Smart Match.  
    /// Initial team assignment suggested by Smart Match.
    /// </summary> 
    _Field_z_ const char* InitialTeam;

    /// <summary>
    /// Xbox User ID of the member.
    /// </summary>
    uint64_t Xuid;

    /// <summary>
    /// The Gamertag of the member.  This is only to be used for debugging purposes as this gamertag may be out of date.  
    /// It is recommended you use social manager's XblSocialManagerCreateSocialUserGroupFromList
    /// or the profile APIs such as XblProfileGetUserProfileAsync to get this information.
    /// </summary>
    _Field_z_ const char* DebugGamertag;

    /// <summary>
    /// Indicates if this member is playing on the local device.
    /// </summary>
    bool IsLocal;

    /// <summary>
    /// Indicates if this member is part of the lobby.
    /// </summary>
    bool IsInLobby;

    /// <summary>
    /// Indicates if this member is part of the game.
    /// </summary>
    bool IsInGame;

    /// <summary>
    /// The status of this member.
    /// </summary>
    XblMultiplayerSessionMemberStatus Status;

    /// <summary>
    /// The address used for network connection.  
    /// This can be used for secure socket connection.
    /// </summary>
    _Field_z_ const char* ConnectionAddress;

    /// <summary>
    /// JSON value that specify the custom properties of the member.
    /// </summary>
    _Field_z_ const char* PropertiesJson;

    /// <summary>
    /// Token that uniquely identifies a device. Used for setting host and QoS measurements.
    /// </summary>
    _Field_z_ const char* DeviceToken;
} XblMultiplayerManagerMember;

/// <summary>
/// Determines whether two members are on the same device.
/// </summary>
/// <param name="first">The first member.</param>
/// <param name="second">The second member.</param>
/// <returns>Returns true if both members are on the same device, false if both members are not on the same device.</returns>
/// <remarks>This function compares the device tokens of both members. If the device tokens match, both members are on the same device. 
/// For more information, see <see cref="XblDeviceToken"/>.</remarks>
STDAPI_(bool) XblMultiplayerManagerMemberAreMembersOnSameDevice(
    _In_ const XblMultiplayerManagerMember* first,
    _In_ const XblMultiplayerManagerMember* second
) XBL_NOEXCEPT;


/// <summary>
/// A handle to multiplayer event arguments that can be used to retrieve additional information for a multiplayer event, depending on the type of event.
/// </summary>
/// <memof><see cref="XblMultiplayerEvent"/></memof>
/// <argof><see cref="XblMultiplayerEventArgsXuid"/></argof>
/// <argof><see cref="XblMultiplayerEventArgsMembersCount"/></argof>
/// <argof><see cref="XblMultiplayerEventArgsMembers"/></argof>
/// <argof><see cref="XblMultiplayerEventArgsMember"/></argof>
/// <argof><see cref="XblMultiplayerEventArgsPropertiesJson"/></argof>
/// <argof><see cref="XblMultiplayerEventArgsFindMatchCompleted"/></argof>
/// <argof><see cref="XblMultiplayerEventArgsTournamentRegistrationStateChanged"/></argof>
/// <argof><see cref="XblMultiplayerEventArgsTournamentGameSessionReady"/></argof>
/// <argof><see cref="XblMultiplayerEventArgsPerformQoSMeasurements"/></argof>
typedef struct XblMultiplayerEventArgs* XblMultiplayerEventArgsHandle;

/// <summary>
/// A multiplayer event that is returned from <see cref="XblMultiplayerManagerDoWork"/>.
/// </summary>
typedef struct XblMultiplayerEvent
{
    /// <summary>
    /// The error code indicating the result of the operation.
    /// </summary>
    HRESULT Result;

    /// <summary>
    /// Call-specific debug information if the API operation fails.  
    /// The debug information is not localized; use only for debugging purposes.
    /// </summary>
    _Field_z_ const char* ErrorMessage;

    /// <summary>
    /// A pointer to the application-defined data passed into the initiating method.
    /// </summary>
    void* Context;

    /// <summary>
    /// The type of the event triggered.
    /// </summary>
    XblMultiplayerEventType EventType;

    /// <summary>
    /// A handle to the event arguments for the multiplayer event.
    /// </summary>
    XblMultiplayerEventArgsHandle EventArgsHandle;

    /// <summary>
    /// The multiplayer session type this event was triggered for.  
    /// Depending upon the session type, you can then retrieve the latest lobby or game session.
    /// </summary>
    XblMultiplayerSessionType SessionType;
} XblMultiplayerEvent;

/// <summary>
/// A connection address/device token pair to run QoS measurements on.
/// </summary>
typedef struct XblMultiplayerConnectionAddressDeviceTokenPair
{
    /// <summary>
    /// The connection address.
    /// </summary>
    _Field_z_ const char* connectionAddress;

    /// <summary>
    /// The connection device token.
    /// </summary>
    _Field_z_ XblDeviceToken deviceToken;
} XblMultiplayerConnectionAddressDeviceTokenPair;

/// <summary>
/// Event arguments returned for `XblMultiplayerEventType::PerformQosMeasurements` events.
/// </summary>
typedef struct XblMultiplayerPerformQoSMeasurementsArgs
{
    /// <summary>
    /// An array of remote clients to perform QoS measurements on.
    /// </summary>
    const XblMultiplayerConnectionAddressDeviceTokenPair* remoteClients;

    /// <summary>
    /// The size of the `remoteClients` array.
    /// </summary>
    size_t remoteClientsSize;
} XblMultiplayerPerformQoSMeasurementsArgs;

/// <summary>
/// Retrieves additional information for `XblMultiplayerEventType::UserAdded`, `XblMultiplayerEventType::UserRemoved`,
/// and `XblMultiplayerEventType::JoinLobbyCompleted` events.
/// </summary>
/// <param name="argsHandle">The event arguments handle for the multiplayer event.</param>
/// <param name="xuid">The applicable Xbox User ID, depending on the multiplayer event:
/// <para>`XblMultiplayerEventType::UserAdded` - The Xbox User ID of the member that was added.</para>
/// <para>`XblMultiplayerEventType::UserRemoved` - The Xbox User ID of the member that was removed.</para>
/// <para>`XblMultiplayerEventType::JoinLobbyCompleted` - The Xbox User ID of the member that was invited.</para>
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Call this function to get more information about multiplayer events returned by <see cref="XblMultiplayerManagerDoWork"/> 
/// for which the `EventType` member of the <see cref="XblMultiplayerEvent"/> for a multiplayer event is set to either 
/// `XblMultiplayerEventType::UserAdded`, `XblMultiplayerEventType::UserRemoved`, or `XblMultiplayerEventType::JoinLobbyCompleted`.
/// <para>The event arguments handle for a multiplayer event can be retrieved from the `EventArgsHandle` member of 
/// the <see cref="XblMultiplayerEvent"/> structure for that multiplayer event.</para>
/// <para>For more information about multiplayer events, see 
/// <see href="live-multiplayer-manager-api-overview.md">Multiplayer Manager API overview</see>.</para></remarks>
/// <seealso cref="XblMultiplayerEventArgsFindMatchCompleted"/>
/// <seealso cref="XblMultiplayerEventArgsMember"/>
/// <seealso cref="XblMultiplayerEventArgsMembers"/>
/// <seealso cref="XblMultiplayerEventArgsMembersCount"/>
/// <seealso cref="XblMultiplayerEventArgsPerformQosMeasurements"/>
/// <seealso cref="XblMultiplayerEventArgsPropertiesJson"/>
/// <seealso cref="XblMultiplayerEventType"/>
/// <seealso cref="XblMultiplayerManagerJoinLobby"/>
STDAPI XblMultiplayerEventArgsXuid(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ uint64_t* xuid
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for `XblMultiplayerEventType::MemberJoined` and `XblMultiplayerEventType::MemberLeft` events.
/// </summary>
/// <param name="argsHandle">The event arguments handle for the multiplayer event.</param>
/// <param name="memberCount">The size of the `members` caller-allocated array for <see cref="XblMultiplayerEventArgsMembers"/>.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Call this function before you call the <see cref="XblMultiplayerEventArgsMembers"/> function, to return 
/// the size of the array you must allocate for the `members` parameter of the <see cref="XblMultiplayerEventArgsMembers"/> function. 
/// <para>The event arguments handle for a multiplayer event can be retrieved from the `EventArgsHandle` member of 
/// the <see cref="XblMultiplayerEvent"/> structure for that multiplayer event.</para>
/// <para>For more information about multiplayer events, see 
/// <see href="live-multiplayer-manager-api-overview.md">Multiplayer Manager API overview</see>.</para></remarks>
/// <seealso cref="XblMultiplayerEventArgsFindMatchCompleted"/>
/// <seealso cref="XblMultiplayerEventArgsMember"/>
/// <seealso cref="XblMultiplayerEventArgsMembers"/>
/// <seealso cref="XblMultiplayerEventArgsPerformQosMeasurements"/>
/// <seealso cref="XblMultiplayerEventArgsPropertiesJson"/>
/// <seealso cref="XblMultiplayerEventArgsXuid"/>
/// <seealso cref="XblMultiplayerEventType"/>
STDAPI XblMultiplayerEventArgsMembersCount(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ size_t* memberCount
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for `XblMultiplayerEventType::MemberJoined` and `XblMultiplayerEventType::MemberLeft` events.
/// </summary>
/// <param name="argsHandle">The event arguments handle for the multiplayer event.</param>
/// <param name="membersCount">The size of the `members` array.</param>
/// <param name="members">A caller-allocated array that passes back a list of members, depending on the multiplayer event:  
/// <para>`XblMultiplayerEventType::MemberJoined` - A list of members that joined the game.</para>
/// <para>`XblMultiplayerEventType::MemberLeft` - A list of members that left the game.</para>
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Call the <see cref="XblMultiplayerEventArgsMembersCount"/> function before you call this function, to return 
/// the size of the array you must allocate for the `members` parameter of this function.
/// <para>The event arguments handle for a multiplayer event can be retrieved from the `EventArgsHandle` member of 
/// the <see cref="XblMultiplayerEvent"/> structure for that multiplayer event.</para>
/// <para>For more information about multiplayer events, see 
/// <see href="live-multiplayer-manager-api-overview.md">Multiplayer Manager API overview</see>.</para></remarks>
/// <seealso cref="XblMultiplayerEventArgsFindMatchCompleted"/>
/// <seealso cref="XblMultiplayerEventArgsMember"/>
/// <seealso cref="XblMultiplayerEventArgsMembersCount"/>
/// <seealso cref="XblMultiplayerEventArgsPerformQosMeasurements"/>
/// <seealso cref="XblMultiplayerEventArgsPropertiesJson"/>
/// <seealso cref="XblMultiplayerEventArgsXuid"/>
/// <seealso cref="XblMultiplayerEventType"/>
STDAPI XblMultiplayerEventArgsMembers(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _In_ size_t membersCount,
    _Out_writes_(membersCount) XblMultiplayerManagerMember* members
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for `XblMultiplayerEventType::HostChanged` and `XblMultiplayerEventType::MemberPropertyChanged` events.
/// </summary>
/// <param name="argsHandle">The event arguments handle for the multiplayer event.</param>
/// <param name="member">The applicable member, depending on the multiplayer event:
/// <para>`XblMultiplayerEventType::HostChanged` - The new host member. If an existing host leaves, there is no new host member to return 
/// in this parameter. In this case, this function returns `HRESULT_FROM_WIN32(ERROR_RESOURCE_DATA_NOT_FOUND)`.</para>
/// <para>`XblMultiplayerEventType::MemberPropertyChanged` - The member whose property changed.</para>
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Call this function to get more information about multiplayer events returned by <see cref="XblMultiplayerManagerDoWork"/> 
/// for which the `EventType` member of the <see cref="XblMultiplayerEvent"/> for a multiplayer event is set to either 
/// `XblMultiplayerEventType::HostChanged` or `XblMultiplayerEventType::MemberPropertyChanged`.
/// <para>The event arguments handle for a multiplayer event can be retrieved from the `EventArgsHandle` member of 
/// the <see cref="XblMultiplayerEvent"/> structure for that multiplayer event.</para>
/// <para>For more information about multiplayer events, see 
/// <see href="live-multiplayer-manager-api-overview.md">Multiplayer Manager API overview</see>.</para></remarks>
/// <seealso cref="XblMultiplayerEventArgsFindMatchCompleted"/>
/// <seealso cref="XblMultiplayerEventArgsMembers"/>
/// <seealso cref="XblMultiplayerEventArgsMembersCount"/>
/// <seealso cref="XblMultiplayerEventArgsPerformQosMeasurements"/>
/// <seealso cref="XblMultiplayerEventArgsPropertiesJson"/>
/// <seealso cref="XblMultiplayerEventArgsXuid"/>
/// <seealso cref="XblMultiplayerEventType"/>
STDAPI XblMultiplayerEventArgsMember(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ XblMultiplayerManagerMember* member
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for `XblMultiplayerEventType::MemberPropertyChanged` 
/// and `XblMultiplayerEventType::SessionPropertyChanged` events.
/// </summary>
/// <param name="argsHandle">The event arguments handle for the multiplayer event.</param>
/// <param name="properties">A pointer to a JSON string, depending on the multiplayer event:
/// <para>`XblMultiplayerEventType::MemberPropertyChanged` - The JSON string of the member property that changed.</para>
/// <para>`XblMultiplayerEventType::SessionPropertyChanged` - The JSON string of the session property that changed.</para>
/// <para>The memory for the pointer remains valid for the life of the `XblMultiplayerEventArgsHandle` object, until the handle is closed.</para>
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Call this function to get more information about multiplayer events returned by <see cref="XblMultiplayerManagerDoWork"/> 
/// for which the `EventType` member of the <see cref="XblMultiplayerEvent"/> for a multiplayer event is set to either 
/// `XblMultiplayerEventType::MemberPropertyChanged` or `XblMultiplayerEventType::SessionPropertyChanged`.
/// <para>The event arguments handle for a multiplayer event can be retrieved from the `EventArgsHandle` member of 
/// the <see cref="XblMultiplayerEvent"/> structure for that multiplayer event.</para>
/// <para>For more information about multiplayer events, see 
/// <see href="live-multiplayer-manager-api-overview.md">Multiplayer Manager API overview</see>.</para></remarks>
/// <seealso cref="XblMultiplayerEventArgsFindMatchCompleted"/>
/// <seealso cref="XblMultiplayerEventArgsMember"/>
/// <seealso cref="XblMultiplayerEventArgsMembers"/>
/// <seealso cref="XblMultiplayerEventArgsMembersCount"/>
/// <seealso cref="XblMultiplayerEventArgsPerformQosMeasurements"/>
/// <seealso cref="XblMultiplayerEventArgsXuid"/>
/// <seealso cref="XblMultiplayerEventType"/>
STDAPI XblMultiplayerEventArgsPropertiesJson(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ const char** properties
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for `XblMultiplayerEventType.FindMatchCompleted` multiplayer events.
/// </summary>
/// <param name="argsHandle">The event arguments handle for the multiplayer event.</param>
/// <param name="matchStatus">A caller-allocated structure that describes the current matchmaking status.</param>
/// <param name="initializationFailureCause">A caller-allocated structure that passes back the cause of why 
/// the initialization failed, or `XblMultiplayerMeasurementFailure::None` if there was no failure.   
/// This value is set when transitioning out of the `XblMultiplayerMatchStatus::Joining` or 
/// `XblMultiplayerMatchStatus::Measuring` initialization stages, if this member doesn't pass the initializaton stage.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Call this function to get more information about multiplayer events returned by <see cref="XblMultiplayerManagerDoWork"/> 
/// for which the `EventType` member of the <see cref="XblMultiplayerEvent"/> for a multiplayer event is set to 
/// `XblMultiplayerEventType.FindMatchCompleted`.
/// <para>The event arguments handle for a multiplayer event can be retrieved from the `EventArgsHandle` member of 
/// the <see cref="XblMultiplayerEvent"/> structure for that multiplayer event.</para>
/// <para>For more information about multiplayer events, see 
/// <see href="live-multiplayer-manager-api-overview.md">Multiplayer Manager API overview</see>.</para></remarks>
/// <seealso cref="XblMultiplayerEventArgsMember"/>
/// <seealso cref="XblMultiplayerEventArgsMembers"/>
/// <seealso cref="XblMultiplayerEventArgsMembersCount"/>
/// <seealso cref="XblMultiplayerEventArgsPerformQosMeasurements"/>
/// <seealso cref="XblMultiplayerEventArgsPropertiesJson"/>
/// <seealso cref="XblMultiplayerEventArgsXuid"/>
/// <seealso cref="XblMultiplayerEventType"/>
STDAPI XblMultiplayerEventArgsFindMatchCompleted(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_opt_ XblMultiplayerMatchStatus* matchStatus,
    _Out_opt_ XblMultiplayerMeasurementFailure* initializationFailureCause
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for XblMultiplayerEventType::TournamentRegistrationStateChanged events.
/// </summary>
/// <param name="argsHandle">The event args handle from the XblMultiplayerEvent.</param>
/// <param name="registrationState">A caller allocated struct that passes back the tournament team registration state.</param>
/// <param name="registrationReason">A caller allocated struct that passes back the tournament team registration reason for the certain registration states.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblMultiplayerEventArgsTournamentRegistrationStateChanged(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_opt_ XblTournamentRegistrationState* registrationState,
    _Out_opt_ XblTournamentRegistrationReason* registrationReason
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for XblMultiplayerEventType::TournamentGameSessionReady events.
/// </summary>
/// <param name="argsHandle">The event args handle from the XblMultiplayerEvent.</param>
/// <param name="startTime">Passes back the game's start time for the tournament.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI_XBL_DEPRECATED XblMultiplayerEventArgsTournamentGameSessionReady(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ time_t* startTime
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for `XblMultiplayerEventType::PerformQosMeasurements` events.
/// </summary>
/// <param name="argsHandle">The event arguments handle for the multiplayer event.</param>
/// <param name="performQoSMeasurementsArgs">A caller-allocated structure that passes back the remote clients for which QoS information is needed.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>Call this function to get more information about multiplayer events returned by <see cref="XblMultiplayerManagerDoWork"/> 
/// for which the `EventType` member of the <see cref="XblMultiplayerEvent"/> for a multiplayer event is set to 
/// `XblMultiplayerEventType::PerformQosMeasurements`.
/// <para>The event arguments handle for a multiplayer event can be retrieved from the `EventArgsHandle` member of 
/// the <see cref="XblMultiplayerEvent"/> structure for that multiplayer event.</para>
/// <para>For more information about multiplayer events, see 
/// <see href="live-multiplayer-manager-api-overview.md">Multiplayer Manager API overview</see>.</para></remarks>
/// <seealso cref="XblMultiplayerEventArgsFindMatchCompleted"/>
/// <seealso cref="XblMultiplayerEventArgsMember"/>
/// <seealso cref="XblMultiplayerEventArgsMembers"/>
/// <seealso cref="XblMultiplayerEventArgsMembersCount"/>
/// <seealso cref="XblMultiplayerEventArgsPropertiesJson"/>
/// <seealso cref="XblMultiplayerEventArgsXuid"/>
/// <seealso cref="XblMultiplayerEventType"/>
STDAPI XblMultiplayerEventArgsPerformQoSMeasurements(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ XblMultiplayerPerformQoSMeasurementsArgs* performQoSMeasurementsArgs
) XBL_NOEXCEPT;

/// <summary>
/// Initializes Multiplayer Manager (MPM).
/// </summary>
/// <param name="lobbySessionTemplateName">The name of the session template for the lobby session to be based on.</param>
/// <param name="asyncQueue">The task queue where all Multiplayer Manager work should be scheduled.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function initializes the lobby session with which Multiplayer Manager (MPM) interacts, based on a session template configured for the title. 
/// You must call this function before calling other MPM functions, otherwise errors may occur. For more information about 
/// configuring session templates, see <see href="live-configure-the-multiplayer-service.md">Configuring the Multiplayer service</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerDoWork"/>
STDAPI XblMultiplayerManagerInitialize(
    _In_z_ const char* lobbySessionTemplateName,
    _In_opt_ XTaskQueueHandle asyncQueue
) XBL_NOEXCEPT;

/// <summary>
/// Maintains game state updates between the title and Multiplayer Manager (MPM).
/// </summary>
/// <param name="multiplayerEvents">An array of multiplayer events for the game to handle. 
/// This is set to null if no multiplayer events occur during this update.</param>
/// <param name="multiplayerEventsCount">The size of the `multiplayerEvents` array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function sends and receives game state updates between the title and Multiplayer Manager (MPM), returning an array 
/// of <see cref="XblMultiplayerEvent"/> structures that represent significant multiplayer events, such as remote players 
/// joining or leaving. You must call this function on a regular and frequent basis, such as once per frame, so that MPM can 
/// properly maintain game state. For more information, see <see href="live-multiplayer-manager-overview.md">Multiplayer Manager overview</see>.  
/// <para>The multiplayer events returned by this function are owned by MPM, and remain valid only until `XblMultiplayerManagerDoWork` is called again. 
/// In addition, the title must be thread-safe when calling `XblMultiplayerManagerDoWork`, because game state changes at the time this function is called.
/// For example, if you're iterating through the list of members on a thread other than the one from which you're calling this function,
/// the list may change when this function is called.</para> 
/// </remarks>
STDAPI XblMultiplayerManagerDoWork(
    _Deref_out_opt_ const XblMultiplayerEvent** multiplayerEvents,
    _Out_ size_t* multiplayerEventsCount
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves the correlation handle for the lobby session. 
/// </summary>
/// <param name="correlationId">Passes back the correlation handle for the lobby session, 
/// or null if a lobby session doesn't exist. The memory for the returned pointer remains 
/// valid until the next call to <see cref="XblMultiplayerManagerDoWork"/>.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function retrieves the correlation handle for the lobby session in Multiplayer Manager (MPM). The correlation handle serves as 
/// an alias for the lobby session, allowing a game to refer to a lobby session by using only the correlation handle. 
/// The correlation handle can be used to query trace logs for entries that relate to the lobby session.
/// For more information, see <see href="live-multiplayer-concepts.md">Multiplayer concepts overview</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionSessionReference"/>
STDAPI XblMultiplayerManagerLobbySessionCorrelationId(
    _Out_ XblGuid* correlationId
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves the session reference for the lobby session.  
/// </summary>
/// <param name="sessionReference">Passes back a pointer to the session reference for the lobby session, 
/// or null if a lobby session doesn't exist. The memory for the returned pointer remains valid until 
/// the next call to <see cref="XblMultiplayerManagerDoWork"/>.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function retrieves a pointer to the session reference for the lobby session, if the lobby session exists in 
/// Multiplayer Manager. The session reference contains the service configuration ID (SCID),
/// session template, and session name for the lobby session, and uniquely identifies 
/// the lobby session in Multiplayer Session Directory (MPSD). For more information about session references, see 
/// <see href="live-mpsd-details.md">Multiplayer Session advanced topics</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionCorrelationId"/>
/// <seealso cref="XblMultiplayerManagerGameSessionSessionReference"/>
STDAPI XblMultiplayerManagerLobbySessionSessionReference(
    _Out_ XblMultiplayerSessionReference* sessionReference
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves the number of local members in the lobby session.  
/// </summary>
/// <returns>The number of members in the lobby session that are local to the device.</returns>
/// <seealso cref="XblMultiplayerManagerLobbySessionMembersCount"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionLocalMembers"/>
STDAPI_(size_t) XblMultiplayerManagerLobbySessionLocalMembersCount() XBL_NOEXCEPT;

/// <summary>
/// Retrieves member information for the local members in the lobby session.
/// </summary>
/// <param name="localMembersCount">The size of the `localMembers` array.  
/// The required size can be obtained by calling <see cref="XblMultiplayerManagerLobbySessionLocalMembersCount"/>.</param>
/// <param name="localMembers">A caller-allocated <see cref="XblMultiplayerManagerMember"/> array to write into.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function returns member information for each member in the lobby session that is local to the device.
/// You can also call the <see cref="XblMultiplayerManagerLobbySessionMembers"/> function to return member 
/// information for all members in the lobby session.
/// <para>The member information returned by this function is valid only until <see cref="XblMultiplayerManagerDoWork"/> is called again.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionMembersCount"/>
STDAPI XblMultiplayerManagerLobbySessionLocalMembers(
    _In_ size_t localMembersCount,
    _Out_writes_(localMembersCount) XblMultiplayerManagerMember* localMembers
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves the number of members in the lobby session.  
/// </summary>
/// <returns>The number of members that are in the lobby session.</returns>
/// <seealso cref="XblMultiplayerManagerGameSessionMembersCount"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionLocalMembersCount"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionMembers"/>
STDAPI_(size_t) XblMultiplayerManagerLobbySessionMembersCount() XBL_NOEXCEPT;

/// <summary>
/// Retrieves member information for the members in the lobby session.
/// </summary>
/// <param name="membersCount">The size of the `members` array.  
/// The required size can be obtained by calling <see cref="XblMultiplayerManagerLobbySessionMembersCount"/>.</param>
/// <param name="members">A caller-allocated <see cref="XblMultiplayerManagerMember"/> array to write into.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function returns member information for each member in the lobby session.
/// You can also call the <see cref="XblMultiplayerManagerLobbySessionLocalMembers"/> function to return member 
/// information for each member in the lobby session that is local to the device.
/// <para>The member information returned by this function is valid only until <see cref="XblMultiplayerManagerDoWork"/> is called again.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionLocalMembersCount"/>
STDAPI XblMultiplayerManagerLobbySessionMembers(
    _In_ size_t membersCount,
    _Out_writes_(membersCount) XblMultiplayerManagerMember* members
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves member information for the host member in the lobby session.  
/// </summary>
/// <param name="hostMember">A caller-allocated structure to be populated with member information for the host member.</param>
/// <returns>HRESULT return code for this API operation.  
/// Returns `__HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER)` if a host member doesn't exist.</returns>
/// <remarks>
/// This function retrieves member information about the member that represents the host for a lobby session.
/// If a lobby session doesn't exist, or if a host member doesn't exist for the lobby session, the function 
/// returns `__HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER)`. The host member is defined as the user with the lowest
/// index on the host device. 
/// <para>The information returned by this function is valid only until <see cref="XblMultiplayerManagerDoWork"/> is called again.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionIsHost"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionLocalMembers"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionMembers"/>
STDAPI XblMultiplayerManagerLobbySessionHost(
    _Out_ XblMultiplayerManagerMember* hostMember
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves the custom properties for the lobby session, as a JSON string.  
/// </summary>
/// <returns>A JSON string that specifies the custom properties for the lobby session, or null if a lobby session doesn't exist.  
/// The memory for the returned pointer remains valid until the next call to <see cref="XblMultiplayerManagerDoWork"/>.</returns>
/// <remarks>
/// This function retrieves the custom properties for the lobby session, represented as a JSON string. These custom properties 
/// can be changed at any time. If custom properties are shared between devices, or may be updated by several devices 
/// at the same time, use the <see cref="XblMultiplayerManagerLobbySessionSetSynchronizedProperties"/> function to change custom properties.
/// Otherwise, you can use the <see cref="XblMultiplayerManagerLobbySessionSetProperties"/> function to change custom properties.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionPropertiesJson"/>
STDAPI_(const char*) XblMultiplayerManagerLobbySessionPropertiesJson() XBL_NOEXCEPT;

/// <summary>
/// Retrieves the session constants associated with the lobby session.
/// </summary>
/// <returns>A pointer to the session constants for the lobby session, or null if a lobby session doesn't exist.  
/// The memory for the returned pointer remains valid until the next call to <see cref="XblMultiplayerManagerDoWork"/>.</returns>
/// <remarks>
/// This function retrieves a pointer to the session constants for a lobby session, if the lobby session exists 
/// in Multiplayer Manager. The session constants contain constants, such as session visibility 
/// and session capabilities, defined by the session template used for the lobby session. Unlike session properties, 
/// session constants can only be set through the session template, and are set at the time the lobby session is created.
/// For more information about session constants, see <see href="concepts/live-mpsd-details.md">Multiplayer Session advanced topics</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionPropertiesJson"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionSessionReference"/>
STDAPI_(const XblMultiplayerSessionConstants*) XblMultiplayerManagerLobbySessionConstants() XBL_NOEXCEPT;

XBL_WARNING_PUSH
XBL_WARNING_DISABLE_DEPRECATED
/// <summary>
/// The known last team result of the tournament.
/// Only applicable if you are using Xbox Live Tournaments.
/// </summary>
/// <returns>A pointer to the team's result for the multiplayer game.  
/// The pointer returned remains valid until the next call to XblMultiplayerManagerDoWork.</returns>
STDAPI_XBL_DEPRECATED_(const XblTournamentTeamResult*) XblMultiplayerManagerLobbySessionLastTournamentTeamResult() XBL_NOEXCEPT;
XBL_WARNING_POP

/// <summary>
/// Joins an Xbox user to the lobby session.  
/// </summary>
/// <param name="user">The user handle of the user joining the lobby session.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function creates a new lobby session and adds the Xbox user specified in <paramref name="user"/> to the session.
/// Subsequent users are added to the newly-hosted lobby session as secondary users. You can send 
/// invites, set session properties, and access members of the lobby session only after the first 
/// local user is added to the lobby session. 
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::JoinLobbyCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para>
/// <para>When attempting to join a lobby session, the service returns `HTTP_E_STATUS_BAD_REQUEST` if the server is full.</para>
/// <para>After joining, you can set the properties for the lobby session by calling  
/// <see cref="XblMultiplayerManagerLobbySessionSetSynchronizedProperties"/>, or you can set the host for the lobby session by
/// calling <see cref="XblMultiplayerManagerLobbySessionSetSynchronizedHost"/> if the lobby session doesn't already have a host.</para>
/// <para>You can also send an invite to another user by calling either 
/// <see cref="XblMultiplayerManagerLobbySessionInviteUsers"/> or <see cref="XblMultiplayerManagerLobbySessionInviteFriends"/>.
/// If you don't need a lobby session, and if you haven't added local users by calling this function, 
/// you can instead call <see cref="XblMultiplayerManagerJoinGame"/> and specify the list of users to join the game.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerJoinability"/>
/// <seealso cref="XblMultiplayerManagerJoinGameFromLobby"/>
STDAPI XblMultiplayerManagerLobbySessionAddLocalUser(
    _In_ XblUserHandle user
) XBL_NOEXCEPT;

/// <summary>
/// Removes the local user from both the lobby session and game session.  
/// </summary>
/// <param name="user">The local user to be removed.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If there are no local users remaining after this function is called, the title cannot 
/// perform any further multiplayer operations. Changes are batched and written to the service 
/// when <see cref="XblMultiplayerManagerDoWork"/> is called.
/// <para>The result of this function is delivered as a multiplayer event with an event type set to 
/// `XblMultiplayerEventType::UserRemoved`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para>
/// <para>After leaving, you can join a different game by calling either <see cref="XblMultiplayerManagerJoinGame"/> or
/// <see cref="XblMultiplayerManagerJoinGameFromLobby"/>, or you can re-add the 
/// local user by calling <see cref="XblMultiplayerManagerLobbySessionAddLocalUser"/>.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerJoinLobby"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionInviteFriends"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionInviteUsers"/>
STDAPI XblMultiplayerManagerLobbySessionRemoveLocalUser(
    _In_ XblUserHandle user
) XBL_NOEXCEPT;

/// <summary>
/// Set a custom property for a local member to the specified JSON string.
/// </summary>
/// <param name="user">The user you want to set the property for.</param>
/// <param name="name">The name of the property to set.</param>
/// <param name="valueJson">Optional. The JSON value to assign to the property.</param>
/// <param name="context">Optional. The application-defined data to correlate the <see cref="XblMultiplayerEvent"/> to the initiating call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function sets the value, represented as a JSON string, of a custom property for a local member in 
/// the lobby session. Custom properties can be changed at any time. Changes are batched and written to the 
/// service when <see cref="XblMultiplayerManagerDoWork"/> is called.
/// <para>The result of this function is delivered as a multiplayer event with an event type set 
/// to `XblMultiplayerEventType::LocalMemberPropertyWriteCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para> 
/// </remarks>
/// <seealso cref="XblMultiplayerManagerGameSessionSetSynchronizedHost"/>
/// <seealso cref="XblMultiplayerEvent"/>
STDAPI XblMultiplayerManagerLobbySessionSetLocalMemberProperties(
    _In_ XblUserHandle user,
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Deletes a custom property from a local member of the lobby session.
/// </summary>
/// <param name="user">The user handle of the local member.</param>
/// <param name="name">The name of the custom property to delete.</param>
/// <param name="context">Optional. The application-defined data to correlate the <see cref="XblMultiplayerEvent"/> to the initiating call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function deletes a custom property from a local member of the lobby session. Custom properties 
/// can be changed at any time. Changes are batched and written to the service when <see cref="XblMultiplayerManagerDoWork"/> is called.
/// <para>The result of this function is delivered as a multiplayer event with an event type set to 
/// `XblMultiplayerEventType::LocalMemberPropertyWriteCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionSetLocalMemberProperties"/>
STDAPI XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties(
    _In_ XblUserHandle user,
    _In_z_ const char* name,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Sets the connection address for the local member.  
/// </summary>
/// <param name="user">The user you want to set the property for.</param>
/// <param name="connectionAddress">The network connection address to set.</param>
/// <param name="context">Optional. The application-defined data to correlate the <see cref="XblMultiplayerEvent"/> to the initiating call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function sets the network connection address of a local member in the lobby session. You can use 
/// the connection address for network and secure socket connections to that local member. Changes are batched 
/// and written to the service when <see cref="XblMultiplayerManagerDoWork"/> is called.
/// <para>The result of this function is delivered as a multiplayer event with an event type set 
/// to `XblMultiplayerEventType::LocalMemberConnectionAddressWriteCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para> 
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionSetLocalMemberProperties"/>
/// <seealso cref="XblMultiplayerEvent"/>
STDAPI XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(
    _In_ XblUserHandle user,
    _In_z_ const char* connectionAddress,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Indicates whether the specified user is the host for the lobby session.
/// </summary>
/// <param name="xuid">The Xbox User ID (XUID) of the user.</param>
/// <returns>Returns true if the XUID is the host of the lobby session; otherwise, false.</returns>
/// <remarks>
/// This function returns false if a lobby session doesn't exist, or if 
/// the Xbox User ID (XUID) specified in `xuid` isn't the host for the lobby session. You can 
/// retrieve the host for a lobby session by calling <see cref="XblMultiplayerManagerLobbySessionHost"/>.
/// For more information, see <see href="live-multiplayer-manager-api-overview.md">Multiplayer Manager API overview</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionSetSynchronizedHost"/>
STDAPI_(bool) XblMultiplayerManagerLobbySessionIsHost(
    _In_ uint64_t xuid
) XBL_NOEXCEPT;

/// <summary>
/// Sets the value of a custom property for the lobby session.
/// </summary>
/// <param name="name">The name of the custom property to set.</param>
/// <param name="valueJson">The value to assign to the property, as a JSON string.</param>
/// <param name="context">Optional. The application-defined data to correlate the <see cref="XblMultiplayerEvent"/> to the initiating call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function sets the value, represented as a JSON string, of a custom property for the lobby session. Custom properties 
/// can be changed at any time. Changes are batched and written to the service when <see cref="XblMultiplayerManagerDoWork"/> is called.
/// If custom properties are shared between devices, or may be updated by several devices 
/// at the same time, use the <see cref="XblMultiplayerManagerLobbySessionSetSynchronizedProperties"/> function to change custom properties.
/// Otherwise, you can use this function to change custom properties.
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::SessionPropertyWriteCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para> 
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionSetSynchronizedHost"/>
/// <seealso cref="XblMultiplayerEvent"/>
STDAPI XblMultiplayerManagerLobbySessionSetProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Sets the value of a custom property for the lobby session, using `XblMultiplayerSessionWriteMode::SynchronizedUpdate`.
/// </summary>
/// <param name="name">The name of the custom property to set.</param>
/// <param name="valueJson">The value to assign to the property, as a JSON string.</param>
/// <param name="context">Optional. The application-defined data to correlate the <see cref="XblMultiplayerEvent"/> to the initiating call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function sets the value, represented as a JSON string, of a custom property for the lobby session. Custom properties 
/// can be changed at any time. If custom properties are shared between devices, or may be updated by several devices 
/// at the same time, use this function to ensure atomicity and resolve any conflicts between devices while changing the values of those custom properties. 
/// If a custom property isn't shared across devices, use the <see cref="XblMultiplayerManagerLobbySessionSetProperties"/> function instead 
/// to change the value of that custom property.
/// <para>The service may reject the request to change the custom property if a race condition occurs due to a conflict.
/// If the request is rejected, the service returns `HTTP_E_STATUS_PRECOND_FAILED`. If a conflict occurs, re-evaluate the need to 
/// change the custom property and, if needed, call this function again to re-submit the request.</para>
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para> 
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionSetSynchronizedHost"/>
/// <seealso cref="XblMultiplayerEvent"/>
STDAPI XblMultiplayerManagerLobbySessionSetSynchronizedProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Sets the host for the lobby session, using `XblMultiplayerSessionWriteMode::SynchronizedUpdate`.
/// </summary>
/// <param name="deviceToken">The device token of the host.</param>
/// <param name="context">Optional. The application-defined data to correlate the <see cref="XblMultiplayerEvent"/> to the initiating call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function sets the host for the lobby session. Use this function to ensure atomicity and resolve any conflicts between devices 
/// trying to set the host at the same time. 
/// <para>The service may reject the request to set the host if a race condition occurs due to a conflict.
/// If the request is rejected, the service returns `HTTP_E_STATUS_PRECOND_FAILED`. If a conflict occurs, re-evaluate the need to 
/// change the host and, if needed, call this function again to re-submit the request.</para>
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::SynchronizedHostWriteCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para>
/// Note that host device tokens are generated from a session member's secure device address, so ensure that the secure device address is set for the 
/// desired host prior to calling this method.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionSetSynchronizedProperties"/>
/// <seealso cref="XblMultiplayerEvent"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress"/>
STDAPI XblMultiplayerManagerLobbySessionSetSynchronizedHost(
    _In_ const char* deviceToken,
    _In_opt_ void* context
) XBL_NOEXCEPT;

#if HC_PLATFORM_IS_MICROSOFT
/// <summary>
/// Displays the standard Xbox UI, allowing the user to select friends or recent players and invite them to the game.
/// </summary>
/// <param name="requestingUser">The user who is sending the invite.</param>
/// <param name="contextStringId">Optional. The custom context string ID, a string that is defined 
/// during Xbox Live ingestion, to identify the custom invitation text 
/// that is added to the standard invitation text.  The ID string must be prefixed with 
/// three slash characters ("///").</param>
/// <param name="customActivationContext">Optional. The activation context string, a game-defined string 
/// that is passed to the invited game client and interpreted as desired by the game.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// If this function is invoked, Multiplayer Manager sends invites to the selected players when the user 
/// confirms the player selection in the standard Xbox UI. If a selected player accepts the invite, the title is notified. 
/// For GDK-based games, the title is notified by invoking the callback function specified 
/// when the <see cref="XGameInviteRegisterForEvent"/> function was invoked. 
/// For games based on other platforms, the title is activated.
/// For more information, see <see href="live-invites-receive.md">Receiving invites</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerJoinLobby"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionAddLocalUser"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionInviteUsers"/>
STDAPI XblMultiplayerManagerLobbySessionInviteFriends(
    _In_ XblUserHandle requestingUser,
    _In_opt_z_ const char* contextStringId,
    _In_opt_z_ const char* customActivationContext
) XBL_NOEXCEPT;
#endif

/// <summary>
/// Invites the specified users to the game without displaying additional UI.  
/// </summary>
/// <param name="user">The user who is sending the invite.</param>
/// <param name="xuids">The array of Xbox User IDs (XUIDs) to be invited.</param>
/// <param name="xuidsCount">The size of the `xuids` array.</param>
/// <param name="contextStringId">Optional. The custom context string ID, a string that is defined 
/// during Xbox Live ingestion, to identify the custom invitation text 
/// that is added to the standard invitation text.  The ID string must be prefixed with 
/// three slash characters ("///").</param>
/// <param name="customActivationContext">Optional. The activation context string, a game-defined string 
/// that is passed to the invited game client and interpreted as desired by the game.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Multiplayer Manager sends invites to the selected players when this function is invoked. If a selected 
/// player accepts the invite, the title is notified. 
/// For GDK-based games, the title is notified by invoking the callback function specified 
/// when the <see cref="XGameInviteRegisterForEvent"/> function was invoked. 
/// For games based on other platforms, the title is activated. 
/// For more information, see <see href="live-invites-receive.md">Receiving invites</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerJoinLobby"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionAddLocalUser"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionInviteFriends"/>
STDAPI XblMultiplayerManagerLobbySessionInviteUsers(
    _In_ XblUserHandle user,
    _In_ const uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_opt_z_ const char* contextStringId,
    _In_opt_z_ const char* customActivationContext
) XBL_NOEXCEPT;

/// <summary>
/// Indicates whether there is an active game session.
/// </summary>
/// <returns>Returns true if a game session exists for the lobby session in Multiplayer Manager; otherwise, false.</returns>
/// <seealso cref="XblMultiplayerManagerGameSessionCorrelationId"/>
/// <seealso cref="XblMultiplayerManagerGameSessionSessionReference"/>
STDAPI_(bool) XblMultiplayerManagerGameSessionActive() XBL_NOEXCEPT;

/// <summary>
/// Retrieves the correlation handle for the game session. 
/// </summary>
/// <returns>The correlation handle for the game session, or null if a game session doesn't exist.  
/// The memory for the returned pointer remains valid until the next call to <see cref="XblMultiplayerManagerDoWork"/>.</returns>
/// <remarks>
/// This function retrieves the correlation handle for the game session. The correlation handle serves as 
/// an alias for the game session, allowing a game to refer to a game session by using only the correlation handle. 
/// The correlation handle can be used to query trace logs for entries that relate to the game session.
/// For more information, see <see href="live-multiplayer-concepts.md">Multiplayer concepts overview</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerGameSessionSessionReference"/>
STDAPI_(const char*) XblMultiplayerManagerGameSessionCorrelationId() XBL_NOEXCEPT;

/// <summary>
/// Retrieves the session reference for the game session.  
/// </summary>
/// <returns>A pointer to the session reference for the game session, or null if a game session doesn't exist.  
/// The memory for the returned pointer remains valid until the next call to <see cref="XblMultiplayerManagerDoWork"/>.</returns>
/// <remarks>
/// This function retrieves a pointer to the session reference for a game session, if the game session exists for 
/// the lobby session in Multiplayer Manager. The session reference contains the service configuration ID (SCID),
/// session template, and session name for the game session, and uniquely identifies 
/// the game session in Multiplayer Session Directory (MPSD). For more information about session references, see 
/// <see href="concepts/live-mpsd-details.md">Multiplayer Session advanced topics</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerGameSessionCorrelationId"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionSessionReference"/>
STDAPI_(const XblMultiplayerSessionReference*) XblMultiplayerManagerGameSessionSessionReference() XBL_NOEXCEPT;

/// <summary>
/// Retrieves the number of members in the game session.  
/// </summary>
/// <returns>The number of members that are in the game session.</returns>
/// <remarks>
/// When a friend accepts a game invite, the corresponding member is added to the lobby and the game session members list. 
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionMembersCount"/>
/// <seealso cref="XblMultiplayerManagerGameSessionMembers"/>
STDAPI_(size_t) XblMultiplayerManagerGameSessionMembersCount() XBL_NOEXCEPT;

/// <summary>
/// Retrieves member information for the members in the game session.
/// </summary>
/// <param name="membersCount">The size of the `members` array.  
/// The required size can be obtained by calling <see cref="XblMultiplayerManagerGameSessionMembersCount"/>.</param>
/// <param name="members">A caller-allocated <see cref="XblMultiplayerManagerMember"/> array to write into.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function returns member information for each member in a game session.
/// <para>The member information returned by this function is valid only until <see cref="XblMultiplayerManagerDoWork"/> is called again.</para>
/// </remarks>
STDAPI XblMultiplayerManagerGameSessionMembers(
    _In_ size_t membersCount,
    _Out_writes_(membersCount) XblMultiplayerManagerMember* members
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves member information for the host member in the game session.  
/// </summary>
/// <param name="hostMember">A caller-allocated structure to be populated with member information for the host member.</param>
/// <returns>HRESULT return code for this API operation.  
/// Returns `__HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER)` if a host member doesn't exist.</returns>
/// <remarks>
/// This function retrieves member information about the member that represents the host for a game session.
/// If a game session doesn't exist, or if a host member doesn't exist for the game session, the function 
/// returns `__HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER)`. The host member is defined as the user with the lowest
/// index on the host device. 
/// <para>The information returned by this function is valid only until <see cref="XblMultiplayerManagerDoWork"/> is called again.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerGameSessionMembers"/>
/// <seealso cref="XblMultiplayerManagerGameSessionIsHost"/>
STDAPI XblMultiplayerManagerGameSessionHost(
    _Out_ XblMultiplayerManagerMember* hostMember
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves the custom properties for the game session, as a JSON string.  
/// </summary>
/// <returns>A JSON string that specifies the custom properties for the game session, or null if a game session doesn't exist.  
/// The memory for the returned pointer remains valid until the next call to <see cref="XblMultiplayerManagerDoWork"/>.</returns>
/// <remarks>
/// This function retrieves the custom properties for the game session, represented as a JSON string. These custom properties 
/// can be changed at any time. If custom properties are shared between devices, or may be updated by several devices 
/// at the same time, use the <see cref="XblMultiplayerManagerGameSessionSetSynchronizedProperties"/> function to change custom properties.
/// Otherwise, you can use the <see cref="XblMultiplayerManagerGameSessionSetProperties"/> function to change custom properties.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerLobbySessionPropertiesJson"/>
STDAPI_(const char*) XblMultiplayerManagerGameSessionPropertiesJson() XBL_NOEXCEPT;

/// <summary>
/// Retrieves the session constants associated with the game session.
/// </summary>
/// <returns>A pointer to the session constants for the game session, or null if a game session doesn't exist.  
/// The memory for the returned pointer remains valid until the next call to <see cref="XblMultiplayerManagerDoWork"/>.</returns>
/// <remarks>
/// This function retrieves a pointer to the session constants for a game session, if the game session exists for 
/// the lobby session in Multiplayer Manager. The session constants contain constants, such as session visibility 
/// and session capabilities, defined by the session template used for the game session. Unlike session properties, 
/// session constants can only be set through the session template, and are set at the time the game session is created.
/// For more information about session constants, see <see href="concepts/live-mpsd-details.md">Multiplayer Session advanced topics</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerGameSessionPropertiesJson"/>
/// <seealso cref="XblMultiplayerManagerGameSessionSessionReference"/>
STDAPI_(const XblMultiplayerSessionConstants*) XblMultiplayerManagerGameSessionConstants() XBL_NOEXCEPT;

/// <summary>
/// Indicates whether the specified user is the host for the game session.
/// </summary>
/// <param name="xuid">The Xbox User ID (XUID) of the user.</param>
/// <returns>Returns true if the XUID is the host of the game session; otherwise, false.</returns>
/// <remarks>
/// This function returns false if a game session doesn't exist for the lobby session, or if 
/// the Xbox User ID (XUID) specified in `xuid` isn't the host for the game session. You can 
/// retrieve the host for a game session by calling <see cref="XblMultiplayerManagerGameSessionHost"/>.
/// For more information, see <see href="live-multiplayer-manager-api-overview.md">Multiplayer Manager API overview</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerGameSessionSetSynchronizedHost"/>
STDAPI_(bool) XblMultiplayerManagerGameSessionIsHost(
    _In_ uint64_t xuid
) XBL_NOEXCEPT;

/// <summary>
/// Sets the value of a custom property for the game session.
/// </summary>
/// <param name="name">The name of the custom property to set.</param>
/// <param name="valueJson">The value to assign to the property, as a JSON string.</param>
/// <param name="context">Optional. The application-defined data to correlate the <see cref="XblMultiplayerEvent"/> to the initiating call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function sets the value, represented as a JSON string, of a custom property for the game session. Custom properties 
/// can be changed at any time. Changes are batched and written to the service when <see cref="XblMultiplayerManagerDoWork"/> is called.
/// If custom properties are shared between devices, or may be updated by several devices 
/// at the same time, use the <see cref="XblMultiplayerManagerGameSessionSetSynchronizedProperties"/> function to change custom properties.
/// Otherwise, you can use this function to change custom properties.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerGameSessionSetSynchronizedHost"/>
STDAPI XblMultiplayerManagerGameSessionSetProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Sets the value of a custom property for the game session, using `XblMultiplayerSessionWriteMode::SynchronizedUpdate`.
/// </summary>
/// <param name="name">The name of the custom property to set.</param>
/// <param name="valueJson">The value to assign to the property, as a JSON string.</param>
/// <param name="context">Optional. The application-defined data to correlate the <see cref="XblMultiplayerEvent"/> to the initiating call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function sets the value, represented as a JSON string, of a custom property for the game session. Custom properties 
/// can be changed at any time. If custom properties are shared between devices, or may be updated by several devices 
/// at the same time, use this function to ensure atomicity and resolve any conflicts between devices while changing the values of those custom properties. 
/// If a custom property isn't shared across devices, use the <see cref="XblMultiplayerManagerGameSessionSetProperties"/> function instead 
/// to change the value of that custom property.
/// <para>The service may reject the request to change the custom property if a race condition occurs due to a conflict.
/// If the request is rejected, the service returns `HTTP_E_STATUS_PRECOND_FAILED`. If a conflict occurs, re-evaluate the need to 
/// change the custom property and, if needed, call this function again to re-submit the request.</para>
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para> 
/// </remarks>
/// <seealso cref="XblMultiplayerManagerGameSessionSetSynchronizedHost"/>
/// <seealso cref="XblMultiplayerEvent"/>
STDAPI XblMultiplayerManagerGameSessionSetSynchronizedProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Sets the host for the game session, using `XblMultiplayerSessionWriteMode::SynchronizedUpdate`.
/// </summary>
/// <param name="deviceToken">The device token of the host.</param>
/// <param name="context">Optional. The application-defined data to correlate the <see cref="XblMultiplayerEvent"/> to the initiating call.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function sets the host for the game session. Use this function to ensure atomicity and resolve any conflicts between devices 
/// trying to set the host at the same time. 
/// <para>The service may reject the request to set the host if a race condition occurs due to a conflict.
/// If the request is rejected, the service returns `HTTP_E_STATUS_PRECOND_FAILED`. If a conflict occurs, re-evaluate the need to 
/// change the host and, if needed, call this function again to re-submit the request.</para>
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::SynchronizedHostWriteCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para>
/// Note that host device tokens are generated from a session member's secure device address, so ensure that the secure device address is set for the 
/// desired host prior to calling this method.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerGameSessionSetSynchronizedProperties"/>
/// <seealso cref="XblMultiplayerEvent"/>
/// <seealso cref="XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress"/>
STDAPI XblMultiplayerManagerGameSessionSetSynchronizedHost(
    _In_ const char* deviceToken,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Joins an Xbox user to a lobby session.  
/// </summary>
/// <param name="handleId">The activity handle for the lobby session.</param>
/// <param name="user">The user handle of the user joining the lobby session.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function joins the Xbox user specified in <em>user</em> to the lobby session specified in <em>handleId</em>. 
/// The activity handle for the lobby session is typically retrieved either from a game invite or from the `HandleId` value 
/// of another user's <see cref="XblMultiplayerActivityDetails"/>, by calling <see cref="XblMultiplayerGetActivitiesForUsersAsync"/>. 
/// For more information about multiplayer activities, see <see href="live-mpa-activities.md">Activities</see>.
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::JoinLobbyCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para>
/// <para>When attempting to join a lobby session, the service returns `HTTP_E_STATUS_BAD_REQUEST` if the server is full.</para>
/// <para>After joining, you can set the properties for the lobby session by calling  
/// <see cref="XblMultiplayerManagerLobbySessionSetSynchronizedProperties"/>, or you can set the host for the lobby session by
/// calling <see cref="XblMultiplayerManagerLobbySessionSetSynchronizedHost"/> if the lobby session doesn't already have a host.</para>
/// <para>You can also send an invite to another user by calling either 
/// <see cref="XblMultiplayerManagerLobbySessionInviteUsers"/> or <see cref="XblMultiplayerManagerLobbySessionInviteFriends"/>.
/// If you don't need a lobby session, and if you haven't added local users by calling <see cref="XblMultiplayerManagerLobbySessionAddLocalUser"/>, 
/// you can instead call <see cref="XblMultiplayerManagerJoinGame"/> and specify the list of users to join the game.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerJoinability"/>
/// <seealso cref="XblMultiplayerManagerJoinGameFromLobby"/>
STDAPI XblMultiplayerManagerJoinLobby(
    _In_z_ const char* handleId,
    _In_ XblUserHandle user
) XBL_NOEXCEPT;

/// <summary>
/// Creates a new game session for the lobby session, or joins an existing game session if one exists for the lobby session.
/// </summary>
/// <param name="sessionTemplateName">The name of the session template for the game session to be based on.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function creates a new game session and adds the current members of the lobby session to the game session, 
/// if a game session doesn't already exist. If a new user joins the lobby session after a game session is already 
/// created, this function finds the existing game session in Multiplayer Session Directory (MPSD) by using a 
/// transfer handle in the lobby session, and then adds the new user to the game session using that transfer handle.
/// For more information, see <see href="live-multiplayer-concepts.md">Multiplayer concepts overview</see>.
/// If a lobby session doesn't exist, likely because <see cref="XblMultiplayerManagerInitialize"/> wasn't called before 
/// calling this function, an error occurs. An error also occurs if matchmaking is in progress. 
/// This function does not migrate existing lobby session properties to the game session.  
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::JoinGameCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para>
/// <para>When attempting to join a lobby session, the service returns `HTTP_E_STATUS_BAD_REQUEST` if the server is full.</para>
/// <para>After joining, you can set the properties for the game session by calling  
/// <see cref="XblMultiplayerManagerGameSessionSetProperties"/> or <see cref="XblMultiplayerManagerGameSessionSetSynchronizedProperties"/>, 
/// or you can set the host for the game session by calling <see cref="XblMultiplayerManagerGameSessionSetSynchronizedHost"/>.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerJoinability"/>
/// <seealso cref="XblMultiplayerManagerJoinGame"/>
/// <seealso cref="XblMultiplayerManagerJoinLobby"/>
STDAPI XblMultiplayerManagerJoinGameFromLobby(
    _In_z_ const char* sessionTemplateName
) XBL_NOEXCEPT;

/// <summary>
/// Joins a game session, using the globally unique session name.  
/// </summary>
/// <param name="sessionName">The globally unique session name for the game session.</param>
/// <param name="sessionTemplateName">The name of the session template for the game session to be based on.</param>
/// <param name="xuids">An array of Xbox User IDs (XUIDs) that represents the users you want to be part of the game.</param>
/// <param name="xuidsCount">The number of elements in the array specified for `xuids`.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function joins a list of Xbox users, specified in `xuids`, to the game session identified by the globally 
/// unique session name specified in `sessionName.` You can get the globally unique session name from the results of the title's third-party matchmaking, 
/// and you should call this function for all clients that need to join the game.
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::JoinGameCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para>
/// <para>When attempting to join a game session, the service returns `HTTP_E_STATUS_BAD_REQUEST` if the server is full.</para>
/// <para>After joining, you can set the properties for the game session by calling  
/// <see cref="XblMultiplayerManagerGameSessionSetProperties"/> or <see cref="XblMultiplayerManagerGameSessionSetSynchronizedProperties"/>, 
/// or you can set the host for the game session by calling <see cref="XblMultiplayerManagerGameSessionSetSynchronizedHost"/>.</para>
/// </remarks>
STDAPI XblMultiplayerManagerJoinGame(
    _In_z_ const char* sessionName,
    _In_z_ const char* sessionTemplateName,
    _In_opt_ const uint64_t* xuids,
    _In_ size_t xuidsCount
) XBL_NOEXCEPT;

/// <summary>
/// Leaves the game session, returning the Xbox user and all other local users to the lobby session.
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function removes the Xbox user from the game session and returns the user back to the lobby session. The game session is set to null, 
/// and all local users are also removed from the game session and returned to the lobby session. Any matchmaking request in progress is 
/// canceled when this function is called.  
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::LeaveGameCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para>
/// <para>After leaving, you can join a different game by calling either <see cref="XblMultiplayerManagerJoinGame"/> or
/// <see cref="XblMultiplayerManagerJoinGameFromLobby"/>.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerJoinGame"/>
/// <seealso cref="XblMultiplayerManagerJoinLobby"/>
STDAPI XblMultiplayerManagerLeaveGame() XBL_NOEXCEPT;

/// <summary>
/// Submits a matchmaking request to the server.
/// </summary>
/// <param name="hopperName">The name of the hopper for this request.</param>
/// <param name="attributesJson">Optional. The attributes of the match ticket for this request, as a JSON string.</param>
/// <param name="timeoutInSeconds">The maximum time, in seconds, to wait for users to join the match.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// This function submits a matchmaking request for the lobby session to Multiplayer Manager (MPM). Before you can use this function, 
/// you must first configure hoppers in the service configuration for your title. A hopper defines the rules that SmartMatch uses 
/// to match players. 
/// For more information about hoppers, see <see href="live-matchmaking-overview.md">Matchmaking overview</see>.
/// If a lobby session doesn't exist, likely because <see cref="XblMultiplayerManagerInitialize"/> wasn't called, or if local users weren't added
/// to the lobby session before calling this function, an error occurs. An error also occurs if matchmaking is already in progress. 
/// <para>The result of this function is delivered as a multiplayer event with an event type set to `XblMultiplayerEventType::FindMatchCompleted`. 
/// You can call <see cref="XblMultiplayerManagerDoWork"/> to retrieve multiplayer events.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerEstimatedMatchWaitTime"/>
STDAPI XblMultiplayerManagerFindMatch(
    _In_z_ const char* hopperName,
    _In_opt_z_ const char* attributesJson,
    _In_ uint32_t timeoutInSeconds
) XBL_NOEXCEPT;

/// <summary>
/// Cancels the match request on the server, if one exists.
/// </summary>
/// <returns></returns>
/// <remarks>
/// This function cancels a previously submitted match ticket. This function is ignored if the status of the match ticket 
/// is set to `XblMultiplayerMatchStatus::None`, `XblMultiplayerMatchStatus::Expired`, `XblMultiplayerMatchStatus::Canceled`, or 
/// `XblMultiplayerMatchStatus::Failed`, or if a different host submitted the match ticket.
/// <para>If this function is called, the status of the match ticket is set to `XblMultiplayerMatchStatus::Canceling` until 
/// the match ticket is canceled on the server.</para> 
/// <para>For more information about match tickets, see <see href="live-multiplayer-concepts.md">Multiplayer concepts overview</see>.</para>
/// </remarks>
/// <seealso cref="XblMultiplayerManagerFindMatch"/>
/// <seealso cref="XblMultiplayerManagerMatchStatus"/>
STDAPI_(void) XblMultiplayerManagerCancelMatch() XBL_NOEXCEPT;

/// <summary>
/// Provides the current status of matchmaking.
/// </summary>
/// <returns>The current status of matchmaking. 'XblMultiplayerMatchStatus::None' if no matchmaking is in progress.</returns>
STDAPI_(XblMultiplayerMatchStatus) XblMultiplayerManagerMatchStatus() XBL_NOEXCEPT;

/// <summary>
/// Retrieves the estimated wait time, in seconds, to complete a matchmaking request in progress.
/// </summary>
/// <returns>The estimated wait time, in seconds.</returns>
/// <remarks>
/// Call this function only after the <see cref="XblMultiplayerManagerFindMatch"/> function has been called 
/// to submit a matchmaking request. The matchmaking request uses SmartMatch to find an existing game that has enough open 
/// player slots for all the members in the lobby session. If a matchmaking request isn't in progress, 
/// this function returns zero (0) seconds. For more information about finding a multiplayer game, 
/// see <see href="live-play-multiplayer-with-matchmaking.md">Enable finding a multiplayer game by using SmartMatch using Multiplayer Manager</see>.
/// </remarks>
/// <seealso cref="XblMultiplayerManagerFindMatch"/>
STDAPI_(uint32_t) XblMultiplayerManagerEstimatedMatchWaitTime() XBL_NOEXCEPT;

/// <summary>
/// Indicates whether the game should auto-fill open slots during gameplay.
/// </summary>
/// <returns>Returns true if the game should auto-fill open slots during gameplay; otherwise, false.</returns>
/// <remarks>Call the <see cref="XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking"/> function to discover  
/// whether the game should use matchmaking to auto-fill open slots during gameplay. You can also call 
/// the <see cref="XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking"/> function to specify whether the game 
/// should auto-fill open slots during gameplay. For more information about matchmaking, 
/// see <see href="live-matchmaking-overview.md">Matchmaking overview</see>.</remarks>
STDAPI_(bool) XblMultiplayerManagerAutoFillMembersDuringMatchmaking() XBL_NOEXCEPT;

/// <summary>
/// Sets if matchmaking should auto fill open slots during gameplay.  
/// This can be changed anytime.
/// </summary>
/// <param name="autoFillMembers">Set true, to search for members during matchmaking if the game has open slots.  
/// Set false, to not allow auto fill.</param>
/// <returns></returns>
STDAPI_(void) XblMultiplayerManagerSetAutoFillMembersDuringMatchmaking(
    _In_ bool autoFillMembers
) XBL_NOEXCEPT;

/// <summary>
/// Sets json representing QoS measurements between the current user and a list of remote clients.  
/// This is only used when the title is manually managing QoS.
/// </summary>
/// <param name="measurementsJson">
/// Json representing the QoS measurements.  
/// Example Json:
/// "e69c43a8": {               // remote client deviceToken
///   "latency": 5953,          // Milliseconds
///   "bandwidthDown" : 19342,  // Kilobits per second
///   "bandwidthUp" : 944,      // Kilobits per second
///   "custom" : { }
/// },
/// ...                         // additional remote client entries
/// </param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerManagerSetQosMeasurements(
    _In_z_ const char* measurementsJson
) XBL_NOEXCEPT;

/// <summary>
/// Indicates which users can join your lobby session.
/// </summary>
/// <returns>The joinability setting for your lobby session.</returns>
STDAPI_(XblMultiplayerJoinability) XblMultiplayerManagerJoinability() XBL_NOEXCEPT;

/// <summary>
/// Restricts who can join the game.
/// </summary>
/// <param name="joinability">The joinability value you want to set.</param>
/// <param name="context">The application-defined data to correlate the XblMultiplayerEvent to the initiating call. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Defaults to JoinableByFriends, meaning only local users and users who are followed 
/// by an existing member of the lobby can join without an invite.  
/// The result is delivered via XblMultiplayerEvent of type JoinabilityStateChanged
/// through XblMultiplayerManagerDoWork().  
/// Changes are batched and written to the service on the next XblMultiplayerManagerDoWork.  
/// All session properties and members contain updated response returned from the server upon 
/// calling XblMultiplayerManagerDoWork.
/// </remarks>
STDAPI XblMultiplayerManagerSetJoinability(
    _In_ XblMultiplayerJoinability joinability,
    _In_opt_ void* context
) XBL_NOEXCEPT;

}
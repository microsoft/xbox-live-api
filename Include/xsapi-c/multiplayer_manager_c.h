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
    /// Indicates that the XblMultiplayerManagerJoinLobby() operation has completed.  
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
/// <returns>Returns true if on same device, false if not on same device.</returns>
STDAPI_(bool) XblMultiplayerManagerMemberAreMembersOnSameDevice(
    _In_ const XblMultiplayerManagerMember* first,
    _In_ const XblMultiplayerManagerMember* second
) XBL_NOEXCEPT;


/// <summary>
/// A handle to multiplayer event args that can be used to additional information depending on the type of event.
/// </summary>
typedef struct XblMultiplayerEventArgs* XblMultiplayerEventArgsHandle;

/// <summary>
/// A multiplayer event that will be returned from XblMultiplayerManagerDoWork().
/// </summary>
typedef struct XblMultiplayerEvent
{
    /// <summary>
    /// The error code indicating the result of the operation.
    /// </summary>
    HRESULT Result;

    /// <summary>
    /// Returns call specific debug information if join fails.  
    /// It is not localized, so only use for debugging purposes.
    /// </summary>
    _Field_z_ const char* ErrorMessage;

    /// <summary>
    /// A pointer to the application-defined data passed into the initiating method.
    /// </summary>
    void* Context;

    /// <summary>
    /// Type of the event triggered.
    /// </summary>
    XblMultiplayerEventType EventType;

    /// <summary>
    /// A handle to the event args.
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
/// Event args returned for XblMultiplayerEventType::PerformQosMeasurements events.
/// </summary>
typedef struct XblMultiplayerPerformQoSMeasurementsArgs
{
    /// <summary>
    /// An array of remote clients to perform QoS measurements on.
    /// </summary>
    const XblMultiplayerConnectionAddressDeviceTokenPair* remoteClients;

    /// <summary>
    /// The number of items in the remoteClients array.
    /// </summary>
    size_t remoteClientsSize;
} XblMultiplayerPerformQoSMeasurementsArgs;

/// <summary>
/// Retrieves additional information for XblMultiplayerEventType::UserAdded, XblMultiplayerEventType::UserRemoved,
/// and XblMultiplayerEventType::JoinLobbyCompleted events.
/// </summary>
/// <param name="argsHandle">The event args handle from the XblMultiplayerEvent.</param>
/// <param name="xuid">Passes back the Xbox User ID for the following events:  
/// XblMultiplayerEventType::UserAdded - Xbox User ID of the member that that was added.  
/// XblMultiplayerEventType::UserRemoved - Xbox User ID of the member that that was removed.  
/// XblMultiplayerEventType::JoinLobbyCompleted - Xbox User ID of the member that was invited.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerEventArgsXuid(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ uint64_t* xuid
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for XblMultiplayerEventType::MemberJoined and XblMultiplayerEventType::MemberLeft events.
/// </summary>
/// <param name="argsHandle">The event args handle from the XblMultiplayerEvent.</param>
/// <param name="memberCount">Passes back the required size of the members array for <see cref="XblMultiplayerEventArgsMembers"/>.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerEventArgsMembersCount(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ size_t* memberCount
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for XblMultiplayerEventType::MemberJoined and XblMultiplayerEventType::MemberLeft events.
/// </summary>
/// <param name="argsHandle">The event args handle from the XblMultiplayerEvent.</param>
/// <param name="membersCount">The size of the members array.</param>
/// <param name="members">A caller allocated array to pass back the following event results:  
/// XblMultiplayerEventType::MemberJoined - A list of members that joined the game.  
/// XblMultiplayerEventType::MemberLeft - A list of members that left the game.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerEventArgsMembers(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _In_ size_t membersCount,
    _Out_writes_(membersCount) XblMultiplayerManagerMember* members
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for XblMultiplayerEventType::HostChanged and XblMultiplayerEventType::MemberPropertyChanged events.
/// </summary>
/// <param name="argsHandle">The event args handle from the XblMultiplayerEvent.</param>
/// <param name="member">Passes back a given member for the following event results:
/// XblMultiplayerEventType::HostChanged - The new host member.  If an existing host leaves, argsHandle will be nullptr.  
/// XblMultiplayerEventType::MemberPropertyChanged - The member whose property changed.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerEventArgsMember(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ XblMultiplayerManagerMember* member
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for XblMultiplayerEventType::MemberPropertyChanged 
/// and XblMultiplayerEventType::SessionPropertyChanged events.
/// </summary>
/// <param name="argsHandle">The event args handle from the XblMultiplayerEvent.</param>
/// <param name="properties">Passes back a pointer for the following event results:
/// XblMultiplayerEventType::MemberPropertyChanged - The JSON of the property that changed.  
/// XblMultiplayerEventType::SessionPropertyChanged - The JSON of the property that changed.  
/// The memory for the pointer remains valid for the life of the XblMultiplayerEventArgsHandle object until it is closed.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerEventArgsPropertiesJson(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ const char** properties
) XBL_NOEXCEPT;

/// <summary>
/// Retrieves additional information for XblMultiplayerEventType::FindMatchCompleted events.
/// </summary>
/// <param name="argsHandle">The event args handle from the XblMultiplayerEvent.</param>
/// <param name="matchStatus">A caller allocated struct that passes back the current matchmaking status.</param>
/// <param name="initializationFailureCause">A caller allocated struct that passes back the cause of why 
/// the initialization failed, or XblMultiplayerMeasurementFailure::None if there was no failure.  
/// Set when transitioning out of the "joining" or "measuring" stage if this member doesn't pass.</param>
/// <returns>HRESULT return code for this API operation.</returns>
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
/// Retrieves additional information for XblMultiplayerEventType::PerformQosMeasurements events.
/// </summary>
/// <param name="argsHandle">The event args handle from the XblMultiplayerEvent.</param>
/// <param name="performQoSMeasurementsArgs">A caller allocated struct that passes back the 
/// args containing info about the remote clients for which QoS info is needed.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerEventArgsPerformQoSMeasurements(
    _In_ XblMultiplayerEventArgsHandle argsHandle,
    _Out_ XblMultiplayerPerformQoSMeasurementsArgs* performQoSMeasurementsArgs
) XBL_NOEXCEPT;

/// <summary>
/// Initializes MultiplayerManager.
/// </summary>
/// <param name="lobbySessionTemplateName">The name of the template for the lobby session to be based on.</param>
/// <param name="asyncQueue">Queue where all MultiplayerManager work should be scheduled.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// There are two sessions that are maintained when using multiplayer manager.  
/// One is the "lobby session" which is where friends you invite will join.  
/// Another is the "game session" which contains people that your lobby has been matched with.
/// </remarks>
STDAPI XblMultiplayerManagerInitialize(
    _In_z_ const char* lobbySessionTemplateName,
    _In_opt_ XTaskQueueHandle asyncQueue
) XBL_NOEXCEPT;

/// <summary>
/// Ensures proper game state updates are maintained between the title and the Xbox Live Multiplayer Service.  
/// To ensure best performance, XblMultiplayerManagerDoWork() must be called frequently, such as once per frame.
/// </summary>
/// <param name="multiplayerEvents">An array of all events for the game to handle.  
/// Will be null if no events are triggered during this update.  
/// The returned events are owned by MultiplayerManager and are valid until XblMultiplayerManagerDoWork is called again.</param>
/// <param name="multiplayerEventsCount">Passes back the number of events in the multiplayerEvents array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Title needs to be thread safe when calling XblMultiplayerManagerDoWork() since this is when the state will change.  
/// For example, if you looping through the list of members on a different thread than your calling 
/// XblMultiplayerManagerDoWork() on, it may change when XblMultiplayerManagerDoWork() is called.
/// </remarks>
STDAPI XblMultiplayerManagerDoWork(
    _Deref_out_opt_ const XblMultiplayerEvent** multiplayerEvents,
    _Out_ size_t* multiplayerEventsCount
) XBL_NOEXCEPT;

/// <summary>
/// A unique ID to the session used to query trace logs for entries that relate to the session.
/// </summary>
/// <param name="correlationId">Passes back the unique ID given to the session.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerManagerLobbySessionCorrelationId(
    _Out_ XblGuid* correlationId
) XBL_NOEXCEPT;

/// <summary>
/// Returns identifying information for the lobby session.
/// </summary>
/// <param name="sessionReference">Passes back a reference that uniquely identifies the session.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerManagerLobbySessionSessionReference(
    _Out_ XblMultiplayerSessionReference* sessionReference
) XBL_NOEXCEPT;

/// <summary>
/// The number of members that are local to the device.
/// </summary>
/// <returns>The local member count for the lobby session.</returns>
STDAPI_(size_t) XblMultiplayerManagerLobbySessionLocalMembersCount() XBL_NOEXCEPT;

/// <summary>
/// A collection of members that are local to this device.
/// </summary>
/// <param name="localMembersCount">The size of the localMembers array.  
/// The required size can be obtained with XblMultiplayerLobbySessionLocalMembersCount.</param>
/// <param name="localMembers">A caller allocated array that passes back the list of local members.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerManagerLobbySessionLocalMembers(
    _In_ size_t localMembersCount,
    _Out_writes_(localMembersCount) XblMultiplayerManagerMember* localMembers
) XBL_NOEXCEPT;

/// <summary>
/// A number of members that are in the lobby.  
/// When a friend accepts a game invite, members will be added to the lobby.
/// </summary>
/// <returns>The number of members that are in the lobby session.</returns>
STDAPI_(size_t) XblMultiplayerManagerLobbySessionMembersCount() XBL_NOEXCEPT;

/// <summary>
/// A collection of members that are in the lobby.
/// </summary>
/// <param name="membersCount">The size of the members array. 
/// The required size can be obtained with XblMultiplayerLobbySessionMembersCount.</param>
/// <param name="members">A caller allocated array that passes back the list of lobby members.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerManagerLobbySessionMembers(
    _In_ size_t membersCount,
    _Out_writes_(membersCount) XblMultiplayerManagerMember* members
) XBL_NOEXCEPT;

/// <summary>
/// Returns the host member for the lobby.  
/// The host is defined as the user with the lowest index on the host device.
/// </summary>
/// <param name="hostMember">Passes back the host member.</param>
/// <returns>HRESULT return code for this API operation.  
/// Will return __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER) if there is no host.</returns>
STDAPI XblMultiplayerManagerLobbySessionHost(
    _Out_ XblMultiplayerManagerMember* hostMember
) XBL_NOEXCEPT;

/// <summary>
/// JSON string that specify the custom properties for the game.
/// </summary>
/// <returns>The JSON string that specifies the custom properties for the game.  
/// The memory for the returned string pointer remains valid until the next call to XblMultiplayerManagerDoWork.</returns>
STDAPI_(const char*) XblMultiplayerManagerLobbySessionPropertiesJson() XBL_NOEXCEPT;

/// <summary>
/// A set of constants associated with this session.  These can only be set through the session template.
/// </summary>
/// <returns>A pointer to the constant values for the multiplayer session.  
/// The pointer returned remains valid until the next call to XblMultiplayerManagerDoWork.</returns>
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
/// Hosts a new lobby when the first user is added.  
/// For all other users, they will be added to the existing lobby as secondary users.  
/// This API will also advertise the lobby for friends to join.
/// </summary>
/// <param name="user">The associated system User.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// You can send invites, set lobby properties, and access lobby members only once you've added the local user.  
/// Changes are batched and written to the service on the next XblMultiplayerManagerDoWork().  
/// All session properties and members contain updated response returned from the server 
/// upon calling XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerLobbySessionAddLocalUser(
    _In_ XblUserHandle user
) XBL_NOEXCEPT;

/// <summary>
/// Removes the local user from the lobby and game session.  
/// After this method is called, if no local users are active, title will not be able to perform 
/// any further multiplayer operations.
/// </summary>
/// <param name="user">The associated system User.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// You can join another game or re-add the local user.  
/// Changes are batched and written to the service on the next XblMultiplayerManagerDoWork().  
/// All session properties and members contain updated response returned from the server 
/// upon calling XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerLobbySessionRemoveLocalUser(
    _In_ XblUserHandle user
) XBL_NOEXCEPT;

/// <summary>
/// Set a custom property on the local member to the specified JSON string.
/// </summary>
/// <param name="user">The associated system User you want to set the property for.</param>
/// <param name="name">The name of the property to set.</param>
/// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
/// <param name="context">The application-defined data to correlate the XblMultiplayerEvent to the initiating call. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The result is delivered via XblMultiplayerEvent of type LocalMemberPropertyWriteCompleted 
/// through XblMultiplayerManagerDoWork().  
/// Changes are batched and written to the service on the next XblMultiplayerManagerDoWork().  
/// All session properties and members contain updated response returned from the server 
/// upon calling XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerLobbySessionSetLocalMemberProperties(
    _In_ XblUserHandle user,
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Delete a custom property on the local member.
/// </summary>
/// <param name="user">The associated system User you want to delete the property for.</param>
/// <param name="name">The name of the property to delete.</param>
/// <param name="context">The application-defined data to correlate the XblMultiplayerEvent to the initiating call. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The result is delivered via XblMultiplayerEvent of type LocalMemberPropertyWriteCompleted 
/// through XblMultiplayerManagerDoWork().  
/// Changes are batched and written to the service on the next XblMultiplayerManagerDoWork().  
/// All session properties and members contain updated response returned from the server 
/// upon calling XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties(
    _In_ XblUserHandle user,
    _In_z_ const char* name,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Set connection address for the local member.  
/// The address can be used for network and secure socket connection.
/// </summary>
/// <param name="user">The associated system User you want to set the property for.</param>
/// <param name="connectionAddress">The network connection address to set.</param>
/// <param name="context">The application-defined data to correlate the XblMultiplayerEvent to the initiating call. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The result is delivered via XblMultiplayerEvent of type XblMultiplayerEventType::LocalMemberConnectionAddressWriteCompleted 
/// through XblMultiplayerManagerDoWork().  
/// Changes are batched and written to the service on the next XblMultiplayerManagerDoWork().  
/// All session properties and members contain updated response returned from the server 
/// upon calling XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(
    _In_ XblUserHandle user,
    _In_z_ const char* connectionAddress,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Whether or not the Xbox User ID is the host.
/// </summary>
/// <param name="xuid">The Xbox User ID of the user.</param>
/// <returns>Returns true if Xuid is host, false if Xuid is not host.</returns>
STDAPI_(bool) XblMultiplayerManagerLobbySessionIsHost(
    _In_ uint64_t xuid
) XBL_NOEXCEPT;

/// <summary>
/// Set a custom game property to the specified JSON string.
/// </summary>
/// <param name="name">The name of the property to set.</param>
/// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
/// <param name="context">The application-defined data to correlate the XblMultiplayerEvent to the initiating call. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The result is delivered via XblMultiplayerEvent of type XblMultiplayerEventType::SessionPropertyWriteCompleted 
/// through XblMultiplayerManagerDoWork().  
/// Changes are batched and written to the service on the next XblMultiplayerManagerDoWork().  
/// All session properties and members contain updated response returned from the server 
/// upon calling XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerLobbySessionSetProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Sets a custom property to the specified JSON string using XblMultiplayerSessionWriteMode::SynchronizedUpdate.
/// </summary>
/// <param name="name">The name of the property to set.</param>
/// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
/// <param name="context">The application-defined data to correlate the XblMultiplayerEvent to the initiating call. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Use this method to resolve any conflicts between devices while trying to set properties to a shared portion 
/// that other devices can also modify.  It ensures that updates to the session are atomic.  
/// If writing to non-sharable properties, use XblMultiplayerLobbySessionSetProperties() instead.  
/// The service may reject your request if a race condition occurred (due to a conflict) resulting in HTTP_E_STATUS_PRECOND_FAILED (HTTP status 412).  
/// To resolve this, evaluate the need to write again and re-submit if needed.  
/// The result is delivered via XblMultiplayerEvent of type XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted
/// through XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerLobbySessionSetSynchronizedProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Sets the host for the game using XblMultiplayerSessionWriteMode::SynchronizedUpdate.
/// </summary>
/// <param name="deviceToken">The device token of the host.</param>
/// <param name="context">The application-defined data to correlate the XblMultiplayerEvent to the initiating call. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Use this method to resolve any conflicts between devices trying to set the host at the same time.  
/// It ensures that updates to the session are atomic.  
/// The service may reject your request if a race condition occurred(due to a conflict) resulting in HTTP_E_STATUS_PRECOND_FAILED (HTTP status 412).  
/// To resolve this, evaluate the need to write again and re-submit if needed.  
/// The result is delivered via XblMultiplayerEvent of type XblMultiplayerEventType::SynchronizedHostWriteCompleted 
/// through XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerLobbySessionSetSynchronizedHost(
    _In_ const char* deviceToken,
    _In_opt_ void* context
) XBL_NOEXCEPT;

#if HC_PLATFORM_IS_MICROSOFT
/// <summary>
/// Displays the invite UI and allows the user to select people from the user's people list and invite them to join the user's party.  
/// If a user accepts that notification the title will be notified.
/// </summary>
/// <param name="requestingUser">The user who is sending the invite.</param>
/// <param name="contextStringId">The custom context string ID.  
/// This string ID is defined during Xbox Live ingestion to identify the invitation text that is additional to the standard invitation text.  
/// The ID string must be prefixed with "///". (Optional)</param>
/// <param name="customActivationContext">The activation context string.  
/// A game defined string that is passed to the invited game client and interpreted as desired. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// In the case of GDK based games, the title will be notified via the callback set by XGameInviteRegisterForEvent.  
/// For other platforms, the title will be activated.
/// </remarks>
STDAPI XblMultiplayerManagerLobbySessionInviteFriends(
    _In_ XblUserHandle requestingUser,
    _In_opt_z_ const char* contextStringId,
    _In_opt_z_ const char* customActivationContext
) XBL_NOEXCEPT;
#endif

/// <summary>
/// Invites the specified users to a game without additional UI.  
/// This will result in a notification being shown to each invited user using standard invite text.  
/// If a user accepts that notification the title will be notified.
/// </summary>
/// <param name="user">The associated system User.</param>
/// <param name="xuids">The array of xbox user IDs who will be invited.</param>
/// <param name="xuidsCount">The size of the xuids array.</param>
/// <param name="contextStringId">The custom context string ID.  This string ID is defined 
/// during Xbox Live ingestion to identify the invitation text that is additional to the standard 
/// invitation text. The ID string must be prefixed with "///". (Optional)</param>
/// <param name="customActivationContext">The activation context string.  
/// A game defined string that is passed to the invited game client and interpreted as desired. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// In the case of GDK based games, the title will be notified via the callback 
/// set by XGameInviteRegisterForEvent.  
/// For other platforms, the title will be activated.
/// </remarks>
STDAPI XblMultiplayerManagerLobbySessionInviteUsers(
    _In_ XblUserHandle user,
    _In_ const uint64_t* xuids,
    _In_ size_t xuidsCount,
    _In_opt_z_ const char* contextStringId,
    _In_opt_z_ const char* customActivationContext
) XBL_NOEXCEPT;

/// <summary>
/// Returns whether or not there is an active multiplayer manager game session.
/// </summary>
/// <returns>Returns true if active game session, false if not active game session.</returns>
STDAPI_(bool) XblMultiplayerManagerGameSessionActive() XBL_NOEXCEPT;

/// <summary>
/// A unique ID to the session used to query trace logs for entries that relate to the session.  
/// Returns null if a game session has not yet been established.
/// </summary>
/// <returns>The unique ID given to the game session.  
/// The memory for the returned string pointer remains valid until the next call to XblMultiplayerManagerDoWork.</returns>
STDAPI_(const char*) XblMultiplayerManagerGameSessionCorrelationId() XBL_NOEXCEPT;

/// <summary>
/// Object containing identifying information for the session.  
/// Returns null if a game session has not yet been established.
/// </summary>
/// <returns>A pointer to the the multiplayer session reference.  
/// The memory for the returned string pointer remains valid until the next call to XblMultiplayerManagerDoWork.</returns>
STDAPI_(const XblMultiplayerSessionReference*) XblMultiplayerManagerGameSessionSessionReference() XBL_NOEXCEPT;

/// <summary>
/// A number of members that are in the game.  
/// When a friend accepts a game invite, members will be added to the lobby and the game session members list.
/// </summary>
/// <returns>The number of members that are in the game.</returns>
STDAPI_(size_t) XblMultiplayerManagerGameSessionMembersCount() XBL_NOEXCEPT;

/// <summary>
/// A collection of members that are in the game.
/// </summary>
/// <param name="membersCount">The size of the members array.  
/// The required size can be obtained with XblMultiplayerManagerGameSessionMembersCount.</param>
/// <param name="members">A caller allocated array of XblMultiplayerManagerMember to write into.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblMultiplayerManagerGameSessionMembers(
    _In_ size_t membersCount,
    _Out_writes_(membersCount) XblMultiplayerManagerMember* members
) XBL_NOEXCEPT;

/// <summary>
/// Returns the host member for the game.  
/// The host is defined as the user with the lowest index on the host device.
/// </summary>
/// <param name="hostMember">A caller allocated struct to be filled with the host member reference to populate.</param>
/// <returns>HRESULT return code for this API operation.  
/// Will return __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER) if there is no host.</returns>
STDAPI XblMultiplayerManagerGameSessionHost(
    _Out_ XblMultiplayerManagerMember* hostMember
) XBL_NOEXCEPT;

/// <summary>
/// JSON string that specify the custom properties for the game.  
/// These can be changed anytime.
/// </summary>
/// <returns>A JSON string that specifies the custom properties for the game.  
/// The memory for the returned string pointer remains valid until the next call to XblMultiplayerManagerDoWork.  
/// Will return null if the game session has not yet been established.</returns>
STDAPI_(const char*) XblMultiplayerManagerGameSessionPropertiesJson() XBL_NOEXCEPT;

/// <summary>
/// A set of constants associated with this session.  
/// These can only be set through the session template.
/// </summary>
/// <returns>A pointer to the constant values for the multiplayer session.  
/// The memory for the returned string pointer remains valid until the next call to XblMultiplayerManagerDoWork.  
/// Will return null if the game session has not yet been established.</returns>
STDAPI_(const XblMultiplayerSessionConstants*) XblMultiplayerManagerGameSessionConstants() XBL_NOEXCEPT;

/// <summary>
/// Whether or not the Xbox User ID is the host.
/// </summary>
/// <param name="xuid">The Xbox User ID of the user.</param>
/// <returns>Returns true if Xuid is host, false if Xuid is not host.</returns>
STDAPI_(bool) XblMultiplayerManagerGameSessionIsHost(
    _In_ uint64_t xuid
) XBL_NOEXCEPT;

/// <summary>
/// Set a custom game property to the specified JSON string.
/// </summary>
/// <param name="name">The name of the property to set.</param>
/// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
/// <param name="context">The application-defined data to correlate the XblMultiplayerEvent to the initiating call. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Changes are batched and written to the service on the next XblMultiplayerManagerDoWork().  
/// All session properties and members contain updated response returned from the server 
/// upon calling XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerGameSessionSetProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Sets a custom property to the specified JSON string using XblMultiplayerSessionWriteMode::SynchronizedUpdate.
/// </summary>
/// <param name="name">The name of the property to set.</param>
/// <param name="valueJson">The JSON value to assign to the property. (Optional)</param>
/// <param name="context">The application-defined data to correlate the XblMultiplayerEvent to the initiating call. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Use this method to resolve any conflicts between devices while trying to set properties to a shared portion that other devices can also modify.  
/// It ensures that updates to the session are atomic.  
/// If writing to non-sharable properties, use XblMultiplayerManagerGameSessionSetProperties() instead.  
/// The service may reject your request if a race condition occurred (due to a conflict) resulting in HTTP_E_STATUS_PRECOND_FAILED (HTTP status 412).  
/// To resolve this, evaluate the need to write again and re-submit if needed.  
/// The result is delivered via XblMultiplayerEvent of type XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted 
/// through XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerGameSessionSetSynchronizedProperties(
    _In_z_ const char* name,
    _In_z_ const char* valueJson,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Sets the host for the game using XblMultiplayerSessionWriteMode::SynchronizedUpdate.
/// </summary>
/// <param name="deviceToken">The device token of the host.</param>
/// <param name="context">The application-defined data to correlate the XblMultiplayerEvent to the initiating call. (Optional)</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Use this method to resolve any conflicts between devices trying to set the host at the same time.  
/// It ensures that updates to the session are atomic.  
/// The service may reject your request if a race condition occurred(due to a conflict) resulting in HTTP_E_STATUS_PRECOND_FAILED (HTTP status 412).  
/// To resolve this, evaluate the need to write again and re-submit if needed.  
/// The result is delivered via XblMultiplayerEvent of type XblMultiplayerEventType::SynchronizedHostWriteCompleted 
/// through XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerGameSessionSetSynchronizedHost(
    _In_ const char* deviceToken,
    _In_opt_ void* context
) XBL_NOEXCEPT;

/// <summary>
/// Joins a lobby session given a session handle ID.  
/// Callers will usually get the handle ID from another member's XblMultiplayerActivityDetails 
/// via the XblMultiplayerGetActivitiesForUsersAsync() API, or an invite.
/// </summary>
/// <param name="handleId">A multiplayer handle id, which uniquely identifies the game session you want to join.</param>
/// <param name="user">The system User joining the lobby.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// You can send an invite to another user via XblMultiplayerManagerLobbySessionInviteUsers or XblMultiplayerManagerLobbySessionInviteFriends.
/// The result of XblMultiplayerManagerJoinLobby is delivered via an event returned 
/// from XblMultiplayerManagerDoWork() of the type XblMultiplayerEventType::JoinLobbyCompleted.  
/// After joining, you can set the host via XblMultiplayerManagerLobbySessionSetSynchronizedHost() if one doesn't exist.  
/// Instead, if you don't need a lobby session, and if you haven't added the local users through 
/// XblMultiplayerManagerLobbySessionAddLocalUser(), you can pass in the list of users via the XblMultiplayerManagerJoinGame() API.
/// </remarks>
STDAPI XblMultiplayerManagerJoinLobby(
    _In_z_ const char* handleId,
    _In_ XblUserHandle user
) XBL_NOEXCEPT;

/// <summary>
/// Join the lobby's game session if one exists and if there is room.  
/// If the session doesn't exist, it creates a new game session with the existing lobby members.
/// </summary>
/// <param name="sessionTemplateName">The name of the template for the game session to be based on.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The result is delivered via an event of type XblMultiplayerEventType::JoinGameCompleted 
/// through XblMultiplayerManagerDoWork().  
/// This does not migrate existing lobby session properties over to the game session.  
/// After joining, you can set the properties or the host via XblMultiplayerManagerGameSessionSetSynchronized APIs.
/// </remarks>
STDAPI XblMultiplayerManagerJoinGameFromLobby(
    _In_z_ const char* sessionTemplateName
) XBL_NOEXCEPT;

/// <summary>
/// Joins a game given a globally unique session name.  
/// Callers can get the unique session name as a result of the title's third party matchmaking.  
/// Call this on all clients that needs to join this game.
/// </summary>
/// <param name="sessionName">A unique name for the session.</param>
/// <param name="sessionTemplateName">The name of the template for the game session to be based on.</param>
/// <param name="xuids">Array of xbox user IDs you want to be part of the game.</param>
/// <param name="xuidsCount">The number of elements in the xuids array.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The result is delivered via an event of type XblMultiplayerEventType::JoinGameCompleted through XblMultiplayerManagerDoWork().  
/// After joining, you can set the properties or the host via XblMultiplayerManagerGameSessionSetSynchronized APIs.
/// </remarks>
STDAPI XblMultiplayerManagerJoinGame(
    _In_z_ const char* sessionName,
    _In_z_ const char* sessionTemplateName,
    _In_opt_ const uint64_t* xuids,
    _In_ size_t xuidsCount
) XBL_NOEXCEPT;

/// <summary>
/// Leaving the game will put you back into the lobby.  
/// This will remove all local users from the game.
/// </summary>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// The result is delivered via and event of type XblMultiplayerEventType::LeaveGameCompelted through XblMultiplayerManagerDoWork().
/// </remarks>
STDAPI XblMultiplayerManagerLeaveGame() XBL_NOEXCEPT;

/// <summary>
/// Sends a matchmaking request to the server.
/// </summary>
/// <param name="hopperName">The name of the hopper.</param>
/// <param name="attributesJson">The ticket attributes for the match. (Optional)</param>
/// <param name="timeoutInSeconds">The maximum time to wait for members to join the match.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// When a match is found, the manager will join the game and notify the title via an event of type XblMultiplayerEventType::FindMatchCompleted.
/// </remarks>
STDAPI XblMultiplayerManagerFindMatch(
    _In_z_ const char* hopperName,
    _In_opt_z_ const char* attributesJson,
    _In_ uint32_t timeoutInSeconds
) XBL_NOEXCEPT;

/// <summary>
/// Cancels the match request on the server, if one exists.
/// </summary>
/// <returns></returns>
STDAPI_(void) XblMultiplayerManagerCancelMatch() XBL_NOEXCEPT;

/// <summary>
/// Provides the current status of matchmaking.
/// </summary>
/// <returns>The current status of matchmaking. 'XblMultiplayerMatchStatus::None' if no matchmaking is in progress.</returns>
STDAPI_(XblMultiplayerMatchStatus) XblMultiplayerManagerMatchStatus() XBL_NOEXCEPT;

/// <summary>
/// Estimated wait time in seconds for a match request to be matched with other members.  
/// Only applies after XblMultiplayerManagerFindMatch() has been called.
/// </summary>
/// <returns>The wait time in seconds.</returns>
STDAPI_(uint32_t) XblMultiplayerManagerEstimatedMatchWaitTime() XBL_NOEXCEPT;

/// <summary>
/// Indicates whether the game should auto fill open slots during gameplay.
/// </summary>
/// <returns>Returns true if should auto fill, false if should not auto fill.</returns>
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
/// Indicates who can join your game via the lobby.
/// </summary>
/// <returns>The joinability settings for your lobby.</returns>
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
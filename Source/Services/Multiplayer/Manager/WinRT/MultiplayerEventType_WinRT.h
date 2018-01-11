// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Defines values used to indicate event types for a multiplayer lobby or game.
/// </summary>
public enum class MultiplayerEventType
{
    /// <summary>
    /// Indicates the user was added.
    /// </summary>
    UserAdded = xbox::services::multiplayer::manager::multiplayer_event_type::user_added,

    /// <summary>
    /// Indicates the user was removed.
    /// </summary>
    UserRemoved = xbox::services::multiplayer::manager::multiplayer_event_type::user_removed,

    /// <summary>
    /// Indicates a new member has joined the session. The EventArgs object should be cast
    /// to a MemberJoinedEventArgs object for more information.
    /// </summary>
    MemberJoined = xbox::services::multiplayer::manager::multiplayer_event_type::member_joined,

    /// <summary>
    /// Indicates a member has left the session. The EventArgs object should be cast
    /// to a MemberLeftEventArgs object for more information.
    /// </summary>
    MemberLeft = xbox::services::multiplayer::manager::multiplayer_event_type::member_left,

    /// <summary>
    /// Indicates a member property has changed. The EventArgs object should be cast
    /// to a MemberPropertyChangedEventArgs object for more information.
    /// </summary>
    MemberPropertyChanged = xbox::services::multiplayer::manager::multiplayer_event_type::member_property_changed,

    /// <summary>
    /// Indicates that the SetLocalMemberProperties() or DeleteLocalMemberProperties() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can write local member properties by calling the SetLocalMemberProperties() operation.
    /// </summary>
    LocalMemberPropertyWriteCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::local_member_property_write_completed,

    /// <summary>
    /// Indicates that the SetLocalMemberConnectionAddress() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can write the local member connect address by calling the SetLocalMemberConnectionAddress() operation.
    /// </summary>
    LocalMemberConnectionAddressWriteCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::local_member_connection_address_write_completed,

    /// <summary>
    /// Indicates a session (lobby or game) property has changed. The EventArgs object should be cast
    /// to a SessionPropertyChangedEventArgs object for more information.
    /// </summary>
    SessionPropertyChanged = xbox::services::multiplayer::manager::multiplayer_event_type::session_property_changed,

    /// <summary>
    /// Indicates that the SetProperties() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can write session properties by calling the SetProperties() operation.
    /// </summary>
    SessionPropertyWriteCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::session_property_write_completed,

    /// <summary>
    /// Indicates that the SetSynchronizedProperties() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can write synchronized session properties by calling the SetSynchronizedProperties() operation.
    /// </summary>
    SessionSynchronizedPropertyWriteCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::session_synchronized_property_write_completed,

    /// <summary>
    /// Indicates host has changed. The EventArgs object should be cast
    /// to a HostChangedEventArgs object for more information.
    /// </summary>
    HostChanged = xbox::services::multiplayer::manager::multiplayer_event_type::host_changed,

    /// <summary>
    /// Indicates that the SetSynchronizedHost() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can set the synchronized host by calling the SetSynchronizedHost() operation.
    /// </summary>
    SynchronizedHostWriteCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::synchronized_host_write_completed,

    /// <summary>
    /// Indicates that the joinability value has changed.
    /// A game can change the state by calling the SetJoinability() operation.
    /// </summary>
    JoinabilityStateChanged = xbox::services::multiplayer::manager::multiplayer_event_type::joinability_state_changed,

    /// <summary>
    /// Fired when a match has been found, and the client has joined the target game session.
    /// When this event occurs, the title should provide qos measurement results (via SetQualityOfServiceMeasurements)
    /// between itself and a list of remote clients.
    /// Note: If your title does not require QoS (based on the session template), this event will not be triggered.
    /// </summary>
    PerformQosMeasurements = xbox::services::multiplayer::manager::multiplayer_event_type::perform_qos_measurements,

    /// <summary>
    /// Indicates the FindMatch() operation has completed. The EventArgs object should be cast
    /// to a FindMatchCompletedEventArgs object for more information.
    /// </summary>
    FindMatchCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::find_match_completed,

    /// <summary>
    /// Indicates that the JoinGame() operation has completed. Once the join succeeds,
    /// the member is now part of the game session, and can use data in the session
    // to connect to other game members.
    /// A game can be joined by calling the JoinGame() operation.
    /// </summary>
    JoinGameCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::join_game_completed,

    /// <summary>
    /// Indicates that the LeaveGame() operation has completed. After recieving this event,
    /// the game session object will be set to null. You can join another game by calling
    /// JoinGame() or JoinGameFromLobby().
    /// </summary>
    LeaveGameCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::leave_game_completed,

    /// <summary>
    /// Indicates that the JoinLobby() operation has completed. Once the join succeeds,
    /// the member is now part of the lobby session, and can use data in the session
    // to connect to other lobby members.
    /// The EventArgs object should be cast to a JoinLobbyCompletedEventArgs object for more information.
    /// </summary>
    JoinLobbyCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::join_lobby_completed,

    /// <summary>
    /// Fired when the title's connection to MPSD using the real-time activity service is lost.
    /// When this event occurs, the title should shut down the multiplayer.
    /// </summary>
    ClientDisconnectedFromMultiplayerService = xbox::services::multiplayer::manager::multiplayer_event_type::client_disconnected_from_multiplayer_service,

    /// <summary>
    /// Indicates that the invite API operation has been completed.
    /// </summary>
    InviteSent    = xbox::services::multiplayer::manager::multiplayer_event_type::invite_sent,

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.
    /// Triggered when the tournament's team registration state changes. The EventArgs object
    /// should be cast to a TournamentRegistrationStateChangedEventArgs object for more information.
    /// </summary>
    TournamentRegistrationStateChanged = xbox::services::multiplayer::manager::multiplayer_event_type::tournament_registration_state_changed,

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.
    /// Triggered when a new game has been scheduled. The EventArgs object
    /// should be cast to a TournamentGameSessionReadyEventArgs object for more information.
    /// </summary>
    TournamentGameSessionReady = xbox::services::multiplayer::manager::multiplayer_event_type::tournament_game_session_ready,

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.
    /// Triggered when arbitration is complete and game results have been written to the game session.
    /// </summary>
    ArbitrationComplete = xbox::services::multiplayer::manager::multiplayer_event_type::arbitration_complete
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END

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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Defines values used to indicate change types for a multiplayer game.
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
    /// Indicates a new member has joined.
    /// </summary>
    MemberJoined = xbox::services::multiplayer::manager::multiplayer_event_type::member_joined,

    /// <summary>
    /// Indicates a member has left.
    /// </summary>
    MemberLeft = xbox::services::multiplayer::manager::multiplayer_event_type::member_left,

    /// <summary>
    /// Indicates a member property has changed.
    /// </summary>
    MemberPropertyChanged = xbox::services::multiplayer::manager::multiplayer_event_type::member_property_changed,

    /// <summary>
    /// Indicates that the set_local_member_properties() or delete_local_member_properties() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can be write local member properties by calling the set_local_member_properties() operation.
    /// </summary>
    LocalMemberPropertyWriteCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::local_member_property_write_completed,

    /// <summary>
    /// Indicates that the set_local_member_connection_address() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can be write local member properties by calling the set_local_member_connection_address() operation.
    /// </summary>
    LocalMemberConnectionAddressWriteCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::local_member_connection_address_write_completed,

    /// <summary>
    /// Indicates a game property has changed.
    /// </summary>
    SessionPropertyChanged = xbox::services::multiplayer::manager::multiplayer_event_type::session_property_changed,

    /// <summary>
    /// Indicates that the set_synchronized_properties() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can be write synchronized properties by calling the set_properties() operation.
    /// </summary>
    SessionPropertyWriteCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::session_property_write_completed,

    /// <summary>
    /// Indicates that the set_synchronized_properties() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can be write synchronized properties by calling the set_synchronized_properties() operation.
    /// </summary>
    SessionSynchronizedPropertyWriteCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::session_synchronized_property_write_completed,

    /// <summary>
    /// Indicates host has changed.
    /// </summary>
    HostChanged = xbox::services::multiplayer::manager::multiplayer_event_type::host_changed,

    /// <summary>
    /// Indicates that the set_synchronized_host() operation has completed.
    /// Upon completion, the game can view the err() to see if the write succeeded.
    /// A game can be write synchronized host by calling the set_synchronized_host() operation.
    /// </summary>
    SynchronizedHostWriteCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::synchronized_host_write_completed,

    /// <summary>
    /// Indicates that the joinability value has changed.
    /// A game can be change the state by calling the set_joinability() operation.
    /// </summary>
    JoinabilityStateChanged = xbox::services::multiplayer::manager::multiplayer_event_type::joinability_state_changed,

    /// <summary>
    /// Fired when a match has been found, and the client has joined the target game session.
    /// When this event occurs, title should provide qos measurement results between itself and a list of remote clients.
    /// Note: If your title does not require QoS (based on the session template), this event will not be triggered.
    /// </summary>
    PerformQosMeasurements = xbox::services::multiplayer::manager::multiplayer_event_type::perform_qos_measurements,

    /// <summary>
    /// Indicates the find_match() operation has completed.
    /// </summary>
    FindMatchCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::find_match_completed,

    /// <summary>
    /// Indicates that the join_game() operation has completed. Once the join succeeds, 
    /// the member is now part of the game session, and can use data in the session 
    // to connect to other game members.
    /// A game can be joined by calling the join_game() operation.
    /// </summary>
    JoinGameCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::join_game_completed,

    /// <summary>
    /// Indicates that the leave_game() operation has completed. After recieving this event,
    /// the game session object will be set to null. You can join another game by calling 
    /// join_game() or join_game_from_lobby().
    /// </summary>
    LeaveGameCompleted = xbox::services::multiplayer::manager::multiplayer_event_type::leave_game_completed,

    /// <summary>
    /// Indicates that the join_lobby() operation has completed. Once the join succeeds, 
    /// the member is now part of the lobby session, and can use data in the session 
    // to connect to other lobby members.
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
    /// Triggered when the tournament's team registration state changes. The event_args object 
    /// should be cast to a tournament_registration_state_changed_event_args object for more information.
    /// </summary>
    TournamentRegistrationStateChanged = xbox::services::multiplayer::manager::multiplayer_event_type::tournament_registration_state_changed,

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.
    /// Triggered when a new game has been scheduled. The event_args object 
    /// should be cast to a tournament_game_session_ready_event_args object for more information.
    /// </summary>
    TournamentGameSessionReady = xbox::services::multiplayer::manager::multiplayer_event_type::tournament_game_session_ready,

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.
    /// Triggered when arbitration is complete and game results have been written to the game session.
    /// </summary>
    ArbitrationComplete = xbox::services::multiplayer::manager::multiplayer_event_type::arbitration_complete
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
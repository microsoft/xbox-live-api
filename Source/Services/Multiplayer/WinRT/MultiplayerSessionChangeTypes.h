// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Defines enumberation values that indicate the types of changes to a session that can be subscribed to.
/// </summary>
public enum class MultiplayerSessionChangeTypes
{
    /// <summary>
    /// None.
    /// </summary>
    None = xbox::services::multiplayer::multiplayer_session_change_types::none,

    /// <summary>
    /// Changes to anything in the session.
    /// </summary>
    Everything = xbox::services::multiplayer::multiplayer_session_change_types::everything,

    /// <summary>
    /// Changes to the host device token.
    /// </summary>
    HostDeviceTokenChange = xbox::services::multiplayer::multiplayer_session_change_types::host_device_token_change,

    /// <summary>
    /// Changes to the stage of initialization has changed.
    /// </summary>
    InitializationStateChange = xbox::services::multiplayer::multiplayer_session_change_types::initialization_state_change,

    /// <summary>
    /// Changes to the matchmaking status (e.g. match found or expired) occured.
    /// </summary>
    MatchmakingStatusChange = xbox::services::multiplayer::multiplayer_session_change_types::matchmaking_status_change,

    /// <summary>
    /// A member joined or left the session.
    /// </summary>
    MemberListChange = xbox::services::multiplayer::multiplayer_session_change_types::member_list_change,

    /// <summary>
    /// Changes to the status of a member.
    /// </summary>
    MemberStatusChange = xbox::services::multiplayer::multiplayer_session_change_types::member_status_change,

    /// <summary>
    /// Changes to the joinability of the session.
    /// </summary>
    SessionJoinabilityChange = xbox::services::multiplayer::multiplayer_session_change_types::session_joinability_change,

    /// <summary>
    /// Changes within the /properties/custom object.
    /// </summary>
    CustomPropertyChange = xbox::services::multiplayer::multiplayer_session_change_types::custom_property_change,

    /// <summary>
    /// Changed within the /member/properties/custom object, for any of the members.
    /// </summary>
    MemberCustomPropertyChange = xbox::services::multiplayer::multiplayer_session_change_types::member_custom_property_change,

    /// <summary>
    /// Changes within tournament server properties. Such as next game, last game, or registration.
    /// </summary>
    TournamentPropertyChange = xbox::services::multiplayer::multiplayer_session_change_types::tournament_property_change,

    /// <summary>
    /// Changes within tournament server properties. Such as next game, last game, or registration.
    /// </summary>
    ArbitrationPropertyChange = xbox::services::multiplayer::multiplayer_session_change_types::arbitration_property_change
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
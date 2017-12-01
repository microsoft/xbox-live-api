// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Defines values used to indicate status for the matchmaking stages.
/// </summary>
public enum class MatchStatus
{
    /// <summary>
    /// Indicates no matchmaking search has been started.
    /// </summary>
    None = xbox::services::multiplayer::manager::match_status::none,

    /// <summary>
    /// Indicates that a match ticket was submitted for matchmaking.
    /// </summary>
    SubmittingMatchTicket = xbox::services::multiplayer::manager::match_status::submitting_match_ticket,

    /// <summary>
    /// Indicates the matchmaking is still searching.
    /// </summary>
    Searching = xbox::services::multiplayer::manager::match_status::searching,

    /// <summary>
    /// Indicates the matchmaking search has found a match.
    /// </summary>
    Found = xbox::services::multiplayer::manager::match_status::found,

    /// <summary>
    /// Joining initialization stage.
    /// Matchmaking creates the game session and adds users to it.
    /// The client has until the joining timeout to join the session during this phase.
    /// </summary>
    Joining = xbox::services::multiplayer::manager::match_status::joining,

    /// <summary>
    /// Waiting for remote clients to join the game session.
    /// The client has until the joining timeout to join the session during this phase.
    /// </summary>
    WaitingForRemoteClientsToJoin = xbox::services::multiplayer::manager::match_status::waiting_for_remote_clients_to_join,

    /// <summary>
    /// Measuring initialization stage.
    /// Stage where QoS measurement happens.
    /// The client has until the measurement timeout to upload qos measurements to the service during this phase.
    /// </summary>
    Measuring = xbox::services::multiplayer::manager::match_status::measuring,

    /// <summary>
    /// Uploading QoS measurement results to the service.
    /// The client has until the measurement timeout to upload qos measurements to the service during this phase.
    /// </summary>
    UploadingQosMeasurements = xbox::services::multiplayer::manager::match_status::uploading_qos_measurements,

    /// <summary>
    /// Waiting for remote clients to upload QoS measurement results to the service.
    /// The client has until the measurement timeout to upload qos measurements to the service during this phase.
    /// </summary>
    WaitingForRemoteClientsToUploadQos = xbox::services::multiplayer::manager::match_status::waiting_for_remote_clients_to_upload_qos,

    /// <summary>
    /// Evaluating initialization stage.
    /// If auto evaluate is true, then this stage is skipped.
    /// Otherwise the title will do its own evaluation.
    /// </summary>
    Evaluating = xbox::services::multiplayer::manager::match_status::evaluating,

    /// <summary>
    /// Match was found and QoS measurement was successful.
    /// </summary>
    Completed = xbox::services::multiplayer::manager::match_status::completed,

    /// <summary>
    /// If the match that was found was not successful and is resubmitting.
    /// </summary>
    Resubmitting = xbox::services::multiplayer::manager::match_status::resubmitting,

    /// <summary>
    /// Indicates the matchmaking search has expired.
    /// </summary>
    Expired = xbox::services::multiplayer::manager::match_status::expired,

    /// <summary>
    /// Indicates that matchmaking is in the process of canceling the search.
    /// </summary>
    Canceling = xbox::services::multiplayer::manager::match_status::canceling,

    /// <summary>
    /// Indicates the matchmaking search has been canceled.
    /// </summary>
    Canceled = xbox::services::multiplayer::manager::match_status::canceled,

    /// <summary>
    /// Failed initialization stage.
    /// The initialization failed.
    /// </summary>
    Failed = xbox::services::multiplayer::manager::match_status::failed,
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END

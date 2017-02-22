// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Defines values that indicate the current status during the initialization stage.
/// </summary>
public enum class MultiplayerInitializationStage
{
    /// <summary>
    /// Unknown. 
    /// </summary>
    Unknown = xbox::services::multiplayer::multiplayer_initialization_stage::unknown,

    /// <summary>
    /// The initialization stage not set.
    /// </summary>
    None = xbox::services::multiplayer::multiplayer_initialization_stage::none,

    /// <summary>
    /// Players are joining the initialization stage.  Typically matchmaking creates the session and adds users to it.
    /// Players have up to the joining timeout to join the session during this phase.
    /// </summary>
    Joining = xbox::services::multiplayer::multiplayer_initialization_stage::joining,

    /// <summary>
    /// The measuring initialization stage for QoS measurement is active. This is the stage where QoS measurement happens.  
    /// The title must manually manage QoS during this stage.
    /// </summary>
    Measuring = xbox::services::multiplayer::multiplayer_initialization_stage::measuring,

    /// <summary>
    /// The evaluating initialization stage is active.
    /// If auto evaluate is true, then this stage is skipped; otherwise, the title will do its own evaluation.
    /// </summary>
    Evaluating = xbox::services::multiplayer::multiplayer_initialization_stage::evaluating,

    /// <summary>
    /// Failed initialization stage.
    /// If episode 1 didn't succeed, then it indicates failed permanently. 
    /// </summary>
    Failed = xbox::services::multiplayer::multiplayer_initialization_stage::failed
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
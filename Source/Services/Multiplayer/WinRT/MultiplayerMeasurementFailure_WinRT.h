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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Defines values used to indicate points of measurement failure during the metric stage.
/// </summary>
public enum class MultiplayerMeasurementFailure
{
    /// <summary>
    /// Unknown measurement failure.
    /// </summary>
    Unknown = xbox::services::multiplayer::multiplayer_measurement_failure::unknown,

    /// <summary>
    /// This player has no measurement failure.
    /// </summary>
    None = xbox::services::multiplayer::multiplayer_measurement_failure::none,

    /// <summary>
    /// This player failed because timeout measurement test failed. 
    /// </summary>
    Timeout = xbox::services::multiplayer::multiplayer_measurement_failure::timeout,

    /// <summary>
    /// This player failed because latency measurement test failed. 
    /// </summary>
    Latency = xbox::services::multiplayer::multiplayer_measurement_failure::latency,

    /// <summary>
    /// This player failed because bandwidth up measurement test failed.
    /// </summary>
    BandwidthUp = xbox::services::multiplayer::multiplayer_measurement_failure::bandwidth_up,

    /// <summary>
    /// This player failed because bandwidth down measurement test failed.
    /// </summary>
    BandwidthDown = xbox::services::multiplayer::multiplayer_measurement_failure::bandwidth_down,

    /// <summary>
    /// This player failed because another player in their group failed.
    /// </summary>
    Group = xbox::services::multiplayer::multiplayer_measurement_failure::group,

    /// <summary>
    /// This player failed due to a network error, such as the member was unable to reach another member.
    /// </summary>
    Network = xbox::services::multiplayer::multiplayer_measurement_failure::network,

    /// <summary>
    /// This player failed because your episode failed.  This likely happened because there weren't enough users in the session.
    /// </summary>
    Episode = xbox::services::multiplayer::multiplayer_measurement_failure::episode
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
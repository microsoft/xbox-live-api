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
/// Defines values used to indicate the host selection metrics for measuring matchmaking QoS for a session.
/// </summary>
public enum class MultiplayMetrics
{
    /// <summary>
    /// Unknown metric.
    /// </summary>
    Unknown = xbox::services::multiplayer::multiplay_metrics::unknown,

    /// <summary>
    /// Bandwidth up host selection metric.
    /// </summary>
    BandwidthUp = xbox::services::multiplayer::multiplay_metrics::bandwidth_up,

    /// <summary>
    /// Bandwidth down host selection metric.
    /// </summary>
    BandwidthDown = xbox::services::multiplayer::multiplay_metrics::bandwidth_down,

    /// <summary>
    /// Bandwidth host selection metric.
    /// </summary>
    Bandwidth = xbox::services::multiplayer::multiplay_metrics::bandwidth,

    /// <summary>
    /// Latency host selection metric.
    /// </summary>
    Latency = xbox::services::multiplayer::multiplay_metrics::latency
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
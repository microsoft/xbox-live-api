// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "Macros_WinRT.h"
#include "MultiplayMetrics_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Sets threshold requirements that apply to each connection between a host candidate and a session member. 
/// </summary>
/// <example>
/// The following example shows the format of the JSON object that represents
/// this class:
/// <code>
/// "peerToHostRequirements": {
///    "latencyMaximum": 250,
///    "bandwidthDownMinimum" : 100000,
///    "bandwidthUpMinimum" : 1000,
///    "hostSelectionMetric" : "bandwidthUp"
/// },
/// </code>
/// </example>

public ref class MultiplayerPeerToHostRequirements sealed
{

public:
    /// <summary>
    /// The maximum latency for the peer to host connection.
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(LatencyMaximum, latency_maximum);

    /// <summary>
    /// The minimum bandwidth in kilobits per second for information sent from the host to the peer.
    /// </summary>
    DEFINE_PROP_GET_OBJ(BandwidthDownMinimumInKilobitsPerSecond, bandwidth_down_minimum_in_kilobits_per_second, uint64);

    /// <summary>
    /// The minimum bandwidth in kilobits per second for information sent from the peer to the host.
    /// </summary>
    DEFINE_PROP_GET_OBJ(BandwidthUpMinimumInKilobitsPerSecond, bandwidth_up_minimum_in_kilobits_per_second, uint64);

    /// <summary>
    /// Indicates which metric was used to select the host. 
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(HostSelectionMetric, host_selection_metric, MultiplayMetrics);

internal:
    MultiplayerPeerToHostRequirements(
        _In_ xbox::services::multiplayer::multiplayer_peer_to_host_requirements cppObj
        );

private:
    xbox::services::multiplayer::multiplayer_peer_to_host_requirements m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
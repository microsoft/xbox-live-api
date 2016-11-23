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
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Sets thresholds requirements that apply to each pairwise connection for all members in a session.
/// </summary>
/// <example>
/// The following example shows the format of the JSON object that represents
/// this class:
/// <code>
/// "peerToPeerRequirements": {
///     "latencyMaximum": 250,
///     "bandwidthMinimum" : 10000
/// },
/// </code>
/// </example>

public ref class MultiplayerPeerToPeerRequirements sealed
{
public:
    /// <summary>
    /// The maximum latency for the peer to peer connection.
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(LatencyMaximum, latency_maximum);

    /// <summary>
    /// The minimum bandwidth in kilobits per second for the peer to peer connection.
    /// </summary>
    DEFINE_PROP_GET_OBJ(BandwidthMinimumInKilobitsPerSecond, bandwidth_minimum_in_kilobits_per_second, uint64);

internal:
    MultiplayerPeerToPeerRequirements(
        _In_ xbox::services::multiplayer::multiplayer_peer_to_peer_requirements cppObj
        );

private:
    xbox::services::multiplayer::multiplayer_peer_to_peer_requirements m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
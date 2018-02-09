// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN
multiplayer_peer_to_peer_requirements::multiplayer_peer_to_peer_requirements():
    m_bandwidthMinimumInKilobitsPerSecond(0)
{
}

multiplayer_peer_to_peer_requirements::multiplayer_peer_to_peer_requirements(
    _In_ std::chrono::milliseconds latencyMaximum,
    _In_ uint32_t bandwidthMinimumInKilobitsPerSecond
    ) :
    m_latencyMaximum(std::move(latencyMaximum)),
    m_bandwidthMinimumInKilobitsPerSecond(bandwidthMinimumInKilobitsPerSecond)
{
}

uint64_t
multiplayer_peer_to_peer_requirements::bandwidth_minimum_in_kilobits_per_second() const
{
    return m_bandwidthMinimumInKilobitsPerSecond;
}

const std::chrono::milliseconds&
multiplayer_peer_to_peer_requirements::latency_maximum() const
{
    return m_latencyMaximum;
}

web::json::value 
multiplayer_peer_to_peer_requirements::_Serialize()
{
    web::json::value serializedObject;
    serializedObject[_T("latencyMaximum")] = utils::serialize_uint52_to_json(m_latencyMaximum.count());
    serializedObject[_T("bandwidthMinimum")] = web::json::value(m_bandwidthMinimumInKilobitsPerSecond);
    return serializedObject;
}

xbox_live_result<multiplayer_peer_to_peer_requirements>
multiplayer_peer_to_peer_requirements::_Deserialize(_In_ const web::json::value& json)
{
    if (json.is_null()) return xbox_live_result<multiplayer_peer_to_peer_requirements>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto response = multiplayer_peer_to_peer_requirements(
        std::chrono::milliseconds(utils::extract_json_uint52(json, "latencyMaximum", errc)),
        utils::extract_json_int(json, _T("bandwidthMinimum"), errc)
        );

    return xbox_live_result<multiplayer_peer_to_peer_requirements>(response, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
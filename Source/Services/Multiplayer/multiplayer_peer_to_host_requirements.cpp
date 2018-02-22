// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_peer_to_host_requirements::multiplayer_peer_to_host_requirements():
    m_bandwidthDownMinimumInKilobitsPerSecond(0),
    m_bandwidthUpMinimumInKilobitsPerSecond(0),
    m_hostSelectionMetric(multiplay_metrics::unknown)
{
}

multiplayer_peer_to_host_requirements::multiplayer_peer_to_host_requirements(
    _In_ std::chrono::milliseconds latencyMaximum,
    _In_ uint32_t bandwidthDownMinimumInKilobitsPerSecond,
    _In_ uint32_t bandwidthUpMinimumInKilobitsPerSecond,
    _In_ multiplay_metrics hostSelectionMetric
    ) :
    m_latencyMaximum(std::move(latencyMaximum)),
    m_bandwidthDownMinimumInKilobitsPerSecond(bandwidthDownMinimumInKilobitsPerSecond),
    m_bandwidthUpMinimumInKilobitsPerSecond(bandwidthUpMinimumInKilobitsPerSecond),
    m_hostSelectionMetric(hostSelectionMetric)
{
}

const std::chrono::milliseconds&
multiplayer_peer_to_host_requirements::latency_maximum() const
{
    return m_latencyMaximum;
}

uint64_t 
multiplayer_peer_to_host_requirements::bandwidth_down_minimum_in_kilobits_per_second() const
{
    return m_bandwidthDownMinimumInKilobitsPerSecond;
}

uint64_t 
multiplayer_peer_to_host_requirements::bandwidth_up_minimum_in_kilobits_per_second() const
{
    return m_bandwidthUpMinimumInKilobitsPerSecond;
}

multiplay_metrics 
multiplayer_peer_to_host_requirements::host_selection_metric() const
{
    return m_hostSelectionMetric;
}

web::json::value 
multiplayer_peer_to_host_requirements::_Serialize()
{
    web::json::value serializedObject;
    serializedObject[_T("latencyMaximum")] = utils::serialize_uint52_to_json(m_latencyMaximum.count());
    serializedObject[_T("bandwidthDownMinimum")] = web::json::value(m_bandwidthDownMinimumInKilobitsPerSecond);
    serializedObject[_T("bandwidthUpMinimum")] = web::json::value(m_bandwidthUpMinimumInKilobitsPerSecond);

    auto hostSelectionMetricResult = multiplayer_session::_Convert_multiplayer_host_selection_metric_to_string(m_hostSelectionMetric);

    if (!hostSelectionMetricResult.err())
    {
        serializedObject[_T("hostSelectionMetric")] = web::json::value::string(
            hostSelectionMetricResult.payload()
            );
    }

    return serializedObject;
}

xbox_live_result<multiplayer_peer_to_host_requirements>
multiplayer_peer_to_host_requirements::_Deserialize(
    _In_ const web::json::value& json
    )
{
    if (json.is_null()) return xbox_live_result<multiplayer_peer_to_host_requirements>();

    std::error_code errc = xbox_live_error_code::no_error;
    auto result = multiplayer_peer_to_host_requirements(
        std::chrono::milliseconds(utils::extract_json_uint52(json, "latencyMaximum", errc)),
        utils::extract_json_int(json, _T("bandwidthDownMinimum"), errc),
        utils::extract_json_int(json, _T("bandwidthUpMinimum"), errc),
        multiplayer_session::_Convert_string_to_multiplayer_host_selection_metric(utils::extract_json_string(json, _T("hostSelectionMetric"), errc))
        );

    return xbox_live_result<multiplayer_peer_to_host_requirements>(result, errc);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
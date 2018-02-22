// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "utils.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_quality_of_service_measurements::multiplayer_quality_of_service_measurements():
    m_bandwidthDownInKilobitsPerSecond(0),
    m_bandwidthUpInKilobitsPerSecond(0)
{
}

multiplayer_quality_of_service_measurements::multiplayer_quality_of_service_measurements(
    _In_ string_t memberDeviceToken,
    _In_ std::chrono::milliseconds latency,
    _In_ uint64_t bandwidthDownInKilobitsPerSecond,
    _In_ uint64_t bandwidthUpInKilobitsPerSecond,
    _In_ string_t customJson
    ) : 
    m_memberDeviceToken(std::move(memberDeviceToken)),
    m_latency(std::move(latency)),
    m_bandwidthDownInKilobitsPerSecond(bandwidthDownInKilobitsPerSecond),
    m_bandwidthUpInKilobitsPerSecond(bandwidthUpInKilobitsPerSecond)
{
    std::error_code errc;
    m_customJson = web::json::value::parse(std::move(customJson), errc);
    assert(!errc);
}

const string_t&
multiplayer_quality_of_service_measurements::member_device_token() const
{
    return m_memberDeviceToken;
}

void
multiplayer_quality_of_service_measurements::_Set_member_device_token(
    _In_ const string_t& memberDeviceToken
    )
{
    m_memberDeviceToken = memberDeviceToken;
}

const std::chrono::milliseconds&
multiplayer_quality_of_service_measurements::latency() const
{
    return m_latency;
}

uint64_t
multiplayer_quality_of_service_measurements::bandwidth_down_in_kilobits_per_second() const
{
    return m_bandwidthDownInKilobitsPerSecond;
}

uint64_t
multiplayer_quality_of_service_measurements::bandwidth_up_in_kilobits_per_second() const
{
    return m_bandwidthUpInKilobitsPerSecond;
}

const web::json::value&
multiplayer_quality_of_service_measurements::custom_json() const
{
    return m_customJson;
}

xbox_live_result<multiplayer_quality_of_service_measurements>
multiplayer_quality_of_service_measurements::_Deserialize(
    _In_ const web::json::value& json
    )
{
    multiplayer_quality_of_service_measurements returnResult;
    if (json.is_null()) return xbox_live_result<multiplayer_quality_of_service_measurements>(returnResult);

    std::error_code errc = xbox_live_error_code::no_error;
    returnResult.m_latency = std::chrono::milliseconds(utils::extract_json_uint52(json, "latency", errc));
    returnResult.m_bandwidthDownInKilobitsPerSecond = utils::extract_json_uint52(json, "bandwidthDown", errc);
    returnResult.m_bandwidthUpInKilobitsPerSecond = utils::extract_json_uint52(json, "bandwidthUp", errc);
    returnResult.m_customJson = utils::extract_json_field(json, _T("custom"), errc, false);

    return returnResult;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
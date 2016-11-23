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
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Represents matchmaking quality of service (QoS) measurements for the network used by a session member.
/// </summary>
/// <example>
/// The following example shows the format of the JSON object that represents
/// this class:
/// <code>
/// "measurement=": {
///     "latency": 5953,
///     "bandwidthDown" : 19342,
///     "bandwidthUp" : 944,
///     "custom" : {}
/// }
/// </code>
/// </example>

public ref class MultiplayerQualityOfServiceMeasurements sealed
{
public:
    /// <summary>
    /// Creates a new MultiplayerQualityOfServiceMeasurements object for a session member.
    /// </summary>
    /// <param name="memberDeviceToken">The device token of the session member that this measurement is for.</param>
    /// <param name="latency">The measured latency, in milliseconds, for the session member.</param>
    /// <param name="bandwidthDownInKilobitsPerSecond">The minimum bandwidth down, in kilobits per second, for downloading data from the host to the session member.</param>
    /// <param name="bandwidthUpInKilobitsPerSecond">The minimum bandwidth up, in kilobits per second, for uploading data from the session member to the host.</param>
    /// <param name="customJson">A JSON string that specifies the custom QoS properties.</param>
    MultiplayerQualityOfServiceMeasurements(
        _In_ Platform::String^ memberDeviceToken,
        _In_ Windows::Foundation::TimeSpan latency,
        _In_ uint64 bandwidthDownInKilobitsPerSecond,
        _In_ uint64 bandwidthUpInKilobitsPerSecond,
        _In_ Platform::String^ customJson
        );

    /// <summary>
    /// The device token of the session member that this measurement is for.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(MemberDeviceToken, member_device_token);

    /// <summary>
    /// The measured latency, in milliseconds, for the session member.
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(Latency, latency);

    /// <summary>
    /// The minimum bandwidth down, in kilobits per second, for downloading data from the host to the session member.
    /// </summary>
    DEFINE_PROP_GET_OBJ(BandwidthDownInKilobitsPerSecond, bandwidth_down_in_kilobits_per_second, uint64);

    /// <summary>
    /// The minimum bandwidth up, in kilobits per second, for uploading data from the session member to the host.
    /// </summary>
    DEFINE_PROP_GET_OBJ(BandwidthUpInKilobitsPerSecond, bandwidth_up_in_kilobits_per_second, uint64);

    /// <summary>
    /// A JSON string that specifies the custom QoS properties.
    /// </summary>
    DEFINE_PROP_GET_STR_FROM_JSON_OBJ(CustomJson, custom_json);

internal:
    MultiplayerQualityOfServiceMeasurements(
        _In_ xbox::services::multiplayer::multiplayer_quality_of_service_measurements cppObj
        );

    const xbox::services::multiplayer::multiplayer_quality_of_service_measurements& GetCppObj() const;

private:
    xbox::services::multiplayer::multiplayer_quality_of_service_measurements m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
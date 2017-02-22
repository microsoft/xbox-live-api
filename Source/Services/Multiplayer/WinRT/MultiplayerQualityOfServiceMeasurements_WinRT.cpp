// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "MultiplayerQualityOfServiceMeasurements_WinRT.h"
#include "Utils_WinRT.h"

using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerQualityOfServiceMeasurements::MultiplayerQualityOfServiceMeasurements(
    _In_ xbox::services::multiplayer::multiplayer_quality_of_service_measurements cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

MultiplayerQualityOfServiceMeasurements::MultiplayerQualityOfServiceMeasurements(
    _In_ Platform::String^ memberDeviceToken,
    _In_ Windows::Foundation::TimeSpan latency,
    _In_ uint64 bandwidthDownInKilobitsPerSecond,
    _In_ uint64 bandwidthUpInKilobitsPerSecond,
    _In_ Platform::String^ customJson
    )
{
    m_cppObj = xbox::services::multiplayer::multiplayer_quality_of_service_measurements(
        STRING_T_FROM_PLATFORM_STRING(memberDeviceToken),
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::milliseconds>(latency),
        bandwidthDownInKilobitsPerSecond,
        bandwidthUpInKilobitsPerSecond,
        STRING_T_FROM_PLATFORM_STRING(customJson)
        );
}

const xbox::services::multiplayer::multiplayer_quality_of_service_measurements&
MultiplayerQualityOfServiceMeasurements::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
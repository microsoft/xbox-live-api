//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

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
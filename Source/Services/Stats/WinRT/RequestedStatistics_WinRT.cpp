// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/user_statistics.h"
#include "RequestedStatistics_winrt.h" 
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::user_statistics;

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

RequestedStatistics::RequestedStatistics( 
    Platform::String^ serviceConfigurationId,
    IVectorView<String^>^ statistics
    ) :
    m_cppObj(STRING_T_FROM_PLATFORM_STRING(serviceConfigurationId), UtilsWinRT::CovertVectorViewToStdVectorString(statistics)),
    m_statistics(statistics)
{
}

IVectorView<String^>^
RequestedStatistics::Statistics::get()
{ 
    return m_statistics;
}

const requested_statistics& 
RequestedStatistics::GetCppObj() const
{
    return m_cppObj;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END
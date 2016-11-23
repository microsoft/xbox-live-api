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
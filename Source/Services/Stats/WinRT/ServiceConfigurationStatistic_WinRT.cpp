// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ServiceConfigurationStatistic_winrt.h" 
#include "Utils_WinRT.h"

using namespace xbox::services::user_statistics;
using namespace Microsoft::Xbox::Services::System;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

ServiceConfigurationStatistic::ServiceConfigurationStatistic(
    _In_ service_configuration_statistic cppObj
    ) :
    m_cppObj(cppObj)
{
    m_stats = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<Statistic, statistic>(m_cppObj.statistics());
}

IVectorView<Statistic^>^
ServiceConfigurationStatistic::Statistics::get()
{ 
    return m_stats->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END

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
#include "Statistic_winrt.h" 
#include "ServiceConfigurationStatistic_winrt.h"
#include "UserStatisticsResult_winrt.h" 
#include "Utils_WinRT.h"

using namespace xbox::services::user_statistics;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

UserStatisticsResult::UserStatisticsResult(
    _In_ user_statistics_result cppObj
    ) :
    m_cppObj(cppObj)
{
    m_serviceConfigStatistics = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<ServiceConfigurationStatistic, service_configuration_statistic>(m_cppObj.service_configuration_statistics());

}

Windows::Foundation::Collections::IVectorView<ServiceConfigurationStatistic^>^ 
UserStatisticsResult::ServiceConfigurationStatistics::get()
{
    return m_serviceConfigStatistics->GetView();
}

} // UserStats Namespace
} // Services Namespace
} // Xbox Namespace
} // Microsoft Namespace

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

#include "Statistic_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN 

/// <summary>
/// Contains statistical information from a service configuration.
/// </summary>
public ref class ServiceConfigurationStatistic sealed
{
public:
    /// <summary>
    /// The service configuration ID associated with the leaderboard.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// A collection of statistics used in leaderboards.
    /// </summary>        
    property Windows::Foundation::Collections::IVectorView<Statistic^>^ Statistics
    { 
        Windows::Foundation::Collections::IVectorView<Statistic^>^ get(); 
    }

internal:
    ServiceConfigurationStatistic(
        _In_ xbox::services::user_statistics::service_configuration_statistic cppObj
        );

private:
    Windows::Foundation::Collections::IVector<Statistic^>^ m_stats;
    xbox::services::user_statistics::service_configuration_statistic m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END
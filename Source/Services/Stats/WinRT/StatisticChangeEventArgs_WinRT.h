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
#include "xsapi/user_statistics.h"
#include "Statistic_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

/// <summary>
/// Contains information about a change to a subscribed statistic.
/// </summary>
public ref class StatisticChangeEventArgs sealed
{
public:
    /// <summary>
    /// The Xbox user ID used to create the subscription.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// The service configuration ID used to create the subscription.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// The statistic with an updated value.
    /// </summary>
    property Statistic^ LatestStatistic{ Statistic^ get(); }

internal:
    StatisticChangeEventArgs(_In_ xbox::services::user_statistics::statistic_change_event_args cppObj);

private:
    xbox::services::user_statistics::statistic_change_event_args m_cppObj;
    Statistic^ m_latestStatistic;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END
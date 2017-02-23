// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
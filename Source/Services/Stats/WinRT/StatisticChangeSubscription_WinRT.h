// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/user_statistics.h"
#include "Statistic_WinRT.h"
#include "RealTimeActivitySubscriptionState_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

/// <summary>
/// Handles notification when the state of a statistic subscription changes.
/// </summary>
public ref class StatisticChangeSubscription sealed
{
public:
    /// <summary>
    /// Indicates the state of the subscription.
    /// </summary>
    property Microsoft::Xbox::Services::RealTimeActivity::RealTimeActivitySubscriptionState State{ Microsoft::Xbox::Services::RealTimeActivity::RealTimeActivitySubscriptionState get(); }

    /// <summary>
    /// The resource uri for the request.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(ResourceUri, resource_uri);

    /// <summary>
    /// The unique subscription id for the request.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(SubscriptionId, subscription_id, uint32_t);

    /// <summary>
    /// The xbox user id the subscription is for
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// The service configuration id the subscription is under
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(ServiceConfigurationId, service_configuration_id);

    /// <summary>
    /// The Statistic name the subscription is for
    /// </summary>
    property Platform::String^ StatisticName
    {
        Platform::String^ get();
    };

internal:
    StatisticChangeSubscription(_In_ std::shared_ptr<xbox::services::user_statistics::statistic_change_subscription> cppObj);
    std::shared_ptr<xbox::services::user_statistics::statistic_change_subscription> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::user_statistics::statistic_change_subscription> m_cppObj;
    Platform::String^ m_statisticName;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END
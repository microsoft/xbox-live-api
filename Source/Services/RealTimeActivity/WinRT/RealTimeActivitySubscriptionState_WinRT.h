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

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_BEGIN

/// <summary>
/// Enumeration for the possible states of a statistic subscription request
/// to the real-time activity service.
/// </summary>
public enum class RealTimeActivitySubscriptionState
{
    /// <summary>
    /// The subscription state is unknown.
    /// </summary>
    Unknown = xbox::services::real_time_activity::real_time_activity_subscription_state::unknown,

    /// <summary>
    /// Waiting for the server to respond to the subscription request.
    /// </summary>
    PendingSubscribe = xbox::services::real_time_activity::real_time_activity_subscription_state::pending_subscribe,

    /// <summary>
    /// Subscription confirmed.
    /// </summary>
    Subscribed = xbox::services::real_time_activity::real_time_activity_subscription_state::subscribed,

    /// <summary>
    /// Waiting for the server to respond to the unsubscribe request.
    /// </summary>
    PendingUnsubscribe = xbox::services::real_time_activity::real_time_activity_subscription_state::pending_unsubscribe,

    /// <summary>
    /// Unsubscribe confirmed.
    /// </summary>
    Closed = xbox::services::real_time_activity::real_time_activity_subscription_state::closed
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_END
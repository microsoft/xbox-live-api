// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
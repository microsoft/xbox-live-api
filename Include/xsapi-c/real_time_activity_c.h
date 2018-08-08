// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
/// <summary>
/// Enumeration for the possible states of a statistic subscription request
/// to the real-time activity service.
/// </summary>
typedef enum XblRealTimeActivitySubscriptionState
{
	/// <summary>
	/// The subscription state is unknown.
	/// </summary>
	XblRealTimeActivitySubscriptionState_Unknown,

	/// <summary>
	/// Waiting for the server to respond to the subscription request.
	/// </summary>
	XblRealTimeActivitySubscriptionState_PendingSubscribe,

	/// <summary>
	/// Subscription confirmed.
	/// </summary>
	XblRealTimeActivitySubscriptionState_Subscribed,

	/// <summary>
	/// Waiting for the server to respond to the unsubscribe request.
	/// </summary>
	XblRealTimeActivitySubscriptionState_PpendingUnsubscribe,

	/// <summary>
	/// Unsubscribe confirmed.
	/// </summary>
	XblRealTimeActivitySubscriptionState_Closed
} XblRealTimeActivitySubscriptionState;
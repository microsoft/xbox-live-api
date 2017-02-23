// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/presence.h"
#include "PresenceRecord_WinRT.h"
#include "RealTimeActivitySubscriptionState_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

/// <summary>
/// Contains information about a device presence subscription.
/// </summary>
public ref class DevicePresenceChangeSubscription sealed
{
public:
    /// <summary>
    /// The current state of the subscription.
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

internal:
    DevicePresenceChangeSubscription(_In_ std::shared_ptr<xbox::services::presence::device_presence_change_subscription> cppObj);
    std::shared_ptr<xbox::services::presence::device_presence_change_subscription> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::presence::device_presence_change_subscription> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END
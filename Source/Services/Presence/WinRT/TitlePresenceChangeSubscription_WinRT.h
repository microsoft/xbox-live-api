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
#include "xsapi/presence.h"
#include "PresenceRecord_WinRT.h"
#include "RealTimeActivitySubscriptionState_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

/// <summary>
/// Contains information about a title presence subscription.
/// </summary>
public ref class TitlePresenceChangeSubscription sealed
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

    /// <summary>
    /// The title for the subscription
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(TitleId, title_id, uint32_t);

internal:
    TitlePresenceChangeSubscription(_In_ std::shared_ptr<xbox::services::presence::title_presence_change_subscription> cppObj);
    std::shared_ptr<xbox::services::presence::title_presence_change_subscription> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::presence::title_presence_change_subscription> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"
#include "xsapi/system.h"
#include "multiplayer_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN
using namespace xbox::services::real_time_activity;

class MockMultiplayerSubscription : public multiplayer_subscription
{
public:
    MockMultiplayerSubscription(
        _In_ const std::function<void(const multiplayer_session_change_event_args&)>& multiplayerSessionChangeHandler,
        _In_ const std::function<void()>& multiplayerSubscriptionLostHandler,
        _In_ const std::function<void(const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args&)>& subscriptionErrorHandler)
        : multiplayer_subscription(multiplayerSessionChangeHandler, multiplayerSubscriptionLostHandler, subscriptionErrorHandler)
    {
    }

    void MockMultiplayerSubscription::_Set_state(_In_ xbox::services::real_time_activity::real_time_activity_subscription_state newState);

    void OnSetState(_In_ const std::function<void(real_time_activity_subscription_state)>& callback) 
    {
        m_stateCallback = callback;
    }


private:
    std::function<void(real_time_activity_subscription_state)> m_stateCallback;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END


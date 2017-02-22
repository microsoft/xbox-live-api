// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MockMultiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

void MockMultiplayerSubscription::_Set_state(_In_ xbox::services::real_time_activity::real_time_activity_subscription_state newState)
{
    multiplayer_subscription::_Set_state(newState);

    if (m_stateCallback)
    {
        m_stateCallback(newState);
    }
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END

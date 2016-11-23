c//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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

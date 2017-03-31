// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TeamChangeSubscription_WinRT.h"

using namespace Microsoft::Xbox::Services::RealTimeActivity;
NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TeamChangeSubscription::TeamChangeSubscription(
    _In_ std::shared_ptr<xbox::services::tournaments::team_change_subscription> cppObj
    ) : 
    m_cppObj(cppObj)
{
}

RealTimeActivitySubscriptionState
TeamChangeSubscription::State::get()
{
    return static_cast<RealTimeActivitySubscriptionState>(m_cppObj->state());
}

std::shared_ptr<xbox::services::tournaments::team_change_subscription>
TeamChangeSubscription::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
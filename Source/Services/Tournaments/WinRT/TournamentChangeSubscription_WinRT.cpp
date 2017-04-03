// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TournamentChangeSubscription_WinRT.h"

using namespace Microsoft::Xbox::Services::RealTimeActivity;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TournamentChangeSubscription::TournamentChangeSubscription(
    _In_ std::shared_ptr<xbox::services::tournaments::tournament_change_subscription> cppObj
    ) : 
    m_cppObj(cppObj)
{
}

RealTimeActivitySubscriptionState
TournamentChangeSubscription::State::get()
{
    return static_cast<RealTimeActivitySubscriptionState>(m_cppObj->state());
}

std::shared_ptr<xbox::services::tournaments::tournament_change_subscription>
TournamentChangeSubscription::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "TitlePresenceChangeSubscription_WinRT.h"

using namespace Microsoft::Xbox::Services::RealTimeActivity;
NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_BEGIN

TitlePresenceChangeSubscription::TitlePresenceChangeSubscription(
    _In_ std::shared_ptr<xbox::services::presence::title_presence_change_subscription> cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

RealTimeActivitySubscriptionState
TitlePresenceChangeSubscription::State::get()
{
    return static_cast<RealTimeActivitySubscriptionState>(m_cppObj->state());
}

std::shared_ptr<xbox::services::presence::title_presence_change_subscription> 
TitlePresenceChangeSubscription::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_PRESENCE_END
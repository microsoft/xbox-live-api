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
#include "StatisticChangeSubscription_WinRT.h"

using namespace Microsoft::Xbox::Services::RealTimeActivity;
NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_BEGIN

StatisticChangeSubscription::StatisticChangeSubscription(
    _In_ std::shared_ptr<xbox::services::user_statistics::statistic_change_subscription> cppObj
    ) :
    m_cppObj(cppObj)
{
    m_statisticName = ref new Platform::String(m_cppObj->statistic().statistic_name().c_str());
}

RealTimeActivitySubscriptionState
StatisticChangeSubscription::State::get()
{
    return static_cast<RealTimeActivitySubscriptionState>(m_cppObj->state());
}

Platform::String^
StatisticChangeSubscription::StatisticName::get()
{
    return m_statisticName;
}

std::shared_ptr<xbox::services::user_statistics::statistic_change_subscription> 
StatisticChangeSubscription::GetCppObj() const
{
    return m_cppObj;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_END
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
#include "xsapi/real_time_activity.h"
#include "RealTimeActivitySubscriptionError_WinRT.h"
#include "RealTimeActivitySubscriptionState_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_BEGIN

public ref class RealTimeActivitySubscriptionErrorEventArgs sealed
{
public:
    property RealTimeActivitySubscriptionState State
    {
        RealTimeActivitySubscriptionState get();
    };

    property uint32_t SubscriptionId
    {
        uint32_t get();
    };

    property Platform::String^ ResourceUri
    {
        Platform::String^ get();
    };

    property RealTimeActivitySubscriptionError SubscriptionError
    {
        RealTimeActivitySubscriptionError get();
    };

    property Platform::String^ ErrorMessage
    {
        Platform::String^ get();
    };

internal:
    RealTimeActivitySubscriptionErrorEventArgs(_In_ xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args);

private:
    xbox::services::real_time_activity::real_time_activity_subscription_error_event_args m_cppObj;
    RealTimeActivitySubscriptionError m_subscriptionError;
    Platform::String^ m_resourceUri;
    RealTimeActivitySubscriptionState m_state;
    uint32_t m_subscriptionId;
    Platform::String^ m_errorMessage;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_END
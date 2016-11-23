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
#include "RealTimeActivitySubscriptionErrorEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_BEGIN

RealTimeActivitySubscriptionErrorEventArgs::RealTimeActivitySubscriptionErrorEventArgs(
    _In_ xbox::services::real_time_activity::real_time_activity_subscription_error_event_args args
    ) :
    m_cppObj(std::move(args)),
    m_errorMessage(ref new Platform::String(utility::conversions::utf8_to_utf16(m_cppObj.err_message()).c_str())),
    m_subscriptionError(static_cast<RealTimeActivitySubscriptionError>(args.err().value())),
    m_subscriptionId(args.subscription().subscription_id()),
    m_resourceUri(ref new Platform::String(args.subscription().resource_uri().c_str())),
    m_state(static_cast<RealTimeActivitySubscriptionState>(args.subscription().state()))
{
}

Platform::String^
RealTimeActivitySubscriptionErrorEventArgs::ErrorMessage::get()
{
    return m_errorMessage;
}

RealTimeActivitySubscriptionState
RealTimeActivitySubscriptionErrorEventArgs::State::get()
{
    return m_state;
}

uint32_t
RealTimeActivitySubscriptionErrorEventArgs::SubscriptionId::get()
{
    return m_subscriptionId;
}

Platform::String^
RealTimeActivitySubscriptionErrorEventArgs::ResourceUri::get()
{
    return m_resourceUri;
}

RealTimeActivitySubscriptionError
RealTimeActivitySubscriptionErrorEventArgs::SubscriptionError::get()
{
    return m_subscriptionError;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_END
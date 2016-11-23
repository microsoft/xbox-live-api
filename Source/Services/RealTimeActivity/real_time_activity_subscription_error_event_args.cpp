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
#include "xsapi/real_time_activity.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

real_time_activity_subscription_error_event_args::real_time_activity_subscription_error_event_args(
    real_time_activity_subscription subscription,
    std::error_code errc,
    std::string errMessage
    ) :
    m_subscription(std::move(subscription)),
    m_err(std::move(errc)),
    m_errorMessage(std::move(errMessage))
{
}

const real_time_activity_subscription&
real_time_activity_subscription_error_event_args::subscription()
{
    return m_subscription;
}

const std::error_code&
real_time_activity_subscription_error_event_args::err()
{
    return m_err;
}

const std::string&
real_time_activity_subscription_error_event_args::err_message()
{
    return m_errorMessage;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END
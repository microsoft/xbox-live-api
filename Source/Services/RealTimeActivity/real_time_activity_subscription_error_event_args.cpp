// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
real_time_activity_subscription_error_event_args::err() const
{
    return m_err;
}

const std::string&
real_time_activity_subscription_error_event_args::err_message() const
{
    return m_errorMessage;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END

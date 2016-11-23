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

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_BEGIN

public enum class RealTimeActivitySubscriptionError
{
    NoError = xbox::services::xbox_live_error_code::no_error,

    JsonError = xbox::services::xbox_live_error_code::json_error,

    RTAGenericError = xbox::services::xbox_live_error_code::rta_generic_error,

    RTASubscriptionLimit = xbox::services::xbox_live_error_code::rta_subscription_limit_reached,

    RTAAccessDenied = xbox::services::xbox_live_error_code::rta_access_denied
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_END
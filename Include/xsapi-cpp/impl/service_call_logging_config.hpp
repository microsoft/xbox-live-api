// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if HC_PLATFORM_IS_MICROSOFT

std::shared_ptr<service_call_logging_config> service_call_logging_config::get_singleton_instance()
{
    static std::shared_ptr<service_call_logging_config> s_instance = std::shared_ptr<service_call_logging_config>(new service_call_logging_config);
    return s_instance;
}

void service_call_logging_config::enable() {}
void service_call_logging_config::disable() {}
#if HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_UWP || XSAPI_UNIT_TESTS
void service_call_logging_config::_Register_for_protocol_activation() {}
#endif

#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
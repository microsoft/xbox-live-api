// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "xsapi/service_call_logging_config.h"
#include "service_call_logger.h"
#include "local_config.h"
#if TV_API || UWP_API || UNIT_TEST_SERVICES
#include "service_call_logger_protocol.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<service_call_logging_config> service_call_logging_config::get_singleton_instance()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->m_singletonLock);
    if (xsapiSingleton->m_serviceLoggingConfigSingleton == nullptr)
    {
        xsapiSingleton->m_serviceLoggingConfigSingleton = std::shared_ptr<service_call_logging_config>(new service_call_logging_config());
    }
    return xsapiSingleton->m_serviceLoggingConfigSingleton;
}

service_call_logging_config::service_call_logging_config()
{
}

void service_call_logging_config::enable()
{
    service_call_logger::get_singleton_instance()->enable();
}

void service_call_logging_config::disable()
{
    service_call_logger::get_singleton_instance()->disable();
}

#if TV_API || UWP_API || UNIT_TEST_SERVICES
void service_call_logging_config::_Register_for_protocol_activation()
{
    service_call_logger_protocol::get_singleton_instance()->register_for_protocol_activation();
}
#endif

void service_call_logging_config::_ReadLocalConfig()
{
#if !TV_API
    if (local_config::get_local_config_singleton()->get_bool_from_config("ServiceCallLogging", false, false))
    {
        enable();
    }
#endif
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

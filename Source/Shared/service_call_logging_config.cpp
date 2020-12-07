// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#include "service_call_logging_config.h"
#include "service_call_logger.h"
#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK || XSAPI_UNIT_TESTS
#include "service_call_logger_protocol.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if HC_PLATFORM_IS_MICROSOFT

std::shared_ptr<service_call_logging_config> service_call_logging_config::get_singleton_instance()
{
    auto xsapiSingleton = get_xsapi_singleton();
    if (xsapiSingleton)
    {
        if (xsapiSingleton->m_serviceLoggingConfigSingleton == nullptr)
        {
            std::lock_guard<std::mutex> guard(xsapiSingleton->m_singletonLock);
            if (xsapiSingleton->m_serviceLoggingConfigSingleton == nullptr)
            {
                xsapiSingleton->m_serviceLoggingConfigSingleton = std::shared_ptr<service_call_logging_config>(new service_call_logging_config());
            }
        }
        return xsapiSingleton->m_serviceLoggingConfigSingleton;
    }
    else
    {
        LOGS_DEBUG << "Service Call logging can't be configured until XSAPI is initialized.";
        return nullptr;
    }
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

#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK || XSAPI_UNIT_TESTS
void service_call_logging_config::_Register_for_protocol_activation()
{
    service_call_logger_protocol::get_singleton_instance()->register_for_protocol_activation();
}
#endif

#endif // HC_PLATFORM_IS_MICROSOFT

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

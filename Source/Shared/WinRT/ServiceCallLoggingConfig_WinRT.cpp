// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "ServiceCallLoggingConfig_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

ServiceCallLoggingConfig^
ServiceCallLoggingConfig::SingletonInstance::get()
{
    auto xsapiSingleton = xbox::services::get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->s_singletonLock);

    if (xsapiSingleton->s_winrt_serviceCallLoggingConfigInstance == nullptr)
    {
        xsapiSingleton->s_winrt_serviceCallLoggingConfigInstance = ref new ServiceCallLoggingConfig();
    }

    return xsapiSingleton->s_winrt_serviceCallLoggingConfigInstance;
}

ServiceCallLoggingConfig::ServiceCallLoggingConfig()
{
    m_cppObj = xbox::services::service_call_logging_config::get_singleton_instance();
}

void ServiceCallLoggingConfig::Enable()
{
    m_cppObj->enable();
}

void ServiceCallLoggingConfig::Disable()
{
    m_cppObj->disable();
}

void ServiceCallLoggingConfig::RegisterForProtocolActivation()
{
    m_cppObj->_Register_for_protocol_activation();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_END

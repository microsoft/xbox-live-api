// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ServiceCallLoggingConfig_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

static std::mutex s_singletonLock;

ServiceCallLoggingConfig^
ServiceCallLoggingConfig::SingletonInstance::get()
{
    std::lock_guard<std::mutex> guard(s_singletonLock);

    static ServiceCallLoggingConfig^ instance;
    if (instance == nullptr)
    {
        instance = ref new ServiceCallLoggingConfig();
    }

    return instance;
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

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

#include "xsapi/service_call_logging_config.h"
#include "service_call_logger.h"
#include "local_config.h"
#if TV_API || UWP_API || UNIT_TEST_SERVICES
#include "service_call_logger_protocol.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

static XBOX_LIVE_NAMESPACE::system::xbox_live_mutex g_serviceLoggingConfigSingletonLock;
static std::shared_ptr<service_call_logging_config> g_serviceLoggingConfigSingleton;

std::shared_ptr<service_call_logging_config> service_call_logging_config::get_singleton_instance()
{
    std::lock_guard<std::mutex> guard(g_serviceLoggingConfigSingletonLock.get());
    if (g_serviceLoggingConfigSingleton == nullptr)
    {
        g_serviceLoggingConfigSingleton = std::shared_ptr<service_call_logging_config>(new service_call_logging_config());
    }
    return g_serviceLoggingConfigSingleton;
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
    if (local_config::get_local_config_singleton()->get_bool_from_config(_T("ServiceCallLogging"), false, false))
    {
        enable();
    }
#endif
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

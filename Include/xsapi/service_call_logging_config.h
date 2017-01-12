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

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xbox_live_context_impl;

class service_call_logging_config
{
public:

    /// <summary>
    /// Gets the service_call_logger singleton instance
    /// </summary>
    _XSAPIIMP static std::shared_ptr<service_call_logging_config> get_singleton_instance();

    /// <summary>
    /// Enables the tracking of service calls
    /// </summary>
    _XSAPIIMP void enable();

    /// <summary>
    /// Disables the tracking of service calls
    /// </summary>
    _XSAPIIMP void disable();

#if TV_API || UWP_API || UNIT_TEST_SERVICES
    /// <summary>
    /// Internal API
    /// Enables Logs to be enabled/disabled through protocal activation
    /// </summary>
    void _Register_for_protocol_activation();
#endif

    /// <summary>
    /// Internal API
    /// </summary>
    void _ReadLocalConfig();

private:

    service_call_logging_config();
    service_call_logging_config(const service_call_logging_config&);
    void operator=(const service_call_logging_config&);
    friend xbox_live_context_impl;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
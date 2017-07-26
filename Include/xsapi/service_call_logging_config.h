// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    /// Enables Logs to be enabled/disabled through protocol activation
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
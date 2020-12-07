// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if HC_PLATFORM_IS_MICROSOFT

class service_call_logging_config
{
public:

    /// <summary>
    /// Gets the service_call_logger singleton instance
    /// </summary>
    static std::shared_ptr<service_call_logging_config> get_singleton_instance();

    /// <summary>
    /// Enables the tracking of service calls
    /// </summary>
    void enable();

    /// <summary>
    /// Disables the tracking of service calls
    /// </summary>
    void disable();

#if HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_UWP || XSAPI_UNIT_TESTS
    /// <summary>
    /// Internal API
    /// Enables Logs to be enabled/disabled through protocol activation
    /// </summary>
    void _Register_for_protocol_activation();
#endif

private:
    service_call_logging_config();
    service_call_logging_config(const service_call_logging_config&);
    void operator=(const service_call_logging_config&);
    friend struct ::XblContext;
};
#endif // HC_PLATFORM_IS_MICROSOFT

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
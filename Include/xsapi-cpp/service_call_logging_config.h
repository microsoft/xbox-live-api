// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if HC_PLATFORM_IS_MICROSOFT

class service_call_logging_config
{
public:

    /// <summary>
    /// Deprecated. Service call logging feature is no longer supported.
    /// </summary>
    _XSAPICPP_DEPRECATED inline static std::shared_ptr<service_call_logging_config> get_singleton_instance();

    /// <summary>
    /// Deprecated. Service call logging feature is no longer supported.
    /// </summary>
    _XSAPICPP_DEPRECATED inline void enable();

    /// <summary>
    /// Deprecated. Service call logging feature is no longer supported.
    /// </summary>
    _XSAPICPP_DEPRECATED inline void disable();

#if HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_UWP || XSAPI_UNIT_TESTS
    /// <summary>
    /// Deprecated
    /// </summary>
    _XSAPICPP_DEPRECATED inline void _Register_for_protocol_activation();
#endif

private:
    service_call_logging_config() = default;
    service_call_logging_config(const service_call_logging_config&) = delete;
    void operator=(const service_call_logging_config&) = delete;
};
#endif // HC_PLATFORM_IS_MICROSOFT

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#include "impl/service_call_logging_config.hpp"
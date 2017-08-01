// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/system.h"
#include "xsapi/service_call_logging_config.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

/// <summary>
/// Wrapper for Win32 service_call_logging_config object.
/// </summary>
public ref class ServiceCallLoggingConfig sealed
{
public:
    /// <summary>
    /// Gets the ServiceCallLoggingConfig singleton instance
    /// </summary>
    static property ServiceCallLoggingConfig^ SingletonInstance { ServiceCallLoggingConfig^ get(); }

    /// <summary>
    /// Enables service call logging
    /// </summary>
    void Enable();

    /// <summary>
    /// Disables service call logging
    /// </summary>
    void Disable();

    /// <summary>
    /// Enables Logs to be enabled/disabled through protocol activation
    /// </summary>
    void RegisterForProtocolActivation();

private:
    ServiceCallLoggingConfig();

    std::shared_ptr<xbox::services::service_call_logging_config> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END

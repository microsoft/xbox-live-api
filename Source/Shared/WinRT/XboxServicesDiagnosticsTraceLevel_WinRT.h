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

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

/// <summary>
/// Enumeration values that indicate the trace levels of debug output for service diagnostics.
///
/// Setting the debug trace level to Error or higher reports the last HRESULT, the current
/// function, the source file, and the line number for many trace points in the Xbox Live code.
/// </summary>
public enum class XboxServicesDiagnosticsTraceLevel
{
    /// <summary>
    /// Output no tracing and debugging messages.
    /// </summary>
    Off = xbox::services::xbox_services_diagnostics_trace_level::off,

    /// <summary>
    /// Output error-handling messages.
    /// </summary>
    Error = xbox::services::xbox_services_diagnostics_trace_level::error,

    /// <summary>
    /// Output warnings and error-handling messages.
    /// </summary>
    Warning = xbox::services::xbox_services_diagnostics_trace_level::warning,

    /// <summary>
    /// Output informational messages, warnings, and error-handling messages.
    /// </summary>
    Info = xbox::services::xbox_services_diagnostics_trace_level::info,

    /// <summary>
    /// Output all debugging and tracing messages.
    /// </summary>
    Verbose = xbox::services::xbox_services_diagnostics_trace_level::verbose,
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_END
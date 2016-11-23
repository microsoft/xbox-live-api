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
#include "shared_macros.h"
#include "Macros_WinRT.h"
#include "xsapi/system.h"
#include "Utils_WinRT.h"
#include "XboxServicesDiagnosticsTraceLevel_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

/// <summary>
/// Contains information about a service call.
/// </summary>
public ref class XboxLiveLogCallEventArgs sealed
{
public:
    /// <summary>
    /// Returns the level of the log call.
    /// </summary>
    property XboxServicesDiagnosticsTraceLevel Level { XboxServicesDiagnosticsTraceLevel get(); }

    /// <summary>
    /// Returns the category of the log call.
    /// </summary>
    property Platform::String^ Category { Platform::String^ get(); }

    /// <summary>
    /// Returns the message of the log call
    /// </summary>
    property Platform::String^ Message { Platform::String^ get(); }

internal:
    XboxLiveLogCallEventArgs(
        _In_ xbox::services::xbox_services_diagnostics_trace_level level, 
        _In_ const std::string& category, 
        _In_ const std::string& message
        );

private:
    XboxServicesDiagnosticsTraceLevel m_diagnosticLevel;
    Platform::String^ m_category;
    Platform::String^ m_message;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_END

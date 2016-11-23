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
#include "XboxLiveLogCallEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

class XboxLiveLoggingEventBind : public std::enable_shared_from_this<XboxLiveLoggingEventBind>
{
public:
    XboxLiveLoggingEventBind();

    void AddXboxLiveLoggingEvent();

    void RemoveXboxLiveLoggingEvent();

    void XboxLiveLoggingHandler(
        _In_ xbox::services::xbox_services_diagnostics_trace_level level, 
        _In_ const std::string& category, 
        _In_ const std::string& message
        );

private:
    function_context m_loggingHandlerContext;
};

/// <summary>
/// Represents settings for an HTTP call.
/// </summary>
public ref class XboxLiveServicesSettings sealed
{
public:
    /// <summary>
    /// Gets the XboxLiveServicesSettings singleton instance
    /// </summary>
    static property XboxLiveServicesSettings^ SingletonInstance
    {
        XboxLiveServicesSettings^ get();
    }

    /// <summary>
    /// Registers for log calls.  Event handlers will receive an XboxLiveLogCallEventArgs object.
    /// </summary>
    event Windows::Foundation::EventHandler<Microsoft::Xbox::Services::XboxLiveLogCallEventArgs^>^ LogCallRouted;

    /// <summary>
    /// Indicates the level of debug messages to send to the debugger's Output window.
    /// </summary>
    property XboxServicesDiagnosticsTraceLevel DiagnosticsTraceLevel 
    { 
        XboxServicesDiagnosticsTraceLevel get();
        void set(_In_ XboxServicesDiagnosticsTraceLevel value);
    }

internal:
    XboxLiveServicesSettings();

    void RaiseLogCallRouted(_In_ Microsoft::Xbox::Services::XboxLiveLogCallEventArgs^ args);

    std::shared_ptr<xbox::services::system::xbox_live_services_settings> GetCppObj() { return xbox::services::system::xbox_live_services_settings::get_singleton_instance(); }

private:
    ~XboxLiveServicesSettings();
    std::shared_ptr<XboxLiveLoggingEventBind> m_xboxLiveServicesSettingsEventBind;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END

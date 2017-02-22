// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "Macros_WinRT.h"
#include "xsapi/system.h"
#include "Utils_WinRT.h"
#include "XboxServicesDiagnosticsTraceLevel_WinRT.h"
#include "XboxLiveLogCallEventArgs_WinRT.h"
#include "XboxLiveWnsEventArgs_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

class XboxLiveServicesSettingsEventBind : public std::enable_shared_from_this<XboxLiveServicesSettingsEventBind>
{
public:
    XboxLiveServicesSettingsEventBind(
        _In_ Platform::WeakReference setting,
        _In_ std::shared_ptr<xbox::services::system::xbox_live_services_settings> cppObj);

    ~XboxLiveServicesSettingsEventBind();

    void AddEventBinding();

    void XboxLiveLoggingRouter(
        _In_ xbox::services::xbox_services_diagnostics_trace_level level, 
        _In_ const std::string& category, 
        _In_ const std::string& message
        );

    void XboxLiveWnsRouter(
        _In_ const xbox::services::system::xbox_live_wns_event_args& args
        );

private:
    function_context m_loggingHandlerContext;
    function_context m_wnsHandlerContext;
    Platform::WeakReference m_setting;
    std::shared_ptr<xbox::services::system::xbox_live_services_settings> m_cppObj;
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

    /// <summary>
    /// Registers for WNS notification.  Event handlers will receive an XboxLiveWNSEventArgs object.
    /// </summary>
    event Windows::Foundation::EventHandler<Microsoft::Xbox::Services::XboxLiveWnsEventArgs^>^ WnsEventRecevied;

internal:
    XboxLiveServicesSettings(std::shared_ptr<xbox::services::system::xbox_live_services_settings> cppObj);

    void RaiseLogCallRouted(_In_ Microsoft::Xbox::Services::XboxLiveLogCallEventArgs^ args);

    void RaiseWns(_In_ Microsoft::Xbox::Services::XboxLiveWnsEventArgs^ args);

    std::shared_ptr<xbox::services::system::xbox_live_services_settings> GetCppObj() { return m_cppObj; }

private:
    std::shared_ptr<xbox::services::system::xbox_live_services_settings> m_cppObj;
    std::shared_ptr<XboxLiveServicesSettingsEventBind> m_xboxLiveServicesSettingsEventBind;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END

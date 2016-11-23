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
#include "XboxLiveServicesSettings_WinRT.h"

using namespace Concurrency;
using namespace Platform;
using namespace std;
using namespace xbox::services::system;
using namespace Microsoft::Xbox::Services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

XboxLiveServicesSettings^
XboxLiveServicesSettings::SingletonInstance::get()
{
    static XboxLiveServicesSettings^ xboxLiveServicesSettingsManager = ref new XboxLiveServicesSettings();
    return xboxLiveServicesSettingsManager;
}

XboxLiveServicesSettings::XboxLiveServicesSettings()
{
    m_xboxLiveServicesSettingsEventBind = make_shared<XboxLiveLoggingEventBind>();
    m_xboxLiveServicesSettingsEventBind->AddXboxLiveLoggingEvent();
}

XboxLiveServicesSettings::~XboxLiveServicesSettings()
{
    m_xboxLiveServicesSettingsEventBind->RemoveXboxLiveLoggingEvent();
}

XboxLiveLoggingEventBind::XboxLiveLoggingEventBind()
{
}

void
XboxLiveLoggingEventBind::RemoveXboxLiveLoggingEvent()
{
    xbox_live_services_settings::get_singleton_instance()->remove_logging_handler(m_loggingHandlerContext);
}

void XboxLiveLoggingEventBind::XboxLiveLoggingHandler(
    _In_ xbox::services::xbox_services_diagnostics_trace_level level, 
    _In_ const std::string& category, 
    _In_ const std::string& message
    )
{
    XboxLiveServicesSettings^ setting = XboxLiveServicesSettings::SingletonInstance;
    if (setting != nullptr)
    {
        setting->RaiseLogCallRouted(ref new XboxLiveLogCallEventArgs(level, category, message));
    }
}

void XboxLiveLoggingEventBind::AddXboxLiveLoggingEvent()
{
    std::weak_ptr<XboxLiveLoggingEventBind> thisWeakPtr = shared_from_this();
    m_loggingHandlerContext = xbox_live_services_settings::get_singleton_instance()->add_logging_handler([thisWeakPtr](_In_ xbox::services::xbox_services_diagnostics_trace_level level, _In_ const std::string& category, _In_ const std::string& message)
    {
        std::shared_ptr<XboxLiveLoggingEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->XboxLiveLoggingHandler(level, category, message);
        }
    });
}

void XboxLiveServicesSettings::RaiseLogCallRouted(_In_ XboxLiveLogCallEventArgs^ args)
{
    LogCallRouted(this, args);
}

XboxServicesDiagnosticsTraceLevel
XboxLiveServicesSettings::DiagnosticsTraceLevel::get()
{
    return static_cast<XboxServicesDiagnosticsTraceLevel>(xbox_live_services_settings::get_singleton_instance()->diagnostics_trace_level());
}

void
XboxLiveServicesSettings::DiagnosticsTraceLevel::set(_In_ XboxServicesDiagnosticsTraceLevel value)
{
    xbox_live_services_settings::get_singleton_instance()->set_diagnostics_trace_level(static_cast<xbox::services::xbox_services_diagnostics_trace_level>(value));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END

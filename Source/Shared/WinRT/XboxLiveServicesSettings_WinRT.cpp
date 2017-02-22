// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    static XboxLiveServicesSettings^ xboxLiveServicesSettingsManager = ref new XboxLiveServicesSettings(
        xbox_live_services_settings::get_singleton_instance()
    );
    return xboxLiveServicesSettingsManager;
}

XboxLiveServicesSettingsEventBind::~XboxLiveServicesSettingsEventBind()
{
    m_cppObj->remove_logging_handler(m_loggingHandlerContext);
    m_cppObj->remove_wns_handler(m_wnsHandlerContext);
}

XboxLiveServicesSettingsEventBind::XboxLiveServicesSettingsEventBind(
    _In_ Platform::WeakReference setting,
    _In_ std::shared_ptr<xbox::services::system::xbox_live_services_settings> cppObj
    ):
    m_setting(setting),
    m_cppObj(std::move(cppObj)),
    m_loggingHandlerContext(0),
    m_wnsHandlerContext(0)
{
}

void XboxLiveServicesSettingsEventBind::AddEventBinding()
{
    std::weak_ptr<XboxLiveServicesSettingsEventBind> thisWeakPtr = shared_from_this();

    m_loggingHandlerContext = m_cppObj->add_logging_handler([thisWeakPtr](_In_ xbox::services::xbox_services_diagnostics_trace_level level, _In_ const std::string& category, _In_ const std::string& message)
    {
        std::shared_ptr<XboxLiveServicesSettingsEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->XboxLiveLoggingRouter(level, category, message);
        }
    });

    m_wnsHandlerContext = m_cppObj->add_wns_handler([thisWeakPtr](_In_ const xbox::services::system::xbox_live_wns_event_args& args)
    {
        std::shared_ptr<XboxLiveServicesSettingsEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->XboxLiveWnsRouter(args);
        }
    });
}

void XboxLiveServicesSettingsEventBind::XboxLiveLoggingRouter(
    _In_ xbox::services::xbox_services_diagnostics_trace_level level, 
    _In_ const std::string& category, 
    _In_ const std::string& message
    )
{
    XboxLiveServicesSettings^ serviceSetting = m_setting.Resolve<XboxLiveServicesSettings>();

    if (serviceSetting != nullptr)
    {
        serviceSetting->RaiseLogCallRouted(ref new XboxLiveLogCallEventArgs(level, category, message));
    }
}

void XboxLiveServicesSettingsEventBind::XboxLiveWnsRouter(
    _In_ const xbox::services::system::xbox_live_wns_event_args& args
    )
{
    XboxLiveServicesSettings^ serviceSetting = m_setting.Resolve<XboxLiveServicesSettings>();

    if (serviceSetting != nullptr)
    {
        serviceSetting->RaiseWns(ref new XboxLiveWnsEventArgs(args));
    }
}

XboxLiveServicesSettings::XboxLiveServicesSettings(
    std::shared_ptr<xbox::services::system::xbox_live_services_settings> cppObj
    ):
    m_cppObj(std::move(cppObj))
{
    m_xboxLiveServicesSettingsEventBind = make_shared<XboxLiveServicesSettingsEventBind>(Platform::WeakReference(this), m_cppObj);
    m_xboxLiveServicesSettingsEventBind->AddEventBinding();
}


void XboxLiveServicesSettings::RaiseLogCallRouted(_In_ XboxLiveLogCallEventArgs^ args)
{
    LogCallRouted(this, args);
}

void XboxLiveServicesSettings::RaiseWns(_In_ Microsoft::Xbox::Services::XboxLiveWnsEventArgs^ args)
{
    WnsEventRecevied(this, args);
}

XboxServicesDiagnosticsTraceLevel
XboxLiveServicesSettings::DiagnosticsTraceLevel::get()
{
    return static_cast<XboxServicesDiagnosticsTraceLevel>(m_cppObj->diagnostics_trace_level());
}

void
XboxLiveServicesSettings::DiagnosticsTraceLevel::set(_In_ XboxServicesDiagnosticsTraceLevel value)
{
    m_cppObj->set_diagnostics_trace_level(static_cast<xbox::services::xbox_services_diagnostics_trace_level>(value));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END

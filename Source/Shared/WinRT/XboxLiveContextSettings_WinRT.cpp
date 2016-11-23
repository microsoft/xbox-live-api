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
#include "XboxLiveContextSettings_WinRT.h"

using namespace Concurrency;
using namespace Platform;
using namespace std;

NAMESPACE_MICROSOFT_XBOX_SERVICES_BEGIN

XboxLiveContextSettings::XboxLiveContextSettings()
{
    m_cppObj = make_shared<xbox::services::xbox_live_context_settings>();
    m_xboxLiveContextSettingsEventBind = make_shared<XboxLiveContextSettingsEventBind>( Platform::WeakReference(this), m_cppObj);
    m_xboxLiveContextSettingsEventBind->AddXboxLiveContextSettingsEvent();
}

XboxLiveContextSettings::XboxLiveContextSettings(
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_xboxLiveContextSettingsEventBind = make_shared<XboxLiveContextSettingsEventBind>( Platform::WeakReference(this), m_cppObj);
    m_xboxLiveContextSettingsEventBind->AddXboxLiveContextSettingsEvent();
}

XboxLiveContextSettings::~XboxLiveContextSettings()
{
    m_xboxLiveContextSettingsEventBind->RemoveServiceCallRoutedEvent(m_cppObj);
}

XboxLiveContextSettingsEventBind::XboxLiveContextSettingsEventBind(
    _In_ Platform::WeakReference setting,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> cppObj
    ) :
    m_setting(setting),
    m_cppObj(std::move(cppObj))
{
}

void
XboxLiveContextSettingsEventBind::RemoveServiceCallRoutedEvent(
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> cppObj
    )
{
    cppObj->remove_service_call_routed_handler(m_serviceCallHandlerContext);
}

void XboxLiveContextSettingsEventBind::ServiceCallRoutedHandler(_In_ const xbox::services::xbox_service_call_routed_event_args& callRoutedEventArgs)
{
    XboxLiveContextSettings^ setting = m_setting.Resolve<XboxLiveContextSettings>();
    if (setting != nullptr)
    {
        if (setting->UseCoreDispatcherForEventRouting && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([setting, callRoutedEventArgs]()
            {
                setting->RaiseServiceCallRouted(ref new XboxServiceCallRoutedEventArgs(callRoutedEventArgs));
            }));
        }
        else
        {
            setting->RaiseServiceCallRouted(ref new XboxServiceCallRoutedEventArgs(callRoutedEventArgs));
        }
    }
}

void XboxLiveContextSettingsEventBind::AddXboxLiveContextSettingsEvent()
{
    std::weak_ptr<XboxLiveContextSettingsEventBind> thisWeakPtr = shared_from_this();
    m_serviceCallHandlerContext = m_cppObj->add_service_call_routed_handler([thisWeakPtr](_In_ const xbox::services::xbox_service_call_routed_event_args& callRoutedEventArgs)
    {
        std::shared_ptr<XboxLiveContextSettingsEventBind> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->ServiceCallRoutedHandler(callRoutedEventArgs);
        }
    });
}

void XboxLiveContextSettings::RaiseServiceCallRouted(_In_ XboxServiceCallRoutedEventArgs^ args)
{
    ServiceCallRouted(this, args);
}

XboxServicesDiagnosticsTraceLevel
XboxLiveContextSettings::DiagnosticsTraceLevel::get()
{
    return static_cast<XboxServicesDiagnosticsTraceLevel>(xbox::services::system::xbox_live_services_settings::get_singleton_instance()->diagnostics_trace_level());
}

void
XboxLiveContextSettings::DiagnosticsTraceLevel::set(_In_ XboxServicesDiagnosticsTraceLevel value)
{
    xbox::services::system::xbox_live_services_settings::get_singleton_instance()->set_diagnostics_trace_level(static_cast<xbox::services::xbox_services_diagnostics_trace_level>(value));
}

void XboxLiveContextSettings::DisableAssertsForXboxLiveThrottlingInDevSandboxes(_In_ XboxLiveContextThrottleSetting setting)
{
    if (setting == XboxLiveContextThrottleSetting::ThisCodeNeedsToBeChangedToAvoidThrottling)
    {
        m_cppObj->disable_asserts_for_xbox_live_throttling_in_dev_sandboxes(xbox::services::xbox_live_context_throttle_setting::this_code_needs_to_be_changed_to_avoid_throttling);
    }
}

void XboxLiveContextSettings::DisableAssertsForMaximumNumberOfWebsocketsActivated(_In_ XboxLiveContextRecommendedSetting setting)
{
    if (setting == XboxLiveContextRecommendedSetting::ThisCodeNeedsToBeChangedToFollowBestPractices)
    {
        m_cppObj->disable_asserts_for_maximum_number_of_websockets_activated(xbox::services::xbox_live_context_recommended_setting::this_code_needs_to_be_changed_to_follow_best_practices);
    }
}

Windows::UI::Core::CoreDispatcher^ XboxLiveContextSettings::Dispatcher::get()
{
#if TV_API
    return nullptr;
#else
    return xbox::services::xbox_live_context_settings::_s_dispatcher;
#endif
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_END

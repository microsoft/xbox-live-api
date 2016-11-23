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
#include "RealTimeActivityService_WinRT.h"
#include "Macros_WinRT.h"
#include "XboxLiveContextSettings_WinRT.h"

using namespace Windows::Foundation;
NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_BEGIN

RealTimeActivityService::RealTimeActivityService(
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> cppObj
    ) : m_cppObj(cppObj)
{
    m_connectionStateChangeEventBind = std::make_shared<ConnectionStateChangeEventBind>(Platform::WeakReference(this), m_cppObj);
    m_subscriptionErrorEventBind = std::make_shared<SubscriptionErrorEventBind>(Platform::WeakReference(this), m_cppObj);
    m_resyncEventBind = std::make_shared<ResyncEventBind>(Platform::WeakReference(this), m_cppObj);

    m_connectionStateChangeEventBind->AddConnectionStateChangeEvent();
    m_subscriptionErrorEventBind->AddSubscriptionErrorEvent();
    m_resyncEventBind->AddResyncEvent();
}

RealTimeActivityService::~RealTimeActivityService()
{
    m_connectionStateChangeEventBind->RemoveConnectionStateChangeEvent();
    m_subscriptionErrorEventBind->RemoveSubscriptionErrorEvent();
    m_resyncEventBind->RemoveResyncEvent();
}

void
RealTimeActivityService::Activate()
{
    m_connectionStateChangeEventBind->AddConnectionStateChangeEvent();
    m_subscriptionErrorEventBind->AddSubscriptionErrorEvent();
    m_resyncEventBind->AddResyncEvent();

    CONVERT_STD_EXCEPTION(
        m_cppObj->activate();
    );
}

void
RealTimeActivityService::Deactivate()
{
    m_connectionStateChangeEventBind->RemoveConnectionStateChangeEvent();
    m_subscriptionErrorEventBind->RemoveSubscriptionErrorEvent();
    m_resyncEventBind->RemoveResyncEvent();

    CONVERT_STD_EXCEPTION(
        m_cppObj->deactivate();
    );
}

std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service>
RealTimeActivityService::GetCppObj() const
{
    return m_cppObj;
}

void
RealTimeActivityService::RaiseRealTimeActivityConnectedStateChange(
_In_ RealTimeActivityConnectionState args
)
{
    RealTimeActivityConnectionStateChange(this, args);
}

void
RealTimeActivityService::RaiseRealTimeActivitySubscriptionError(
_In_ RealTimeActivitySubscriptionErrorEventArgs^ args
)
{
    RealTimeActivitySubscriptionError(this, args);
}

void
RealTimeActivityService::RaiseRealTimeActivityResync(
    _In_ RealTimeActivityResyncEventArgs^ args
    )
{
    RealTimeActivityResync(this, args);
}

ConnectionStateChangeEventBind::ConnectionStateChangeEventBind(
    _In_ Platform::WeakReference setting,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> cppObj
    ) : 
    m_setting(setting),
    m_rtaCppObj(std::move(cppObj)),
    m_functionContext(0)
{
}

void
ConnectionStateChangeEventBind::AddConnectionStateChangeEvent()
{
    if (m_functionContext == 0)
    {
        std::weak_ptr<ConnectionStateChangeEventBind> thisWeakPtr = shared_from_this();
        m_functionContext = m_rtaCppObj->add_connection_state_change_handler([thisWeakPtr](_In_ xbox::services::real_time_activity::real_time_activity_connection_state state)
        {
            std::shared_ptr<ConnectionStateChangeEventBind> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->RealTimeActivityConnectionStateChangeRouter(state);
            }
        });
    }
}

void
ConnectionStateChangeEventBind::RemoveConnectionStateChangeEvent()
{
    if (m_functionContext != 0)
    {
        m_rtaCppObj->remove_connection_state_change_handler(m_functionContext);
        m_functionContext = 0;
    }
}

void
ConnectionStateChangeEventBind::RealTimeActivityConnectionStateChangeRouter(
    _In_ xbox::services::real_time_activity::real_time_activity_connection_state state
    )
{
    RealTimeActivityService^ rtaService = m_setting.Resolve<RealTimeActivityService>();
    if (rtaService != nullptr)
    {
        if (m_rtaCppObj->_Xbox_live_context_settings()->use_core_dispatcher_for_event_routing() && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([rtaService, state]()
            {
                rtaService->RaiseRealTimeActivityConnectedStateChange(static_cast<RealTimeActivityConnectionState>(state));
            }));
        }
        else
        {
            rtaService->RaiseRealTimeActivityConnectedStateChange(static_cast<RealTimeActivityConnectionState>(state));
        }
    }
}

SubscriptionErrorEventBind::SubscriptionErrorEventBind(
    _In_ Platform::WeakReference setting,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> cppObj
    ) :
    m_setting(setting),
    m_rtaCppObj(std::move(cppObj)),
    m_functionContext(0)
{
}

void
SubscriptionErrorEventBind::AddSubscriptionErrorEvent()
{
    if (m_functionContext == 0)
    {
        std::weak_ptr<SubscriptionErrorEventBind> thisWeakPtr = shared_from_this();
        m_functionContext = m_rtaCppObj->add_subscription_error_handler([thisWeakPtr](_In_ const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& args)
        {
            std::shared_ptr<SubscriptionErrorEventBind> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->RealTimeActivitySubscriptionErrorRouter(args);
            }
        });
    }
}

void
SubscriptionErrorEventBind::RemoveSubscriptionErrorEvent()
{
    if (m_functionContext != 0)
    {
        m_rtaCppObj->remove_subscription_error_handler(m_functionContext);
        m_functionContext = 0;
    }
}

void
SubscriptionErrorEventBind::RealTimeActivitySubscriptionErrorRouter(
    _In_ const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& args
    )
{
    RealTimeActivityService^ rtaService = m_setting.Resolve<RealTimeActivityService>();
    if (rtaService != nullptr)
    {
        if (m_rtaCppObj->_Xbox_live_context_settings()->use_core_dispatcher_for_event_routing() && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([rtaService, args]()
            {
                rtaService->RaiseRealTimeActivitySubscriptionError(ref new RealTimeActivitySubscriptionErrorEventArgs(args));
            }));
        }
        else
        {
            rtaService->RaiseRealTimeActivitySubscriptionError(ref new RealTimeActivitySubscriptionErrorEventArgs(args));
        }
    }
}

ResyncEventBind::ResyncEventBind(
    _In_ Platform::WeakReference setting,
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> cppObj
    ) :
    m_setting(setting),
    m_rtaCppObj(std::move(cppObj)),
    m_functionContext(0)
{
}

void
ResyncEventBind::AddResyncEvent()
{
    if (m_functionContext == 0)
    {
        std::weak_ptr<ResyncEventBind> thisWeakPtr = shared_from_this();
        m_functionContext = m_rtaCppObj->add_resync_handler([thisWeakPtr](void)
        {
            std::shared_ptr<ResyncEventBind> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->RealTimeActivityResyncRouter();
            }
        });
    }
}

void
ResyncEventBind::RemoveResyncEvent()
{
    if (m_functionContext != 0)
    {
        m_rtaCppObj->remove_resync_handler(m_functionContext);
        m_functionContext = 0;
    }
}

void
ResyncEventBind::RealTimeActivityResyncRouter()
{
    RealTimeActivityService^ rtaService = m_setting.Resolve<RealTimeActivityService>();
    if (rtaService != nullptr)
    {
        if (m_rtaCppObj->_Xbox_live_context_settings()->use_core_dispatcher_for_event_routing() && Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
        {
            Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
                Windows::UI::Core::CoreDispatcherPriority::Normal,
                ref new Windows::UI::Core::DispatchedHandler([rtaService]()
            {
                rtaService->RaiseRealTimeActivityResync(ref new RealTimeActivityResyncEventArgs());
            }));
        }
        else
        {
            rtaService->RaiseRealTimeActivityResync(ref new RealTimeActivityResyncEventArgs());
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_END
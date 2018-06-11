// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/system.h"
#include "xbox_system_factory.h"
#include "xbox_service_call_routed_event_args_internal.h"
#include "xbox_live_app_config_internal.h"
#if XSAPI_A
#include "Logger/android/logcat_output.h"
#else
#include "Logger/debug_output.h"
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if TV_API || UWP_API || UNIT_TEST_SERVICES
Windows::UI::Core::CoreDispatcher^ xbox_live_context_settings::_s_dispatcher;
#endif

#if UWP_API || UNIT_TEST_SERVICES
void
xbox_live_context_settings::_Set_dispatcher(
    _In_opt_ Platform::Object^ coreDispatcherObj
    )
{
    if (coreDispatcherObj != nullptr)
    {
        _s_dispatcher = dynamic_cast<Windows::UI::Core::CoreDispatcher^>(coreDispatcherObj);
    }
    else // if no dispatcher passed in, try to get use direct caller's dispatcher.
    {
        try
        {
#if !UNIT_TEST_SERVICES
            auto currentView = Windows::ApplicationModel::Core::CoreApplication::GetCurrentView();
            if (currentView != nullptr && currentView->CoreWindow != nullptr)
            {
                _s_dispatcher = currentView->CoreWindow->Dispatcher;
            }
#endif
        }
        catch (Platform::Exception^ ex)
        {
            // Get caller's dispatcher failed. Move on with empty dispatcher.
        }
    }

    if (_s_dispatcher != nullptr)
    {
        _s_dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Normal,
            ref new Windows::UI::Core::DispatchedHandler([]()
        {
            xbox::services::utils::generate_locales();
        }));
    }
}
#endif

xbox_live_context_settings::xbox_live_context_settings() :
    m_enableServiceCallRoutedEvents(false),
    m_httpTimeout(std::chrono::seconds(DEFAULT_HTTP_TIMEOUT_SECONDS)),
    m_longHttpTimeout(std::chrono::seconds(DEFAULT_LONG_HTTP_TIMEOUT_SECONDS)),
    m_websocketTimeoutWindow(std::chrono::seconds(DEFAULT_WEBSOCKET_TIMEOUT_SECONDS)),
    m_httpRetryDelay(std::chrono::seconds(DEFAULT_RETRY_DELAY_SECONDS)),
    m_httpTimeoutWindow(std::chrono::seconds(DEFAULT_HTTP_RETRY_WINDOW_SECONDS)),
    m_useCoreDispatcherForEventRouting(false),
#if TV_API
    m_useXplatQosServer(false)
#else
    m_useXplatQosServer(true)
#endif
{
}

function_context xbox_live_context_settings::add_service_call_routed_handler(_In_ std::function<void(const xbox_service_call_routed_event_args&)> handler)
{
    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> lock(singleton->m_serviceCallRoutedHandlersLock);

    function_context context = -1;
    if (handler != nullptr)
    {
        context = singleton->m_serviceCallRoutedHandlersCounter++;
        singleton->m_serviceCallRoutedHandlers[context] = std::move(handler);
        HCAddCallRoutedHandler([](hc_call_handle_t call, void* context)
        {
            auto singleton = get_xsapi_singleton();
            std::lock_guard<std::mutex> lock(singleton->m_serviceCallRoutedHandlersLock);

            auto iter = singleton->m_serviceCallRoutedHandlers.find(static_cast<function_context>((int64)context));
            if (iter != singleton->m_serviceCallRoutedHandlers.end())
            {
                (iter->second)(xsapi_allocate_shared<xbox_service_call_routed_event_args_internal>(call));
            }
        },
        (void*)((int64_t)context));
    }

    return context;
}

void xbox_live_context_settings::remove_service_call_routed_handler(_In_ function_context context)
{
    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> lock(singleton->m_serviceCallRoutedHandlersLock);
    singleton->m_serviceCallRoutedHandlers.erase(context);
}

bool xbox_live_context_settings::enable_service_call_routed_events() const
{
    return m_enableServiceCallRoutedEvents;
}

xbox_services_diagnostics_trace_level xbox_live_context_settings::diagnostics_trace_level() const
{
    return system::xbox_live_services_settings::get_singleton_instance()->diagnostics_trace_level();
}

void xbox_live_context_settings::set_diagnostics_trace_level(_In_ xbox_services_diagnostics_trace_level value)
{
    system::xbox_live_services_settings::get_singleton_instance()->set_diagnostics_trace_level(value);
}

void xbox_live_context_settings::_Raise_service_call_routed_event(_In_ const xbox::services::xbox_service_call_routed_event_args& result)
{
    auto singleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> lock(singleton->m_serviceCallRoutedHandlersLock);

    for (auto& handler : singleton->m_serviceCallRoutedHandlers)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(result);
            }
            catch (...)
            {
                LOG_ERROR("raise_service_call_routed_event failed.");
            }
        }
    }
}

void xbox_live_context_settings::set_enable_service_call_routed_events(_In_ bool value)
{
    m_enableServiceCallRoutedEvents = value;
}

const std::chrono::seconds& xbox_live_context_settings::http_timeout() const
{
    return m_httpTimeout;
}

void xbox_live_context_settings::set_http_timeout(_In_ std::chrono::seconds value)
{
    m_httpTimeout = std::move(value);
}

const std::chrono::seconds& xbox_live_context_settings::long_http_timeout() const
{
    return m_longHttpTimeout;
}

void xbox_live_context_settings::set_long_http_timeout(_In_ std::chrono::seconds value)
{
    m_longHttpTimeout = std::move(value);
}

const std::chrono::seconds& xbox_live_context_settings::http_retry_delay() const
{
    return m_httpRetryDelay;
}

void xbox_live_context_settings::set_http_retry_delay(_In_ std::chrono::seconds value)
{
    m_httpRetryDelay = std::move(value);
    m_httpRetryDelay = std::chrono::seconds(__max(m_httpRetryDelay.count(), MIN_RETRY_DELAY_SECONDS));
}

const std::chrono::seconds& xbox_live_context_settings::http_timeout_window() const
{
    return m_httpTimeoutWindow;
}

void xbox_live_context_settings::set_http_timeout_window(_In_ std::chrono::seconds value)
{
    m_httpTimeoutWindow = std::move(value);
}

const std::chrono::seconds& xbox_live_context_settings::websocket_timeout_window() const
{
    return m_websocketTimeoutWindow;
}

void xbox_live_context_settings::set_websocket_timeout_window(_In_ std::chrono::seconds value)
{
    m_websocketTimeoutWindow = std::move(value);
}

bool xbox_live_context_settings::use_core_dispatcher_for_event_routing() const
{
    return m_useCoreDispatcherForEventRouting;
}

void xbox_live_context_settings::set_use_core_dispatcher_for_event_routing(_In_ bool value)
{
    m_useCoreDispatcherForEventRouting = value;
}

void xbox_live_context_settings::disable_asserts_for_xbox_live_throttling_in_dev_sandboxes(
    _In_ xbox_live_context_throttle_setting setting
    )
{
    xbox_live_app_config_internal::get_app_config_singleton()->disable_asserts_for_xbox_live_throttling_in_dev_sandboxes(setting);
}

void xbox_live_context_settings::disable_asserts_for_maximum_number_of_websockets_activated(
    _In_ xbox_live_context_recommended_setting setting
    )
{
    xbox_live_app_config_internal::get_app_config_singleton()->disable_asserts_for_maximum_number_of_websockets_activated(setting);
}

bool xbox_live_context_settings::use_crossplatform_qos_servers() const
{
    return m_useXplatQosServer;
}

void xbox_live_context_settings::set_use_crossplatform_qos_servers(_In_ bool value)
{
    m_useXplatQosServer = value;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

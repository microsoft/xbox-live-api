// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "xsapi-c/xbox_live_global_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

xbox_live_context_settings::xbox_live_context_settings(XblContextHandle xblContextHandle)
{
    XblContextDuplicateHandle(xblContextHandle, &m_xblContextHandle);
}

xbox_live_context_settings::~xbox_live_context_settings()
{
    XblContextCloseHandle(m_xblContextHandle);
}

struct xbox_live_context_settings::HandlerContext
{
    XblFunctionContext internalContext{ 0 };
    std::function<void(const xbox::services::xbox_service_call_routed_event_args&)> handler;
};

function_context xbox_live_context_settings::add_service_call_routed_handler(
    _In_ std::function<void(const xbox::services::xbox_service_call_routed_event_args&)> handler
)
{
    auto context = new HandlerContext{};
    context->handler = std::move(handler);

    context->internalContext = XblAddServiceCallRoutedHandler(
        [](XblServiceCallRoutedArgs internalArgs, void* context)
    {
        auto handlerContext{ static_cast<HandlerContext*>(context) };
        handlerContext->handler(xbox_service_call_routed_event_args{ internalArgs });
    }, context);

    return context;
}

void xbox_live_context_settings::remove_service_call_routed_handler(_In_ function_context context)
{
    auto handlerContext{ static_cast<HandlerContext*>(context) };
    XblRemoveServiceCallRoutedHandler(handlerContext->internalContext);
    delete handlerContext;
}

std::chrono::seconds xbox_live_context_settings::long_http_timeout() const
{
    uint32_t timeout;
    XblContextSettingsGetLongHttpTimeout(m_xblContextHandle, &timeout);
    return std::chrono::seconds(timeout);
}

void xbox_live_context_settings::set_long_http_timeout(_In_ std::chrono::seconds value)
{
    XblContextSettingsSetLongHttpTimeout(m_xblContextHandle, static_cast<uint32_t>(value.count()));
}

std::chrono::seconds xbox_live_context_settings::xbox_live_context_settings::http_retry_delay() const
{
    uint32_t delay;
    XblContextSettingsGetHttpRetryDelay(m_xblContextHandle, &delay);
    return std::chrono::seconds(delay);
}

void xbox_live_context_settings::set_http_retry_delay(_In_ std::chrono::seconds value)
{
    XblContextSettingsSetHttpRetryDelay(m_xblContextHandle, static_cast<uint32_t>(value.count()));
}

std::chrono::seconds xbox_live_context_settings::http_timeout_window() const
{
    uint32_t window;
    XblContextSettingsGetHttpTimeoutWindow(m_xblContextHandle, &window);
    return std::chrono::seconds(window);
}

void xbox_live_context_settings::set_http_timeout_window(_In_ std::chrono::seconds value)
{
    XblContextSettingsSetHttpTimeoutWindow(m_xblContextHandle, static_cast<uint32_t>(value.count()));
}

std::chrono::seconds xbox_live_context_settings::websocket_timeout_window() const
{
    uint32_t window;
    XblContextSettingsGetWebsocketTimeoutWindow(m_xblContextHandle, &window);
    return std::chrono::seconds(window);
}

void xbox_live_context_settings::set_websocket_timeout_window(_In_ std::chrono::seconds value)
{
    XblContextSettingsSetWebsocketTimeoutWindow(m_xblContextHandle, static_cast<uint32_t>(value.count()));
}

bool xbox_live_context_settings::use_core_dispatcher_for_event_routing() const
{
    return false;
}

void xbox_live_context_settings::set_use_core_dispatcher_for_event_routing(_In_ bool value)
{
    UNREFERENCED_PARAMETER(value);
}

void xbox_live_context_settings::disable_asserts_for_xbox_live_throttling_in_dev_sandboxes(
    _In_ xbox_live_context_throttle_setting setting
)
{
    UNREFERENCED_PARAMETER(setting);
    XblDisableAssertsForXboxLiveThrottlingInDevSandboxes(XblConfigSetting::ThisCodeNeedsToBeChanged);
}

void xbox_live_context_settings::disable_asserts_for_maximum_number_of_websockets_activated(
    _In_ xbox_live_context_recommended_setting setting
)
{
    UNREFERENCED_PARAMETER(setting);
}

bool xbox_live_context_settings::use_crossplatform_qos_servers() const
{
    bool value;
    XblContextSettingsGetUseCrossPlatformQosServers(m_xblContextHandle, &value);
    return value;
}

void xbox_live_context_settings::set_use_crossplatform_qos_servers(_In_ bool value)
{
    XblContextSettingsSetUseCrossPlatformQosServers(m_xblContextHandle, value);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
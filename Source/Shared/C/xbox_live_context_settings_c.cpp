// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/presence_c.h"
#include "xsapi-c/xbox_live_context_settings_c.h"
#include "xbox_live_context_internal_c.h"

using namespace xbox::services;

STDAPI
XblContextSettingsDisableAssertsForXboxLiveThrottlingInDevSandboxes(
    _In_ xbl_context_handle context,
    _In_ XblContextSetting setting
    ) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(setting);
    RETURN_C_INVALIDARGUMENT_IF(context == nullptr);
    context->contextImpl->settings()->disable_asserts_for_xbox_live_throttling_in_dev_sandboxes(xbox_live_context_throttle_setting::this_code_needs_to_be_changed_to_avoid_throttling);
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblContextSettingsDisableAssertsForMaximumNumberOfWebsocketsActivated(
    _In_ xbl_context_handle context,
    _In_ XblContextSetting setting
    ) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(setting);
    RETURN_C_INVALIDARGUMENT_IF(context == nullptr);
    context->contextImpl->settings()->disable_asserts_for_maximum_number_of_websockets_activated(xbox_live_context_recommended_setting::this_code_needs_to_be_changed_to_follow_best_practices);
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblContextSettingsGetHttpRetryDelay(
    _In_ xbl_context_handle context,
    _Out_ uint64_t* delayInSeconds
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(context == nullptr || delayInSeconds == nullptr);
    *delayInSeconds = context->contextImpl->settings()->http_retry_delay().count();
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblContextSettingsSetHttpRetryDelay(
    _In_ xbl_context_handle context,
    _In_ uint64_t delayInSeconds
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(context == nullptr);
    context->contextImpl->settings()->set_http_retry_delay(std::chrono::seconds(delayInSeconds));
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblContextSettingsGetHttpTimeoutWindow(
    _In_ xbl_context_handle context,
    _Out_ uint64_t* timeoutWindowInSeconds
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(context == nullptr || timeoutWindowInSeconds == nullptr);
    *timeoutWindowInSeconds = context->contextImpl->settings()->http_timeout_window().count();
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblContextSettingsSetHttpTimeoutWindow(
    _In_ xbl_context_handle context,
    _In_ uint64_t timeoutWindowInSeconds
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(context == nullptr);
    context->contextImpl->settings()->set_http_timeout_window(std::chrono::seconds(timeoutWindowInSeconds));
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblContextSettingsGetWebsocketTimeoutWindow(
    _In_ xbl_context_handle context,
    _Out_ uint64_t* timeoutWindowInSeconds
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(context == nullptr || timeoutWindowInSeconds == nullptr);
    *timeoutWindowInSeconds = context->contextImpl->settings()->websocket_timeout_window().count();
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblContextSettingsSetWebsocketTimeoutWindow(
    _In_ xbl_context_handle context,
    _In_ uint64_t timeoutWindowInSeconds
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(context == nullptr);
    context->contextImpl->settings()->set_websocket_timeout_window(std::chrono::seconds(timeoutWindowInSeconds));
    return S_OK;
}
CATCH_RETURN()
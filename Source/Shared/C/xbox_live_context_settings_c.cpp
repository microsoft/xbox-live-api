// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/presence_c.h"
#include "xsapi-c/xbox_live_context_settings_c.h"
#include "xbox_live_context_internal_c.h"

using namespace xbox::services;

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
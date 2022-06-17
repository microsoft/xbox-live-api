// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_live_context_internal.h"
#include "xbox_live_app_config_internal.h"

#if HC_PLATFORM == HC_PLATFORM_ANDROID
#include <httpClient/trace.h>
#endif

using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

#if __cplusplus_winrt
Windows::UI::Core::CoreDispatcher^ XboxLiveContextSettings::_s_dispatcher;
#endif

uint32_t XboxLiveContextSettings::LongHttpTimeout() const
{
    return m_longHttpTimeoutInSeconds;
}

void XboxLiveContextSettings::SetLongHttpTimeout(_In_ uint32_t timeoutInSeconds)
{
    m_longHttpTimeoutInSeconds = timeoutInSeconds;
}

uint32_t XboxLiveContextSettings::HttpRetryDelay() const
{
    return m_httpRetryDelayInSeconds;
}

void XboxLiveContextSettings::SetHttpRetryDelay(_In_ uint32_t delayInSeconds)
{
    m_httpRetryDelayInSeconds = __max(delayInSeconds, MIN_RETRY_DELAY_SECONDS);
}

uint32_t XboxLiveContextSettings::HttpTimeoutWindow() const
{
    return m_httpTimeoutWindowInSeconds;
}

void XboxLiveContextSettings::SetHttpTimeoutWindow(_In_ uint32_t timeoutWindowInSeconds)
{
    m_httpTimeoutWindowInSeconds = timeoutWindowInSeconds;
}

uint32_t XboxLiveContextSettings::WebsocketTimeoutWindow() const
{
    return m_websocketTimeoutWindowInSeconds;
}

void XboxLiveContextSettings::SetWebsocketTimeoutWindow(_In_ uint32_t timeoutInSeconds)
{
    m_websocketTimeoutWindowInSeconds = timeoutInSeconds;
}

HttpCallAgent XboxLiveContextSettings::HttpUserAgent() const
{
    return m_userAgent;
}

void XboxLiveContextSettings::SetHttpUserAgent(_In_ HttpCallAgent userAgent)
{
    m_userAgent = userAgent;
}

#if XSAPI_WINRT
bool XboxLiveContextSettings::UseCoreDispatcherForEventRouting() const
{
    return m_useCoreDispatcherForEventRouting;
}

void XboxLiveContextSettings::SetUseCoreDispatcherForEventRouting(_In_ bool value)
{
    m_useCoreDispatcherForEventRouting = value;
}
#endif

bool XboxLiveContextSettings::UseCrossplatformQosServers() const
{
    return m_useXplatQosServer;
}

void XboxLiveContextSettings::SetUseCrossplatformQosServers(_In_ bool value)
{
    m_useXplatQosServer = value;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

STDAPI XblContextSettingsGetLongHttpTimeout(
    _In_ XblContextHandle context,
    _Out_ uint32_t* timeoutInSeconds
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr || timeoutInSeconds == nullptr);
    *timeoutInSeconds = context->Settings()->LongHttpTimeout();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblContextSettingsSetLongHttpTimeout(
    _In_ XblContextHandle context,
    _In_ uint32_t timeoutInSeconds
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr);
    context->Settings()->SetLongHttpTimeout(timeoutInSeconds);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblContextSettingsGetHttpRetryDelay(
    _In_ XblContextHandle context,
    _Out_ uint32_t* delayInSeconds
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr || delayInSeconds == nullptr);
    *delayInSeconds = context->Settings()->HttpRetryDelay();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblContextSettingsSetHttpRetryDelay(
    _In_ XblContextHandle context,
    _In_ uint32_t delayInSeconds
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr);
    context->Settings()->SetHttpRetryDelay(delayInSeconds);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblContextSettingsGetHttpTimeoutWindow(
    _In_ XblContextHandle context,
    _Out_ uint32_t* timeoutWindowInSeconds
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr || timeoutWindowInSeconds == nullptr);
    *timeoutWindowInSeconds = context->Settings()->HttpTimeoutWindow();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblContextSettingsSetHttpTimeoutWindow(
    _In_ XblContextHandle context,
    _In_ uint32_t timeoutWindowInSeconds
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr);
    context->Settings()->SetHttpTimeoutWindow(timeoutWindowInSeconds);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblContextSettingsGetWebsocketTimeoutWindow(
    _In_ XblContextHandle context,
    _Out_ uint32_t* timeoutWindowInSeconds
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr || timeoutWindowInSeconds == nullptr);
    *timeoutWindowInSeconds = context->Settings()->WebsocketTimeoutWindow();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblContextSettingsSetWebsocketTimeoutWindow(
    _In_ XblContextHandle context,
    _In_ uint32_t timeoutWindowInSeconds
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr);
    context->Settings()->SetWebsocketTimeoutWindow(timeoutWindowInSeconds);
    return S_OK;
}
CATCH_RETURN()

#if XSAPI_WINRT
STDAPI XblContextSettingsGetUseCoreDispatcherForEventRouting(
    _In_ XblContextHandle context,
    _Out_ bool* useDispatcher
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr || useDispatcher == nullptr);
    *useDispatcher = context->Settings()->UseCoreDispatcherForEventRouting();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblContextSettingsSetUseCoreDispatcherForEventRouting(
    _In_ XblContextHandle context,
    _In_ bool useDispatcher
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr);
    context->Settings()->SetUseCoreDispatcherForEventRouting(useDispatcher);
    return S_OK;
}
CATCH_RETURN()
#endif

STDAPI XblContextSettingsGetUseCrossPlatformQosServers(
    _In_ XblContextHandle context,
    _Out_ bool* value
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(context == nullptr || value == nullptr);
    *value = context->Settings()->UseCrossplatformQosServers();
    return S_OK;
}
CATCH_RETURN()

STDAPI XblContextSettingsSetUseCrossPlatformQosServers(
    _In_ XblContextHandle context,
    _In_ bool value
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(context);
    context->Settings()->SetUseCrossplatformQosServers(value);
    return S_OK;
}
CATCH_RETURN()

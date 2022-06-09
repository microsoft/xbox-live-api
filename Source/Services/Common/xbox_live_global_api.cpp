// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi-c/errors_c.h"

using namespace xbox::services;
using namespace xbox::services::system;

#ifndef MAKE_HTTP_HRESULT
#define MAKE_HTTP_HRESULT(code) MAKE_HRESULT(1, 0x019, code)
#endif

STDAPI XblMemSetFunctions(
    _In_opt_ XblMemAllocFunction memAllocFunc,
    _In_opt_ XblMemFreeFunction memFreeFunc
) XBL_NOEXCEPT
try
{
    if (GlobalState::Get())
    {
        return E_XBL_ALREADY_INITIALIZED;
    }

    if (memAllocFunc && memFreeFunc)
    {
        g_pMemAllocHook = memAllocFunc;
        g_pMemFreeHook = memFreeFunc;
    }
    else if (!memAllocFunc && !memFreeFunc)
    {
        g_pMemAllocHook = DefaultAlloc;
        g_pMemFreeHook = DefaultFree;
    }
    else
    {
        // Require that the hooks be set together
        return E_INVALIDARG;
    }

    // Try to set memory hooks for libHttpClient as well. If it is already initialized (either by Xal or by the client),
    // there is nothing we can do. We can't log an error either because GlobalState has not yet been set up.
    auto hr = HCMemSetFunctions(memAllocFunc, memFreeFunc);
    if (FAILED(hr) && hr != E_HC_ALREADY_INITIALISED)
    {
        return hr;
    }

    return S_OK;
}
CATCH_RETURN()

STDAPI XblMemGetFunctions(
    _Out_ XblMemAllocFunction* memAllocFunc,
    _Out_ XblMemFreeFunction* memFreeFunc
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF(memAllocFunc == nullptr || memFreeFunc == nullptr);

    *memAllocFunc = g_pMemAllocHook;
    *memFreeFunc = g_pMemFreeHook;
    return S_OK;
}
CATCH_RETURN()

STDAPI XblInitialize(
    _In_ const XblInitArgs* args
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(args);

    HRESULT hr = GlobalState::Create(args);
    LOGS_DEBUG << __FUNCTION__;
    return hr;
}
CATCH_RETURN()

STDAPI XblCleanupAsync(
    XAsyncBlock* async
) XBL_NOEXCEPT
try
{
    LOGS_DEBUG << __FUNCTION__;
    return GlobalState::CleanupAsync(async);
}
CATCH_RETURN()

STDAPI_(XTaskQueueHandle)
XblGetAsyncQueue() XBL_NOEXCEPT
try
{
    XTaskQueueHandle duplicatedHandle{ nullptr };
    auto state = GlobalState::Get();
    if (state && state->Queue().GetHandle())
    {
        XTaskQueueDuplicateHandle(state->Queue().GetHandle(), &duplicatedHandle);
    }
    return duplicatedHandle;
}
CATCH_RETURN_WITH(nullptr)

STDAPI XblGetScid(
    _Out_ const char** scid
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(scid);
    VERIFY_XBL_INITIALIZED();

    *scid = AppConfig::Instance()->Scid().data();
    return S_OK;
}
CATCH_RETURN();

STDAPI_(XblErrorCondition)
XblGetErrorCondition(
    _In_ HRESULT inHr
    ) XBL_NOEXCEPT
try
{
    if (SUCCEEDED(inHr))
    {
        return XblErrorCondition::NoError;
    }
    else if (HRESULT_FACILITY(inHr) == FACILITY_HTTP)
    {
        switch (inHr)
        {
        case HTTP_E_STATUS_NOT_MODIFIED:
            return XblErrorCondition::Http304NotModified;
        case HTTP_E_STATUS_NOT_FOUND:
            return XblErrorCondition::Http404NotFound;
        case HTTP_E_STATUS_PRECOND_FAILED:
            return XblErrorCondition::Http412PreconditionFailed;
        case MAKE_HTTP_HRESULT(429):
            return XblErrorCondition::Http429TooManyRequests;
        case HTTP_E_STATUS_REQUEST_TIMEOUT:
        case HTTP_E_STATUS_SERVICE_UNAVAIL:
        case HTTP_E_STATUS_GATEWAY_TIMEOUT:
            return XblErrorCondition::HttpServiceTimeout;
        default:
            return XblErrorCondition::HttpGeneric;
        }
    }
    else if (HRESULT_FACILITY(inHr) == FACILITY_INTERNET)
    {
        return XblErrorCondition::Network;
    }
    else if ((inHr >= (unsigned)xbl_error_code::AM_E_XASD_UNEXPECTED && inHr <= (unsigned)xbl_error_code::AM_E_XTITLE_TIMEOUT) ||
             (inHr >= (unsigned)xbl_error_code::XO_E_DEVMODE_NOT_AUTHORIZED && inHr <= (unsigned)xbl_error_code::XO_E_CONTENT_NOT_AUTHORIZED))
    {
        return XblErrorCondition::Auth;
    }
    else
    {
        switch (inHr)
        {
        case ONL_E_ACTION_REQUIRED:
        case E_XBL_AUTH_UNKNOWN_ERROR:
        case E_XBL_AUTH_RUNTIME_ERROR:
        case E_XBL_AUTH_NO_TOKEN:
        case __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER):
        case __HRESULT_FROM_WIN32(ERROR_CANCELLED):
            return XblErrorCondition::Auth;
        case E_BOUNDS:
            return XblErrorCondition::GenericOutOfRange;
        default:
            return XblErrorCondition::GenericError;
        }
    }
}
CATCH_RETURN_WITH(XblErrorCondition::GenericError)

STDAPI_(void) XblDisableAssertsForXboxLiveThrottlingInDevSandboxes(
    _In_ XblConfigSetting setting
) XBL_NOEXCEPT
try
{
    UNREFERENCED_PARAMETER(setting);
    AppConfig::Instance()->DisableAssertsForXboxLiveThrottlingInDevSandboxes();
}
CATCH_RETURN_WITH(;)

STDAPI XblSetOverrideConfiguration(
    _In_ const char* overrideScid,
    _In_ uint32_t overrideTitleId
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(overrideScid);
    VERIFY_XBL_INITIALIZED();
    AppConfig::Instance()->SetOverrideScid(overrideScid);
    AppConfig::Instance()->SetOverrideTitleId(overrideTitleId);
    return S_OK;
}
CATCH_RETURN()

STDAPI XblSetOverrideLocale(
    _In_ char const* overrideLocale
) XBL_NOEXCEPT
try
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(overrideLocale);
    VERIFY_XBL_INITIALIZED();
    auto state{ GlobalState::Get() };
    if (state)
    {
        state->OverrideLocale(overrideLocale);
        return S_OK;
    }
    else 
    {
        return E_FAIL;
    }
}
CATCH_RETURN()

STDAPI_(XblFunctionContext) XblAddServiceCallRoutedHandler(
    _In_ XblCallRoutedHandler handler,
    _In_opt_ void* context
) XBL_NOEXCEPT
try
{
    auto state{ GlobalState::Get() };
    if (state && handler)
    {
        return state->AddServiceCallRoutedHandler(handler, context);
    }
    else
    {
        return XblFunctionContext{ 0 };
    }
}
CATCH_RETURN()

STDAPI_(void) XblRemoveServiceCallRoutedHandler(
    _In_ XblFunctionContext token
) XBL_NOEXCEPT
try
{
    auto state{ GlobalState::Get() };
    if (state)
    {
        state->RemoveServiceCallRoutedHandler(token);
    }
}
CATCH_RETURN_WITH(;)


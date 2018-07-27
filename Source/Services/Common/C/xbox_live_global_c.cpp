// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

using namespace xbox::services;
using namespace xbox::services::system;

#ifndef MAKE_HTTP_HRESULT
#define MAKE_HTTP_HRESULT(code) MAKE_HRESULT(1, 0x019, code)
#endif

STDAPI
XblMemSetFunctions(
    _In_opt_ XblMemAllocFunction memAllocFunc,
    _In_opt_ XblMemFreeFunction memFreeFunc
    ) XBL_NOEXCEPT
{
    if (get_xsapi_singleton(false) != nullptr)
    {
        return E_XBL_ALREADY_INITIALISED;
    }

    auto hr = HCMemSetFunctions(memAllocFunc, memFreeFunc);
    if (FAILED(hr))
    {
        return hr;
    }

    if (memAllocFunc != nullptr)
    {
        g_pMemAllocHook = memAllocFunc;
    }
    if (memFreeFunc != nullptr)
    {
        g_pMemFreeHook = memFreeFunc;
    }
    return S_OK;
}

STDAPI
XblMemGetFunctions(
    _Out_ XblMemAllocFunction* memAllocFunc,
    _Out_ XblMemFreeFunction* memFreeFunc
    ) XBL_NOEXCEPT
{
    if (memAllocFunc == nullptr || memFreeFunc == nullptr)
    {
        return E_INVALIDARG;
    }

    *memAllocFunc = g_pMemAllocHook;
    *memFreeFunc = g_pMemFreeHook;
    return S_OK;
}

STDAPI
XblInitialize() XBL_NOEXCEPT
try
{
    (void) get_xsapi_singleton();
    return S_OK;
}
CATCH_RETURN()

STDAPI_(void)
XblCleanup() XBL_NOEXCEPT
try
{
    cleanup_xsapi_singleton();
}
CATCH_RETURN_WITH(;)

STDAPI_(XblErrorCondition)
XblGetErrorCondition(
    _In_ HRESULT hr
    ) XBL_NOEXCEPT
{
    if (SUCCEEDED(hr))
    {
        return XblErrorCondition_NoError;
    }
    else if (HRESULT_FACILITY(hr) == FACILITY_HTTP)
    {
        switch (hr)
        {
        case HTTP_E_STATUS_NOT_FOUND:
            return XblErrorCondition_Http404NotFound;
        case HTTP_E_STATUS_PRECOND_FAILED:
            return XblErrorCondition_Http412PreconditionFailed;
        case MAKE_HTTP_HRESULT(429):
            return XblErrorCondition_Http429TooManyRequests;
        case HTTP_E_STATUS_REQUEST_TIMEOUT:
        case HTTP_E_STATUS_SERVICE_UNAVAIL:
        case HTTP_E_STATUS_GATEWAY_TIMEOUT:
            return XblErrorCondition_HttpServiceTimeout;
        default:
            return XblErrorCondition_HttpGeneric;
        }
    }
    else if (HRESULT_FACILITY(hr) == FACILITY_INTERNET)
    {
        return XblErrorCondition_Network;
    }
    else if (hr >= (unsigned)xbox_live_error_code::AM_E_XASD_UNEXPECTED && hr <= (unsigned)xbox_live_error_code::AM_E_XTITLE_TIMEOUT || 
             hr >= (unsigned)xbox_live_error_code::XO_E_DEVMODE_NOT_AUTHORIZED && hr <= (unsigned)xbox_live_error_code::XO_E_CONTENT_NOT_AUTHORIZED)
    {
        return XblErrorCondition_Auth;
    }
    else
    {
        switch (hr)
        {
        case ONL_E_ACTION_REQUIRED:
        case E_XBL_AUTH_UNKNOWN_ERROR:
        case E_XBL_AUTH_RUNTIME_ERROR:
        case E_XBL_AUTH_NO_TOKEN:
        case __HRESULT_FROM_WIN32(ERROR_NO_SUCH_USER):
        case __HRESULT_FROM_WIN32(ERROR_CANCELLED):
            return XblErrorCondition_Auth;
        case E_BOUNDS:
            return XblErrorCondition_GenericOutOfRange;
        default:
            return XblErrorCondition_GenericError;
        }
    }
}
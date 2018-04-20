// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

using namespace xbox::services;
using namespace xbox::services::system;

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
XblGlobalInitialize() XBL_NOEXCEPT
try
{
    (void) get_xsapi_singleton();
    return S_OK;
}
CATCH_RETURN()

STDAPI_(void)
XblGlobalCleanup() XBL_NOEXCEPT
try
{
    HCGlobalCleanup();
}
CATCH_RETURN_WITH(;)
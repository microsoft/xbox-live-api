// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "threadpool.h"

using namespace xbox::services;
using namespace xbox::services::system;

XBL_API XBL_RESULT XBL_CALLING_CONV
XblMemSetFunctions(
    _In_opt_ XBL_MEM_ALLOC_FUNC memAllocFunc,
    _In_opt_ XBL_MEM_FREE_FUNC memFreeFunc
    ) XBL_NOEXCEPT
{
    if (get_xsapi_singleton(false) != nullptr)
    {
        return XBL_RESULT{ XBL_ERROR_CONDITION_GENERIC_ERROR, XBL_ERROR_CODE_ALREADYINITITIALIZED };
    }

    auto hcResult = HCMemSetFunctions(memAllocFunc, memFreeFunc);
    if (hcResult != HC_OK)
    {
        return utils::create_xbl_result(hcResult);
    }

    if (memAllocFunc != nullptr)
    {
        g_pMemAllocHook = memAllocFunc;
    }
    if (memFreeFunc != nullptr)
    {
        g_pMemFreeHook = memFreeFunc;
    }
    return XBL_RESULT_OK;
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblMemGetFunctions(
    _Out_ XBL_MEM_ALLOC_FUNC* memAllocFunc,
    _Out_ XBL_MEM_FREE_FUNC* memFreeFunc
    ) XBL_NOEXCEPT
{
    if (memAllocFunc == nullptr || memFreeFunc == nullptr)
    {
        return XBL_RESULT{ XBL_ERROR_CONDITION_GENERIC_ERROR, XBL_ERROR_CODE_INVALID_ARGUMENT };
    }

    *memAllocFunc = g_pMemAllocHook;
    *memFreeFunc = g_pMemFreeHook;
    return XBL_RESULT_OK;
}

XBL_API XBL_RESULT XBL_CALLING_CONV
XblGlobalInitialize() XBL_NOEXCEPT
try
{
    // Force init
    get_xsapi_singleton();
    return XBL_RESULT_OK;
}
CATCH_RETURN()

XBL_API void XBL_CALLING_CONV
XblGlobalCleanup() XBL_NOEXCEPT
try
{
    auto singleton = get_xsapi_singleton();
    if (singleton != nullptr)
    {
        singleton->m_threadpool->shutdown_active_threads();
    }
    HCGlobalCleanup();
}
CATCH_RETURN_WITH(;)
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "threadpool.h"

using namespace xbox::services;

XBL_API void XBL_CALLING_CONV
XsapiMemSetFunctions(
    _In_opt_ XSAPI_MEM_ALLOC_FUNC memAllocFunc,
    _In_opt_ XSAPI_MEM_FREE_FUNC memFreeFunc
    ) XBL_NOEXCEPT
{
    HCMemSetFunctions(memAllocFunc, memFreeFunc);
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
XsapiMemGetFunctions(
    _Out_ XSAPI_MEM_ALLOC_FUNC* memAllocFunc,
    _Out_ XSAPI_MEM_FREE_FUNC* memFreeFunc
    ) XBL_NOEXCEPT
{
    return utils_c::xsapi_result_from_hc_result(HCMemGetFunctions(memAllocFunc, memFreeFunc));
}

XBL_API XSAPI_RESULT XBL_CALLING_CONV
XsapiGlobalInitialize() XBL_NOEXCEPT
try
{
    // Force init
    auto singleton = get_xsapi_singleton();
    return XSAPI_RESULT_OK;
}
CATCH_RETURN()

XBL_API void XBL_CALLING_CONV
XsapiGlobalCleanup() XBL_NOEXCEPT
try
{
    auto singleton = get_xsapi_singleton();
    // TODO this should not be needed
    if (singleton != nullptr)
    {
        singleton->m_threadpool->shutdown_active_threads();
    }
    HCGlobalCleanup();
}
CATCH_RETURN_WITH(;)
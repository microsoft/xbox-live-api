// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

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
    auto singleton = get_xsapi_singleton_c(true);
    auto hcr = HCGlobalInitialize();
    if (hcr != HC_OK)
    {
        return utils_c::xsapi_result_from_hc_result(hcr);
    }
    singleton->m_threadPool->start_threads();

    return XSAPI_RESULT_OK;
}
CATCH_RETURN()

XBL_API void XBL_CALLING_CONV
XsapiGlobalCleanup() XBL_NOEXCEPT
try
{
    auto singleton = get_xsapi_singleton_c();
    if (singleton != nullptr)
    {
        singleton->m_threadPool->shutdown_active_threads();
        cleanup_xsapi_singleton_c();
    }
    HCGlobalCleanup();
}
CATCH_RETURN_WITH(;)
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/mem.h"
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

_Ret_maybenull_ _Post_writable_byte_size_(dwSize) 
void* xsapi_memory::mem_alloc(
    _In_ size_t dwSize
    )
{
    try
    {
        init_mem_hooks();
        return g_pMemAllocHook(dwSize, 0);
    }
    catch (...)
    {
        LOG_ERROR("mem_alloc callback failed.");
        return nullptr;
    }
}

void xsapi_memory::mem_free(
    _In_ void* pAddress
    )
{
    try
    {
        init_mem_hooks();
        if (pAddress)
        {
            g_pMemFreeHook(pAddress, 0);
        }
    }
    catch (...)
    {
        LOG_ERROR("mem_alloc callback failed.");
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "xsapi-c/xbox_live_global_c.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

_Ret_maybenull_ _Post_writable_byte_size_(dwSize)
void* xsapi_memory::mem_alloc(
    _In_ size_t dwSize
)
{
    XblMemAllocFunction allocHook{ nullptr };
    XblMemFreeFunction freeHook{ nullptr };

    HRESULT hr = XblMemGetFunctions(&allocHook, &freeHook);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    try
    {
        return allocHook(dwSize, 0);
    }
    catch (...)
    {
        return nullptr;
    }
}

void xsapi_memory::mem_free(
    _In_opt_ void* pAddress
)
{
    XblMemAllocFunction allocHook{ nullptr };
    XblMemFreeFunction freeHook{ nullptr };

    HRESULT hr = XblMemGetFunctions(&allocHook, &freeHook);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);

    try
    {
        freeHook(pAddress, 0);
    }
    catch (...)
    {
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
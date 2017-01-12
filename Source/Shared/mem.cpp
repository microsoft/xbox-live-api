//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/mem.h"
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN



void* xsapi_memory::mem_alloc(
    _In_ size_t dwSize
    )
{
    std::function<_Ret_maybenull_ _Post_writable_byte_size_(dwSize) void*(_In_ size_t dwSize)> pMemAlloc;
    auto xboxLiveServiceSettings = XBOX_LIVE_NAMESPACE::system::xbox_live_services_settings::get_singleton_instance();
    if (xboxLiveServiceSettings != nullptr)
    {
        pMemAlloc = XBOX_LIVE_NAMESPACE::system::xbox_live_services_settings::get_singleton_instance()->m_pMemAllocHook;
    }
    if (pMemAlloc == nullptr)
    {
        return new (std::nothrow) int8_t[dwSize];
    }
    else
    {
        try
        {
            return pMemAlloc(dwSize);
        }
        catch (...)
        {
            LOG_ERROR("mem_alloc callback failed.");
            return nullptr;
        }
    }
}

void xsapi_memory::mem_free(
    _In_ void* pAddress
    )
{
    std::function<void(_In_ void* pAddress)> pMemFree = nullptr;
    auto xboxLiveServiceSettings = XBOX_LIVE_NAMESPACE::system::xbox_live_services_settings::get_singleton_instance();
    if (xboxLiveServiceSettings != nullptr)
    {
        pMemFree = xboxLiveServiceSettings->m_pMemFreeHook;
    }
    if (pMemFree == nullptr)
    {
        delete[] pAddress;
    }
    else
    {
        try
        {
            return pMemFree(pAddress);
        }
        catch (...)
        {
            LOG_ERROR("mem_free callback failed.");
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

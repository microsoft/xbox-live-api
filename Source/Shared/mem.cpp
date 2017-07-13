// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/mem.h"
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN


static bool using_custom_mem_alloc = false;

void* xsapi_memory::mem_alloc(
    _In_ size_t dwSize
    )
{
    std::function<_Ret_maybenull_ _Post_writable_byte_size_(dwSize) void*(_In_ size_t dwSize)> pMemAlloc;
    auto xboxLiveServiceSettings = xbox::services::system::xbox_live_services_settings::get_singleton_instance();
    if (xboxLiveServiceSettings != nullptr)
    {
        pMemAlloc = xbox::services::system::xbox_live_services_settings::get_singleton_instance()->m_pMemAllocHook;
    }
    if (pMemAlloc == nullptr)
    {
        return malloc(dwSize);
    }
    else
    {
        try
        {
            using_custom_mem_alloc = true;
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
    std::function<void(_In_ void* pAddress)> pMemFreeHook = nullptr;
    auto xboxLiveServiceSettings = xbox::services::system::xbox_live_services_settings::get_singleton_instance(false);
    if (xboxLiveServiceSettings != nullptr)
    {
        pMemFreeHook = xboxLiveServiceSettings->m_pMemFreeHook;
        if (pMemFreeHook == nullptr)
        {
            free(pAddress);
        }
        else
        {
            try
            {
                return pMemFreeHook(pAddress);
            }
            catch (...)
            {
                LOG_ERROR("mem_free callback failed.");
            }
        }
    }
    else
    {
        if (!using_custom_mem_alloc)
        {
            delete[] pAddress;
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

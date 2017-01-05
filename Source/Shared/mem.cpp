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

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

// TODO: dealing with inline "xbox::services" on many of the signatures here, is this safe/the right move?
#ifdef XBOX_LIVE_NAMESPACE
#undef XBOX_LIVE_NAMESPACE
#endif
#if BEAM_API
#define XBOX_LIVE_NAMESPACE xbox::services::beam
#else
#define XBOX_LIVE_NAMESPACE xbox::services
#endif

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

#ifdef BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

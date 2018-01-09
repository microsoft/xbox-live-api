// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "singleton_c.h"

static std::mutex g_xblSingletonLock;
static std::unique_ptr<xsapi_singleton_c> g_xblSingleton;

xsapi_singleton_c::xsapi_singleton_c()
{
    m_threadPool = std::make_unique<xsapi_thread_pool>();
    m_titleStorageState = std::make_unique<title_storage_state>();
}

xsapi_singleton_c* get_xsapi_singleton_c(_In_ bool createIfRequired)
{
    if (createIfRequired)
    {
        std::lock_guard<std::mutex> guard(g_xblSingletonLock);
        if (g_xblSingleton == nullptr)
        {
            g_xblSingleton = std::make_unique<xsapi_singleton_c>();

            g_xblSingleton->m_achievementsState = std::make_unique<XSAPI_ACHIEVEMENTS_STATE>();
        }
    }

    return g_xblSingleton.get();
}

void cleanup_xsapi_singleton_c()
{
    std::lock_guard<std::mutex> guard(g_xblSingletonLock);
    g_xblSingleton = nullptr;
}

void verify_global_init()
{
    if (g_xblSingleton == nullptr)
    {
        //HC_TRACE_ERROR(XSAPI_C_TRACE, "Call HCGlobalInitialize() first");
        assert(g_xblSingleton != nullptr);
    }
}

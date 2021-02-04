// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "public_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

std::shared_ptr<xbox_live_services_settings> xbox_live_services_settings::get_singleton_instance(_In_ bool createIfRequired)
{
    UNREFERENCED_PARAMETER(createIfRequired);

    static std::shared_ptr<xbox_live_services_settings> instance = std::shared_ptr<xbox_live_services_settings>(new xbox_live_services_settings);
    return instance;
}

void xbox_live_services_settings::set_memory_allocation_hooks(
    _In_ const std::function<_Ret_maybenull_ _Post_writable_byte_size_(dwSize) void*(_In_ size_t dwSize)>& memAllocHandler,
    _In_ const std::function<void(_In_ void* pAddress)>& memFreeHandler
)
{
    static std::function<void*(size_t)> allocHook{ nullptr };
    static std::function<void(void*)> freeHook{ nullptr };

    if (memAllocHandler != nullptr && memFreeHandler != nullptr)
    {
        allocHook = memAllocHandler;
        freeHook = memFreeHandler;

        XblMemSetFunctions(
            [](size_t size, HCMemoryType)
            {
                return allocHook(size);
            },
            [](void* pointer, HCMemoryType)
            {
                freeHook(pointer);
            }
        );
    }
    else if (memAllocHandler == nullptr && memFreeHandler == nullptr)
    {
        XblMemSetFunctions(nullptr, nullptr);
    }
}


function_context xbox_live_services_settings::add_logging_handler(
    std::function<void(xbox_services_diagnostics_trace_level, const std::string&, const std::string&)> handler
)
{
    UNREFERENCED_PARAMETER(handler);
    return function_context{};
}

void xbox_live_services_settings::remove_logging_handler(_In_ function_context context)
{
    UNREFERENCED_PARAMETER(context);
}

xbox_services_diagnostics_trace_level xbox_live_services_settings::diagnostics_trace_level() const
{
    HCTraceLevel traceLevel{};
    HCSettingsGetTraceLevel(&traceLevel);

    switch (traceLevel)
    {
    case HCTraceLevel::Off: return xbox_services_diagnostics_trace_level::off;
    case HCTraceLevel::Error: return xbox_services_diagnostics_trace_level::error;
    case HCTraceLevel::Warning: return xbox_services_diagnostics_trace_level::warning;
    case HCTraceLevel::Important: return xbox_services_diagnostics_trace_level::info;
    case HCTraceLevel::Verbose: return xbox_services_diagnostics_trace_level::verbose;
    default: return xbox_services_diagnostics_trace_level::off;
    }
}

void xbox_live_services_settings::set_diagnostics_trace_level(_In_ xbox_services_diagnostics_trace_level value)
{
    HCTraceLevel traceLevel{ HCTraceLevel::Off };
    switch (value)
    {
    case xbox_services_diagnostics_trace_level::off: traceLevel = HCTraceLevel::Off; break;
    case xbox_services_diagnostics_trace_level::error: traceLevel = HCTraceLevel::Error; break;
    case xbox_services_diagnostics_trace_level::warning: traceLevel = HCTraceLevel::Warning; break;
    case xbox_services_diagnostics_trace_level::info: traceLevel = HCTraceLevel::Information; break;
    case xbox_services_diagnostics_trace_level::verbose: traceLevel = HCTraceLevel::Verbose; break;
    default: break;
    }
    HCSettingsSetTraceLevel(traceLevel);
}

function_context xbox_live_services_settings::add_wns_handler(_In_ const std::function<void(const xbox_live_wns_event_args&)>& handler)
{
    UNREFERENCED_PARAMETER(handler);
    return function_context{};
}

void xbox_live_services_settings::remove_wns_handler(_In_ function_context context)
{
    UNREFERENCED_PARAMETER(context);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
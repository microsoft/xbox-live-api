// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_live_services_settings.h"
#include "Logger/log_hc_output.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

std::function<_Ret_maybenull_ _Post_writable_byte_size_(dwSize) void*(_In_ size_t dwSize)> g_pCppMemAllocHook = nullptr;
std::function<void(_In_ void* pAddress)> g_pCppMemFreeHook = nullptr;

std::shared_ptr<xbox_live_services_settings> xbox_live_services_settings::get_singleton_instance(_In_ bool createIfRequired)
{
    UNREFERENCED_PARAMETER(createIfRequired);

    std::shared_ptr<xsapi_singleton> xsapiSingleton = get_xsapi_singleton();
    if (xsapiSingleton == nullptr)
    {
        return nullptr;
    }

    {
        std::lock_guard<std::mutex> guard(xsapiSingleton->m_serviceSettingsLock);
        if (xsapiSingleton->m_xboxServiceSettingsSingleton == nullptr)
        {
            xsapiSingleton->m_xboxServiceSettingsSingleton = std::shared_ptr<xbox_live_services_settings>(new xbox_live_services_settings());
        }
        return xsapiSingleton->m_xboxServiceSettingsSingleton;
    }
}

xbox_live_services_settings::xbox_live_services_settings() :
    m_traceLevel(xbox_services_diagnostics_trace_level::off),
    m_loggingHandlersCounter(0),
    m_wnsHandlersCounter(0)
{
}

void *custom_mem_alloc_wrapper(_In_ size_t size, _In_ uint32_t memoryType)
{
    UNREFERENCED_PARAMETER(memoryType);
    if (g_pCppMemAllocHook == nullptr)
    {
        XSAPI_ASSERT(true && L"Custom mem hook function not set!");
        return nullptr;
    }
    else
    {
        try
        {
            return g_pCppMemAllocHook(size);
        }
        catch (...)
        {
            LOG_ERROR("mem_alloc callback failed.");
            return nullptr;
        }
    }
}

void custom_mem_free_wrapper(_In_ void *pointer, _In_ uint32_t memoryType)
{
    UNREFERENCED_PARAMETER(memoryType);
    if (g_pCppMemFreeHook == nullptr)
    {
        XSAPI_ASSERT(true && L"Custom mem hook function not set!");
        return;
    }
    else
    {
        try
        {
            return g_pCppMemFreeHook(pointer);
        }
        catch (...)
        {
            LOG_ERROR("mem_free callback failed.");
        }
    }
}

void xbox_live_services_settings::set_memory_allocation_hooks(
    _In_ const std::function<_Ret_maybenull_ _Post_writable_byte_size_(dwSize) void*(_In_ size_t dwSize)>& memAllocHandler,
    _In_ const std::function<void(_In_ void* pAddress)>& memFreeHandler
    )
{
    // Both params must either be null or both must be non-null
    if (memAllocHandler != nullptr || memFreeHandler != nullptr)
    {
        THROW_CPP_INVALIDARGUMENT_IF(memAllocHandler == nullptr || memFreeHandler == nullptr);
    }

    g_pCppMemAllocHook = memAllocHandler;
    g_pCppMemFreeHook = memFreeHandler;

    g_pMemAllocHook = (g_pCppMemAllocHook == nullptr) ? nullptr : custom_mem_alloc_wrapper;
    g_pMemFreeHook = (g_pCppMemFreeHook == nullptr) ? nullptr : custom_mem_free_wrapper;

    HCMemSetFunctions(g_pMemAllocHook, g_pMemFreeHook);
}

function_context xbox_live_services_settings::add_logging_handler(_In_ std::function<void(xbox_services_diagnostics_trace_level, const std::string&, const std::string&)> handler)
{
    std::lock_guard<std::mutex> lock(m_loggingWriteLock);

    function_context context = 0;
    if (handler != nullptr)
    {
        context = (function_context)(uint64_t)(++m_loggingHandlersCounter);
        m_loggingHandlers[m_loggingHandlersCounter] = std::move(handler);
    }

    return context;
}

void xbox_live_services_settings::remove_logging_handler(_In_ function_context context)
{
    std::lock_guard<std::mutex> lock(m_loggingWriteLock);
    m_loggingHandlers.erase((uint32_t)(uint64_t)context);
}

function_context xbox_live_services_settings::add_wns_handler(_In_ const std::function<void(const xbox_live_wns_event_args&)>& handler)
{

    function_context context = 0;
    if (handler != nullptr)
    {
        context = (function_context)(uint64_t)(++m_wnsHandlersCounter);
        m_wnsHandlers[m_wnsHandlersCounter] = handler;
    }

    return context;
}

void xbox_live_services_settings::remove_wns_handler(_In_ function_context context)
{
    std::lock_guard<std::mutex> lock(m_wnsEventLock);
    m_wnsHandlers.erase((uint32_t)(uint64_t)context);
}

xbox_services_diagnostics_trace_level xbox_live_services_settings::diagnostics_trace_level() const
{
    return m_traceLevel;
}

void xbox_live_services_settings::set_diagnostics_trace_level(_In_ xbox_services_diagnostics_trace_level value)
{
    m_traceLevel = value;
    set_log_level_from_diagnostics_trace_level();
}

void xbox_live_services_settings::_Raise_logging_event(_In_ xbox_services_diagnostics_trace_level level, _In_ const std::string& category, _In_ const std::string& message)
{
    std::lock_guard<std::mutex> lock(m_loggingWriteLock);

    for (auto& handler : m_loggingHandlers)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(level, category, message);
            }
            catch (...)
            {
                LOG_ERROR("xbox_live_services_settings::raise_logging_event failed.");
            }
        }
    }
}

void xbox_live_services_settings::_Raise_wns_event(
    _In_ const string_t& xbox_user_id, 
    _In_ const string_t& notification_type, 
    _In_ const string_t& content
)
{
    std::lock_guard<std::mutex> lock(m_wnsEventLock);

    xbox_live_wns_event_args arg(xbox_user_id, notification_type, content);
    for (auto& handler : m_wnsHandlers)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(arg);
            }
            catch (...)
            {
                LOG_ERROR("xbox_live_services_settings::raise_wns_event failed.");
            }
        }
    }
}

bool xbox_live_services_settings::_Is_at_diagnostics_trace_level(_In_ xbox_services_diagnostics_trace_level level)
{
    return (int)m_traceLevel >= (int)level;
}

void xbox_live_services_settings::set_log_level_from_diagnostics_trace_level()
{
    HCTraceLevel logLevel = HCTraceLevel::Off;
    switch (m_traceLevel)
    {
    case xbox_services_diagnostics_trace_level::off: logLevel = HCTraceLevel::Off; break;
    case xbox_services_diagnostics_trace_level::error: logLevel = HCTraceLevel::Error; break;
    case xbox_services_diagnostics_trace_level::warning: logLevel = HCTraceLevel::Warning; break;
    case xbox_services_diagnostics_trace_level::info: logLevel = HCTraceLevel::Information; break;
    case xbox_services_diagnostics_trace_level::verbose: logLevel = HCTraceLevel::Verbose; break;
    }
    logger::get_logger()->set_log_level(logLevel);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

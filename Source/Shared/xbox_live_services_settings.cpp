// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/system.h"
#include "xsapi/social_manager.h"
#if XSAPI_A
#include "Logger/android/logcat_output.h"
#else
#include "Logger/debug_output.h"
#endif
#include "Logger/custom_output.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

std::shared_ptr<xbox_live_services_settings> xbox_live_services_settings::get_singleton_instance(_In_ bool createIfRequired)
{
    std::shared_ptr<xsapi_singleton> xsapiSingleton = get_xsapi_singleton(createIfRequired);
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
    m_pCustomMemAllocHook(nullptr),
    m_pCustomMemFreeHook(nullptr),
    m_loggingHandlersCounter(0),
    m_wnsHandlersCounter(0),
    m_traceLevel(xbox_services_diagnostics_trace_level::off)
{
}

void *custom_mem_alloc_wrapper(_In_ size_t size, _In_ uint32_t memoryType)
{
    UNREFERENCED_PARAMETER(memoryType);
    auto xboxLiveServiceSettings = xbox::services::system::xbox_live_services_settings::get_singleton_instance();
    if (xboxLiveServiceSettings == nullptr || xboxLiveServiceSettings->m_pCustomMemAllocHook == nullptr)
    {
        XSAPI_ASSERT(true && L"Custom mem hook function not set!");
        return nullptr;
    }
    else
    {
        try
        {
            return xbox::services::system::xbox_live_services_settings::get_singleton_instance()->m_pCustomMemAllocHook(size);
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
    auto xboxLiveServiceSettings = xbox::services::system::xbox_live_services_settings::get_singleton_instance(false);
    if (xboxLiveServiceSettings == nullptr || xboxLiveServiceSettings->m_pCustomMemFreeHook == nullptr)
    {
        XSAPI_ASSERT(true && L"Custom mem hook function not set!");
        return;
    }
    else
    {
        try
        {
            return xboxLiveServiceSettings->m_pCustomMemFreeHook(pointer);
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

    m_pCustomMemAllocHook = memAllocHandler;
    m_pCustomMemFreeHook = memFreeHandler;

    g_pMemAllocHook = m_pCustomMemAllocHook == nullptr ? nullptr : custom_mem_alloc_wrapper;
    g_pMemFreeHook = m_pCustomMemFreeHook == nullptr ? nullptr : custom_mem_free_wrapper;

    HCMemSetFunctions(g_pMemAllocHook, g_pMemFreeHook);
}

function_context xbox_live_services_settings::add_logging_handler(_In_ std::function<void(xbox_services_diagnostics_trace_level, const std::string&, const std::string&)> handler)
{
    std::lock_guard<std::mutex> lock(m_loggingWriteLock);

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_loggingHandlersCounter;
        m_loggingHandlers[m_loggingHandlersCounter] = std::move(handler);
    }

    return context;
}

void xbox_live_services_settings::remove_logging_handler(_In_ function_context context)
{
    std::lock_guard<std::mutex> lock(m_loggingWriteLock);
    m_loggingHandlers.erase(context);
}

function_context xbox_live_services_settings::add_wns_handler(_In_ const std::function<void(const xbox_live_wns_event_args&)>& handler)
{

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_wnsHandlersCounter;
        m_wnsHandlers[m_wnsHandlersCounter] = handler;
    }

    return context;
}

void xbox_live_services_settings::remove_wns_handler(_In_ function_context context)
{
    std::lock_guard<std::mutex> lock(m_wnsEventLock);
    m_wnsHandlers.erase(context);
}

xbox_services_diagnostics_trace_level xbox_live_services_settings::diagnostics_trace_level() const
{
    return m_traceLevel;
}

void xbox_live_services_settings::set_diagnostics_trace_level(_In_ xbox_services_diagnostics_trace_level value)
{
    m_traceLevel = value;
    social::manager::social_manager::get_singleton_instance()->set_diagnostics_trace_level(value);
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
    if (m_traceLevel == xbox_services_diagnostics_trace_level::off)
    {
        logger::release_logger();
        return;
    }
    else if (logger::get_logger() == nullptr)
    {
        logger::create_logger();
#if XSAPI_A
        logger::get_logger()->add_log_output(std::make_shared<logcat_output>());
#else
        logger::get_logger()->add_log_output(std::make_shared<debug_output>());
#endif
        logger::get_logger()->add_log_output(std::make_shared<custom_output>());
    }

    log_level logLevel = log_level::off;
    switch (m_traceLevel)
    {
    case xbox_services_diagnostics_trace_level::off: logLevel = log_level::off; break;
    case xbox_services_diagnostics_trace_level::error: logLevel = log_level::error; break;
    case xbox_services_diagnostics_trace_level::warning: logLevel = log_level::warn; break;
    case xbox_services_diagnostics_trace_level::info: logLevel = log_level::info; break;
    case xbox_services_diagnostics_trace_level::verbose: logLevel = log_level::debug; break;
    }
    logger::get_logger()->set_log_level(logLevel);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

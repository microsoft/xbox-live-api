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
#include "xsapi/system.h"
#if XSAPI_A
#include "Logger/android/logcat_output.h"
#else
#include "Logger/debug_output.h"
#endif
#include "Logger/custom_output.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

std::shared_ptr<xbox_live_services_settings> xbox_live_services_settings::get_singleton_instance()
{
    std::shared_ptr<xsapi_singleton> xsapiSingleton = get_xsapi_singleton();
    
    std::lock_guard<std::mutex> guard(xsapiSingleton->s_singletonLock);
    if (xsapiSingleton->s_xboxServiceSettingsSingleton == nullptr)
    {
        xsapiSingleton->s_xboxServiceSettingsSingleton = std::shared_ptr<xbox_live_services_settings>(new xbox_live_services_settings());
    }
    return xsapiSingleton->s_xboxServiceSettingsSingleton;
}

xbox_live_services_settings::xbox_live_services_settings() :
    m_pMemAllocHook(nullptr),
    m_pMemFreeHook(nullptr)
{
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

    m_pMemAllocHook = memAllocHandler;
    m_pMemFreeHook = memFreeHandler;
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
                LOG_ERROR("raise_logging_routed_event failed.");
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

#ifdef BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

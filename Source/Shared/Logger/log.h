// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "internal_mem.h"

#if XSAPI_ANDROID_STUDIO
#include "trace.h"
#endif

#define DEFAULT_LOGGER xbox::services::logger::get_logger()

#if HC_PLATFORM_IS_MICROSOFT
    #define LOG(logger, level, category, msg)  \
        __pragma(warning( push )) \
        __pragma(warning( disable : 26444 )) \
        { auto logInst = logger; if (logInst) { logInst->add_log({ level, category, msg }); } } \
        __pragma(warning( pop ))
#else
    #define LOG(logger, level, category, msg) \
        { auto logInst = logger; if (logInst) { logInst->add_log({ level, category, msg }); } }
#endif

#define LOGS(level, category) xbox::services::logger_raii() += xbox::services::log_entry(level, category)

// default logging macro
const char defaultCategory[] = "";
#define IF_LOG_ERROR_ENABLED(x) { auto logger = DEFAULT_LOGGER; if (logger && logger->is_log_enabled(HCTraceLevel::Error)) { x; } }
#define LOG_ERROR(msg) LOG(DEFAULT_LOGGER, HCTraceLevel::Error, defaultCategory, msg)
#define LOG_ERROR_IF(boolean_expression, msg) if(boolean_expression) LOG_ERROR(msg)
#define LOGS_ERROR LOGS(HCTraceLevel::Error, defaultCategory)
#define LOGS_ERROR_IF(boolean_expression) if(boolean_expression) LOGS_ERROR

#define IF_LOG_WARN_ENABLED(x) { auto logger = DEFAULT_LOGGER; if (logger && logger->is_log_enabled(HCTraceLevel::Warning)) { x; } }
#define LOG_WARN(msg) LOG(DEFAULT_LOGGER, HCTraceLevel::Warning, defaultCategory, msg)
#define LOG_WARN_IF(boolean_expression, msg) if(boolean_expression) LOG_WARN(msg)
#define LOGS_WARN LOGS(HCTraceLevel::Warning, defaultCategory)
#define LOGS_WARN_IF(boolean_expression) if(boolean_expression) LOGS_WARN

#define IF_LOG_INFO_ENABLED(x) { auto logger = DEFAULT_LOGGER; if (logger && logger->is_log_enabled(HCTraceLevel::Information)) { x; } }
#define LOG_INFO(msg) LOG(DEFAULT_LOGGER, HCTraceLevel::Information, defaultCategory, msg)
#define LOG_INFO_IF(boolean_expression, msg) if(boolean_expression) LOG_INFO(msg)
#define LOGS_INFO LOGS(HCTraceLevel::Information, defaultCategory)
#define LOGS_INFO_IF(boolean_expression) if(boolean_expression) LOGS_INFO

#define IF_LOG_DEBUG_ENABLED(x) { auto logger = DEFAULT_LOGGER; if (logger && logger->is_log_enabled(HCTraceLevel::Verbose)) { x; } }
#define LOG_DEBUG(msg) LOG(DEFAULT_LOGGER, HCTraceLevel::Verbose, defaultCategory, msg)
#define LOG_DEBUG_IF(boolean_expression, msg) if(boolean_expression) LOG_DEBUG(msg)
#define LOGS_DEBUG LOGS(HCTraceLevel::Verbose, defaultCategory)
#define LOGS_DEBUG_IF(boolean_expression) if(boolean_expression) LOGS_DEBUG

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class log_entry
{
public:
    log_entry(HCTraceLevel level, xsapi_internal_string category);

    log_entry(HCTraceLevel level, xsapi_internal_string category, xsapi_internal_string msg);

    xsapi_internal_string level_to_string() const;

    const xsapi_internal_stringstream& msg_stream() const { return m_message; }

    const xsapi_internal_string& category() const { return m_category; }

    HCTraceLevel get_log_level() const { return m_logLevel; }

    log_entry& operator<<(const char* data)
    {
        m_message << data;
        return *this;
    }

    log_entry& operator<<(const xsapi_internal_string& data)
    {
        m_message << data;
        return *this;
    }

#if HC_PLATFORM_IS_MICROSOFT
    log_entry& operator<<(const wchar_t* data)
    {
        m_message << xbox::services::convert::to_utf8string(data);
        return *this;
    }

    log_entry& operator<<(const xsapi_internal_wstring& data)
    {
        m_message << xbox::services::convert::to_utf8string(data);
        return *this;
    }
#endif

    template<typename T>
    log_entry& operator<<(const T& data)
    {
        m_message << data;
        return *this;
    }

private:
    HCTraceLevel m_logLevel;
    xsapi_internal_string m_category;
    xsapi_internal_stringstream m_message;
};

class log_output
{
public:
    log_output();

    virtual void add_log(_In_ const log_entry& entry);

    bool log_level_enabled(HCTraceLevel level) const { return get_log_level() >= level; }

    HCTraceLevel get_log_level() const
    {
        HCTraceLevel traceLevel = HCTraceLevel::Off;
        HCSettingsGetTraceLevel(&traceLevel);
        return traceLevel;
    }

    virtual ~log_output() = default;

protected:
    // This function is to write the string to the final output, don't need to be thread safe.
    virtual void write(_In_ HCTraceLevel level, _In_ const xsapi_internal_string& msg);

    virtual xsapi_internal_string format_log(_In_ const log_entry& entry);

private:
    mutable std::mutex m_mutex;
};

class logger
{
public:
    logger() {}

    static std::shared_ptr<logger> get_logger();

    void set_log_level(HCTraceLevel level);
    void add_log_output(std::shared_ptr<log_output> output);

    void add_log(const log_entry& entry);
    bool is_log_enabled(HCTraceLevel level);
    void operator+=(const log_entry& record);

private:
    Vector<std::shared_ptr<log_output>> m_log_outputs;
};

class logger_raii
{
public:
    logger_raii()
    {
        m_logger = xbox::services::logger::get_logger();
    }

    void set_log_level(HCTraceLevel level)
    {
        if (m_logger) m_logger->set_log_level(level);
    }

    void add_log_output(std::shared_ptr<log_output> output)
    {
        if (m_logger) m_logger->add_log_output(output);
    }

    void add_log(const log_entry& entry)
    {
        if (m_logger) m_logger->add_log(entry);
    }

    void operator+=(const log_entry& record)
    {
        add_log(record);
    }

private:
    std::shared_ptr<logger> m_logger;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

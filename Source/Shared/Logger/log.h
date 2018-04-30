// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

 

#define DEFAULT_LOGGER xbox::services::logger::get_logger()
#define IF_LOGGER_ENABLED(logger) if(logger != nullptr)

#define LOG(logger, level, category, msg) IF_LOGGER_ENABLED(logger) logger->add_log(xbox::services::log_entry(level, category, msg))
#define LOGS(logger, level, category) IF_LOGGER_ENABLED(logger) *logger += xbox::services::log_entry(level, category)

// default logging macro
const char defaultCategory[] = "";
#define IF_LOG_ERROR() IF_LOG_LEVEL_ENABLED(DEFAULT_LOGGER, xbox::services::log_level::error)
#define LOG_ERROR(msg) LOG(DEFAULT_LOGGER, xbox::services::log_level::error, defaultCategory, msg)
#define LOG_ERROR_IF(boolean_expression, msg) if(boolean_expression) LOG_ERROR(msg)
#define LOGS_ERROR LOGS(DEFAULT_LOGGER, xbox::services::log_level::error, defaultCategory)
#define LOGS_ERROR_IF(boolean_expression) if(boolean_expression) LOGS_ERROR

#define IF_LOG_WARN() IF_LOG_LEVEL_ENABLED(DEFAULT_LOGGER, xbox::services::log_level::warn)
#define LOG_WARN(msg) LOG(DEFAULT_LOGGER, xbox::services::log_level::warn, defaultCategory, msg)
#define LOG_WARN_IF(boolean_expression, msg) if(boolean_expression) LOG_WARN(msg)
#define LOGS_WARN LOGS(DEFAULT_LOGGER, xbox::services::log_level::warn, defaultCategory)
#define LOGS_WARN_IF(boolean_expression) if(boolean_expression) LOGS_WARN

#define IF_LOG_INFO() IF_LOG_LEVEL_ENABLED(DEFAULT_LOGGER, xbox::services::log_level::info)
#define LOG_INFO(msg) LOG(DEFAULT_LOGGER, xbox::services::log_level::info, defaultCategory, msg)
#define LOG_INFO_IF(boolean_expression, msg) if(boolean_expression) LOG_INFO(msg)
#define LOGS_INFO LOGS(DEFAULT_LOGGER, xbox::services::log_level::info, defaultCategory)
#define LOGS_INFO_IF(boolean_expression) if(boolean_expression) LOGS_INFO

#define IF_LOG_DEBUG() IF_LOG_LEVEL_ENABLED(DEFAULT_LOGGER, xbox::services::log_level::debug)
#define LOG_DEBUG(msg) LOG(DEFAULT_LOGGER, xbox::services::log_level::debug, defaultCategory, msg)
#define LOG_DEBUG_IF(boolean_expression, msg) if(boolean_expression) LOG_DEBUG(msg)
#define LOGS_DEBUG LOGS(DEFAULT_LOGGER, xbox::services::log_level::debug, defaultCategory)
#define LOGS_DEBUG_IF(boolean_expression) if(boolean_expression) LOGS_DEBUG

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

enum class log_level
{
    off,
    error,
    warn,
    info,
    debug
};

class log_entry
{
public:
    log_entry(log_level level, std::string category);

    log_entry(log_level level, std::string category, std::string msg);

    std::string level_to_string() const;

    const std::stringstream& msg_stream() const { return m_message; }

    const std::string& category() const { return m_category; }
    log_level get_log_level() const { return m_logLevel;  }

    log_entry& operator<<(const char* data)
    {
        m_message << utility::conversions::to_utf8string(data);
        return *this;
    }

    log_entry& operator<<(const std::string& data)
    {
        m_message << utility::conversions::to_utf8string(data);
        return *this;
    }

#if !XSAPI_U
    log_entry& operator<<(const wchar_t* data)
    {
        m_message << utility::conversions::to_utf8string(data);
        return *this;
    }

    log_entry& operator<<(const std::wstring& data)
    {
        m_message << utility::conversions::to_utf8string(data);
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
    log_level m_logLevel;
    std::string m_category;
    std::stringstream m_message;
};

enum log_output_level_setting
{
    use_logger_setting,
    use_own_setting
};

class log_output
{
public:
    // When log_output_type is set to use_logger_setting, the level parameter will be ignored.
    log_output(log_output_level_setting type, log_level level);

    virtual void add_log(_In_ const log_entry& entry);

    log_output_level_setting level_setting() const { return m_levelSetting; }

    bool log_level_enabled(log_level level) const { return level <= m_logLevel; }

    void set_log_level(log_level level) { m_logLevel = level; }

protected:
    // This function is to write the string to the final output, don't need to be thread safe.
    virtual void write(_In_ const std::string& msg);

    virtual std::string format_log(_In_ const log_entry& entry);

private:
    log_output_level_setting m_levelSetting;
    log_level m_logLevel;
    mutable std::mutex m_mutex;
};

class logger
{
public:
    logger() : m_logLevel(log_level::warn) {}

    static void create_logger() { get_xsapi_singleton()->m_logger = std::make_shared<logger>();  }
    static void release_logger() 
    { 
        auto singleton = get_xsapi_singleton(false);
        if( singleton ) singleton->m_logger = nullptr;
    }
    static const std::shared_ptr<logger>& get_logger() { return get_xsapi_singleton()->m_logger; }

    void set_log_level(log_level level);

    void add_log_output(std::shared_ptr<log_output> output);

    void add_log(const log_entry& entry);
    void operator+=(const log_entry& record);


private:
    std::vector<std::shared_ptr<log_output>> m_log_outputs;
    log_level m_logLevel;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

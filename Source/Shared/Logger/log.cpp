// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "log.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void logger::add_log_output(std::shared_ptr<log_output> output)
{
    m_log_outputs.emplace_back(output); 
    if (output->level_setting() == log_output_level_setting::use_logger_setting)
    {
        output->set_log_level(m_logLevel);
    }
};

void logger::set_log_level(log_level level)
{
    m_logLevel = level;

    HCTraceLevel traceLevel = HCTraceLevel::HCTraceLevel_Off;
    switch (level)
    {
        case log_level::off: traceLevel = HCTraceLevel::HCTraceLevel_Off; break;
        case log_level::error: traceLevel = HCTraceLevel::HCTraceLevel_Error; break;
        case log_level::warn: traceLevel = HCTraceLevel::HCTraceLevel_Warning; break;
        case log_level::info: traceLevel = HCTraceLevel::HCTraceLevel_Information; break;
        case log_level::debug: traceLevel = HCTraceLevel::HCTraceLevel_Verbose; break;
    }
    HCSettingsSetTraceLevel(traceLevel);

    for (const auto& output : m_log_outputs)
    {
        if (output->level_setting() == log_output_level_setting::use_logger_setting)
        {
            output->set_log_level(level);
        }
    }
}

void logger::add_log(const log_entry& logEntry)
{
    for(const auto& output : m_log_outputs)
    {
        if (output->log_level_enabled(logEntry.get_log_level()))
        {
            output->add_log(logEntry);
        }
    }
}

void logger::operator+=(const log_entry& logEntry)
{
    add_log(logEntry);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

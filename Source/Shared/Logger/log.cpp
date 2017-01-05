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
#include "log.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
#endif

std::shared_ptr<logger> logger::s_logger = nullptr;

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

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#endif

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "log.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<logger> logger::get_logger()
{
    auto state = GlobalState::Get();
    if (state)
    {
        return state->Logger();
    }
    return nullptr;
}

void logger::add_log_output(std::shared_ptr<log_output> output)
{
    m_log_outputs.emplace_back(output); 
};

void logger::set_log_level(HCTraceLevel level)
{
    HCSettingsSetTraceLevel(level);
}

bool logger::is_log_enabled(HCTraceLevel level)
{
    for (const auto& output : m_log_outputs)
    {
        if (output->log_level_enabled(level))
        {
            return true;
        }
    }

    return false;
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

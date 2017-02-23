// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "log.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

log_entry::log_entry(log_level level, std::string category) :
    m_logLevel(level),
    m_category(std::move(category))
{

}

log_entry::log_entry(log_level level, std::string category, std::string msg) :
    m_logLevel(level),
    m_category(std::move(category))
{
    m_message << msg;
}

std::string log_entry::level_to_string() const 
{
    switch (m_logLevel)
    {
    case log_level::error:
        return "error";
    case log_level::warn:
        return "warn";
    case log_level::info:
        return "info";
    case log_level::debug:
        return "debug";
    }

    return "";
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

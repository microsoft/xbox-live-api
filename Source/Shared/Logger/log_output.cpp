// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Logger/log.h"
#include <iomanip>

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

log_output::log_output()
{
}

void log_output::add_log(_In_ const log_entry& entry)
{
    xsapi_internal_string msg = format_log(entry);
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        write(entry.get_log_level(), msg);
    }
}

void log_output::write(_In_ HCTraceLevel level, _In_ const xsapi_internal_string& msg)
{
    UNREFERENCED_PARAMETER(level);
    UNREFERENCED_PARAMETER(msg);
}

xsapi_internal_string
log_output::format_log(_In_ const log_entry& entry)
{
    return entry.msg_stream().str();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "custom_output.h"
#include "xsapi/system.h"

using namespace xbox::services::system;
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void custom_output::add_log(_In_ const log_entry& entry)
{
    xbox_services_diagnostics_trace_level logLevel = xbox_services_diagnostics_trace_level::off;
    switch (entry.get_log_level())
    {
        case log_level::off: logLevel = xbox_services_diagnostics_trace_level::off; break;
        case log_level::error: logLevel = xbox_services_diagnostics_trace_level::error; break;
        case log_level::warn: logLevel = xbox_services_diagnostics_trace_level::warning; break;
        case log_level::info: logLevel = xbox_services_diagnostics_trace_level::info; break;
        case log_level::debug: logLevel = xbox_services_diagnostics_trace_level::verbose; break;
    }

    xbox_live_services_settings::get_singleton_instance()->_Raise_logging_event(logLevel, entry.category(), entry.msg_stream().str());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

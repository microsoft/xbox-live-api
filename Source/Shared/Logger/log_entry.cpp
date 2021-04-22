// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "log.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

log_entry::log_entry(HCTraceLevel level, xsapi_internal_string category) :
    m_logLevel(level),
    m_category(std::move(category))
{
}

log_entry::log_entry(HCTraceLevel level, xsapi_internal_string category, xsapi_internal_string msg) :
    m_logLevel(level),
    m_category(std::move(category))
{
    m_message << msg;
}

xsapi_internal_string log_entry::level_to_string() const
{
    switch (m_logLevel)
    {
        case HCTraceLevel::Error: return "L1";
        case HCTraceLevel::Warning: return "L2";
        case HCTraceLevel::Important: return "L3";
        case HCTraceLevel::Information: return "L4";
        case HCTraceLevel::Verbose: return "L5";
        default: break;
    }

    return "";
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

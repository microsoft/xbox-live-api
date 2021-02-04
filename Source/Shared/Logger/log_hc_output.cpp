// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "log_hc_output.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void log_hc_output::write(_In_ HCTraceLevel level, _In_ const xsapi_internal_string& msg)
{
    HC_TRACE_MESSAGE(XSAPI, level, msg.c_str());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

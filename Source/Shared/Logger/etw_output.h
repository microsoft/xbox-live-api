// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "log.h"
#include <evntrace.h>
#include "telemetry.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class etw_output : public log_output
{
public:
    etw_output();

    ~etw_output();

    void add_log(_In_ const log_entry& entry) override;

private:
    // Use template for LEVEl as _TraceLoggingLevel itself is a template method, cannot use variable on it.
    template<UCHAR LEVEL>
    void add_log_t(_In_ const log_entry& entry)
    {
#pragma warning(suppress: 6386)
        TraceLoggingWrite(
            g_hUnitTestTraceLoggingProvider,
            "XSAPI",
            TraceLoggingLevel(LEVEL),
            TraceLoggingString(entry.category().c_str(), "category"),
            TraceLoggingString(entry.msg_stream().str().c_str(), "message")
        );
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
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
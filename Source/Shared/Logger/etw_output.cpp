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

#ifndef _TRACEHANDLE_DEFINED
#define _TRACEHANDLE_DEFINED
typedef ULONG64 TRACEHANDLE, *PTRACEHANDLE;
#endif
#define EVENT_CONTROL_CODE_DISABLE_PROVIDER 0
#define EVENT_CONTROL_CODE_ENABLE_PROVIDER  1
#define EVENT_CONTROL_CODE_CAPTURE_STATE    2

#include "etw_output.h"
#include "ERA_ETW.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

etw_output::etw_output():
    log_output(log_output_level_setting::use_own_setting, log_level::debug)
{
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN10
    TraceLoggingRegister(g_hUnitTestTraceLoggingProvider);
#elif TV_API
    EventRegisterMicrosoft_Xbox_Services_Tracing();
#endif
}

etw_output::~etw_output()
{
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN10
    TraceLoggingUnregister(g_hUnitTestTraceLoggingProvider);
#elif TV_API
    EventUnregisterMicrosoft_Xbox_Services_Tracing();
#endif
}

void etw_output::add_log(_In_ const log_entry& entry)
{
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN10
    switch (entry.get_log_level())
    {
        case log_level::error: add_log_t<TRACE_LEVEL_ERROR>(entry); break;
        case log_level::warn: add_log_t<TRACE_LEVEL_WARNING>(entry); break;
        case log_level::info: add_log_t<TRACE_LEVEL_INFORMATION>(entry); break;
        case log_level::debug: add_log_t<TRACE_LEVEL_VERBOSE>(entry); break;
    }
    

#elif TV_API
    ULONG status = NO_ERROR;

    switch (entry.get_log_level())
    {
        case log_level::error: status = EventWriteXSAPI_Error(entry.category().c_str(), entry.msg_stream().str().c_str()); break;
        case log_level::warn: status = EventWriteXSAPI_Warn(entry.category().c_str(), entry.msg_stream().str().c_str()); break;
        case log_level::info: status = EventWriteXSAPI_Info(entry.category().c_str(), entry.msg_stream().str().c_str()); break;
        case log_level::debug: status = EventWriteXSAPI_Verbose(entry.category().c_str(), entry.msg_stream().str().c_str()); break;
    }
#endif

}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
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
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN10

#include <TraceLoggingProvider.h>

#define XSAPI_TELEMETRY_KEYWORDS 0x0000400000000000
TRACELOGGING_DECLARE_PROVIDER(g_hTraceLoggingProvider);
TRACELOGGING_DECLARE_PROVIDER(g_hUnitTestTraceLoggingProvider);

#else

#define TraceLoggingWrite(hProvider, eventName, ...) void(0)
#define TraceLoggingUnregister(hProvider) void(0)
#define TraceLoggingRegister(hProvider) void(0)

#endif

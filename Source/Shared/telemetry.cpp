// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"

#if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WIN32_WINNT_WIN10

#include "telemetry.h"

#define TraceLoggingOptionMicrosoftTelemetry() \
    TraceLoggingOptionGroup(0x4f50731a, 0x89cf, 0x4782, 0xb3, 0xe0, 0xdc, 0xe8, 0xc9, 0x4, 0x76, 0xba)

TRACELOGGING_DEFINE_PROVIDER(
    g_hTraceLoggingProvider,
    "Microsoft.Xbox.Services",
    (0xe8f7748f, 0xd38f, 0x4f0d, 0x8f, 0x5d, 0x9, 0xe, 0x39, 0x18, 0xfc, 0xd6), // {E8F7748F-D38F-4F0D-8F5D-090E3918FCD6}
    TraceLoggingOptionMicrosoftTelemetry());

TRACELOGGING_DEFINE_PROVIDER(
    g_hUnitTestTraceLoggingProvider,
    "Microsoft.Xbox.Services.Tracing",
    (0x9594a560, 0xe985, 0x4ee6, 0xb0, 0xb5, 0xd, 0xac, 0x4f, 0x92, 0x41, 0x44), // {9594A560-E985-4EE6-B0B5-0DAC4F924144}
    );

#endif

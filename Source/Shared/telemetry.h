// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

class xsapi_telemetry
{
public:
    static std::shared_ptr<xsapi_telemetry> get_singleton();

    void write_event(xbox_live_user_t user, xsapi_internal_string eventName, xsapi_internal_string jsonPayload, bool isRealtimeEvent = false);

#if HC_PLATFORM == HC_PLATFORM_ANDROID
    static void write_event_jni(JNIEnv *, jclass, jstring jsonPayload);

    static bool register_natives(JNIEnv *env, jobject clsLoader, jmethodID loadClass);
#endif
private:
    xsapi_telemetry();

    xsapi_internal_string m_iKey;
    static std::atomic<uint32_t> s_ticketId;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

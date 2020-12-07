// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "telemetry.h"
#include "jni.h"
#include "jni_utils.h"
#include <android/log.h>

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "XSAPI.Android", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "XSAPI.Android", __VA_ARGS__))

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

void xsapi_telemetry::write_event_jni(JNIEnv* env, jclass clsInterop, jstring json)
{
    jstring_t eventPayload(env, json);

    // Might want to eventually pass the xal user handle as a parameter, but for now just remember the last signed in
    // in user we've seen
    XalUserHandle user = xbox::services::get_xsapi_singleton()->m_lastSignedInUser.load();
    if (user != nullptr)
    {
        xbox::services::xsapi_telemetry::get_singleton()->write_event(user, "JNIEvent", eventPayload.get());
    }
}

bool xsapi_telemetry::register_natives(JNIEnv *env, jobject clsLoader, jmethodID loadClass)
{
    JNINativeMethod telemetryMethods[] =
    {
        {
            "WriteEvent",
            "(Ljava/lang/String;)V",
            (void*)&xbox::services::xsapi_telemetry::write_event_jni
        }
    };

    jstring clsName = env->NewStringUTF("com/microsoft/xbox/telemetry/helpers/UTCTelemetry");
    JNI_ERROR_CHECK(env);
    jclass cls = (jclass)env->CallObjectMethod(clsLoader, loadClass, clsName);
    env->DeleteLocalRef(clsName);
    if (cls == NULL) {
        LOGE("Failed to load class com/microsoft/xbox/telemetry/helpers/UTCTelemetry");
        return false;
    }
    auto size = (sizeof(telemetryMethods) / sizeof (telemetryMethods[0]));

    if (env->RegisterNatives(cls, telemetryMethods, size) != 0) {
        LOGE("Failed to register native telemetryMethods");
        env->DeleteLocalRef(cls);
        return false;
    }
    env->DeleteLocalRef(cls);
    LOGD("Successfully registerered telemetryMethods");
    return true;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#pragma once

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

    JNIEXPORT jboolean JNICALL Java_com_microsoft_xbox_idp_interop_Interop_initializeInterop(JNIEnv* env, jclass clsInterop, jobject context);

    JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_interop_Interop_deinitializeInterop(JNIEnv* env, jclass clsInterop);

#ifdef __cplusplus
}
#endif
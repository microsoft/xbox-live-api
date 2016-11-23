#pragma once

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_microsoft_xbox_idp_interop_LocalConfig
 * Method:    create
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_xbox_idp_interop_LocalConfig_create
  (JNIEnv *, jclass);

/*
 * Class:     com_microsoft_xbox_idp_interop_LocalConfig
 * Method:    delete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_interop_LocalConfig_delete
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_microsoft_xbox_idp_interop_LocalConfig
 * Method:    getCid
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_microsoft_xbox_idp_interop_LocalConfig_getCid
  (JNIEnv *, jclass, jlong);

#ifdef __cplusplus
}
#endif

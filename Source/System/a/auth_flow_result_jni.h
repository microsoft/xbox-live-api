#pragma once

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    getRpsTicket
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_com_microsoft_xbox_idp_util_AuthFlowResult_getRpsTicket
        (JNIEnv *, jclass, jlong);

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    getUserId
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_com_microsoft_xbox_idp_util_AuthFlowResult_getUserId
        (JNIEnv *, jclass, jlong);

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    getGamerTag
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_com_microsoft_xbox_idp_util_AuthFlowResult_getGamerTag
        (JNIEnv *, jclass, jlong);

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    getAgeGroup
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_com_microsoft_xbox_idp_util_AuthFlowResult_getAgeGroup
        (JNIEnv *, jclass, jlong);

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    getPrivileges
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_com_microsoft_xbox_idp_util_AuthFlowResult_getPrivileges
        (JNIEnv *, jclass, jlong);

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    delete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_microsoft_xbox_idp_util_AuthFlowResult_delete
(JNIEnv * , jclass , jlong ) ;

#ifdef __cplusplus
}
#endif

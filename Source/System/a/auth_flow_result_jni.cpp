#include "auth_flow_result_jni.h"
#include "user_auth_android.h"
#include <string>

using namespace xbox::services::system;

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    getRpsTicket
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_com_microsoft_xbox_idp_util_AuthFlowResult_getRpsTicket
        (JNIEnv * env, jclass cls, jlong id)
{
    auth_flow_result* self = reinterpret_cast<auth_flow_result*>(id);
    return env->NewStringUTF(self->rps_ticket.c_str());
}

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    getUserId
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_com_microsoft_xbox_idp_util_AuthFlowResult_getUserId
        (JNIEnv * env, jclass cls, jlong id)
{
    auth_flow_result* self = reinterpret_cast<auth_flow_result*>(id);
    return env->NewStringUTF(self->user_id.c_str());
}

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    getGamerTag
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_com_microsoft_xbox_idp_util_AuthFlowResult_getGamerTag
        (JNIEnv * env, jclass cls, jlong id)
{
    auth_flow_result* self = reinterpret_cast<auth_flow_result*>(id);
    return env->NewStringUTF(self->gamertag.c_str());
}

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    getAgeGroup
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_com_microsoft_xbox_idp_util_AuthFlowResult_getAgeGroup
        (JNIEnv * env, jclass cls, jlong id)
{
    auth_flow_result* self = reinterpret_cast<auth_flow_result*>(id);
    return env->NewStringUTF(self->age_group.c_str());
}

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    getPrivileges
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring
JNICALL Java_com_microsoft_xbox_idp_util_AuthFlowResult_getPrivileges
        (JNIEnv * env, jclass cls, jlong id)
{
    auth_flow_result* self = reinterpret_cast<auth_flow_result*>(id);
    return env->NewStringUTF(self->privileges.c_str());
}

/*
 * Class:     com_microsoft_xbox_idp_util_AuthFlowResult
 * Method:    delete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_com_microsoft_xbox_idp_util_AuthFlowResult_delete
(JNIEnv * env, jclass cls, jlong id)
{
    delete reinterpret_cast<auth_flow_result*>(id);
}


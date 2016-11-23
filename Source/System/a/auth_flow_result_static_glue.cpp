#include "auth_flow_result_jni.h"
#include <jni.h>
#include <android/log.h>

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "XSAPI.Android", __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "XSAPI.Android", __VA_ARGS__))

static JNINativeMethod methods[] = {
        {
                "getRpsTicket",
                "(J)Ljava/lang/String;",
                (void*)Java_com_microsoft_xbox_idp_util_AuthFlowResult_getRpsTicket
        },
        {
                "getUserId",
                "(J)Ljava/lang/String;",
                (void*)Java_com_microsoft_xbox_idp_util_AuthFlowResult_getUserId
        },
        {
                "getGamerTag",
                "(J)Ljava/lang/String;",
                (void*)Java_com_microsoft_xbox_idp_util_AuthFlowResult_getGamerTag
        },
        {
                "getAgeGroup",
                "(J)Ljava/lang/String;",
                (void*)Java_com_microsoft_xbox_idp_util_AuthFlowResult_getAgeGroup
        },
        {
                "getPrivileges",
                "(J)Ljava/lang/String;",
                (void*)Java_com_microsoft_xbox_idp_util_AuthFlowResult_getPrivileges
        },
        {
                "delete",
                "(J)V",
                (void*)Java_com_microsoft_xbox_idp_util_AuthFlowResult_delete
        },
};

bool auth_flow_result_register_natives(JNIEnv *env, jobject clsLoader, jmethodID loadClass)
{
    jstring clsName = env->NewStringUTF("com/microsoft/xbox/idp/util/AuthFlowResult");
    jclass cls = (jclass)env->CallObjectMethod(clsLoader, loadClass, clsName);
    env->DeleteLocalRef(clsName);
    if (cls == NULL) {
        LOGE("Failed to load class com/microsoft/xbox/idp/util/AuthFlowResult");
        return false;
    }
    if (env->RegisterNatives(cls, methods, sizeof methods / sizeof *methods) != 0) {
        LOGE("Failed to register native methods");
        env->DeleteLocalRef(cls);
        return false;
    }
    env->DeleteLocalRef(cls);
    LOGD("Successfully registerered HttpCall methods");
    return true;
}

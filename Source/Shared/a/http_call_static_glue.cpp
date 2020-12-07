#include "pch.h"
#include "http_call_jni.h"
#include <android/log.h>

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "HttpCallStaticGlue", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "HttpCallStaticGlue", __VA_ARGS__))

static JNINativeMethod methods[] = {
    {
        "setRequestBody",
        "(Ljava/lang/String;)V",
        (void*)Java_com_microsoft_xbox_idp_util_HttpCall_setRequestBody__Ljava_lang_String_2
    },
    {
        "setRequestBody",
        "([B)V",
        (void*)Java_com_microsoft_xbox_idp_util_HttpCall_setRequestBody___3B
    },
    {
        "setCustomHeader",
        "(Ljava/lang/String;Ljava/lang/String;)V",
        (void*)Java_com_microsoft_xbox_idp_util_HttpCall_setCustomHeader
    },
    {
        "setRetryAllowed",
        "(Z)V",
        (void*)Java_com_microsoft_xbox_idp_util_HttpCall_setRetryAllowed
    },
    {
        "setLongHttpCall",
        "(Z)V",
        (void*)Java_com_microsoft_xbox_idp_util_HttpCall_setLongHttpCall
    },
    {
        "setContentTypeHeaderValue",
        "(Ljava/lang/String;)V",
        (void*)Java_com_microsoft_xbox_idp_util_HttpCall_setContentTypeHeaderValue
    },
    {
        "setXboxContractVersionHeaderValue",
        "(Ljava/lang/String;)V",
        (void*)Java_com_microsoft_xbox_idp_util_HttpCall_setXboxContractVersionHeaderValue
    },
    {
        "getResponseAsync",
        "(Lcom/microsoft/xbox/idp/util/HttpCall$Callback;)V",
        (void*)Java_com_microsoft_xbox_idp_util_HttpCall_getResponseAsync__Lcom_microsoft_xbox_idp_util_HttpCall_Callback_2
    },
    {
        "create",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)J",
        (void*)Java_com_microsoft_xbox_idp_util_HttpCall_create
    },
    {
        "delete",
        "(J)V",
        (void*)Java_com_microsoft_xbox_idp_util_HttpCall_delete
    }
};

bool http_call_register_natives(JNIEnv *env, jobject clsLoader, jmethodID loadClass)
{
    jstring clsName = env->NewStringUTF("com/microsoft/xbox/idp/util/HttpCall");
    jclass cls = (jclass)env->CallObjectMethod(clsLoader, loadClass, clsName);
    env->DeleteLocalRef(clsName);
    if (cls == NULL) {
        LOGE("Failed to load class com/microsoft/xbox/idp/util/HttpCall");
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

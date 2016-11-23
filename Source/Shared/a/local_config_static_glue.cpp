#include "pch.h"
#include "local_config_jni.h"
#include <android/log.h>
#include "a/jni_utils.h"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "HttpCallStaticGlue", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "HttpCallStaticGlue", __VA_ARGS__))

static JNINativeMethod methods[] = {
    {
        "create",
        "()J",
        (void*)Java_com_microsoft_xbox_idp_interop_LocalConfig_create
    },
    {
        "delete",
        "(J)V",
        (void*)Java_com_microsoft_xbox_idp_interop_LocalConfig_delete
    },
    {
        "getCid",
        "(J)Ljava/lang/String;",
        (void*)Java_com_microsoft_xbox_idp_interop_LocalConfig_getCid
    }
};

bool local_config_register_natives(JNIEnv *env, jobject clsLoader, jmethodID loadClass)
{
    jstring clsName = env->NewStringUTF("com/microsoft/xbox/idp/interop/LocalConfig");
    JNI_ERROR_CHECK(env);
    jclass cls = (jclass)env->CallObjectMethod(clsLoader, loadClass, clsName);
    env->DeleteLocalRef(clsName);
    if (cls == NULL) {
        LOGE("Failed to load class com/microsoft/xbox/idp/interop/LocalConfig");
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

#include "pch.h"
#include "TCUI/Android/title_callable_ui_jni.h"
#include <android/log.h>
#include "a/jni_utils.h"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "XSAPI.Android", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "XSAPI.Android", __VA_ARGS__))

JNINativeMethod tcuiMethods[] =
{
    {
        "tcui_completed_callback",
        "(I)V",
        (void*)&xbox::services::system::title_callable_ui_internal::tcui_completed_callback
    },
};

bool title_callable_ui_register_natives(JNIEnv *env, jobject clsLoader, jmethodID loadClass)
{
    jstring clsName = env->NewStringUTF("com/microsoft/xboxtcui/Interop");
    JNI_ERROR_CHECK(env);
    jclass cls = (jclass)env->CallObjectMethod(clsLoader, loadClass, clsName);
    env->DeleteLocalRef(clsName);
    if (cls == NULL) {
        LOGE("Failed to load class com/microsoft/xboxtcui/Interop");
        return false;
    }
    auto size = (sizeof tcuiMethods / sizeof *tcuiMethods);

    if (env->RegisterNatives(cls, tcuiMethods, size) != 0) {
        LOGE("Failed to register native tcuiMethods");
        env->DeleteLocalRef(cls);
        return false;
    }
    env->DeleteLocalRef(cls);
    LOGD("Successfully registerered HttpCall tcuiMethods");
    return true;
}
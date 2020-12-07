#include "pch.h"
#include "jni_utils.h"
#include "xbox_live_app_config_static_glue.h"
#include "xbox_live_app_config_internal.h"
#include <httpClient/httpClient.h>
#include <android/log.h>

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "XboxLiveAppConfig", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "XboxLiveAppConfig", __VA_ARGS__))

using namespace xbox::services;
using namespace xbox::services::system;

#ifdef __cplusplus
extern "C" {
#endif

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    create
    * Signature: ()J
    */
    JNIEXPORT jlong JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_create
        (JNIEnv * env, jclass cls)
    {
        return reinterpret_cast<jlong>(new std::shared_ptr<AppConfig>(xbox::services::AppConfig::Instance()));
    }

   /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    delete
    * Signature: (J)V
    */
    JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_delete
        (JNIEnv * env, jclass cls, jlong id)
    {
        delete reinterpret_cast<std::shared_ptr<AppConfig>*>(id);
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    getTitleId
    * Signature: (J)I
    */
    JNIEXPORT jint JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getTitleId
        (JNIEnv * env, jclass cls, jlong id)
    {
        std::shared_ptr<AppConfig>* cfg = reinterpret_cast<std::shared_ptr<AppConfig>*>(id);
        return static_cast<jint>((*cfg)->TitleId());
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    getOverrideTitleId
    * Signature: (J)I
    */
    JNIEXPORT jint JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getOverrideTitleId
    (JNIEnv * env, jclass cls, jlong id)
    {
        std::shared_ptr<AppConfig>* cfg = reinterpret_cast<std::shared_ptr<AppConfig>*>(id);
        return static_cast<jint>(std::stoi((*cfg)->OverrideScid().data()));
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    getScid
    * Signature: (J)Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getScid
        (JNIEnv * env, jclass cls, jlong id)
    {
        std::shared_ptr<AppConfig>* cfg = reinterpret_cast<std::shared_ptr<AppConfig>*>(id);
        string_t scid = (*cfg)->Scid().c_str();
        return scid.empty() ? nullptr : env->NewStringUTF(scid.c_str());
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    getEnvironment
    * Signature: (J)Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getEnvironment
        (JNIEnv * env, jclass cls, jlong id)
    {
        return nullptr;
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    getSandbox
    * Signature: (J)Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getSandbox
        (JNIEnv * env, jclass cls, jlong id)
    {
        std::shared_ptr<AppConfig>* cfg = reinterpret_cast<std::shared_ptr<AppConfig>*>(id);
        auto& sandbox = (*cfg)->Sandbox();
        return sandbox.empty() ? nullptr : env->NewStringUTF(sandbox.c_str());
    }

#ifdef __cplusplus
}
#endif

static JNINativeMethod methods[] = {
    {
        "create",
        "()J",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_create
    },
    {
        "delete",
        "(J)V",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_delete
    },
    {
        "getTitleId",
        "(J)I",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getTitleId
    },
    {
        "getOverrideTitleId",
        "(J)I",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getOverrideTitleId
    },
    {
        "getScid",
        "(J)Ljava/lang/String;",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getScid
    },
    {
        "getEnvironment",
        "(J)Ljava/lang/String;",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getEnvironment
    },
    {
        "getSandbox",
        "(J)Ljava/lang/String;",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getSandbox
    }
};

bool xbox_live_app_config_register_natives(JNIEnv *env, jobject clsLoader, jmethodID loadClass)
{
    jstring clsName = env->NewStringUTF("com/microsoft/xbox/idp/interop/XboxLiveAppConfig");
    jclass cls = (jclass)env->CallObjectMethod(clsLoader, loadClass, clsName);
    env->DeleteLocalRef(clsName);
    if (cls == NULL) {
        LOGE("Failed to load class com/microsoft/xbox/idp/interop/XboxLiveAppConfig");
        return false;
    }
    if (env->RegisterNatives(cls, methods, sizeof methods / sizeof *methods) != 0) {
        LOGE("Failed to register native methods");
        env->DeleteLocalRef(cls);
        return false;
    }
    env->DeleteLocalRef(cls);
    LOGD("Successfully registerered XboxLiveAppConfig methods");
    return true;
}

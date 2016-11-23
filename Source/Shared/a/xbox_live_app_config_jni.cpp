#include "pch.h"
#include "jni_utils.h"
#include "xbox_live_app_config_static_glue.h"
#include <cpprest/http_client.h>
#include <a/user_impl_a.h>
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
        return reinterpret_cast<jlong>(new std::shared_ptr<xbox_live_app_config>(xbox::services::xbox_live_app_config::get_app_config_singleton()));
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    delete
    * Signature: (J)V
    */
    JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_delete
        (JNIEnv * env, jclass cls, jlong id)
    {
        delete reinterpret_cast<std::shared_ptr<xbox_live_app_config>*>(id);
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    getTitleId
    * Signature: (J)I
    */
    JNIEXPORT jint JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getTitleId
        (JNIEnv * env, jclass cls, jlong id)
    {
        std::shared_ptr<xbox_live_app_config>* cfg = reinterpret_cast<std::shared_ptr<xbox_live_app_config>*>(id);
        return static_cast<jint>((*cfg)->title_id());
    }

	/*
	* Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
	* Method:    getOverrideTitleId
	* Signature: (J)I
	*/
	JNIEXPORT jint JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getOverrideTitleId
	(JNIEnv * env, jclass cls, jlong id)
	{
		std::shared_ptr<xbox_live_app_config>* cfg = reinterpret_cast<std::shared_ptr<xbox_live_app_config>*>(id);
		return static_cast<jint>((*cfg)->_Override_title_id_for_multiplayer());
	}

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    getScid
    * Signature: (J)Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getScid
        (JNIEnv * env, jclass cls, jlong id)
    {
        std::shared_ptr<xbox_live_app_config>* cfg = reinterpret_cast<std::shared_ptr<xbox_live_app_config>*>(id);
        string_t scid = (*cfg)->scid();
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
        std::shared_ptr<xbox_live_app_config>* cfg = reinterpret_cast<std::shared_ptr<xbox_live_app_config>*>(id);
        string_t environment = (*cfg)->environment();
        return environment.empty() ? nullptr : env->NewStringUTF(environment.c_str());
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    setEnvironment
    * Signature: (JLjava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_setEnvironment
        (JNIEnv * env, jclass cls, jlong id, jstring value)
    {
        std::shared_ptr<xbox_live_app_config>* cfg = reinterpret_cast<std::shared_ptr<xbox_live_app_config>*>(id);
        jstring_t environment(env, value);
        (*cfg)->set_environment(environment);
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    getSandbox
    * Signature: (J)Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getSandbox
        (JNIEnv * env, jclass cls, jlong id)
    {
        std::shared_ptr<xbox_live_app_config>* cfg = reinterpret_cast<std::shared_ptr<xbox_live_app_config>*>(id);
        string_t sandbox = (*cfg)->sandbox();
        return sandbox.empty() ? nullptr : env->NewStringUTF(sandbox.c_str());
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    setSandbox
    * Signature: (JLjava/lang/String;)V
    */
    JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_setSandbox
        (JNIEnv * env, jclass cls, jlong id, jstring value)
    {
        std::shared_ptr<xbox_live_app_config>* cfg = reinterpret_cast<std::shared_ptr<xbox_live_app_config>*>(id);
        jstring_t sandbox(env, value);
        (*cfg)->set_sandbox(sandbox);
    }

    /*
    * Class:     com_microsoft_xbox_idp_interop_XboxLiveAppConfig
    * Method:    getProxy
    * Signature: (J)Ljava/lang/String;
    */
    JNIEXPORT jstring JNICALL Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getProxy
        (JNIEnv * env, jclass cls, jlong id)
    {
        std::shared_ptr<xbox_live_app_config>* cfg = reinterpret_cast<std::shared_ptr<xbox_live_app_config>*>(id);
        web::uri proxy = (*cfg)->_Proxy();
        return proxy.is_empty() ? nullptr : env->NewStringUTF(proxy.to_string().c_str());
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
        "setEnvironment",
        "(JLjava/lang/String;)V",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_setEnvironment
    },
    {
        "getSandbox",
        "(J)Ljava/lang/String;",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getSandbox
    },
    {
        "setSandbox",
        "(JLjava/lang/String;)V",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_setSandbox
    },
    {
        "getProxy",
        "(J)Ljava/lang/String;",
        (void*)Java_com_microsoft_xbox_idp_interop_XboxLiveAppConfig_getProxy
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

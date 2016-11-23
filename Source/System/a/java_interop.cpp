#include "pch.h"
#include "xsapi/system.h"
#include "user_impl.h"
#include <android/log.h>
#include "user_auth_android.h"
#include "a/http_call_static_glue.h"
#include "a/local_config_static_glue.h"
#include "a/xbox_live_app_config_static_glue.h"
#include "Misc/Android/title_callable_static_glue.h"
#include "auth_flow_result_static_glue.h"
#include "a/jni_utils.h"
#include "a/java_interop.h"
#include "Misc/Android/title_callable_ui_jni.h"

using namespace xbox::services::system;

JNINativeMethod methods[] =
{
    {
        "ticket_callback",
        "(Ljava/lang/String;IILjava/lang/String;)V",
        (void *)&xbox::services::system::user_auth_android::ticket_callback
    },
    {
        "auth_flow_callback",
        "(JILjava/lang/String;)V",
        (void*)&xbox::services::system::user_auth_android::auth_flow_callback
    },
    {
        "invoke_xb_login",
        "(JLjava/lang/String;Lcom/microsoft/xbox/idp/interop/Interop$XBLoginCallback;)V",
        (void*)&xbox::services::system::user_auth_android::invoke_xb_login
    },
    {
        "invoke_event_initialization",
        "(JLjava/lang/String;Lcom/microsoft/xbox/idp/interop/Interop$EventInitializationCallback;)V",
        (void*)&xbox::services::system::user_auth_android::invoke_event_initialization
    },
    {
        "invoke_xb_logout",
        "(JLcom/microsoft/xbox/idp/interop/Interop$XBLogoutCallback;)V",
        (void*)&xbox::services::system::user_auth_android::invoke_xb_logout
    },
    {
        "invoke_x_token_acquisition",
        "(JLcom/microsoft/xbox/idp/interop/Interop$Callback;)V",
        (void*)&xbox::services::system::user_auth_android::invoke_x_token_acquisition
    },
    {
        "get_uploader_x_token_callback",
        "(Z)Ljava/lang/String;",
        (void*)&xbox::services::system::user_auth_android::get_uploader_x_token_callback
    },
    {
        "get_supporting_x_token_callback",
        "(Ljava/lang/String;)Ljava/lang/String;",
        (void*)&xbox::services::system::user_auth_android::get_supporting_x_token_callback
    },
    {
        "get_title_telemetry_device_id",
        "()Ljava/lang/String;",
        (void*)&xbox::services::system::user_auth_android::get_title_telemetry_device_id
    },
    {
        "get_title_telemetry_session_id",
        "()Ljava/lang/String;",
        (void*)&xbox::services::system::user_auth_android::get_title_telemetry_session_id
    },
    {
        "sign_out_callback",
        "()V",
        (void*)&xbox::services::system::user_auth_android::sign_out_callback
    },
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

std::shared_ptr<java_interop> java_interop::s_javaInterop;

std::shared_ptr<java_interop> java_interop::java_interop::get_java_interop_singleton()
{
    if (s_javaInterop != nullptr)
    {
        return s_javaInterop;
    }
    s_javaInterop = std::make_shared<java_interop>();
    return s_javaInterop;
}

java_interop::java_interop():
    m_javaVM(nullptr),
    m_activity(nullptr),
    m_marketActivityClass(nullptr),
    m_contextObject(nullptr),
    m_tcuiInteropClass(nullptr),
    m_initialized(false)
{

}

xbox_live_result<void> java_interop::initialize(JavaVM* jvm, jobject activity)
{
    m_javaVM = jvm;
    m_activity = activity;
    cpprest_init(m_javaVM);
    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(m_javaVM, jniEnv)

    jclass acl = jniEnv->GetObjectClass(m_activity);
    if (acl != NULL)
    {
        jmethodID getClassLoader = jniEnv->GetMethodID(acl, "getClassLoader", "()Ljava/lang/ClassLoader;");
        if (getClassLoader != NULL)
        {
            jobject cls = jniEnv->CallObjectMethod(m_activity, getClassLoader);
            if (cls != NULL)
            {
                jclass classLoader = jniEnv->FindClass("java/lang/ClassLoader");
                if (classLoader != NULL)
                {
                    jmethodID findClass = jniEnv->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
                    if (findClass != NULL)
                    {
                        // Get a reference to the core activity for purposes of having a context: https://groups.google.com/forum/#!topic/android-ndk/SHCXgUCE7t4, http://stackoverflow.com/questions/25167806/sending-an-intent-from-c-via-jni
                        // And be able to create an intent which will launch the activity
                        jclass classNativeActivity = jniEnv->FindClass("android/app/NativeActivity");
                        if (classNativeActivity != NULL)
                        {
                            jclass contextClass = jniEnv->FindClass("android/content/Context");
                            if (contextClass != NULL)
                            {
                                jmethodID startActivityMethodId = jniEnv->GetMethodID(contextClass, "startActivity", "(Landroid/content/Intent;)V");
                                if (startActivityMethodId != NULL)
                                {
                                    jmethodID applicationContextMethodId = jniEnv->GetMethodID(classNativeActivity, "getApplicationContext", "()Landroid/content/Context;");
                                    if (applicationContextMethodId != NULL)
                                    {
                                        jobject contextObj = jniEnv->CallObjectMethod(m_activity, applicationContextMethodId);
                                        if (contextObj != NULL)
                                        {
                                            m_contextObject = jniEnv->NewGlobalRef(contextObj);
                                            if (m_contextObject != NULL)
                                            {
                                                jstring strClassNameActivity = jniEnv->NewStringUTF("com/microsoft/xbox/idp/interop/Interop");
                                                if (strClassNameActivity != NULL)
                                                {
                                                    jclass marketActivityClass = (jclass)jniEnv->CallObjectMethod(cls, findClass, strClassNameActivity);
                                                    if (marketActivityClass != NULL)
                                                    {
                                                        m_marketActivityClass = (jclass)jniEnv->NewGlobalRef(marketActivityClass);
                                                        jstring strTCUIClassNameActivity = jniEnv->NewStringUTF("com/microsoft/xboxtcui/Interop");
                                                        if (strTCUIClassNameActivity != NULL)
                                                        {
                                                            jclass tcuiClass = (jclass)jniEnv->CallObjectMethod(cls, findClass, strTCUIClassNameActivity);
                                                            if (tcuiClass != NULL)
                                                            {
                                                                m_tcuiInteropClass = (jclass)jniEnv->NewGlobalRef(tcuiClass);
                                                                if (m_marketActivityClass != NULL && m_tcuiInteropClass != NULL)
                                                                {
                                                                    return finish_initialization(jniEnv, cls, findClass, true);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    JNI_ERROR_CHECK(jniEnv)
    return xbox_live_result<void>(xbox_live_error_code::runtime_error, "Initialize failed");
}


xbox_live_result<void> java_interop::initialize(JNIEnv* env, jclass clsInterop, jobject context)
{
    JavaVM* jvm;
    if (env->GetJavaVM(&jvm) == 0) {
        m_javaVM = jvm;
        cpprest_init(m_javaVM);
        m_contextObject = env->NewGlobalRef(context);
        m_marketActivityClass = (jclass)env->NewGlobalRef(clsInterop);

        jclass clsContext = env->GetObjectClass(m_contextObject);
        if (clsContext != NULL)
        {
            jmethodID getClassLoader = env->GetMethodID(clsContext, "getClassLoader", "()Ljava/lang/ClassLoader;");
            if (getClassLoader != NULL)
            {
                jobject classLoader = env->CallObjectMethod(m_contextObject, getClassLoader);
                if (classLoader != NULL)
                {
                    jmethodID loadClass = env->GetMethodID(env->GetObjectClass(classLoader), "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
                    if (loadClass != NULL)
                    {
                        return finish_initialization(env, classLoader, loadClass, false);
                    }
                }
            }
        }
    }
    JNI_ERROR_CHECK(env)
    return xbox_live_result<void>(xbox_live_error_code::runtime_error, "Initialize failed");
}

jobject java_interop::app_callback_intent()
{
    return m_pendingIntent;
}

void java_interop::set_app_callback_intent(
    jobject pendingIntent
    )
{
    m_pendingIntent = pendingIntent;
}

xbox_live_result<void> java_interop::finish_initialization(JNIEnv* env, jobject clsLoader, jmethodID loadClass, bool useTcui)
{
    if (env->RegisterNatives(m_marketActivityClass, methods, sizeof(methods) / sizeof(*methods)) >= 0)
    {
        if (http_call_register_natives(env, clsLoader, loadClass)
            && auth_flow_result_register_natives(env, clsLoader, loadClass)
            && local_config_register_natives(env, clsLoader, loadClass)
            && xbox_live_app_config_register_natives(env, clsLoader, loadClass)
            && (!useTcui || title_callable_ui_register_natives(env, clsLoader, loadClass))
            )
        {
            xbox_system_factory::get_factory();
            m_initialized = true;
            LOG_ERROR("java_interop initialized");
            init_cll();
            return xbox_live_result<void>(xbox_live_error_code::no_error);
        }
    }
    LOG_ERROR("error registering native methods");
    return xbox_live_result<void>(xbox_live_error_code::runtime_error, "Registration error");
}

void java_interop::deinitialize()
{
    if (m_javaVM == nullptr)
    {
        return;
    }

    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(m_javaVM, jniEnv)

    if (m_marketActivityClass != nullptr)
    {
        jniEnv->DeleteGlobalRef(m_marketActivityClass);
    }
    if (m_contextObject != nullptr)
    {
        jniEnv->DeleteGlobalRef(m_contextObject);
    }

    JNI_ERROR_CHECK(jniEnv)

    m_initialized = false;
    m_javaVM = nullptr;
    m_activity = nullptr;
    m_marketActivityClass = nullptr;
    m_contextObject = nullptr;
}

xbox_live_result<void> java_interop::log_cll(const string_t& xuid, const string_t& eventName, const string_t& eventData)
{
    if (!m_initialized)
    {
        LOG_ERROR("java_interop not initialized");
        assert(false);
    }
    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(m_javaVM, jniEnv)

    auto localCapacityResult = jniEnv->EnsureLocalCapacity(24); // the size of the three pointers we allocate below (assuming they are 8 bytes each)
    if (localCapacityResult == 0)
    {
        jmethodID initMethodId = jniEnv->GetStaticMethodID(m_marketActivityClass, "LogCLL", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        if (initMethodId != NULL)
        {
            JNI_ERROR_CHECK(jniEnv)

            jstring jxuid = jniEnv->NewStringUTF(xuid.c_str());
            jstring jeventName = jniEnv->NewStringUTF(eventName.c_str());
            jstring jeventData = jniEnv->NewStringUTF(eventData.c_str());

            if (jniEnv->ExceptionCheck())
            {
                JNI_ERROR_CHECK(jniEnv)
                jniEnv->DeleteLocalRef(jxuid);
                jniEnv->DeleteLocalRef(jeventName);
                jniEnv->DeleteLocalRef(jeventData);
                LOG_ERROR("failure to allocated");
                return xbox_live_result<void>(xbox_live_error_code::runtime_error, "failed to allocate");
            }
            jniEnv->CallStaticVoidMethod(m_marketActivityClass, initMethodId, jxuid, jeventName, jeventData);

            jniEnv->DeleteLocalRef(jxuid);
            jniEnv->DeleteLocalRef(jeventName);
            jniEnv->DeleteLocalRef(jeventData);

            if (!jniEnv->ExceptionCheck())
            {
                return xbox_live_result<void>(xbox_live_error_code::no_error);
            }
        }
    }

    JNI_ERROR_CHECK(jniEnv)
    return xbox_live_result<void>(xbox_live_error_code::runtime_error, "cll logging failed");
}

xbox_live_result<void> java_interop::log_telemetry_signin(bool silentAPI, const string_t& state)
{
    if (!m_initialized)
    {
        LOG_ERROR("java_interop not initialized");
        assert(false);
    }
    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(m_javaVM, jniEnv)

    auto localCapacityResult = jniEnv->EnsureLocalCapacity(16); // the size of the two pointers we allocate below (assuming they are 8 bytes each)
    if (localCapacityResult == 0)
    {
        jmethodID initMethodId = jniEnv->GetStaticMethodID(m_marketActivityClass, "LogTelemetrySignIn", "(Ljava/lang/String;Ljava/lang/String;)V");
        if (initMethodId != NULL)
        {
            JNI_ERROR_CHECK(jniEnv)

            jstring japi;
            if (silentAPI)
            {
               japi = jniEnv->NewStringUTF("API - signin_silently - ");
            }
            else
            {
                japi = jniEnv->NewStringUTF("API - signin - ");
            }
            jstring jstate = jniEnv->NewStringUTF(state.c_str());
            if (jniEnv->ExceptionCheck())
            {
                JNI_ERROR_CHECK(jniEnv)
                jniEnv->DeleteLocalRef(japi);
                jniEnv->DeleteLocalRef(jstate);
                LOG_ERROR("failure to allocated");
                return xbox_live_result<void>(xbox_live_error_code::runtime_error, "failed to allocate");
            }
            jniEnv->CallStaticVoidMethod(m_marketActivityClass, initMethodId, japi, jstate);

            jniEnv->DeleteLocalRef(japi);
            jniEnv->DeleteLocalRef(jstate);

            if (!jniEnv->ExceptionCheck())
            {
                return xbox_live_result<void>(xbox_live_error_code::no_error);
            }
        }
    }

    JNI_ERROR_CHECK(jniEnv)
    return xbox_live_result<void>(xbox_live_error_code::runtime_error, "cll logging failed");
}

xbox_live_result<void> java_interop::init_cll()
{
    if (!m_initialized)
    {
        LOG_ERROR("java_interop not initialized");
        assert(false);
    }

    string_t iKey = system::xbox_system_factory::get_factory()->create_local_config()->get_value_from_config("iKey", false, string_t());
    if (iKey.empty())
    {
        stringstream_t defaultIKey;
        defaultIKey << _T("P-XBL-T") << xbox::services::xbox_live_app_config::get_app_config_singleton()->title_id();
        iKey = defaultIKey.str();
    }

    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(m_javaVM, jniEnv)

    jstring jiKey = jniEnv->NewStringUTF(iKey.c_str());
    jmethodID initMethodId = jniEnv->GetStaticMethodID(m_marketActivityClass, "InitCLL", "(Landroid/content/Context;Ljava/lang/String;)V");
    if (initMethodId != NULL)
    {
        jniEnv->CallStaticVoidMethod(m_marketActivityClass, initMethodId, m_contextObject, jiKey);
        if (!jniEnv->ExceptionCheck())
        {
            return xbox_live_result<void>(xbox_live_error_code::no_error);
        }
    }

    JNI_ERROR_CHECK(jniEnv)
    return xbox_live_result<void>(xbox_live_error_code::runtime_error, "init sign in activity failed");
}

string_t java_interop::read_config_file()
{
    if (!m_initialized)
    {
        LOG_ERROR("java_interop not initialized");
        assert(false);
    }
    string_t configText;
    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(m_javaVM, jniEnv)
    
    jmethodID initMethodId = jniEnv->GetStaticMethodID(m_marketActivityClass, "ReadConfigFile", "(Landroid/content/Context;)Ljava/lang/String;");
    if (initMethodId != NULL)
    {
        jobject result = jniEnv->CallStaticObjectMethod(m_marketActivityClass, initMethodId, m_contextObject);
        if (jniEnv->ExceptionCheck())
        {
            return configText;
        }
        const char* str = jniEnv->GetStringUTFChars((jstring)result, NULL);
        configText = str;
        jniEnv->ReleaseStringUTFChars((jstring)result, str);
    }
    
    JNI_ERROR_CHECK(jniEnv)

    return configText;
}

string_t java_interop::get_local_storage_path()
{
    if (!m_initialized)
    {
        LOG_ERROR("java_interop not initialized");
        assert(false);
    }
    std::lock_guard<std::mutex> guard(m_localStoragePathLock);
    if (m_localStoragePath.empty())
    {
        JNIEnv* jniEnv;
        JNI_ATTACH_THREAD(m_javaVM, jniEnv)

            jmethodID initMethodId = jniEnv->GetStaticMethodID(m_marketActivityClass, "GetLocalStoragePath", "(Landroid/content/Context;)Ljava/lang/String;");
        if (initMethodId != NULL)
        {
            jobject result = jniEnv->CallStaticObjectMethod(m_marketActivityClass, initMethodId, m_contextObject);
            if (jniEnv->ExceptionCheck())
            {
                return m_localStoragePath;
            }
            const char* str = jniEnv->GetStringUTFChars((jstring)result, NULL);
            m_localStoragePath = str;
            jniEnv->ReleaseStringUTFChars((jstring)result, str);
        }

        JNI_ERROR_CHECK(jniEnv)
    }
    return m_localStoragePath;
}

JavaVM* java_interop::get_java_vm()
{
    return m_javaVM;
}

jobject java_interop::get_activity()
{
    return m_activity;
}

jclass java_interop::get_market_activity_class()
{
    return m_marketActivityClass;
}

jclass java_interop::get_tcui_interop_class()
{
    return m_tcuiInteropClass;
}

jobject java_interop::get_context_object()
{
    return m_contextObject;
}

void java_interop::register_natives(
    JNINativeMethod nativeMethods[]
    )
{
    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(m_javaVM, jniEnv)
    jniEnv->RegisterNatives(m_marketActivityClass, nativeMethods, 1);
    JNI_ERROR_CHECK(jniEnv)
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

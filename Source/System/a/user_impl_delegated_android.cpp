#include "pch.h"
#include "user_auth_android.h"
#include "java_interop.h"
#include "a/jni_utils.h"


using namespace xbox::services;
using namespace xbox::services::system;

xbox_live_result<void> user_auth_android::invoke_brokered_msa_auth()
{
    std::shared_ptr<java_interop> interop = java_interop::get_java_interop_singleton();
    jclass marketActivityClass = interop->get_market_activity_class();
    jobject context = interop->get_context_object();
    JavaVM* javaVM = interop->get_java_vm();
    JVM_CHECK_RETURN_RESULT_VOID(javaVM, "java interop not initialized properly")

    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(javaVM, jniEnv)

    auto createAccountMethodId = jniEnv->GetStaticMethodID(marketActivityClass, "InvokeBrokeredMSA", "(Landroid/content/Context;Z)V");
    if (createAccountMethodId != NULL)
    {
        // This allocation ensures the object won't be deleted until we return from UI
        jniEnv->CallStaticVoidMethod(marketActivityClass, createAccountMethodId, context, is_prod());
    }
    else
    {
        LOG_ERROR("Could not find the InvokeBrokeredMSA methodID");
        return xbox_live_result<void>(xbox_live_error_code::runtime_error);
    }
    JNI_ERROR_CHECK(jniEnv)
    return xbox_live_result<void>();
}

xbox_live_result<void> user_auth_android::invoke_brokered_pending_intent()
{
    std::shared_ptr<java_interop> interop = java_interop::get_java_interop_singleton();
    jclass marketActivityClass = interop->get_market_activity_class();
    jobject activity = interop->get_activity();
    JavaVM* javaVM = interop->get_java_vm();
    JVM_CHECK_RETURN_RESULT_VOID(javaVM, "java interop not initialized properly")

    JNIEnv* jniEnv;
    jobject appIntent = interop->app_callback_intent();
    JNI_ATTACH_THREAD(javaVM, jniEnv)

    auto invokeMethodIntent = jniEnv->GetStaticMethodID(marketActivityClass, "InvokeLatestIntent", "(Landroid/app/Activity;Ljava/lang/Object;)V");
    if (invokeMethodIntent != NULL)
    {
        // This allocation ensures the object won't be deleted until we return from UI
        jniEnv->CallStaticVoidMethod(marketActivityClass, invokeMethodIntent, activity, appIntent);
    }
    else
    {
        LOG_ERROR("InvokeMethodIntent was null - probably a JNI issue");
        return xbox_live_result<void>(xbox_live_error_code::runtime_error);
    }
    JNI_ERROR_CHECK(jniEnv)
    return xbox_live_result<void>();
}
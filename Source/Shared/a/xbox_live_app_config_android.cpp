//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi\xbox_live_app_config.h"
#include "a/java_interop.h"
#include "jni_utils.h"

using namespace xbox::services;

string_t
xbox_live_app_config::get_proxy_string()
{
    auto javaInterop = java_interop::get_java_interop_singleton();
    auto javaVM = javaInterop->get_java_vm();
    if (javaVM == nullptr)
    {
        LOG_ERROR("java interop not initialized properly");
        return string_t();
    }
    auto marketActivityClass = javaInterop->get_market_activity_class();
    if (javaVM != nullptr && marketActivityClass != NULL)
    {
        JNIEnv* jniEnv;
        JNI_ATTACH_THREAD(javaVM, jniEnv);
        jmethodID getSystemProxyMethod = jniEnv->GetStaticMethodID(marketActivityClass, "getSystemProxy", "()Ljava/lang/String;");
        if (getSystemProxyMethod == nullptr)
        {
            return string_t();
        }
        jstring result = (jstring)jniEnv->CallStaticObjectMethod(marketActivityClass, getSystemProxyMethod);
        if (result == nullptr)
        {
            return string_t();
        }

        return jniEnv->GetStringUTFChars(result, nullptr);
    }

    return string_t();
}
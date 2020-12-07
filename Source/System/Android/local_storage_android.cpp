// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "local_storage.h"
#include "a/java_interop.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
String LocalStorage::GetDefaultStoragePath()
{
    std::shared_ptr<java_interop> interop{ java_interop::get_java_interop_singleton() };

    JNIEnv* jniEnv{ interop->GetJniEnv() };
    jclass localStorageClass = interop->GetLocalStorageClass();
    if (localStorageClass == nullptr)
    {
        LOGS_ERROR << "Couldn't find Storage class in Jni Environment.";
        assert(false);
    }

    jmethodID getStoragePathMethodId = jniEnv->GetStaticMethodID(localStorageClass, "getPath", "(Landroid/content/Context;)Ljava/lang/String;");
    jstring jStr = static_cast<jstring>(jniEnv->CallStaticObjectMethod(localStorageClass, getStoragePathMethodId, interop->get_activity()));
    if (jStr == nullptr)
    {
        LOGS_ERROR << "getStoragePath returned a null path";
        assert(false);
    }

    return java_interop::StringFromJString(jniEnv, jStr) + "/";
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

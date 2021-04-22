//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include <jni.h>
#include "a/rwlock_guard.h"

namespace xbox { namespace services { 

class java_interop : public std::enable_shared_from_this<java_interop>
{
public:

    static std::shared_ptr<java_interop> get_java_interop_singleton();

    java_interop();

    xbl_result<void> initialize(JavaVM* jvm, jobject activity);
    xbl_result<void> initialize(JNIEnv* env, jclass clsInterop, jobject context);
    void deinitialize();

    xbl_result<void> log_cll(const string_t& xuid, const string_t& eventName, const string_t& eventData);

    xbl_result<void> log_telemetry_signin(bool silentAPI, const string_t& state);

    string_t read_config_file();

    string_t get_local_storage_path();

    JavaVM* get_java_vm();

    jobject get_activity();

    jclass get_market_activity_class();

    jclass get_tcui_interop_class();

    jobject get_context_object();

    void register_natives(JNINativeMethod nativeMethods[]);

    jobject app_callback_intent();

    void set_app_callback_intent(jobject pendingIntent);

    pthread_rwlock_t java_interop_singletonLock = PTHREAD_RWLOCK_INITIALIZER;

    JNIEnv* GetJniEnv() const;

    static String StringFromJString(JNIEnv* env, jstring jStr);

    jclass GetLocalStorageClass() const { return m_localStorageClass; }

    // TODO This is a temporary workaround for TCUI. XSAPI C++ TCUI API's accept a xal_user_handle, from which we extract
    // XUID and privileges and call into Java code. Java code then calls back into XSAPI to make an HTTP call, but it does not pass and user
    // context or xal_user_handle but it most likely should. Previously we just remembered the last signed in user, so we can 
    // emulate that behavior again for now.
    void StoreUser(User&& user);
    std::shared_ptr<User> GetStoredUser();
    std::shared_ptr<User> ExtractStoredUser();

private:

    JavaVM* m_javaVM;
    jobject m_activity;
    jclass  m_marketActivityClass;
    jclass m_tcuiInteropClass;
    jobject m_contextObject;
    jobject m_pendingIntent;
    static std::shared_ptr<java_interop> s_javaInterop;
    bool m_initialized;

    jclass m_localStorageClass{ nullptr };

    string_t m_localStoragePath;

    std::mutex m_localStoragePathLock;

    xbl_result<void> finish_initialization(JNIEnv* env, jobject clsLoader, jmethodID loadClass, bool useTcui);

    std::shared_ptr<User> m_storedUser{ nullptr };
    std::mutex m_storedUserMutex;
};

}}
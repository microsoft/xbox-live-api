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

namespace xbox { namespace services { 

class java_interop : public std::enable_shared_from_this<java_interop>
{
public:

    static std::shared_ptr<java_interop> get_java_interop_singleton();

    java_interop();

    xbox_live_result<void> initialize(JavaVM* jvm, jobject activity);
    xbox_live_result<void> initialize(JNIEnv* env, jclass clsInterop, jobject context);
    void deinitialize();

    xbox_live_result<void> log_cll(const string_t& xuid, const string_t& eventName, const string_t& eventData);

    xbox_live_result<void> log_telemetry_signin(bool silentAPI, const string_t& state);

    xbox_live_result<void> init_cll();

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

private:

    JavaVM* m_javaVM;
    jobject m_activity;
    jclass  m_marketActivityClass;
    jclass m_tcuiInteropClass;
    jobject m_contextObject;
    jobject m_pendingIntent;
    static std::shared_ptr<java_interop> s_javaInterop;
    bool m_initialized;

    string_t m_localStoragePath;

    std::mutex m_localStoragePathLock;

    xbox_live_result<void> finish_initialization(JNIEnv* env, jobject clsLoader, jmethodID loadClass, bool useTcui);
};

}}

#pragma once
#include <memory>
#include "xsapi_utils.h"

template <class T> struct ref_holder {
    ref_holder(T ref) : m_ref(ref) { }
    operator T() const { return m_ref; }
protected:
    T m_ref;
}; 

struct thread_holder : ref_holder<JavaVM*> {
    thread_holder() : ref_holder<JavaVM*>(nullptr) {}
    thread_holder(JavaVM* jvm) : ref_holder<JavaVM*>(jvm) {}
    ~thread_holder() { if (m_ref != nullptr) { LOG_INFO(_T("thread detached")); m_ref->DetachCurrentThread(); } }
    void set_ref(JavaVM* ref) { m_ref = ref; }
};

struct global_ref_holder : ref_holder<std::pair<JNIEnv*, jobject>> {
    global_ref_holder(std::pair<JNIEnv*, jobject> ref) : ref_holder<std::pair<JNIEnv*, jobject>>(ref) { }
    ~global_ref_holder() { m_ref.first->DeleteGlobalRef(m_ref.second); }
};

struct local_ref_holder : ref_holder<std::pair<JNIEnv*, jobject>> {
    local_ref_holder(std::pair<JNIEnv*, jobject> ref) : ref_holder<std::pair<JNIEnv*, jobject>>(ref) { }
    ~local_ref_holder() { m_ref.first->DeleteLocalRef(m_ref.second); }
};

class jstring_deleter : ref_holder<JNIEnv*> {
    jstring m_s;
public:
    jstring_deleter(JNIEnv * env, jstring s) : ref_holder<JNIEnv*>(env), m_s(s) { }
    void operator()(const char* s) const { m_ref->ReleaseStringUTFChars(m_s, s); }
};

struct jstring_t : std::shared_ptr<const char> {
public:
    jstring_t(JNIEnv* env, jstring s) : std::shared_ptr<const char>(env->GetStringUTFChars(s, NULL), jstring_deleter(env, s)) { }
    operator string_t() const { return get(); }
    operator xbox::services::uri() const { return get(); }
    operator const char*() const { return get(); }
};

#define JNI_ATTACH_THREAD(jvm, env) \
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6); \
    thread_holder th; \
    if (env == nullptr) \
    { \
        jvm->AttachCurrentThread(&env, nullptr); \
        th.set_ref(jvm); \
    } 

#define JNI_ERROR_CHECK(env) \
    if (env->ExceptionCheck()) \
    { \
        env->ExceptionDescribe(); \
        env->ExceptionClear(); \
    }

#define JVM_CHECK_RETURN_TASK_RESULT_VOID(jvm, msg) \
    if (jvm == nullptr) \
    { \
        LOG_ERROR(msg); \
        return pplx::task_from_result<xbox_live_result<void>>(xbox_live_result<void>(xbox_live_error_code::runtime_error, msg)); \
    }

#define JVM_CHECK_RETURN_RESULT_VOID(jvm, msg) \
    if (jvm == nullptr) \
    { \
        LOG_ERROR(msg); \
        return xbox_live_result<void>(xbox_live_error_code::runtime_error, msg); \
    }
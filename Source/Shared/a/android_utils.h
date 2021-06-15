#pragma once

#include <jni.h>

#if XSAPI_ANDROID_STUDIO
#include "types.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

/// <summary>
/// Thread unsafe wrapper for the jni jstring object to ensure we don't leak memory
/// </summary>
class JString
{
public:
    JString(JNIEnv* env, jstring string) noexcept;
    ~JString() noexcept;

    JString(JString const&) = delete;
    JString(JString&&) = delete;
    JString& operator=(JString const&) = delete;
    JString& operator=(JString&&) = delete;

    char const* c_str();

private:
    JNIEnv * m_env;
    jstring m_string;
    char const* m_cStr;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbl_guid.h"
#include "android_utils.h"
#include "a/java_interop.h"
#include "a/jni_utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN
#define UNREFERENCED_LOCAL(x) (void)(x);
template<class TBuffer>
void FormatHelper(TBuffer& buffer, _In_z_ _Printf_format_string_ char const* format, va_list args)
{
    va_list args1{};
    va_copy(args1, args);
    int required = vsnprintf(nullptr, 0, format, args1);
    va_end(args1);

    assert(required > 0);

    size_t originalSize = buffer.size();

    va_list args2{};
    va_copy(args2, args);
    buffer.resize(originalSize + static_cast<size_t>(required) + 1); // add space for null terminator
    int written = vsnprintf(reinterpret_cast<char*>(&buffer[originalSize]), buffer.size(), format, args2);
    va_end(args2);

    assert(written == required);
    UNREFERENCED_LOCAL(written);

    buffer.resize(buffer.size() - 1); // drop null terminator
}

inline xsapi_internal_string Format(_In_z_ _Printf_format_string_ char const* format, ...)
{
    xsapi_internal_string s;

    va_list args{};
    va_start(args, format);
    FormatHelper(s, format, args);
    va_end(args);

    return s;
}

xsapi_internal_string generate_guid()
{
    auto javaInterop = java_interop::get_java_interop_singleton();
    auto javaVM = javaInterop->get_java_vm();
    if (javaVM == nullptr)
    {
        LOG_ERROR("java interop not initialized properly");
        return "";
    }
    if (javaVM != nullptr)
    {
        JNIEnv* jniEnv;
        JNI_ATTACH_THREAD(javaVM, jniEnv);
        jclass guidClass = jniEnv->FindClass("java/util/UUID");
        if (guidClass == nullptr)
        {
            LOG_ERROR("Could not find UUID class");
            return "";
        }
        jclass m_guidClass = reinterpret_cast<jclass>(jniEnv->NewGlobalRef(guidClass));
        jmethodID generateUuidMethodId = jniEnv->GetStaticMethodID(m_guidClass, "randomUUID", "()Ljava/util/UUID;");
        jmethodID uuidToStringMethodId = jniEnv->GetMethodID(m_guidClass, "toString", "()Ljava/lang/String;");

        jobject uuidObject = jniEnv->CallStaticObjectMethod(m_guidClass, generateUuidMethodId);
        JString rawUuid{ jniEnv, static_cast<jstring>(jniEnv->CallObjectMethod(uuidObject, uuidToStringMethodId)) };

        return Format("{%s}", rawUuid.c_str());
    }
    return "";
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
#include "pch.h"
#include "a/java_interop.h"
#include <rapidjson/allocators.hpp>
#include "uri_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

    JNIEXPORT jboolean JNICALL Java_com_microsoft_xbox_idp_interop_Interop_initializeInterop(JNIEnv* env, jclass clsInterop, jobject context)
    {
        xbox::services::xbl_result<void> result = xbox::services::java_interop::get_java_interop_singleton()->initialize(env, clsInterop, context);
        return result.err() ? 0 : 1;
    }

    JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_interop_Interop_deinitializeInterop(JNIEnv* env, jclass clsInterop)
    {
        xbox::services::java_interop::get_java_interop_singleton()->deinitialize();
    }

#ifdef __cplusplus
}
#endif
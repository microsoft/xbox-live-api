#include "local_config_jni.h"

#include "xsapi/system.h"
#include "xbox_system_factory.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace xbox::services;
using namespace xbox::services::system;

/*
 * Class:     com_microsoft_xbox_idp_interop_LocalConfig
 * Method:    create
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL 
Java_com_microsoft_xbox_idp_interop_LocalConfig_create(JNIEnv * env, jclass cls)
{
    std::shared_ptr<local_config>* cfg = new std::shared_ptr<local_config>(xbox_system_factory::get_factory()->create_local_config());
    return reinterpret_cast<jlong>(cfg);
}

/*
 * Class:     com_microsoft_xbox_idp_interop_LocalConfig
 * Method:    delete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL 
Java_com_microsoft_xbox_idp_interop_LocalConfig_delete(JNIEnv * env, jclass cls, jlong id)
{
    delete reinterpret_cast<std::shared_ptr<local_config>*>(id);
}

/*
 * Class:     com_microsoft_xbox_idp_interop_LocalConfig
 * Method:    getCid
 * Signature: (J)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL 
Java_com_microsoft_xbox_idp_interop_LocalConfig_getCid(JNIEnv * env, jclass cls, jlong id)
{
    std::shared_ptr<local_config>* cfg = reinterpret_cast<std::shared_ptr<local_config>*>(id);
    string_t cid = (*cfg)->get_value_from_local_storage("cid");
    return env->NewStringUTF(cid.c_str());
}

#ifdef __cplusplus
}
#endif

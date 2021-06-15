#include "pch.h"
#include "http_call_jni.h"
#include <string>
#include <android/log.h>
#include "jni_utils.h"
#include "http_call_legacy.h"
#include "a/java_interop.h"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "HttpCall", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "HttpCall", __VA_ARGS__))

#define JENV_CHECK_MSG(var, env, msg)  \
    if (var == nullptr) \
    { \
    clear_and_log_exception(env); \
    LOG_ERROR(msg); \
    return; \
    }

#define JENV_CHECK_MSG_RET(var, env, msg, ret) \
    if (var == nullptr) \
    { \
    clear_and_log_exception(env); \
    LOG_ERROR(msg); \
    return ret; \
    }

static xbox::services::legacy::http_call* getHttpCall(JNIEnv * env, jobject httpCall)
{
    jclass cls = env->GetObjectClass(httpCall);
    jfieldID id = env->GetFieldID(cls, "id", "J");
    if (id == nullptr)
    {
        XSAPI_ASSERT(false);
    }
    return reinterpret_cast<std::shared_ptr<xbox::services::legacy::http_call>*>(env->GetLongField(httpCall, id))->get();
}

static bool clear_and_log_exception(JNIEnv* env)
{
    bool ret = false;
    if (env->ExceptionCheck())
    {
        jthrowable e = env->ExceptionOccurred();
        env->ExceptionClear();
        jclass cls = env->GetObjectClass(e);
        jmethodID getMessage = env->GetMethodID(cls, "getMessage", "()Ljava/lang/String;");
        jstring_t msg(env, static_cast<jstring>(env->CallObjectMethod(e, getMessage)));
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"
        LOGE(msg);
#pragma clang diagnostic pop
        ret = true;
    }
    return ret;
}

static bool prepare_stream(JNIEnv * myEnv, std::shared_ptr<xbox::services::legacy::http_call_response> response, jobject& stream, jmethodID& closeStream)
{
    if(!response)
    {
        return false;
    }
    auto v = response->response_body_vector();
    auto sz = v.size();
    LOGD("Received %zu bytes", sz);

    jbyteArray array = myEnv->NewByteArray(sz);
    JENV_CHECK_MSG_RET(array, myEnv, "Failed creating Java byte array", false)

    jbyte* buf = myEnv->GetByteArrayElements(array, nullptr);
    JENV_CHECK_MSG_RET(buf, myEnv, "Failed getting array elements", false)

    memcpy(buf, v.data(), sz);
    myEnv->ReleaseByteArrayElements(array, buf, 0);

    jclass clsStream = myEnv->FindClass("java/io/ByteArrayInputStream");
    JENV_CHECK_MSG_RET(clsStream, myEnv, "Could not find ByteArrayInputStream class", false)

    jmethodID initStream = myEnv->GetMethodID(clsStream, "<init>", "([B)V");
    JENV_CHECK_MSG_RET(initStream, myEnv, "Failed getting <init> method ID", false)

    closeStream = myEnv->GetMethodID(clsStream, "close", "()V");
    JENV_CHECK_MSG_RET(closeStream, myEnv, "Failed getting close method ID", false)

    stream = myEnv->NewObject(clsStream, initStream, array);
    JENV_CHECK_MSG_RET(stream, myEnv, "Failed creating ByteArrayInputStream instance", false)

    return true;
}

static bool prepare_headers(JNIEnv* myEnv, std::shared_ptr<xbox::services::legacy::http_call_response> response, jclass clsHeaders, jobject& headers)
{
    if (!response)
    {
        return false;
    }

    jmethodID initHeaders = myEnv->GetMethodID(clsHeaders, "<init>", "()V");
    JENV_CHECK_MSG_RET(initHeaders, myEnv, "Failed getting <init> method ID", false)

    jmethodID addHeader = myEnv->GetMethodID(clsHeaders, "add", "(Ljava/lang/String;Ljava/lang/String;)V");
    JENV_CHECK_MSG_RET(addHeader, myEnv, "Failed getting add() method ID", false)

    headers = myEnv->NewObject(clsHeaders, initHeaders);
    JENV_CHECK_MSG_RET(headers, myEnv, "Failed creating ByteArrayInputStream instance", false)

    web::http::http_headers hdr = response->response_headers();
    for (web::http::http_headers::iterator itr = hdr.begin(); itr != hdr.end(); ++itr)
    {
        jstring key = myEnv->NewStringUTF(itr->first.c_str());
        JENV_CHECK_MSG_RET(key, myEnv, "Error allocating string for a key", false)

        local_ref_holder lrKey(std::pair<JNIEnv*, jstring>(myEnv, key));

        jstring value = myEnv->NewStringUTF(itr->second.c_str());
        JENV_CHECK_MSG_RET(value, myEnv, "Error allocating string for a value", false)

        local_ref_holder lrValue(std::pair<JNIEnv*, jstring>(myEnv, value));

        myEnv->CallVoidMethod(headers, addHeader, key, value);
    }

    return true;
}

/*
* Class:     com_microsoft_xbox_idp_util_HttpCall
* Method:    setRequestBody
* Signature: (Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_util_HttpCall_setRequestBody__Ljava_lang_String_2(
    JNIEnv * env, jobject httpCall, jstring value)
{
    if (value != nullptr)
    {
        jstring_t strValue(env, value);
        getHttpCall(env, httpCall)->set_request_body(strValue);
    }
}

/*
* Class:     com_microsoft_xbox_idp_util_HttpCall
* Method:    setRequestBody
* Signature: ([B)V
*/
JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_util_HttpCall_setRequestBody___3B(
    JNIEnv * env, jobject httpCall, jbyteArray value)
{
    jbyte* buf = env->GetByteArrayElements(value, nullptr);
    std::vector<uint8_t> v(reinterpret_cast<uint8_t*>(buf), reinterpret_cast<uint8_t*>(buf) + env->GetArrayLength(value));
    getHttpCall(env, httpCall)->set_request_body(v);
    env->ReleaseByteArrayElements(value, buf, JNI_ABORT);
}

/*
* Class:     com_microsoft_xbox_idp_util_HttpCall
* Method:    setCustomHeader
* Signature: (Ljava/lang/String;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_util_HttpCall_setCustomHeader(
    JNIEnv * env, jobject httpCall, jstring name, jstring value)
{
    if (name != nullptr && value != nullptr)
    {
        jstring_t strName(env, name);
        jstring_t strValue(env, value);
        getHttpCall(env, httpCall)->set_custom_header(strName, strValue);
    }
}

/*
* Class:     com_microsoft_xbox_idp_util_HttpCall
* Method:    setRetryAllowed
* Signature: (Z)V
*/
JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_util_HttpCall_setRetryAllowed(
    JNIEnv * env, jobject httpCall, jboolean value)
{
    getHttpCall(env, httpCall)->set_retry_allowed(value);
}

/*
* Class:     com_microsoft_xbox_idp_util_HttpCall
* Method:    setLongHttpCall
* Signature: (Z)V
*/
JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_util_HttpCall_setLongHttpCall(
    JNIEnv * env, jobject httpCall, jboolean value)
{
    getHttpCall(env, httpCall)->set_long_http_call(value);
}

/*
* Class:     com_microsoft_xbox_idp_util_HttpCall
* Method:    setContentTypeHeaderValue
* Signature: (Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_util_HttpCall_setContentTypeHeaderValue(
    JNIEnv * env, jobject httpCall, jstring value)
{
    if (value != nullptr)
    {
        jstring_t strValue(env, value);
        getHttpCall(env, httpCall)->set_content_type_header_value(strValue);
    }
}

/*
* Class:     com_microsoft_xbox_idp_util_HttpCall
* Method:    setXboxContractVersionHeaderValue
* Signature: (Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_util_HttpCall_setXboxContractVersionHeaderValue(
    JNIEnv * env, jobject httpCall, jstring value)
{
    if (value != nullptr)
    {
        jstring_t strValue(env, value);
        getHttpCall(env, httpCall)->set_xbox_contract_version_header_value(strValue);
    }
}

/*
* Class:     com_microsoft_xbox_idp_util_HttpCall
* Method:    getResponseAsync
* Signature: (Lcom/microsoft/xbox/idp/util/HttpCall/Callback;)V
*/
JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_util_HttpCall_getResponseAsync__Lcom_microsoft_xbox_idp_util_HttpCall_Callback_2
    (JNIEnv * env, jobject httpCall, jobject callback)
{

    jclass clsHeaders = env->FindClass("com/microsoft/xbox/idp/util/HttpHeaders");
    if (clsHeaders == nullptr) {
        LOG_ERROR("Could not find HttpHeaders class");
        return;
    }
    jclass myClsHeaders = (jclass)env->NewGlobalRef(clsHeaders);

    JavaVM* jvm;
    env->GetJavaVM(&jvm);
    jobject myCallback = env->NewGlobalRef(callback);

    xbox::services::legacy::http_call* http_call = getHttpCall(env, httpCall);
    auto task = http_call->get_response_with_auth(
        xbox::services::legacy::http_call_response_body_type::vector_body
        )
        .then([jvm, myCallback, myClsHeaders](std::shared_ptr<xbox::services::legacy::http_call_response> response)
    {
        if (response)
        {
            LOGD("HttpCall response error code %d", response->http_status());
        }

        JNIEnv* myEnv;
        jvm->AttachCurrentThread(&myEnv, nullptr);
        JNI_ERROR_CHECK(myEnv);

        thread_holder th(jvm);
        global_ref_holder grCallback(std::pair<JNIEnv*, jobject>(myEnv, myCallback));
        global_ref_holder grClsHeaders(std::pair<JNIEnv*, jobject>(myEnv, myClsHeaders));

        jclass clsCallback = myEnv->GetObjectClass(myCallback);
        jmethodID processResponse = myEnv->GetMethodID(clsCallback, "processResponse", "(ILjava/io/InputStream;Lcom/microsoft/xbox/idp/util/HttpHeaders;)V");
        if (processResponse == nullptr)
        {
            LOG_ERROR("Failed getting processResponse method ID");
            return;
        }

        jobject stream;
        jmethodID closeStream;

        if (!prepare_stream(myEnv, response, stream, closeStream))
        {
            LOG_ERROR("prepare_stream error");
            return;
        }

        local_ref_holder lrStream(std::pair<JNIEnv*, jobject>(myEnv, stream));

        jobject headers = nullptr;
        if (!prepare_headers(myEnv, response, myClsHeaders, headers))
        {
            clear_and_log_exception(myEnv);
            LOG_ERROR("prepare_headers error");
            return;
        }

        local_ref_holder lrHeaders(std::pair<JNIEnv*, jobject>(myEnv, headers));

        jint httpStatus = response->http_status();

        myEnv->CallVoidMethod(myCallback, processResponse, httpStatus, stream, headers);
        clear_and_log_exception(myEnv);

        myEnv->CallVoidMethod(stream, closeStream);
        clear_and_log_exception(myEnv);
    });
}

/*
* Class:     com_microsoft_xbox_idp_util_HttpCall
* Method:    create
* Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Z)J
*/
JNIEXPORT jlong JNICALL Java_com_microsoft_xbox_idp_util_HttpCall_create
(JNIEnv * env, jclass cls, jstring method, jstring endpoint, jstring pathAndQuery, jboolean addDefaultHeaders)
{
    jstring_t strMethod(env, method);
    jstring_t strEndpoint(env, endpoint);
    jstring_t strPathAndQuery(env, pathAndQuery);

    LOGD("Create HttpCall with uri %s", strEndpoint.get());

    std::shared_ptr<xbox::services::legacy::http_call>* legacyHttpCallPtr{ nullptr };

    auto user = java_interop::get_java_interop_singleton()->GetStoredUser();
    if (user)
    {
        auto userCopyResult = user->Copy();
        if (Succeeded(userCopyResult))
        {
            String fullUrl{ strEndpoint.get() };
            xbox::services::uri path{ strPathAndQuery.get() };

            if (!path.is_empty())
            {
                fullUrl += path.to_string();
            }

            auto httpCall = MakeShared<XblHttpCall>(userCopyResult.ExtractPayload());
            HRESULT hr = httpCall->Init(
                std::shared_ptr<XboxLiveContextSettings>{ new xbox::services::XboxLiveContextSettings },
                strMethod.get(),
                fullUrl,
                xbox::services::xbox_live_api::unspecified
            );

            if (SUCCEEDED(hr))
            {
                // legacy http_call doesn't duplicate the provided XblHttpCall handle so do it before passing.
                // This seems like a poor design, but keeping behavior same for now
                httpCall->AddRef();

                web::uri_builder uriBuilder;
                uriBuilder.append(strPathAndQuery.get());
                auto legacyHttpCall = std::make_shared<legacy::http_call>(httpCall.get(), strMethod.get(), strEndpoint.get(), uriBuilder.to_uri());
                legacyHttpCallPtr = new std::shared_ptr<xbox::services::legacy::http_call>(std::move(legacyHttpCall));

                (*legacyHttpCallPtr)->set_add_default_headers(addDefaultHeaders);
            }
        }
    }

    return reinterpret_cast<jlong>(legacyHttpCallPtr);
}

/*
 * Class:     com_microsoft_xbox_idp_util_HttpCall
 * Method:    delete
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_microsoft_xbox_idp_util_HttpCall_delete(JNIEnv * env, jclass cls, jlong id)
{
    delete reinterpret_cast<std::shared_ptr<xbox::services::legacy::http_call>*>(id);
}


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
#include "user_impl.h"
#include "user_impl_a.h"
#include <android/log.h>
#include "user_auth_android.h"
#include "a/http_call_static_glue.h"
#include "auth_flow_result_static_glue.h"
#include "a/jni_utils.h"
#include "a/java_interop.h"
#include "Misc/notification_service.h"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "XSAPI.Android", __VA_ARGS__))

using namespace pplx;
using namespace xbox::services;
using namespace xbox::services::system;

// copied from java activity
enum class auth_request_type
{
    PURPOSE_NONE = 0,
    PURPOSE_OPPORTUNISTIC_SIGN_IN = 1,
    PURPOSE_EXPLICIT_SIGN_IN = 2,
    PURPOSE_REACQUIRE_PREVIOUS_ACCOUNT = 3,
    PURPOSE_GET_TICKET = 4,
    PURPOSE_GET_VORTEX_TICKET = 5,
    PURPOSE_SIGN_OUT = 6
};

enum class auth_error_type
{
    NO_ERROR = 0,
    ERROR_UI_INTERACTION_REQUIRED = 1,
    ERROR_USER_CANCEL = 2,
    ERROR_OTHER = 3
};

string_t get_string_t_from_jstring(JNIEnv* jniEnv, jstring stringToCopy)
{
    if (jniEnv != nullptr && stringToCopy != nullptr)
    {
        return jniEnv->GetStringUTFChars(stringToCopy, 0);
    }
    
    return string_t();
}

system::auth_flow_result& system::auth_flow_result::init(const token_and_signature_result& result)
{
    user_id = result.xbox_user_id();
    gamertag = result.gamertag();
    age_group = result.age_group();
    privileges = result.privileges();
    return *this;
}

void system::auth_flow_result::clear()
{
    status = auth_flow_status::NO_ERROR;
    rps_ticket.clear();
    user_id.clear();
    gamertag.clear();
    age_group.clear();
    privileges.clear();
    cid.clear();
    event_token_result.clear();
}

void xbox::services::system::user_auth_android::ticket_callback(JNIEnv* jniEnv, jclass, jstring rpsTicket, jint state, jint errorCode, jstring errorMessage)
{
    java_rps_ticket javaRPSTicket;

    javaRPSTicket.ticket = get_string_t_from_jstring(jniEnv, rpsTicket);
    javaRPSTicket.errorCode = errorCode;
    javaRPSTicket.errorMessage = get_string_t_from_jstring(jniEnv, errorMessage);

    stringstream_t stream;
    stream << javaRPSTicket.errorMessage;

    auto str = stream.str();
    if (state == static_cast<int>(auth_request_type::PURPOSE_GET_VORTEX_TICKET))
    {
        s_vortexRPSTicketCompletionEvent.set(javaRPSTicket);
    }
    else
    {
        s_rpsTicketCompletionEvent.set(javaRPSTicket);
    }
}

void xbox::services::system::user_auth_android::sign_out_callback(JNIEnv*, jclass)
{
    s_signOutCompleteEvent.set(xbox_live_result<void>());
}


void xbox::services::system::user_auth_android::auth_flow_callback(JNIEnv* jniEnv, jclass cls, jlong userPtr, jint authStatusCode, jstring cid)
{
    // Get pSelf back
    std::shared_ptr<user_auth_android>* pSelf = reinterpret_cast<std::shared_ptr<user_auth_android>*>(userPtr);
    
    if (cid != nullptr) 
    {
        (*pSelf)->m_last_auth_flow_result.cid = jstring_t(jniEnv, cid).operator string_t();
    }
    else 
    {
        (*pSelf)->m_last_auth_flow_result.cid.clear();
    }
    (*pSelf)->m_last_auth_flow_result.status = static_cast<auth_flow_status>(authStatusCode);
    (*pSelf)->m_authFlowCompletionEvent.set((*pSelf)->m_last_auth_flow_result);

    // We are done with UI, delete pSelf
    delete pSelf;
}

void xbox::services::system::user_auth_android::invoke_xb_login(JNIEnv* jniEnv, jclass cls, jlong userPtr, jstring rpsTicket, jobject callback)
{
    // Get pSelf back
    std::shared_ptr<user_auth_android>* pSelf = reinterpret_cast<std::shared_ptr<user_auth_android>*>(userPtr);

    JavaVM* jvm;
    jniEnv->GetJavaVM(&jvm);

    jobject myCallback = jniEnv->NewGlobalRef(callback);
    jstring_t javaRpsTicket(jniEnv, rpsTicket);
    string_t strRpsTicket = javaRpsTicket;

    pplx::create_task([pSelf, jvm, myCallback, strRpsTicket]()
    {
        JNIEnv* myEnv;
        jvm->AttachCurrentThread(&myEnv, NULL);

        thread_holder th(jvm);
        global_ref_holder grCallback(std::pair<JNIEnv*, jobject>(myEnv, myCallback));

        (*pSelf)->invoke_xb_login_internal(myEnv, myCallback, strRpsTicket);
    });
}

void user_auth_android::invoke_event_initialization(JNIEnv* jniEnv, jclass, jlong userPtr, jstring rpsTicket, jobject callback)
{
    // Get pSelf back
    std::shared_ptr<user_auth_android>* pSelf = reinterpret_cast<std::shared_ptr<user_auth_android>*>(userPtr);

    JavaVM* jvm;
    jniEnv->GetJavaVM(&jvm);

    jobject myCallback = jniEnv->NewGlobalRef(callback);
    jstring_t javaRpsTicket(jniEnv, rpsTicket);
    string_t strRpsTicket = javaRpsTicket;

    pplx::create_task([pSelf, jvm, myCallback, strRpsTicket]()
    {
        JNIEnv* myEnv;
        jvm->AttachCurrentThread(&myEnv, NULL);

        thread_holder th(jvm);
        global_ref_holder grCallback(std::pair<JNIEnv*, jobject>(myEnv, myCallback));

        (*pSelf)->invoke_event_initialization_internal(myEnv, myCallback, strRpsTicket);
    });
}


void xbox::services::system::user_auth_android::invoke_x_token_acquisition(JNIEnv* jniEnv, jclass cls, jlong userPtr, jobject callback)
{
    // Get pSelf back
    std::shared_ptr<user_auth_android>* pSelf = reinterpret_cast<std::shared_ptr<user_auth_android>*>(userPtr);

    JavaVM* jvm;
    jniEnv->GetJavaVM(&jvm);

    jobject myCallback = jniEnv->NewGlobalRef(callback);

    pplx::create_task([pSelf, jvm, myCallback]()
    {
        JNIEnv* myEnv;
        jvm->AttachCurrentThread(&myEnv, NULL);

        thread_holder th(jvm);
        global_ref_holder grCallback(std::pair<JNIEnv*, jobject>(myEnv, myCallback));

        (*pSelf)->invoke_x_token_acquisition_internal(myEnv, myCallback);
    });
}

void xbox::services::system::user_auth_android::invoke_xb_logout(JNIEnv* jniEnv, jclass cls, jlong userPtr, jobject callback)
{
    // Get pSelf back
    std::shared_ptr<user_auth_android>* pSelf = reinterpret_cast<std::shared_ptr<user_auth_android>*>(userPtr);

    JavaVM* jvm;
    jniEnv->GetJavaVM(&jvm);

    jobject myCallback = jniEnv->NewGlobalRef(callback);

    pplx::create_task([pSelf, jvm, myCallback]()
    {
        JNIEnv* myEnv;
        jvm->AttachCurrentThread(&myEnv, NULL);

        thread_holder th(jvm);
        global_ref_holder grCallback(std::pair<JNIEnv*, jobject>(myEnv, myCallback));

        (*pSelf)->invoke_xb_logout_internal(myEnv, myCallback);
    });
}

jstring xbox::services::system::user_auth_android::get_uploader_x_token_callback(JNIEnv* jniEnv, jclass cls, jboolean forceRefresh)
{
    if (user_auth_android::get_instance() != nullptr && !user_auth_android::get_instance()->m_isSignedIn)
    {
        return jniEnv->NewStringUTF(_T(""));
    }
    if(forceRefresh)
    {
        auth_manager::get_auth_manager_instance()->initialize_default_nsal().get();
    }

    auth_manager::get_auth_manager_instance()->get_auth_config()->set_xtoken_composition({ token_identity_type::u_token, token_identity_type::d_token, token_identity_type::t_token });
    string_t xToken = auth_manager::get_auth_manager_instance()->internal_get_token_and_signature(
        _T("GET"),
        _T("https://test.vortex.data.microsoft.com"),
        _T("https://test.vortex.data.microsoft.com"),
        string_t(),
        std::vector<uint8_t>(),
        false,
        forceRefresh).then([](xbox_live_result<token_and_signature_result> result)
    {
        if (!result.err())
        {
            token_and_signature_result res = result.payload();
            return res.token();
        }
        return string_t();
    }).get();

    return jniEnv->NewStringUTF(xToken.c_str());
}

jstring xbox::services::system::user_auth_android::get_supporting_x_token_callback(JNIEnv* jniEnv, jclass cls, jstring xuid)
{
    string_t xToken = xbox::services::system::xbox_system_factory::get_factory()->create_local_config()->get_value_from_local_storage(get_string_t_from_jstring(jniEnv, xuid));
    return jniEnv->NewStringUTF(xToken.c_str());
}

jstring xbox::services::system::user_auth_android::get_title_telemetry_device_id(JNIEnv* jniEnv, jclass cls)
{
    string_t xToken = xbox::services::xbox_live_app_config::get_app_config_singleton()->title_telemetry_device_id();
    return jniEnv->NewStringUTF(xToken.c_str());
}

jstring xbox::services::system::user_auth_android::get_title_telemetry_session_id(JNIEnv* jniEnv, jclass cls)
{
    string_t xToken;
    if (user_auth_android::get_instance() != nullptr)
    {
        auto user = xbox::services::system::user_impl_android::get_instance()->user().lock();
        if (user)
        {
            xToken = user->_Title_telemetry_session_id();
        }
    }
    return jniEnv->NewStringUTF(xToken.c_str());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

pplx::task_completion_event<java_rps_ticket> user_auth_android::s_rpsTicketCompletionEvent;
pplx::task_completion_event<java_rps_ticket> user_auth_android::s_vortexRPSTicketCompletionEvent;
pplx::task_completion_event<xbox_live_result<void>> user_auth_android::s_signOutCompleteEvent;


user_auth_android::user_auth_android() :
    m_hasInitialized(false),
    m_newAccount(false)
{
    m_authManager = auth_manager::get_auth_manager_instance();
    m_localConfig = xbox_system_factory::get_factory()->create_local_config();
}

void user_auth_android::set_signed_out_callback(_In_ const std::function<void()>& signedOutCallback)
{
    m_userSignedOutCallback = signedOutCallback;
}

user_auth_android::~user_auth_android()
{
}

std::shared_ptr<user_auth_android> user_auth_android::m_singletonInstance;

std::shared_ptr<user_auth_android> user_auth_android::get_instance()
{
    return m_singletonInstance;
}

void user_auth_android::start_sign_in()
{
    m_signInTaskEvent = pplx::task_completion_event<xbox_live_result<sign_in_result>>();
    m_signInTask = create_task(m_signInTaskEvent);

    m_hasInitialized = true;
}

void user_auth_android::finish_sign_in(auth_flow_status authStatus, string_t cid)
{
    m_last_auth_flow_result.status = static_cast<auth_flow_status>(authStatus);
    m_last_auth_flow_result.cid = cid;
    complete_sign_in_with_ui(m_last_auth_flow_result);
}

bool user_auth_android::is_prod() const
{
    return utils::str_icmp(m_authManager->get_auth_config()->environment(), _T(".dnet")) != 0;
}

xbox_live_result<void> user_auth_android::init_sign_in_activity(int32_t requestID)
{
    std::shared_ptr<java_interop> interop = java_interop::get_java_interop_singleton();
    jclass marketActivityClass = interop->get_market_activity_class();
    jobject contextObject = interop->get_context_object();
    JavaVM* javaVM = interop->get_java_vm();
    JVM_CHECK_RETURN_RESULT_VOID(javaVM, "java interop not initialized properly")

    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(javaVM, jniEnv)
    
    jint jRequestId = requestID;
    jmethodID initMethodId = jniEnv->GetStaticMethodID(marketActivityClass, "InvokeMSA", "(Landroid/content/Context;IZLjava/lang/String;)V");
    if (initMethodId != nullptr)
    {
        m_cid = m_localConfig->get_value_from_local_storage("cid");
        jstring cid = jniEnv->NewStringUTF(m_cid.c_str());
        if (cid != nullptr)
        {
            jniEnv->CallStaticVoidMethod(marketActivityClass, initMethodId, contextObject, jRequestId, is_prod(), cid);
            jniEnv->DeleteLocalRef(cid);
            if (!jniEnv->ExceptionCheck())
            {
                return xbox_live_result<void>(xbox_live_error_code::no_error);
            }
        }
    }

    JNI_ERROR_CHECK(jniEnv)

    return xbox_live_result<void>(xbox_live_error_code::runtime_error, "init sign in activity failed");
}

void
user_auth_android::invoke_auth_flow()
{
    std::shared_ptr<java_interop> interop = java_interop::get_java_interop_singleton();
    jclass marketActivityClass = interop->get_market_activity_class();
    jobject activity = interop->get_activity();
    JavaVM* javaVM = interop->get_java_vm();
    if (javaVM == nullptr)
    {
        LOG_ERROR("java interop not initialized properly");
        return;
    }
    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(javaVM, jniEnv)

    auto createAccountMethodId = jniEnv->GetStaticMethodID(marketActivityClass, "InvokeAuthFlow", "(JLandroid/app/Activity;Z)V");
    if (createAccountMethodId != NULL)
    {
        // This allocation ensures the object won't be deleted until we return from UI
        std::shared_ptr<user_auth_android>* pSelf = new std::shared_ptr<user_auth_android>(std::dynamic_pointer_cast<user_auth_android>(shared_from_this()));
        jlong userPtr = reinterpret_cast<jlong>(pSelf);
        jniEnv->CallStaticVoidMethod(marketActivityClass, createAccountMethodId, userPtr, activity, is_prod());
    }

    JNI_ERROR_CHECK(jniEnv)
}

void
user_auth_android::initiate_sign_in_flow(
    _In_ bool forceRefresh
    )
{
    m_authManager->get_auth_config()->set_xtoken_composition({ token_identity_type::u_token, token_identity_type::d_token, token_identity_type::t_token  });
    auto result = m_authManager->internal_get_token_and_signature(
        _T("GET"),
        m_authManager->get_auth_config()->xbox_live_endpoint(),
        m_authManager->get_auth_config()->xbox_live_endpoint(),
        string_t(),
        std::vector<uint8_t>(),
        false,
        forceRefresh
        ).get();

    if (result.err())
    {
        if (m_authManager->get_auth_config()->detail_error() == static_cast<uint32_t>(xbox_live_error_code::XO_E_ACCOUNT_CREATION_REQUIRED))
        {
            m_signInlFlowCompleteEvent.set(xbox_live_result<sign_in_and_auth_result>(sign_in_and_auth_result{ xbox_shell_sign_in_status::user_interaction_required }));
        }
        else
        {
            m_signInlFlowCompleteEvent.set(xbox_live_result<sign_in_and_auth_result>(result.err(), result.err_message()));
        }
    }
    else
    {
        m_signInlFlowCompleteEvent.set(xbox_live_result<sign_in_and_auth_result>(sign_in_and_auth_result{ xbox_shell_sign_in_status::success, result.payload() }));
    }
}

void
user_auth_android::clear_pending_intent()
{
    auto javaInterop = java_interop::get_java_interop_singleton();
    auto javaVM = javaInterop->get_java_vm();
    if (javaVM == nullptr)
    {
        LOG_ERROR("clear_pending_intent failed because java interop was not initalized");
        return;
    }

    auto interopClass = javaInterop->get_market_activity_class();
    
    JNIEnv* jniEnv;
    JNI_ATTACH_THREAD(javaVM, jniEnv);
    jmethodID clearIntentId = jniEnv->GetStaticMethodID(interopClass, "ClearIntent", "()V");
    if (clearIntentId != NULL)
    {
        jniEnv->CallStaticVoidMethod(interopClass, clearIntentId);
    }
    else
    {
        LOG_ERROR("clear_pending_intent failed becuase ClearIntent JNI method was not found");
    }
}

xbox_live_result<sign_in_result>
user_auth_android::msa_sign_in()
{
    xbox_live_result<void> errc;
    if (!m_localConfig->use_brokered_authorization())
    {
        errc = init_sign_in_activity(static_cast<int32_t>(auth_request_type::PURPOSE_OPPORTUNISTIC_SIGN_IN));
    }
    else
    {
        errc = invoke_brokered_msa_auth();
    }

    if (errc.err() != xbox_live_error_code::no_error)
    {
        return xbox_live_result<sign_in_result>(errc.err(), errc.err_message());
    }

    return xbox_live_result<sign_in_result>();
}

pplx::task<xbox_live_result<void>> 
user_auth_android::internal_signout()
{
    s_signOutCompleteEvent = pplx::task_completion_event<xbox_live_result<void>>();
    auto xblError = init_sign_in_activity(static_cast<int32_t>(auth_request_type::PURPOSE_SIGN_OUT));
    if (xblError.err() != xbox_live_error_code::no_error)
    {
        return task_from_result<xbox_live_result<void>>(xbox_live_result<void>(xblError.err(), xblError.err_message()));
    }

    std::weak_ptr<user_auth_android> pThisWeak = std::dynamic_pointer_cast<user_auth_android>(shared_from_this());
    auto signOutTask = create_task(s_signOutCompleteEvent);
    signOutTask.then([pThisWeak](xbox_live_result<void>)
    {
        std::shared_ptr<user_auth_android> pThis(pThisWeak.lock());
        if (pThis)
        {
            pThis->m_userSignedOutCallback();
            pThis->m_authManager->auth_token_manager()->clear_token_cache();
        }
    });
    return signOutTask;
}

void user_auth_android::write_offline_data()
{
    m_localConfig->write_value_to_local_storage("gamertag", m_gamertag);
    m_localConfig->write_value_to_local_storage("xuid", m_xboxUserId);
    m_localConfig->write_value_to_local_storage("cid", m_cid);
}

void user_auth_android::delete_offline_data()
{
    m_localConfig->delete_value_from_local_storage("gamertag");
    m_localConfig->delete_value_from_local_storage("xuid");
    m_localConfig->delete_value_from_local_storage("cid");
    m_localConfig->delete_value_from_local_storage("endpointId");
    // Synchronize cached values with local storage
    m_gamertag.clear();
    m_xboxUserId.clear();
    m_cid.clear();
    m_isSignedIn = false;
}

void user_auth_android::read_offline_data()
{
    m_gamertag = m_localConfig->get_value_from_local_storage("gamertag");
    m_xboxUserId = m_localConfig->get_value_from_local_storage("xuid");
    if (!m_gamertag.empty() && !m_xboxUserId.empty() && !m_cid.empty())
    {
        m_isSignedIn = true;
    }
}

pplx::task<xbox::services::xbox_live_result<sign_in_result>>
user_auth_android::sign_in_silent(
    _In_ bool forceRefresh
    )
{
    if (m_hasInitialized && !m_signInTask.is_done())
    {
        return pplx::task_from_result<xbox_live_result<sign_in_result>>(xbox_live_result<sign_in_result>(xbox_live_error_code::auth_runtime_error, "Previous sign in task has not completed"));
    }

    s_rpsTicketCompletionEvent = pplx::task_completion_event<java_rps_ticket>();

    m_signInTaskEvent = pplx::task_completion_event<xbox_live_result<sign_in_result>>();
    m_signInTask = create_task(m_signInTaskEvent);

    std::shared_ptr<auth_manager> authManager = m_authManager;
    auto authConfig = m_authManager->get_auth_config();

    std::weak_ptr<user_auth_android> pThisWeak = std::dynamic_pointer_cast<user_auth_android>(shared_from_this());
    pplx::create_task([pThisWeak, authManager, authConfig, forceRefresh]()
    {
        bool isSignInComplete = true;
        std::shared_ptr<user_auth_android> pThis(pThisWeak.lock());
        if (pThis == nullptr)
        {
            // Object was deleted, this is not valid.
            return;
        }
        do
        {
            s_rpsTicketCompletionEvent = task_completion_event<java_rps_ticket>();
            auto signInUi = pThis->msa_sign_in();
            if (signInUi.err())
            {
                pThis->m_signInTaskEvent.set(signInUi);
                return;
            }

            auto signInStatus = pplx::create_task(s_rpsTicketCompletionEvent)
            .then([pThis, authManager, authConfig, forceRefresh, &isSignInComplete](task<java_rps_ticket> ticketTask)
            {
                auto javaRPSTicket = ticketTask.get();
                auto errorCode = static_cast<auth_error_type>(javaRPSTicket.errorCode);

                switch (errorCode)
                {
                    case auth_error_type::NO_ERROR:
                        break;
                    case auth_error_type::ERROR_UI_INTERACTION_REQUIRED:
                        pThis->delete_offline_data();
                        return xbox_live_result<sign_in_result>(sign_in_result(sign_in_status::user_interaction_required));
                    case auth_error_type::ERROR_USER_CANCEL:
                        return xbox_live_result<sign_in_result>(sign_in_result(sign_in_status::user_cancel));
                    case auth_error_type::ERROR_OTHER:
                    default:
                    {
                        pThis->read_offline_data();
                        if (pThis->m_isSignedIn)
                        {
                            return xbox_live_result<sign_in_result>(sign_in_result(sign_in_status::success));
                        }
                        return xbox_live_result<sign_in_result>(xbox_live_error_code::auth_runtime_error, javaRPSTicket.errorMessage);
                    }
                }

                auto rpsTicket = javaRPSTicket.ticket;

                authManager->set_rps_ticket(rpsTicket);
                auto signInResult = authManager->initialize_default_nsal()
                .then([pThis, authManager, authConfig, forceRefresh, &isSignInComplete](xbox_live_result<void> result)
                {
                    if (result.err())
                    {
                        return xbox_live_result<xbox_shell_sign_in_status>(result.err(), result.err_message());
                    }

                    pThis->m_signInlFlowCompleteEvent = pplx::task_completion_event<xbox_live_result<sign_in_and_auth_result>>();
                    pThis->initiate_sign_in_flow(forceRefresh);
                    auto signInResult = create_task(pThis->m_signInlFlowCompleteEvent).get();

                    if (signInResult.err())
                    {
                        return xbox_live_result<xbox_shell_sign_in_status>(signInResult.err(), signInResult.err_message());
                    }

                    authConfig->set_xtoken_composition({ token_identity_type::u_token, token_identity_type::d_token, token_identity_type::t_token });
                    xbox_live_result<string_t> eventTokenResult = authManager->internal_get_token_and_signature(
                        _T("GET"),
                        _T("https://vortex-events.xboxlive.com"),
                        _T("https://vortex-events.xboxlive.com"),
                        string_t(),
                        std::vector<uint8_t>(),
                        false,
                        forceRefresh)
                        .then([](xbox_live_result<token_and_signature_result> result)
                    {
                        if (!result.err())
                        {
                            token_and_signature_result res = result.payload();
                            return xbox_live_result<string_t>(res.token());
                        }
                        return xbox_live_result<string_t>(result.err(), result.err_message());
                    }).get();

                    if (eventTokenResult.err())
                    {
                        return xbox_live_result<xbox_shell_sign_in_status>(eventTokenResult.err(), eventTokenResult.err_message());
                    }
                    stringstream_t str;
                    str << xbox_live_app_config::get_app_config_singleton()->title_id();
                    auto strStr = str.str();

                    auto titleNSALInit = authManager->initialize_title_nsal(strStr).get();
                    if (titleNSALInit.err())
                    {
                        return xbox_live_result<xbox_shell_sign_in_status>(titleNSALInit.err(), titleNSALInit.err_message());
                    }

                    pThis->m_xboxUserId = signInResult.payload().authResult.xbox_user_id();
                    pThis->m_gamertag = signInResult.payload().authResult.gamertag();
                    pThis->m_ageGroup = signInResult.payload().authResult.age_group();
                    pThis->m_privileges = signInResult.payload().authResult.privileges();

                    pThis->m_localConfig->write_value_to_local_storage(pThis->m_xboxUserId, eventTokenResult.payload());
                    pThis->write_offline_data();
                    pThis->m_isSignedIn = true;
                    
                    if (pThis->m_localConfig->use_brokered_authorization())
                    {
                        pThis->clear_pending_intent();
                    }

                    return xbox_live_result<xbox_shell_sign_in_status>(signInResult.payload().signInResult);
                }).get();

                if (signInResult.err())
                {
                    isSignInComplete = true;
                    pThis->read_offline_data();
                    if (pThis->m_isSignedIn)
                    {
                        return xbox_live_result<sign_in_result>(sign_in_result(sign_in_status::success));
                    }
                    pThis->internal_signout().get();
                    return xbox_live_result<sign_in_result>(signInResult.err(), signInResult.err_message());
                }
                else
                {
                    switch (signInResult.payload())
                    {
                    case xbox_shell_sign_in_status::switch_account:
                        isSignInComplete = false;
                        pThis->internal_signout().get();
                        return xbox_live_result<sign_in_result>();
                        break;
                    case xbox_shell_sign_in_status::success:
                    case xbox_shell_sign_in_status::user_interaction_required:
                        isSignInComplete = true;
                        return xbox_live_result<sign_in_result>(xbox_live_result<sign_in_result>(sign_in_result(static_cast<sign_in_status>(signInResult.payload())), signInResult.err(), signInResult.err_message()));
                    case xbox_shell_sign_in_status::user_cancel:
                        isSignInComplete = true;
                        pThis->internal_signout().get();
                        return xbox_live_result<sign_in_result>(xbox_live_result<sign_in_result>(sign_in_result(static_cast<sign_in_status>(signInResult.payload())), signInResult.err(), signInResult.err_message()));
                    }
                }
            }).get();

            if (isSignInComplete)
            {
                pThis->m_signInTaskEvent.set(signInStatus);
            }
        } while (!isSignInComplete);
    });

    m_hasInitialized = true;
    return m_signInTask;
}

pplx::task<xbox::services::xbox_live_result<sign_in_result>>
user_auth_android::sign_in_with_ui()
{
    if (m_hasInitialized && !m_signInTask.is_done())
    {
        return pplx::task_from_result<xbox_live_result<sign_in_result>>(xbox_live_result<sign_in_result>(xbox_live_error_code::auth_runtime_error, "Previous sign in task has not completed"));
    }

    if (java_interop::get_java_interop_singleton()->get_activity() == NULL)
    {
        return pplx::task_from_result<xbox_live_result<sign_in_result>>(xbox_live_result<sign_in_result>(xbox_live_error_code::auth_runtime_error, "User not properly initialized"));
    }
    
    if (m_localConfig->use_brokered_authorization())
    {
        auto result = invoke_brokered_pending_intent();
        return pplx::task_from_result<xbox_live_result<sign_in_result>>(xbox_live_result<sign_in_result>(sign_in_status::success));
    }

    std::weak_ptr<user_auth_android> pThisWeak = std::dynamic_pointer_cast<user_auth_android>(shared_from_this());
    m_signInTaskEvent = pplx::task_completion_event<xbox_live_result<sign_in_result>>();
    m_signInTask = create_task(m_signInTaskEvent);
    pplx::create_task([pThisWeak]()
    {
        std::shared_ptr<user_auth_android> pThis(pThisWeak.lock());
        if (pThis == nullptr)
        {
            // Object was deleted, this is not valid.
            return;
        }
        // At this point we know that this is valid, so we don't need to polute the code with pThis->.
        pThis->m_authFlowCompletionEvent = task_completion_event<auth_flow_result>();
        task<auth_flow_result> authTask = create_task(pThis->m_authFlowCompletionEvent);
        pThis->invoke_auth_flow();
        pThis->complete_sign_in_with_ui(authTask.get());
    });

    m_hasInitialized = true;
    return m_signInTask;
}

void user_auth_android::complete_sign_in_with_ui(const auth_flow_result& authFlowResult)
{
    switch (authFlowResult.status)
    {
    case auth_flow_status::NO_ERROR:
    {
        m_xboxUserId = authFlowResult.user_id;
        m_gamertag = authFlowResult.gamertag;
        m_ageGroup = authFlowResult.age_group;
        m_privileges = authFlowResult.privileges;
        m_cid = authFlowResult.cid;
        m_localConfig->write_value_to_local_storage(m_xboxUserId, authFlowResult.event_token_result);
        write_offline_data();
        m_isSignedIn = true;
        bool newAccount = m_newAccount;
        // Clear m_newAccount before setting m_signInTaskEvent event.
        m_newAccount = false;
        m_signInTaskEvent.set(xbox_live_result<sign_in_result>(sign_in_result(sign_in_status::success, newAccount)));
        break;
    }
    case auth_flow_status::ERROR_USER_CANCEL:
        signout().get();
        m_isSignedIn = false;
        // Clear m_newAccount before setting m_signInTaskEvent event.
        m_newAccount = false;
        m_signInTaskEvent.set(xbox_live_result<sign_in_result>(sign_in_status::user_cancel));
        break;
    case auth_flow_status::PROVIDER_ERROR:
    default:
        signout().get();
        m_isSignedIn = false;
        // Clear m_newAccount before setting m_signInTaskEvent event.
        m_newAccount = false;
        m_signInTaskEvent.set(xbox_live_result<sign_in_result>(xbox_live_error_code::runtime_error));
        break;
    }
}

void user_auth_android::invoke_xb_login_internal(JNIEnv* jniEnv, jobject callback, const string_t& rpsTicket)
{
    m_authManager->set_rps_ticket(rpsTicket);

    auto result = m_authManager->initialize_default_nsal().get();

    if (result.err()) 
    {
        send_error(jniEnv, callback, m_authManager->get_auth_config()->http_status_code(), result.err().value(), result.err_message());
        return;
    }

    m_authManager->get_auth_config()->set_xtoken_composition({ token_identity_type::u_token, token_identity_type::d_token, token_identity_type::t_token });
    auto tokenResult = m_authManager->internal_get_token_and_signature(
        _T("GET"),
        m_authManager->get_auth_config()->xbox_live_endpoint(),
        m_authManager->get_auth_config()->xbox_live_endpoint(),
        string_t(),
        std::vector<uint8_t>(),
        false,
        false
    ).get();

    if (tokenResult.err()) 
    {
        if (m_authManager->get_auth_config()->detail_error() == static_cast<uint32_t>(xbox_live_error_code::XO_E_ACCOUNT_CREATION_REQUIRED))
        {
            send_xb_login_success(jniEnv, callback, tokenResult.payload(), true);

        }
        else if (m_authManager->get_auth_config()->detail_error() == static_cast<uint32_t>(xbox_live_error_code::XO_E_ENFORCEMENT_BAN))
        {
            send_error(jniEnv, callback, m_authManager->get_auth_config()->http_status_code(), int(xbox_live_error_code::XO_E_ENFORCEMENT_BAN), result.err_message());
        }
        else if (result.err() == xbox_live_error_condition::network)
        {
            send_error(jniEnv, callback, m_authManager->get_auth_config()->http_status_code(), int(xbox_live_error_code::AM_E_NO_NETWORK), result.err_message());
        }
        else 
        {
            send_error(jniEnv, callback, m_authManager->get_auth_config()->http_status_code(), result.err().value(), result.err_message());
        }
    }
    else 
    {
        send_xb_login_success(jniEnv, callback, tokenResult.payload(), false);
    }
}

void user_auth_android::send_xb_login_success(JNIEnv* jniEnv, jobject callback, const token_and_signature_result& result, bool createAccount)
{
    m_last_auth_flow_result.init(result);
    m_newAccount = createAccount;

    jclass clsCallback = jniEnv->GetObjectClass(callback);
    jmethodID processResponseMethodId = jniEnv->GetMethodID(clsCallback, "onLogin", "(JZ)V");
    if (processResponseMethodId != NULL)
    {
        jniEnv->CallVoidMethod(callback, processResponseMethodId, reinterpret_cast<jlong>(&m_last_auth_flow_result), jboolean(createAccount));
    }

    JNI_ERROR_CHECK(jniEnv)
}

void user_auth_android::invoke_event_initialization_internal(JNIEnv* jniEnv, jobject callback, const string_t& rpsTicket)
{
    xbox_live_result<string_t> eventTokenResult = m_authManager->internal_get_token_and_signature(
        _T("GET"),
        _T("https://vortex-events.xboxlive.com"),
        _T("https://vortex-events.xboxlive.com"),
        string_t(),
        std::vector<uint8_t>(),
        false,
        true)
        .then([](xbox_live_result<token_and_signature_result> result)
    {
        if (!result.err())
        {
            token_and_signature_result res = result.payload();
            return xbox_live_result<string_t>(res.token());
        }
        return xbox_live_result<string_t>(result.err(), result.err_message());
    }).get();

    if (eventTokenResult.err())
    {
        send_error(jniEnv, callback, m_authManager->get_auth_config()->http_status_code(), eventTokenResult.err().value(), eventTokenResult.err_message());
        return;
    }
    if (!rpsTicket.empty())
    {
        m_authManager->set_rps_ticket(rpsTicket);
    }

    stringstream_t str;
    str << xbox_live_app_config::get_app_config_singleton()->title_id();
    auto strStr = str.str();

    auto titleNSALInit = m_authManager->initialize_title_nsal(strStr).get();
    if (titleNSALInit.err())
    {
        send_error(jniEnv, callback, m_authManager->get_auth_config()->http_status_code(), titleNSALInit.err().value(), titleNSALInit.err_message());
        return;
    }
    send_event_initialization_success(jniEnv, callback, eventTokenResult.payload());
}

void user_auth_android::send_event_initialization_success(JNIEnv* jniEnv, jobject callback, const string_t& eventTokenResult)
{
    m_last_auth_flow_result.event_token_result = eventTokenResult;

    jclass clsCallback = jniEnv->GetObjectClass(callback);
    jmethodID processResponseMethodId = jniEnv->GetMethodID(clsCallback, "onSuccess", "()V");
    if (processResponseMethodId != NULL)
    {
        jniEnv->CallVoidMethod(callback, processResponseMethodId);
    }

    JNI_ERROR_CHECK(jniEnv)
}

void user_auth_android::invoke_x_token_acquisition_internal(JNIEnv* jniEnv, jobject callback)
{
    m_authManager->get_auth_config()->set_xtoken_composition({ token_identity_type::u_token, token_identity_type::d_token, token_identity_type::t_token });

    auto result = m_authManager->internal_get_token_and_signature(
        _T("GET"),
        m_authManager->get_auth_config()->xbox_live_endpoint(),
        m_authManager->get_auth_config()->xbox_live_endpoint(),
        string_t(),
        std::vector<uint8_t>(),
        false,
        false).get();

    if (result.err())
    {
        send_error(jniEnv, callback, m_authManager->get_auth_config()->http_status_code(), result.err().value(), result.err_message());
    }
    else
    {
        send_x_token(jniEnv, callback, result.payload());
    }
}

void user_auth_android::send_x_token(JNIEnv* jniEnv, jobject callback, const token_and_signature_result& result)
{
    m_last_auth_flow_result.init(result);

    jclass clsCallback = jniEnv->GetObjectClass(callback);
    jmethodID processResponseMethodId = jniEnv->GetMethodID(clsCallback, "onXTokenAcquired", "(J)V");
    if (processResponseMethodId != NULL)
    {
        jniEnv->CallVoidMethod(callback, processResponseMethodId, reinterpret_cast<jlong>(&m_last_auth_flow_result));
    }

    JNI_ERROR_CHECK(jniEnv)
}

void user_auth_android::send_error(JNIEnv* jniEnv, jobject callback, uint32_t httpStatusCode, uint32_t errorCode, const string_t& errorMessage)
{
    jclass clsCallback = jniEnv->GetObjectClass(callback);
    jmethodID processResponseMethodId = jniEnv->GetMethodID(clsCallback, "onError", "(IILjava/lang/String;)V");
    if (processResponseMethodId != NULL)
    {
        auto javaStringErrorMessage = jniEnv->NewStringUTF(errorMessage.c_str());
        jniEnv->CallVoidMethod(callback, processResponseMethodId, httpStatusCode, errorCode, javaStringErrorMessage);
    }

    JNI_ERROR_CHECK(jniEnv)
}

void user_auth_android::invoke_xb_logout_internal(JNIEnv* jniEnv, jobject callback)
{
    m_authManager->auth_token_manager()->clear_token_cache();
    m_userSignedOutCallback();
    send_xb_logout_finished(jniEnv, callback);
}

void user_auth_android::send_xb_logout_finished(JNIEnv* jniEnv, jobject callback)
{
    jclass clsCallback = jniEnv->GetObjectClass(callback);
    jmethodID onSignedOut = jniEnv->GetMethodID(clsCallback, "onLoggedOut", "()V");
    if (onSignedOut != NULL)
    {
        jniEnv->CallVoidMethod(callback, onSignedOut);
    }

    JNI_ERROR_CHECK(jniEnv)
}

pplx::task<xbox_live_result<sign_in_result>>
user_auth_android::sign_in_impl(_In_ bool showUI, _In_ bool forceRefresh)
{
    auto thisShared = shared_from_this();
    m_singletonInstance = std::dynamic_pointer_cast<user_auth_android>(thisShared);
    if (showUI) 
    {
        LOGD("sign_in_impl: will be showing UI");
        if (!m_localConfig->is_android_native_activity()) 
        {
            // When we don't use native activity, we only use this sign in with UI 
            // to pass user object to user_impl object.
            LOGD("sign_in_impl: not native activity, NOP");
            return pplx::task_from_result<xbox_live_result<sign_in_result>>(xbox_live_result<sign_in_result>());
        }
        else 
        {
            LOGD("sign_in_impl: native activity, continuing on");
        }
    }
    else 
    {
        LOGD("sign_in_impl: will NOT be showing UI");
    }

    auto interop = java_interop::get_java_interop_singleton();
    if (interop == nullptr)
    {
        return pplx::task_from_result<xbox_live_result<sign_in_result>>(xbox_live_result<sign_in_result>());
    }
    interop->log_telemetry_signin(!showUI, "Entering");
    pplx::task<xbox_live_result<sign_in_result>> result;
    auto silentTask = sign_in_silent(forceRefresh);
    if (showUI)
    {
        std::weak_ptr<user_auth_android> pThisWeak = std::dynamic_pointer_cast<user_auth_android>(shared_from_this());
        result = silentTask.then([pThisWeak, showUI](xbox_live_result<sign_in_result> silentResult)
        {
            std::shared_ptr<user_auth_android> pThis(pThisWeak.lock());
            auto interop = java_interop::get_java_interop_singleton();
            if (interop == nullptr)
            {
                return xbox_live_result<sign_in_result>();
            }
            if (pThis)
            {
                if (silentResult.err() || silentResult.payload().status() != sign_in_status::user_interaction_required)
                {
                    return silentResult;
                }
                return pThis->sign_in_with_ui().get();
            }

            return xbox_live_result<sign_in_result>(xbox_live_error_code::runtime_error, "pThis was null");
        });

    }
    else
    {
        result = silentTask;
    }
    return result.then([showUI](xbox_live_result<sign_in_result> result)
    {
        auto interop = java_interop::get_java_interop_singleton();
        if (interop == nullptr)
        {
            return xbox_live_result<sign_in_result>();
        }
        if (!result.err())
        {
            auto payload = result.payload();
            switch (payload.status())
            {
            case sign_in_status::user_cancel:
                interop->log_telemetry_signin(!showUI, "Exiting - user_cancel");
                break;
            case sign_in_status::user_interaction_required:
                interop->log_telemetry_signin(!showUI, "Exiting - user_interaction_required");
                break;
            default:
                interop->log_telemetry_signin(!showUI, "Exiting");
                break;
            }
        }
        else
        {
            string_t message = "Exiting - ";
            message += (result.err_message().empty()) ? result.err().message() : result.err_message();
            interop->log_telemetry_signin(!showUI, message);
        }
        return result;
    });
}

pplx::task<xbox_live_result<void>>
user_auth_android::signout()
{
    std::weak_ptr<user_auth_android> pThisWeak = std::dynamic_pointer_cast<user_auth_android>(shared_from_this());
    xbox::services::notification::notification_service::get_notification_service_singleton()->unsubscribe_from_notifications();
    return internal_signout()
        .then([pThisWeak](xbox_live_result<void> result)
        {
            std::shared_ptr<user_auth_android> pThis(pThisWeak.lock());
            if (pThis)
            {
                pThis->m_last_auth_flow_result.clear();
                pThis->delete_offline_data();
            }
            return result;
        });

}

pplx::task<xbox_live_result<token_and_signature_result> >
user_auth_android::internal_get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& endpointForNsal,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh
    )
{
    UNREFERENCED_PARAMETER(endpointForNsal);

    auto task = m_authManager->internal_get_token_and_signature(
        _T("GET"),
        url,
        endpointForNsal,
        string_t(),
        std::vector<uint8_t>(),
        false,
        forceRefresh
        );

    return utils::create_exception_free_task<token_and_signature_result>(
        task
        );
}

const std::shared_ptr<auth_config>& user_auth_android::auth_config() const
{
    return m_authManager->get_auth_config();
}

const string_t& user_auth_android::xbox_user_id() const
{
    return m_xboxUserId;
}

const string_t& user_auth_android::gamertag() const
{
    return m_gamertag;
}

const string_t& user_auth_android::age_group() const
{
    return m_ageGroup;
}

const string_t& user_auth_android::privileges() const
{
    return m_privileges;
}

bool user_auth_android::is_signed_in() const
{
    return m_isSignedIn;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

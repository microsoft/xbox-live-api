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
#include "user_impl.h"
#include "auth_flow_result_jni.h"
#include <jni.h>

namespace xbox {
    namespace services {
        class java_interop;
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

struct java_rps_ticket
{
    string_t ticket;
    int32_t errorCode = 0;
    std::string errorMessage;
};

enum class xbox_shell_sign_in_status
{
    success = sign_in_status::success,
    user_interaction_required = sign_in_status::user_interaction_required,
    user_cancel = sign_in_status::user_cancel,
    switch_account
};

struct sign_in_and_auth_result
{
    xbox_shell_sign_in_status signInResult;
    token_and_signature_result authResult;
};

enum class auth_flow_status
{
    NO_ERROR = 0,
    ERROR_USER_CANCEL = 1,
    PROVIDER_ERROR = 2,
};

struct auth_flow_result
{
    auth_flow_status status;
    std::string rps_ticket;
    std::string user_id;
    std::string gamertag;
    std::string age_group;
    std::string privileges;
    std::string cid;
    std::string event_token_result;

    auth_flow_result& init(const token_and_signature_result& result);
    void clear();
};

class user_auth_android : public std::enable_shared_from_this<user_auth_android>
{
public:
    pplx::task<xbox::services::xbox_live_result<sign_in_result>> sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh
    );

    pplx::task<xbox::services::xbox_live_result<void>> signout();

    pplx::task<xbox::services::xbox_live_result<void>> sign_in_impl(
        _In_ const string_t& userDelegationTicket,
        _In_ bool forceRefresh
        );

    user_auth_android();

    void set_signed_out_callback(_In_ const std::function<void()>& signedOutCallback);

    static std::shared_ptr<user_auth_android> get_instance();

    void start_sign_in();
    void finish_sign_in(auth_flow_status authStatus, string_t cid);

    pplx::task<xbox::services::xbox_live_result<token_and_signature_result>>
        internal_get_token_and_signature(
            _In_ const string_t& httpMethod,
            _In_ const string_t& url,
            _In_ const string_t& endpointForNsal,
            _In_ const string_t& headers,
            _In_ const std::vector<unsigned char>& bytes,
            _In_ bool promptForCredentialsIfNeeded,
            _In_ bool forceRefresh
        );

    static void ticket_callback(JNIEnv*, jclass, jstring rpsTicket, jint state, jint errorCode, jstring errorMessage);

    static void sign_out_callback(JNIEnv*, jclass);

    static void auth_flow_callback(JNIEnv*, jclass, jlong userPtr, jint authStatusCode, jstring cid);

    static void invoke_xb_login(JNIEnv*, jclass, jlong userPtr, jstring rpsTicket, jobject callback);

    static void invoke_event_initialization(JNIEnv*, jclass, jlong userPtr, jstring rpsTicket, jobject callback);

    static void invoke_x_token_acquisition(JNIEnv*, jclass, jlong userPtr, jobject callback);

    static void invoke_xb_logout(JNIEnv*, jclass, jlong userPtr, jobject callback);

    static jstring get_uploader_x_token_callback(JNIEnv*, jclass, jboolean forceRefresh);

    static jstring get_supporting_x_token_callback(JNIEnv*, jclass, jstring xuid);

    static jstring get_title_telemetry_device_id(JNIEnv*, jclass);

    static jstring get_title_telemetry_session_id(JNIEnv*, jclass);
    
    bool is_prod() const;
    static void send_error(JNIEnv* jniEnv, jobject callback, uint32_t httpStatusCode, uint32_t errorCode, const string_t& errorMessage);

    const std::shared_ptr<auth_config>& auth_config() const;

    ~user_auth_android();

    const string_t& xbox_user_id() const;
    const string_t& gamertag() const;
    const string_t& age_group() const;
    const string_t& privileges() const;
    bool is_signed_in() const;

private:
    pplx::task<xbox::services::xbox_live_result<sign_in_result>> sign_in_silent(_In_ bool forceRefresh);
    pplx::task<xbox::services::xbox_live_result<sign_in_result>> sign_in_with_ui();
    void complete_sign_in_with_ui(const auth_flow_result& result);

    void initiate_sign_in_flow(_In_ bool forceRefresh);
    void clear_pending_intent();

    xbox::services::xbox_live_result<void> init_sign_in_activity(int32_t requestID);
    void invoke_auth_flow();
    xbox::services::xbox_live_result<void> invoke_brokered_msa_auth();
    xbox_live_result<void> invoke_brokered_pending_intent();

    void invoke_xb_login_internal(JNIEnv*, jobject callback, const string_t&  rpsTicket);
    void send_xb_login_success(JNIEnv* jniEnv, jobject callback, const token_and_signature_result& result, bool createAccount);

    void invoke_event_initialization_internal(JNIEnv*, jobject callback, const string_t&  rpsTicket);
    void send_event_initialization_success(JNIEnv* jniEnv, jobject callback, const string_t& eventTokenResult);

    void invoke_x_token_acquisition_internal(JNIEnv* jniEnv, jobject callback);
    void send_x_token(JNIEnv* jniEnv, jobject callback, const token_and_signature_result& result);

    void invoke_xb_logout_internal(JNIEnv* jniEnv, jobject callback);
    void send_xb_logout_finished(JNIEnv* jniEnv, jobject callback);
    xbox_live_result<sign_in_result> msa_sign_in();
    pplx::task<xbox::services::xbox_live_result<void>> internal_signout();

    void write_offline_data();
    void delete_offline_data();
    void read_offline_data();

    string_t m_xboxUserId;
    string_t m_gamertag;
    string_t m_ageGroup;
    string_t m_privileges;
    string_t m_cid;
    bool m_isSignedIn;

    std::function<void()> m_userSignedOutCallback;

    static pplx::task_completion_event<java_rps_ticket> s_rpsTicketCompletionEvent;
    static pplx::task_completion_event<java_rps_ticket> s_vortexRPSTicketCompletionEvent;
    static pplx::task_completion_event<xbox_live_result<void>> s_signOutCompleteEvent;

    pplx::task_completion_event<xbox_live_result<sign_in_and_auth_result>> m_signInlFlowCompleteEvent;
    pplx::task_completion_event<xbox_live_result<sign_in_result>> m_signInTaskEvent;
    pplx::task_completion_event<xbox_live_result<sign_in_result>> m_signInSilentTaskEvent;
    pplx::task_completion_event<auth_flow_result> m_authFlowCompletionEvent;

    pplx::task<xbox_live_result<sign_in_result>> m_signInTask;
    pplx::task<xbox_live_result<sign_in_result>> m_signInSilentTask;
    std::shared_ptr<auth_manager> m_authManager;
    std::shared_ptr<local_config> m_localConfig;
    bool m_hasInitialized;
    // Result is cached here so that it's lifecycle does not have to be managed in Java.
    auth_flow_result m_last_auth_flow_result;
    bool m_newAccount;

    static std::shared_ptr<user_auth_android> m_singletonInstance;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
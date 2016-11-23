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

#ifdef XSAPI_I
#if defined(__OBJC__)
#import <UIKit/UIKit.h>
#import "XBLSignInHandler.h"
#import "MSAAuthentication/MSAAccountManager.h"
#import "sign_in_delegate_ios.h"

#endif
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

enum class msa_sign_in_result
{
    success,
    ui_required,
    cancel,
    no_network,
    unknown_error
};

class user_auth_ios : public std::enable_shared_from_this<user_auth_ios>
{
public:
    
    user_auth_ios();
    
    pplx::task<xbox::services::xbox_live_result<sign_in_result>> sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh
        );
    
    pplx::task<xbox::services::xbox_live_result<void>> signout();
    
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

#if defined(__OBJC__)
    XBLMSASignInDelagate *get_sign_in_delegate();
    void set_launch_view_controller(
        _In_ UIViewController *controller
        );
#endif
    
    static std::shared_ptr<user_auth_ios> get_instance();
    
    const string_t& xbox_user_id() const;
    const string_t& gamertag() const;
    const string_t& age_group() const;
    const string_t& privileges() const;
    bool is_signed_in() const;
    
    void set_signed_out_callback(_In_ const std::function<void()>& signedOutCallback);
    
    void set_title_telemetry_session_id(_In_ const string_t& sessionId);
    const string_t& title_telemetry_session_id();
    
private:    
    std::shared_ptr<auth_manager> m_authManager;
    std::shared_ptr<auth_config> m_authConfig;
    std::shared_ptr<local_config> m_localConfig;
    
    pplx::task_completion_event<xbox::services::xbox_live_result<sign_in_result>> m_signInTCE;
    pplx::task<xbox::services::xbox_live_result<sign_in_result>> m_signInTask;
    bool m_signInTaskInitialized;
    
    pplx::task_completion_event<xbox::services::xbox_live_result<void>> m_signOutTCE;
    
    void InitMSA();
    
    xbox_live_result<sign_in_result> sign_in_internal(
        _In_ bool showUI,
        _In_ bool forceRefresh,
        _In_ bool silentAPI
        );
    
    pplx::task<xbox_live_result<void>> sign_out_internal();
    
    pplx::task<xbox::services::xbox_live_result<msa_sign_in_result>> msa_sign_in(
        _In_ bool showUI,
        _In_ bool silentAPI
        );
    pplx::task<xbox::services::xbox_live_result<sign_in_result>> xbox_sign_in(
        _In_ bool forceRefresh
        );
    pplx::task<xbox::services::xbox_live_result<sign_in_result>> get_token_and_sign_in(
        _In_ bool forceRefresh
        );

    pplx::task<xbox_live_result<sign_in_result>> provision_user_xuid();

    void set_up_xbox_ui();
    pplx::task<xbox::services::xbox_live_result<void>> clean_up_xbox_ui();
#if defined(__OBJC__)
    pplx::task<xbox_live_result<XBLUISignInResult>> launch_xbox_ui(
        bool isNewAccount,
        int error
        );
    
    XBLMSASignInDelagate *m_msaSignInHandler;
    XBLSignInHandler *m_xboxSignInUIHandler;
#endif
    
    string_t m_xboxUserId;
    string_t m_gamertag;
    string_t m_ageGroup;
    string_t m_privileges;
    string_t m_webAccountId;
    bool m_isSignedIn;
    string_t m_titleTelemetrySessionId;
    
    std::function<void()> m_userSignedOutCallback;
    
    static std::shared_ptr<user_auth_ios> s_userAuthSingletonInstance;
    static xbox_live_mutex s_userAuthSingletonLock;

    void read_offline_data();
    void write_offline_data();
    void delete_offline_data();
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

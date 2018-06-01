// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/system.h"
#include "xbox_system_factory.h"
#include "shared_macros.h"
#include "system_internal.h"
#include "auth_config.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

typedef xbox_live_callback<xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>> token_and_signature_callback;

class user_impl : public std::enable_shared_from_this<user_impl>
{
public:
    virtual void sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<sign_in_result>> callback
        ) = 0;

#if XSAPI_A
    virtual void set_user(_In_ const std::weak_ptr<xbox_live_user>& user) = 0;
#endif
    
#if XSAPI_U
    virtual pplx::task<xbox::services::xbox_live_result<void>> signout() = 0;
#endif

    const xsapi_internal_string& xbox_user_id() { return m_xboxUserId; }
    const xsapi_internal_string& gamertag() { return m_gamertag; }
    const xsapi_internal_string& age_group() { return m_ageGroup; }
    const xsapi_internal_string& privileges() { return m_privileges; }

#if XSAPI_U
    const xsapi_internal_string& user_settings_restrictions() { return m_userSettingsRestrictions; }
    const xsapi_internal_string& user_enforcement_restrictions() { return m_userEnforcementRestrictions; }
    const xsapi_internal_string& user_title_restrictions() { return m_userTitleRestrictions; }
    virtual void clear_token_cache() = 0;
    void set_sign_in_options(std::shared_ptr<xbox_sign_in_options> options) { m_signInOptions = options; }
#endif

    const xsapi_internal_string& web_account_id() { return m_webAccountId; }
    std::shared_ptr<auth_config> get_auth_config() { return m_authConfig; }
    const user_creation_context creation_context() { return m_creationContext;  }

#if UNIT_TEST_SERVICES
    void _Set_xbox_user_id(const xsapi_internal_string& xboxUserId) { m_xboxUserId = xboxUserId; }
#endif

    void get_token_and_signature(
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& url,
        _In_ const xsapi_internal_string& headers,
        _In_opt_ async_queue_handle_t queue,
        _In_ token_and_signature_callback callback
        );

    void get_token_and_signature(
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& url,
        _In_ const xsapi_internal_string& headers,
        _In_ const xsapi_internal_string& requestBodyString,
        _In_opt_ async_queue_handle_t queue,
        _In_ token_and_signature_callback callback
        );

    void get_token_and_signature(
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& url,
        _In_ const xsapi_internal_string& headers,
        _In_ const xsapi_internal_vector<unsigned char>& requestBodyArray,
        _In_opt_ async_queue_handle_t queue,
        _In_ token_and_signature_callback callback
        );

    bool is_signed_in() const;
    void set_user_pointer(_In_ std::shared_ptr<system::xbox_live_user> user);
    void set_title_telemetry_session_id(_In_ const xsapi_internal_string& sessionId);
    const xsapi_internal_string& title_telemetry_session_id();

    virtual void internal_get_token_and_signature(
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& url,
        _In_ const xsapi_internal_string& endpointForNsal,
        _In_ const xsapi_internal_string& headers,
        _In_ const xsapi_internal_vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh,
        _In_opt_ async_queue_handle_t queue,
        _In_ token_and_signature_callback callback
        ) = 0;

    static function_context add_sign_in_completed_handler(_In_ xbox_live_callback<const xsapi_internal_string&> handler);
    static void remove_sign_in_completed_handler(_In_ function_context context);

    static function_context add_sign_out_completed_handler(_In_ xbox_live_callback<const sign_out_completed_event_args&> handler);
    static void remove_sign_out_completed_handler(_In_ function_context context);

protected:
    user_impl(_In_ bool initConfig = true, _In_opt_ user_creation_context creationContext = nullptr);

    void user_signed_out();
    void user_signed_in(
        _In_ xsapi_internal_string xboxUserId,
        _In_ xsapi_internal_string gamertag,
        _In_ xsapi_internal_string ageGroup,
        _In_ xsapi_internal_string privileges,
#if XSAPI_U
        _In_ xsapi_internal_string userSettingsRestrictions,
        _In_ xsapi_internal_string userEnforcementRestrictions,
        _In_ xsapi_internal_string userTitleRestrictions,
#endif
        _In_ xsapi_internal_string accountId
        );

    xsapi_internal_string m_xboxUserId;
    xsapi_internal_string m_gamertag;
    xsapi_internal_string m_ageGroup;
    xsapi_internal_string m_privileges;
#if XSAPI_U
    xsapi_internal_string m_userSettingsRestrictions;
    xsapi_internal_string m_userEnforcementRestrictions;
    xsapi_internal_string m_userTitleRestrictions;
    std::shared_ptr<xbox_sign_in_options> m_signInOptions;
#endif
    xsapi_internal_string m_webAccountId;
    xsapi_internal_string m_cid;
    xsapi_internal_string m_titleTelemetrySessionId;
    bool m_isSignedIn;
    user_creation_context m_creationContext;
    std::weak_ptr<system::xbox_live_user> m_weakUserPtr;

    std::shared_ptr<auth_config> m_authConfig;
    std::shared_ptr<local_config> m_localConfig;
    xbox::services::system::xbox_live_mutex m_lock;
};

#if UWP_API
class user_impl_idp : public user_impl
{
public:
    void sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<sign_in_result>> callback
        ) override;

    user_impl_idp(
#if UWP_API 
        Windows::System::User^ systemUser
#endif
        );

    void internal_get_token_and_signature(
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& url,
        _In_ const xsapi_internal_string& endpointForNsal,
        _In_ const xsapi_internal_string& headers,
        _In_ const xsapi_internal_vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh,
        _In_opt_ async_queue_handle_t queue,
        _In_ token_and_signature_callback callback
        ) override;

private:

    void user_signed_out();
    void user_signed_in(
        _In_ xsapi_internal_string xboxUserId,
        _In_ xsapi_internal_string gamertag,
        _In_ xsapi_internal_string ageGroup,
        _In_ xsapi_internal_string privileges,
        _In_ xsapi_internal_string accountId
        );

    void check_user_signed_out();

    void initialize_provider(
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<void> callback
        );

    // sync version of get internal_get_token_and_signature
    xbox::services::xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>
    internal_get_token_and_signature_helper(
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& url,
        _In_ const xsapi_internal_string& headers,
        _In_ const xsapi_internal_vector<uint8_t>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh
        );

    // sync method for request token 
    static Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ request_token_from_idp(
        _In_opt_ Windows::UI::Core::CoreDispatcher^ coreDispatcher,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ Windows::Security::Authentication::Web::Core::WebTokenRequest^ request,
        _In_opt_ Windows::Security::Credentials::WebAccount^ webAccount
        );

    xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>
    convert_web_token_request_result(
        _In_ Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ tokenResult
        );

    static sign_in_result
    convert_web_token_request_status(
        _In_opt_ Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ tokenResult
        );

    static bool is_multi_user_application();

    Windows::Security::Credentials::WebAccountProvider^ m_provider;
    Windows::System::Threading::ThreadPoolTimer^ m_timer;

    // user watcher 
    static void on_system_user_removed(Windows::System::UserWatcher ^sender, Windows::System::UserChangedEventArgs ^args);
};

#endif // #if UWP_API

#if !TV_API
class user_factory
{
public:
    static xbox_live_user create_user(_In_ const std::shared_ptr<user_impl>& userImpl)
    {
        return xbox_live_user(userImpl);
    }

    static std::shared_ptr<user_impl> create_user_impl(user_creation_context userCreationContext);

};
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

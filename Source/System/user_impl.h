// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/system.h"
#include "xbox_system_factory.h"
#include "shared_macros.h"
#include "system_internal.h"
#include "auth_config.h"
#if defined(XSAPI_SERVER)
#include "auth/auth_manager.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class user_impl : public std::enable_shared_from_this<user_impl>
{
public:
    virtual pplx::task<xbox_live_result<sign_in_result>> sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh
        ) = 0;

#if XSAPI_A
    virtual void set_user(_In_ const std::weak_ptr<xbox_live_user>& user) = 0;
#endif
    
#if XSAPI_U
    virtual pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<void>> signout() = 0;
#endif

    virtual pplx::task<xbox_live_result<void>> sign_in_impl(
        _In_ const string_t& userDelegationTicket,
        _In_ bool forceRefresh
        )
    { 
        UNREFERENCED_PARAMETER(userDelegationTicket);
        UNREFERENCED_PARAMETER(forceRefresh);
        return pplx::task_from_exception<XBOX_LIVE_NAMESPACE::xbox_live_result<void>>(std::exception());
    }

    // IUser
    const string_t& xbox_user_id() { return m_xboxUserId; }
    const string_t& gamertag() { return m_gamertag; }
    const string_t& age_group() { return m_ageGroup; }
    const string_t& privileges() { return m_privileges; }
    const string_t& web_account_id() { return m_webAccountId; }
    std::shared_ptr<auth_config> get_auth_config() { return m_authConfig; }
    const user_creation_context creation_context() { return m_creationContext;  }

#if UNIT_TEST_SERVICES
    void _Set_xbox_user_id(const string_t& xboxUserId) { m_xboxUserId = xboxUserId; }
#endif

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result> >
    get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers
        );

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result> >
    get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_ const string_t& requestBodyString
        );

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result> >
    get_token_and_signature_array(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& requestBodyArray
        );

    bool is_signed_in();
    void set_user_pointer(_In_ std::shared_ptr<system::xbox_live_user> user);
    void set_title_telemetry_session_id(_In_ const string_t& sessionId);
    const string_t& title_telemetry_session_id();

    virtual pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result> >
    internal_get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& endpointForNsal,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh
        ) = 0;

    static function_context add_sign_in_completed_handler(_In_ std::function<void(const string_t&)> handler);
    static void remove_sign_in_completed_handler(_In_ function_context context);

    static function_context add_sign_out_completed_handler(_In_ std::function<void(const sign_out_completed_event_args&)> handler);
    static void remove_sign_out_completed_handler(_In_ function_context context);

protected:
    user_impl(_In_ bool initConfig = true, _In_opt_ user_creation_context creationContext = nullptr);

    void user_signed_out();
    void user_signed_in(
        _In_ string_t xboxUserId,
        _In_ string_t gamertag,
        _In_ string_t ageGroup,
        _In_ string_t privileges,
        _In_ string_t accountId
        );

    string_t m_xboxUserId;
    string_t m_gamertag;
    string_t m_ageGroup;
    string_t m_privileges;
    string_t m_webAccountId;
    string_t m_cid;
    string_t m_titleTelemetrySessionId;
    bool m_isSignedIn;
    user_creation_context m_creationContext;
    std::weak_ptr<system::xbox_live_user> m_weakUserPtr;

    std::shared_ptr<auth_config> m_authConfig;
    std::shared_ptr<local_config> m_localConfig;
    static std::unordered_map<function_context, std::function<void(const string_t&)>> s_signInCompletedHandlers;
    static std::unordered_map<function_context, std::function<void(const sign_out_completed_event_args&)>> s_signOutCompletedHandlers;
    static function_context s_signInCompletedHandlerIndexer;
    static function_context s_signOutCompletedHandlerIndexer;
    static XBOX_LIVE_NAMESPACE::system::xbox_live_mutex s_trackingUsersLock;
    XBOX_LIVE_NAMESPACE::system::xbox_live_mutex m_lock;
};

#if UWP_API
class user_impl_idp : public user_impl
{
public:
    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<sign_in_result>> sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh
        ) override;

    // Not supported for user_impl_idp
    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<void>> sign_in_impl(
        _In_ const string_t& userDelegationTicket,
        _In_ bool forceRefresh
        ) override;

    user_impl_idp(Windows::System::User^ systemUser);

    pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result>>
    internal_get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& endpointForNsal,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh
        ) override;

private:

    void user_signed_out();
    void user_signed_in(
        _In_ string_t xboxUserId,
        _In_ string_t gamertag,
        _In_ string_t ageGroup,
        _In_ string_t privileges,
        _In_ string_t accountId
        );

    void check_user_signed_out();

    pplx::task<void> initialize_provider();

    // sync version of get internal_get_token_and_signature
	XBOX_LIVE_NAMESPACE::xbox_live_result<token_and_signature_result>
    internal_get_token_and_signature_helper(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& headers,
        _In_ const std::vector<uint8_t>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh
        );

    // sync method for request token 
    Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ request_token_from_idp(
        _In_opt_ Windows::UI::Core::CoreDispatcher^ coreDispatcher,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ Windows::Security::Authentication::Web::Core::WebTokenRequest^ request
        );

    xbox_live_result<token_and_signature_result>
    convert_web_token_request_result(
        _In_ Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ tokenResult
        );

    static sign_in_result
    convert_web_token_request_status(
        _In_ Windows::Security::Authentication::Web::Core::WebTokenRequestResult^ tokenResult
        );

    static bool is_multi_user_application();

    Windows::Security::Credentials::WebAccountProvider^ m_provider;
    Windows::System::Threading::ThreadPoolTimer^ m_timer;

    // user watcher 
    static Windows::System::UserWatcher^ s_userWatcher;
    static std::unordered_map<string_t, std::shared_ptr<user_impl_idp>> s_trackingUsers;
    static void on_system_user_removed(Windows::System::UserWatcher ^sender, Windows::System::UserChangedEventArgs ^args);
};

#endif // #if UWP_API


#if defined(XSAPI_SERVER)
class user_impl_server : public user_impl
{
public:
    // Not supported for user_impl_server
    pplx::task<xbox::services::xbox_live_result<sign_in_result>> sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh
        ) override;

    pplx::task<xbox::services::xbox_live_result<void>> sign_in_impl(
        _In_ const string_t& userDelegationTicket,
        _In_ bool forceRefresh
        ) override;

    user_impl_server(void*);

    pplx::task<xbox::services::xbox_live_result<token_and_signature_result>>
    internal_get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& endpointForNsal,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh
        ) override;

private:
    std::shared_ptr<auth_manager> m_authManager;
};
#endif

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

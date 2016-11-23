//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#import "pch.h"
#import "user_impl_ios.h"
#import "user_auth_ios.h"
#import "XboxProviderBridge.h"
#import "XBLServiceManager.h"
#import "service_call_logger.h"
#import "XLSCll.h"
#import "MSAAuthentication/MSAAccountManager.h"
#include "notification_service.h"
#import "UTCIDPNames.h"

using namespace pplx;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

enum class XBLSignInState
{
    MSA,
    Xbox,
    Provision,
    UI,
    SwitchAccount
};

std::shared_ptr<user_auth_ios> user_auth_ios::s_userAuthSingletonInstance;
xbox::services::system::xbox_live_mutex user_auth_ios::s_userAuthSingletonLock;

#pragma mark - Sign In

pplx::task<xbox_live_result<sign_in_result>>
user_auth_ios::sign_in_impl(_In_ bool showUI, _In_ bool forceRefresh)
{
    if (m_signInTaskInitialized) {
        return pplx::task_from_result<xbox_live_result<sign_in_result>>(xbox_live_result<sign_in_result>(xbox_live_error_code::auth_runtime_error, "Previous sign in task has not completed"));
    }
    m_signInTaskInitialized = true;
        
    XLSCll *sharedCLL = [XLSCll sharedTelemetryManager];
    [XBLCll setUpWithTelemetryManager:sharedCLL];
    [sharedCLL pageViewEvent:IDP_PageView_Signin_View withData:nil fromPage:@"NA"];
    [sharedCLL setTitleSessionId:[NSString stringWithUTF8String:m_titleTelemetrySessionId.c_str()]];

    [XLSCll apiSignInEvent:!showUI withState:@"Entering"];
    
    XBLServiceManager *manager = [[XBLServiceManager alloc] init];
    [XBLService setUpWithServiceManager:manager];
    
    std::weak_ptr<user_auth_ios> thisWeakPtr = std::dynamic_pointer_cast<user_auth_ios>(shared_from_this());
    auto silentTask = create_task([thisWeakPtr, forceRefresh, showUI]()
    {
        auto pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            return pThis->sign_in_internal(false, forceRefresh, !showUI);
        }
        else
        {
            return xbox_live_result<sign_in_result>(xbox_live_error_code::runtime_error, _T("Sign In Failed"));
        }
    });
    
    pplx::task<xbox_live_result<sign_in_result>> task;
    if(showUI)
    {
        task = silentTask.then([thisWeakPtr, forceRefresh, showUI](xbox_live_result<sign_in_result> silentResult)
        {
            auto pThis(thisWeakPtr.lock());
            if (pThis != nullptr && silentResult.payload().status() == sign_in_status::user_interaction_required)
            {
                return pThis->sign_in_internal(true, forceRefresh, !showUI);
            }
            else
            {
                return silentResult;
            }
        });
    }
    else
    {
        task = silentTask;
    }
    
    return task.then([showUI](xbox_live_result<sign_in_result> result)
    {
        if(!result.err())
        {
            auto payload = result.payload();
            switch(payload.status())
            {
                case sign_in_status::user_cancel:
                    [XLSCll apiSignInEvent:!showUI withState:@"Exiting - user_cancel"];
                    break;
                case sign_in_status::user_interaction_required:
                    [XLSCll apiSignInEvent:!showUI withState:@"Exiting - user_interaction_required"];
                    break;
                default:
                    [XLSCll apiSignInEvent:!showUI withState:@"Exiting"];
                    break;
            }
                             
        }
        else
        {
            string_t message = (result.err_message().empty()) ? result.err().message() : result.err_message();
            [XLSCll apiSignInEvent:!showUI withState:[NSString stringWithFormat:@"Exiting - %@", [NSString stringWithUTF8String:message.c_str()]]];
        }
                         
        return result;
    });
}

xbox_live_result<sign_in_result>
user_auth_ios::sign_in_internal(_In_ bool showUI, _In_ bool forceRefresh, _In_ bool silentAPI)
{
    std::weak_ptr<user_auth_ios> thisWeakPtr = std::dynamic_pointer_cast<user_auth_ios>(shared_from_this());
    
    // Gate the beggining and end of this funtion with set up and tear down of the xbox ui if we show it
    if (showUI)
    {
        set_up_xbox_ui();
    }
    
    XBLSignInState state = XBLSignInState::MSA;
    sign_in_status signInStatus = sign_in_status::user_cancel;
    XBLSignInError signInError = XBLSignInErrorNone;
    bool signInIsComplete = false;
    bool didProvisionAccount = false;
    XLSCll *sharedCLL = [XLSCll sharedTelemetryManager];

    // Step by step process of signing in.
    // Flow is:
    // 1. Sign in MSA -> 2.
    // 2. Sign in Xbox
    //    If account is not provisioned -> 3
    //    Else -> 4
    // 3. Provision -> 2
    // 4. Show UI if needed
    //    If no ui requirement -> Done
    //    Else if ui result is switch accounts -> 5
    //    else -> done
    // 5. Switch Accounts by signing out of the user -> 1.
    do {
        switch (state) {
            case XBLSignInState::MSA:
            {
                [sharedCLL pageActionEvent:@"Signin - Sign in Start"];
                
                // Can happen if Xbox fails sign in, but MSA did not
                if (m_msaSignInHandler.signedIn) {
                    state = XBLSignInState::Xbox;
                    break;
                }
                
                
                auto result = msa_sign_in(showUI, silentAPI).get().payload();
                
                switch (result) {
                    case msa_sign_in_result::success:
                        [sharedCLL pageActionEvent:@"Signin - MSA Account acquired"];
                        state = XBLSignInState::Xbox;
                        break;
                    case msa_sign_in_result::ui_required:
                        signInStatus = sign_in_status::user_interaction_required;
                        signInError = XBLSignInErrorUnknown;
                        [sharedCLL pageActionEvent:@"Signin - Sign in Error: User Interaction Required"];
                        state = XBLSignInState::UI;
                        break;
                    case msa_sign_in_result::no_network:
                        state = XBLSignInState::UI;
                        [sharedCLL pageActionEvent:@"Signin - Sign in Error: Offline"];
                        signInError = XBLSignInErrorOffline;
                        break;
                    case msa_sign_in_result::cancel:
                        state = XBLSignInState::UI;
                        signInStatus = sign_in_status::user_cancel;
                        [sharedCLL pageActionEvent:@"Signin - Sign in Canceled"];
                        signInIsComplete = true;
                        break;
                    case msa_sign_in_result::unknown_error:
                        state = XBLSignInState::UI;
                        [sharedCLL pageActionEvent:@"Signin - Sign in Error: Unknown Error"];
                        signInError = XBLSignInErrorUnknown;
                        break;
                    default:
                        break;
                }
                break;
            }
            case XBLSignInState::Xbox:
            {
                bool refresh = forceRefresh || didProvisionAccount;
                auto result = xbox_sign_in(refresh).get();
                
                if (result.err() == xbox_live_error_code::XO_E_ACCOUNT_CREATION_REQUIRED)
                {
                    state = XBLSignInState::Provision;
                }
                else if (result.err() == xbox_live_error_code::XO_E_ENFORCEMENT_BAN)
                {
                    state = XBLSignInState::UI;
                    signInError = XBLSignInErrorBan;
                }
                else if (result.err())
                {
                    // Enable to enable offline
                    read_offline_data();
                    if (m_isSignedIn)
                    {
                        state = XBLSignInState::UI;
                        signInStatus = sign_in_status::success;
                    }
                    else
                    {
                        //send telemetry on error result
                        NSString *errorCode = [NSString stringWithFormat:@"%d", result.err().value()];
                        NSString *errorText = [NSString stringWithCString:result.err().message().substr(0, 5000).c_str() encoding:[NSString defaultCStringEncoding]];
                        NSString *errorStack = [NSString stringWithCString:result.err_message().substr(0, 5000).c_str() encoding:[NSString defaultCStringEncoding]];
                        [sharedCLL clientErrorEvent:IDP_Client_Errors_Failure errorText:errorText errorCode:errorCode callStack:errorStack withData:@{@"Source" : @"XBLSignInState::Xbox",
                                                                                                                                                           @"isSilent" : [NSNumber numberWithBool:NO]}];
                        
                        state = XBLSignInState::UI;
                        signInError = XBLSignInErrorUnknown;
                    }
                }
                else
                {
                    state = XBLSignInState::UI;
                    signInStatus = sign_in_status::success;
                    [sharedCLL setCurrentUser:[NSString stringWithUTF8String:this->m_xboxUserId.c_str()]];
                }
                break;
            }
            case XBLSignInState::Provision:
            {
                auto result = provision_user_xuid().get();
                m_authConfig->reset();
                m_authManager->auth_token_manager()->clear_token_cache();
                if (result.err())
                {
                    signInError = XBLSignInErrorCreation;
                    state = XBLSignInState::UI;
                }
                else
                {
                    didProvisionAccount = true;
                    state = XBLSignInState::Xbox;
                }
                break;
            }
            case XBLSignInState::UI:
            {
                if (!showUI)
                {
                    signInIsComplete = true;
                }
                else
                {
                    auto status = launch_xbox_ui(didProvisionAccount, signInError).get().payload();
                    signInError = XBLSignInErrorNone;
                    
                    switch (status)
                    {
                        case XBLUISignInResult::Success:
                        {
                            signInStatus = sign_in_status::success;
                            signInIsComplete = true;
                            break;
                        }
                        case XBLUISignInResult::Cancel:
                        {
                            signInStatus = sign_in_status::user_cancel;
                            signInIsComplete = true;
                            break;
                        }
                        case XBLUISignInResult::SwitchAccounts:
                        {
                            state = XBLSignInState::SwitchAccount;
                            break;
                        }
                        case XBLUISignInResult::RetrySignIn:
                        {
                            state = XBLSignInState::MSA;
                            break;
                        }
                        case XBLUISignInResult::RetryAccountCreation:
                        {
                            state = XBLSignInState::Provision;
                            break;
                        }
                    }
                }
                break;
            }
            case XBLSignInState::SwitchAccount:
            {
                sign_out_internal().get();
                state = XBLSignInState::MSA;
                signInError = XBLSignInErrorNone;
                break;
            }
        }
        
    } while(!signInIsComplete);
    
    if (signInStatus == sign_in_status::user_cancel)
    {
        auto result = sign_out_internal().get();
    }
    
    if (showUI)
    {
        clean_up_xbox_ui().get();
    }
    
    auto result = sign_in_result(signInStatus, didProvisionAccount);
    
    m_signInTaskInitialized = false;
    return result;
}

pplx::task<xbox_live_result<msa_sign_in_result>>
user_auth_ios::msa_sign_in(_In_ bool showUI, _In_ bool silentAPI)
{
    if(m_msaSignInHandler.signingIn)
    {
        return pplx::task_from_result<xbox_live_result<msa_sign_in_result>>(xbox_live_result<msa_sign_in_result>(msa_sign_in_result::unknown_error));
    }
    
    auto tce = pplx::task_completion_event<xbox_live_result<msa_sign_in_result>>();
    auto task = create_task(tce);
    
    m_msaSignInHandler.launchViewController = [m_xboxSignInUIHandler topViewController];
    [m_msaSignInHandler signInWithUIAllowed:showUI silentAPI:silentAPI completion:^(BOOL success, NSError *error) {
        msa_sign_in_result result = msa_sign_in_result::unknown_error;
        if (success) {
            result = msa_sign_in_result::success;
        } else if (error) {
            if ([error isKindOfClass:[MSAAuthenticationError class]]) {
                MSAAuthenticationErrorCode code = (MSAAuthenticationErrorCode)error.code;
                switch (code) {
                    case MSAAuthenticationErrorCodeUIRequired:
                        result = msa_sign_in_result::ui_required;
                        break;
                    case MSAAuthenticationErrorCodeUserCancelled:
                        result = msa_sign_in_result::cancel;
                        break;
                    case MSAAuthenticationErrorCodeNetworkError:
                        result = msa_sign_in_result::no_network;
                        break;
                    default:
                        break;
                }
            }
        }
        tce.set(xbox_live_result<msa_sign_in_result>(result));
    }];
    
    return task;
}

pplx::task<xbox_live_result<sign_in_result>>
user_auth_ios::xbox_sign_in(_In_ bool forceRefresh)
{
    std::weak_ptr<user_auth_ios> thisWeakPtr = std::dynamic_pointer_cast<user_auth_ios>(shared_from_this());
    
    return m_authManager->initialize_default_nsal()
    .then([thisWeakPtr, forceRefresh](xbox_live_result<void> nsalResult) {
        if (nsalResult.err())
        {
            return xbox_live_result<sign_in_result>(nsalResult.err(), nsalResult.err_message());
        }
        
        auto pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            return xbox_live_result<sign_in_result>(xbox_live_error_code::runtime_error, "xbox_live_ios_impl shutting down");
        }

        auto result = pThis->m_authManager->internal_get_token_and_signature(
                                                                             _T("GET"),
                                                                             pThis->m_authConfig->xbox_live_endpoint(),
                                                                             pThis->m_authConfig->xbox_live_endpoint(),
                                                                             string_t(),
                                                                             std::vector<uint8_t>(),
                                                                             false,
                                                                             forceRefresh
                                                                             ).get();
        if (result.err())
        {
            if (pThis->m_authConfig->detail_error() != static_cast<uint32_t>(xbox_live_error_code::no_error))
            {
                auto errorResult = static_cast<xbox_live_error_code>(pThis->m_authConfig->detail_error());
                return xbox_live_result<sign_in_result>(errorResult);
            }
            return xbox_live_result<sign_in_result>(result.err(), result.err_message());
        }
        else
        {
            xbox_live_result<string_t> eventTokenResult = pThis->m_authManager->internal_get_token_and_signature(
                                                                                                                 _T("GET"),
                                                                                                                 _T("https://vortex-events.xboxlive.com"),
                                                                                                                 _T("https://vortex-events.xboxlive.com"),
                                                                                                                 string_t(),
                                                                                                                 std::vector<uint8_t>(),
                                                                                                                 false,
                                                                                                                 true)
            .then([](xbox_live_result<token_and_signature_result> result) {
                if (!result.err())
                {
                    token_and_signature_result res = result.payload();
                    return xbox_live_result<string_t>(res.token());
                }
                return xbox_live_result<string_t>(result.err(), result.err_message());
            }).get();
            
            if (eventTokenResult.err())
            {
                return xbox_live_result<sign_in_result>(eventTokenResult.err(), eventTokenResult.err_message());
            }
            
            stringstream_t titleIdStream;
            titleIdStream << xbox_live_app_config::get_app_config_singleton()->title_id();
            auto titleId = titleIdStream.str();
            auto titleNSALInit = pThis->m_authManager->initialize_title_nsal(titleId).get();
            if(titleNSALInit.err())
            {
                return xbox_live_result<sign_in_result>(titleNSALInit.err(), titleNSALInit.err_message());
            }
            auto& payload = result.payload();
            
            pThis->m_gamertag = payload.gamertag();
            pThis->m_xboxUserId = payload.xbox_user_id();
            pThis->m_ageGroup = payload.age_group();
            pThis->m_privileges = payload.privileges();
            pThis->m_webAccountId = payload.web_account_id();
            
            pThis->m_localConfig->write_value_to_local_storage(pThis->m_xboxUserId, eventTokenResult.payload());
            pThis->write_offline_data();
            pThis->m_isSignedIn = true;
            
            return xbox_live_result<sign_in_result>(sign_in_result(sign_in_status::success));
        }
    });
}

void
user_auth_ios::set_up_xbox_ui()
{
    std::weak_ptr<user_auth_ios> thisWeakPtr = std::dynamic_pointer_cast<user_auth_ios>(shared_from_this());
    
    m_xboxSignInUIHandler = [[XBLSignInHandler alloc] init];
    [m_xboxSignInUIHandler launchLoadingScreen];
    
    m_xboxSignInUIHandler.gamertagChangeBlock = ^(string_t gamertag) {
        std::shared_ptr<user_auth_ios> pThis(thisWeakPtr.lock());
        
        if (pThis != nullptr && gamertag.length()) {
            pThis->m_gamertag = gamertag;
        }
    };
}

pplx::task<xbox::services::xbox_live_result<void>>
user_auth_ios::clean_up_xbox_ui()
{
    // Return a task so that we only return to the calling thread when UI animation is over
    auto tce = pplx::task_completion_event<xbox_live_result<void>>();
    std::weak_ptr<user_auth_ios> thisWeakPtr = std::dynamic_pointer_cast<user_auth_ios>(shared_from_this());

    // Xbox UI Cleanup
    [m_xboxSignInUIHandler dismissUIWithCompletion:^{
        std::shared_ptr<user_auth_ios> pThis(thisWeakPtr.lock());
        
        if (pThis != nullptr)
        {
            pThis->m_xboxSignInUIHandler = nil;
        }
        
        tce.set(xbox_live_result<void>());
    }];
    
    return create_task(tce);
}

pplx::task<xbox_live_result<XBLUISignInResult>>
user_auth_ios::launch_xbox_ui(bool isNewAccount, int error)
{
    XBLSignInError signInError = (XBLSignInError)error;
    
    auto tce = pplx::task_completion_event<xbox_live_result<XBLUISignInResult>>();
    auto task = create_task(tce);
    
    [m_xboxSignInUIHandler setTce:tce];
    [m_xboxSignInUIHandler setUser:shared_from_this()];
    [m_xboxSignInUIHandler signedInWithNewAccount:isNewAccount error:signInError];
    
    return task;
}

pplx::task<xbox_live_result<sign_in_result>>
user_auth_ios::provision_user_xuid()
{
    
    m_authConfig->reset();
    m_authManager->auth_token_manager()->clear_token_cache();
    return create_task([]() {
        auto provider = std::make_shared<xbox::services::social::XboxProviderBridge>();
        
        NSLocale *currentLocale = [NSLocale currentLocale];  // get the current locale.
        NSString *countryCode = [currentLocale objectForKey:NSLocaleCountryCode];
        string_t locale = [countryCode UTF8String];
        
        auto response = provider->set_up_microsoft_profile(locale);
        
        
        if (response.err())
        {
            return xbox_live_result<sign_in_result>(response.err());
        }
        else
        {
            return xbox_live_result<sign_in_result>(sign_in_result(sign_in_status::success));
        }
    });
}

pplx::task<xbox_live_result<void>>
user_auth_ios::sign_out_internal()
{
    auto tce = pplx::task_completion_event<xbox_live_result<void>>();
    m_authManager->auth_token_manager()->clear_token_cache();
    m_authManager->set_rps_ticket("");
    delete_offline_data();
    
    [m_msaSignInHandler signOutWithCompletion:^() {
        m_isSignedIn = false;
        
        m_gamertag = _T("");
        m_xboxUserId = _T("");
        m_ageGroup = _T("");
        m_privileges = _T("");
        m_webAccountId = _T("");
        
        tce.set(xbox_live_result<void>());
    }];
    
    return create_task(tce);
}

pplx::task<xbox_live_result<void>>
user_auth_ios::signout()
{
    std::weak_ptr<user_auth_ios> thisWeakPtr = std::dynamic_pointer_cast<user_auth_ios>(shared_from_this());
    [[XLSCll sharedTelemetryManager] pageActionEvent:IDP_PageAction_Signout_Signout withData:nil onPage:@"NA"];
    xbox::services::notification::notification_service::get_notification_service_singleton()->unsubscribe_from_notifications();
    return sign_out_internal()
    .then([thisWeakPtr] (xbox_live_result<void> result) {
        auto pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->m_authManager->auth_token_manager()->clear_token_cache();
            pThis->m_userSignedOutCallback();
        }
        
        return result;
    });
}

#pragma mark - Initialization

user_auth_ios::user_auth_ios() :
m_msaSignInHandler(nil),
m_xboxSignInUIHandler(nil),
m_signInTaskInitialized(false)
{
    m_authManager = auth_manager::get_auth_manager_instance();
    m_authConfig = auth_manager::get_auth_manager_instance()->get_auth_config();
    m_authConfig->set_xtoken_composition({ token_identity_type::u_token, token_identity_type::d_token, token_identity_type::t_token });
    m_localConfig = xbox_system_factory::get_factory()->create_local_config();
    m_msaSignInHandler = [[XBLMSASignInDelagate alloc] init];
}

XBLMSASignInDelagate *
user_auth_ios::get_sign_in_delegate()
{
    return m_msaSignInHandler;
}

pplx::task<xbox_live_result<token_and_signature_result>>
user_auth_ios::internal_get_token_and_signature(
                                                _In_ const string_t& httpMethod,
                                                _In_ const string_t& url,
                                                _In_ const string_t& endpointForNsal,
                                                _In_ const string_t& headers,
                                                _In_ const std::vector<unsigned char>& bytes,
                                                _In_ bool promptForCredentialsIfNeeded,
                                                _In_ bool forceRefresh
                                                )
{
    auto task = m_authManager->internal_get_token_and_signature(
                                                                  _T("GET"),
                                                                  url,
                                                                  endpointForNsal,
                                                                  string_t(),
                                                                  std::vector<uint8_t>(),
                                                                  false,
                                                                  forceRefresh
                                                                  );
    
    return utils::create_exception_free_task<token_and_signature_result>(task);
}

void
user_auth_ios::read_offline_data()
{
    m_gamertag = m_localConfig->get_value_from_local_storage("gamertag");
    m_xboxUserId = m_localConfig->get_value_from_local_storage("xuid");
    if (!m_gamertag.empty() && !m_xboxUserId.empty())
    {
        m_isSignedIn = true;
    }
}

void
user_auth_ios::write_offline_data()
{
    m_localConfig->write_value_to_local_storage("gamertag", m_gamertag);
    m_localConfig->write_value_to_local_storage("xuid", m_xboxUserId);
}

void
user_auth_ios::delete_offline_data()
{
    m_localConfig->delete_value_from_local_storage("gamertag");
    m_localConfig->delete_value_from_local_storage("xuid");
    m_localConfig->delete_value_from_local_storage("endpointId");
    // Synchronize cached values with local storage
    m_gamertag.clear();
    m_xboxUserId.clear();
    m_isSignedIn = false;
}

const string_t& user_auth_ios::xbox_user_id() const
{
    return m_xboxUserId;
}

const string_t& user_auth_ios::gamertag() const
{
    return m_gamertag;
}

const string_t& user_auth_ios::age_group() const
{
    return m_ageGroup;
}

const string_t& user_auth_ios::privileges() const
{
    return m_privileges;
}

bool user_auth_ios::is_signed_in() const
{
    return m_isSignedIn;
}

void user_auth_ios::set_signed_out_callback(_In_ const std::function<void()>& signedOutCallback)
{
    m_userSignedOutCallback = signedOutCallback;
}

std::shared_ptr<user_auth_ios> user_auth_ios::get_instance()
{
    std::lock_guard<std::mutex> guard(s_userAuthSingletonLock.get());
    if(!s_userAuthSingletonInstance)
    {
        s_userAuthSingletonInstance = std::shared_ptr<user_auth_ios>(new user_auth_ios());
    }
    return s_userAuthSingletonInstance;
}

void user_auth_ios::set_title_telemetry_session_id(_In_ const string_t& sessionId)
{
    m_titleTelemetrySessionId = sessionId;
}

const string_t& user_auth_ios::title_telemetry_session_id()
{
    return m_titleTelemetrySessionId;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

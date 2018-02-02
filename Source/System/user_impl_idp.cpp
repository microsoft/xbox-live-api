// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"

#if UWP_API
#include "user_impl.h"
#include "Utils_WinRT.h"
#if !XSAPI_CPP
#include "Macros_WinRT.h"
#include "XboxLiveContextSettings_WinRT.h"
#endif
#include "Event_WinRT.h"
#include "xsapi\presence.h"
#include "presence_internal.h"

using namespace pplx;
using namespace Platform;
using namespace Windows::Security::Authentication::Web::Core;
using namespace Windows::Security::Credentials;
using namespace Windows::System::Threading;
using namespace xbox::services;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::presence;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

pplx::task<xbox_live_result<sign_in_result>>
user_impl_idp::sign_in_impl(_In_ bool showUI, _In_ bool forceRefresh)
{
    UNREFERENCED_PARAMETER(forceRefresh);

    std::weak_ptr<user_impl_idp> thisWeakPtr = std::dynamic_pointer_cast<user_impl_idp>(shared_from_this());
    
    //Initiate user watcher
    if (is_multi_user_application())
    {
        auto xsapiSingleton = get_xsapi_singleton();
        std::lock_guard<std::mutex> lock(xsapiSingleton->m_trackingUsersLock);

        if (xsapiSingleton->m_userWatcher == nullptr)
        {
            xsapiSingleton->m_userWatcher = Windows::System::User::CreateWatcher();
            xsapiSingleton->m_userWatcher->Removed += ref new Windows::Foundation::TypedEventHandler<Windows::System::UserWatcher ^, Windows::System::UserChangedEventArgs ^>(&on_system_user_removed);
        }
    }

    auto task = initialize_provider()
    .then([thisWeakPtr, showUI](void)
    {
        std::shared_ptr<user_impl_idp> pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            return xbox_live_result<sign_in_result>(xbox_live_error_code::runtime_error, "user_impl shutting down");
        }

        xbox_live_result<token_and_signature_result> result =  pThis->internal_get_token_and_signature_helper(
            _T("GET"),
            pThis->m_authConfig->xbox_live_endpoint(),
            string_t(),
            std::vector<unsigned char>(),
            showUI,
            false
            );

        if (result.err())
        {
            return xbox_live_result<sign_in_result>(result.err(), result.err_message());
        }
        else
        {
            const auto& payload = result.payload();

            if (!payload.xbox_user_id().empty())
            {
                //Call MSA IDP once getting first xtoken succeeded to show partner token consent.
                auto localConfig = xbox_system_factory::get_factory()->create_local_config();
                if (!localConfig->msa_sub_target().empty())
                {
                    WebTokenRequest^ msaRequest;
                    WebAccount^ account = create_task(WebAuthenticationCoreManager::FindAccountProviderAsync("https://login.microsoft.com", "consumers"))
                    .then([&msaRequest, payload, localConfig](WebAccountProvider^ provider)
                    {
                        msaRequest = ref new WebTokenRequest(provider, "service::XboxLivePartner.Signin::DELEGATION");
                        msaRequest->Properties->Insert("subpolicy", "JWT");
                        msaRequest->Properties->Insert("subresource", ref new Platform::String(localConfig->msa_sub_target().c_str()));

                        return WebAuthenticationCoreManager::FindAccountAsync(provider, ref new Platform::String(payload.web_account_id().c_str()));
                    }).get();

                    LOG_DEBUG("webaccount:");
                    LOGS_DEBUG << account->Id->Data();
                    LOGS_DEBUG << account->UserName->Data();
                    for (auto property : account->Properties)
                    {
                        LOGS_DEBUG << property->Key->Data() << " : " << property->Value->Data();
                    }

                    WebTokenRequestResult^ patnerTokenResult = request_token_from_idp(
                        xbox_live_context_settings::_s_dispatcher,
                        showUI,
                        msaRequest,
                        account);

                    if (patnerTokenResult != nullptr && patnerTokenResult->ResponseStatus == WebTokenRequestStatus::ProviderError)
                    {
                        std::string providerErrorMsg = utility::conversions::to_utf8string(patnerTokenResult->ResponseError->ErrorMessage->Data());
                        std::stringstream msg;
                        msg << " MSA Provider error: " << providerErrorMsg << ", Error Code: 0x" << std::hex << patnerTokenResult->ResponseError->ErrorCode;

                        std::error_code error = xbox_live_error_code(patnerTokenResult->ResponseError->ErrorCode);
                        return xbox_live_result<sign_in_result>(error, msg.str());
                    }
                    else if (patnerTokenResult == nullptr || patnerTokenResult->ResponseStatus != WebTokenRequestStatus::Success) //other error 
                    {
                        return xbox_live_result<sign_in_result>(convert_web_token_request_status(patnerTokenResult));
                    }
                }

                // Hit presence service to validate the token.
                if (!payload.token().empty())
                {
                    const string_t& xboxUserId = payload.xbox_user_id();
                    string_t pathAndQuery = presence_service_impl::set_presence_sub_path(xboxUserId);

                    std::shared_ptr<http_call> httpCall = xbox_system_factory::get_factory()->create_http_call(
                        std::make_shared<xbox_live_context_settings>(),
                        _T("POST"),
                        utils::create_xboxlive_endpoint(_T("userpresence"), xbox_live_app_config::get_app_config_singleton()),
                        pathAndQuery,
                        xbox_live_api::set_presence_helper
                    );

                    presence_title_request request(true,presence_data());

                    httpCall->set_retry_allowed(false);
                    httpCall->set_request_body(request.serialize().serialize());
                    httpCall->set_xbox_contract_version_header_value(_T("3"));
                    httpCall->set_custom_header(L"Authorization", payload.token());
                    if (!payload.signature().empty())
                    {
                        httpCall->set_custom_header(L"Signature", payload.signature());
                    }

                    auto response = httpCall->get_response(http_call_response_body_type::json_body).get();

                    // If gettig 401, try to refresh token. If we succeeded or failed or any other reason, ignore the result and move on.
                    if (response->err_code().value() == (int)xbox_live_error_code::http_status_401_unauthorized)
                    {
                        auto refreshResult = pThis->internal_get_token_and_signature_helper(
                            _T("GET"),
                            pThis->m_authConfig->xbox_live_endpoint(),
                            string_t(),
                            std::vector<unsigned char>(),
                            showUI,
                            true
                        );

                        // if refresh fails, return the error.
                        if (refreshResult.err())
                        {
                            //if it's silent pass, give user interaction required.
                            if (!showUI)
                            {
                                return xbox_live_result<sign_in_result>(sign_in_status::user_interaction_required);
                            }
                            else
                            {
                                return xbox_live_result<sign_in_result>(refreshResult.err(), refreshResult.err_message());
                            }
                        }
                        else if (refreshResult.payload().xbox_user_id().empty())
                        {
                            return xbox_live_result<sign_in_result>(convert_web_token_request_status(refreshResult.payload().token_request_result()));
                        }
                    }
                }

                pThis->user_signed_in(payload.xbox_user_id(), payload.gamertag(), payload.age_group(), payload.privileges(), payload.web_account_id());

                return xbox_live_result<sign_in_result>(sign_in_status::success);
            }
            else
            {
                return xbox_live_result<sign_in_result>(convert_web_token_request_status(payload.token_request_result()));
            }
        }
    }, pplx::task_continuation_context::use_arbitrary());

    return utils::create_exception_free_task<sign_in_result>(task);
}


pplx::task<void>
user_impl_idp::initialize_provider()
{
    std::weak_ptr<user_impl_idp> thisWeakPtr = std::dynamic_pointer_cast<user_impl_idp>(shared_from_this());

    // If already found a provider, return.
    if (m_provider != nullptr)
    {
        return pplx::task_from_result();
    }

    // First time initialization. 
    if (m_creationContext == nullptr && is_multi_user_application())
    {
        std::string errorMsg = "Xbox Live User object is required to be constructed by a Windows::System::User object in the Multi-User environment.";
        LOG_ERROR(errorMsg);
        return task_from_exception<void>(std::runtime_error(errorMsg));
    }

    pplx::task<WebAccountProvider^> findProviderTask;
    if (m_creationContext == nullptr || !is_multi_user_application())
    {
        findProviderTask = create_task(WebAuthenticationCoreManager::FindAccountProviderAsync("https://xsts.auth.xboxlive.com"));
    }
    else
    {
        findProviderTask = create_task(WebAuthenticationCoreManager::FindAccountProviderAsync("https://xsts.auth.xboxlive.com", "", m_creationContext));
    }

    return findProviderTask.then([thisWeakPtr](WebAccountProvider^ provider)
    {
        std::shared_ptr<user_impl_idp> pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            throw std::runtime_error("user_impl shutting down");
        }

        if (provider == nullptr)
        {
            throw std::runtime_error("Xbox Live identity provider is not found");
        }

        pThis->m_provider = provider;
    }, pplx::task_continuation_context::use_arbitrary());
}

user_impl_idp::user_impl_idp(Windows::System::User^ systemUser) :
    user_impl(true, systemUser)
{
}

pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
user_impl_idp::internal_get_token_and_signature(
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

    std::weak_ptr<user_impl_idp> thisWeakPtr = std::dynamic_pointer_cast<user_impl_idp>(shared_from_this());
    auto task = create_task([thisWeakPtr, httpMethod, url, headers, bytes, promptForCredentialsIfNeeded, forceRefresh]()
    {
        std::shared_ptr<user_impl_idp> pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            return xbox_live_result<token_and_signature_result>(xbox_live_error_code::runtime_error, "user_impl shutting down");
        }

        auto result = pThis->internal_get_token_and_signature_helper(
            httpMethod,
            url,
            headers,
            bytes,
            promptForCredentialsIfNeeded,
            forceRefresh
            );

        // Handle UserInteractionRequired
        if (result.payload().token_request_result() != nullptr
            && result.payload().token_request_result()->ResponseStatus == WebTokenRequestStatus::UserInteractionRequired)
        {
            // Failed to get 'xboxlive.com' token, sign out if already sign in (SPOP or user banned).
            // But for sign in path, it's expected.
            if (url == pThis->get_auth_config()->xbox_live_endpoint() && pThis->is_signed_in())
            {
                pThis->user_signed_out();
            }
            else if (url != pThis->get_auth_config()->xbox_live_endpoint()) // If it's not asking for xboxlive.com's token, we treat UserInteractionRequired as an error
            {
                std::string errorMsg = "Failed to get token for endpoint: " + utility::conversions::to_utf8string(url);
                return xbox_live_result<token_and_signature_result>(xbox_live_error_code::runtime_error, errorMsg);
            }
        }

        return result;
    });

    return utils::create_exception_free_task<token_and_signature_result>(
        task
        );
}

void user_impl_idp::internal_get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& endpointForNsal,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh,
    _In_ uint64_t taskGroupId,
    _In_ xbox_live_callback<xbox::services::xbox_live_result<token_and_signature_result>> callback
    )
{
    UNREFERENCED_PARAMETER(endpointForNsal);

    auto context = utils::store_shared_ptr(xsapi_allocate_shared<get_token_and_signature_context>(
        shared_from_this(),
        httpMethod,
        url,
        headers,
        bytes,
        promptForCredentialsIfNeeded,
        forceRefresh,
        callback
        ));

    HCTaskCreate(HC_SUBSYSTEM_ID::HC_SUBSYSTEM_ID_XSAPI, taskGroupId,
        [](_In_opt_ void *_context, _In_ HC_TASK_HANDLE taskHandle)
        {
            auto context = utils::remove_shared_ptr<get_token_and_signature_context>(_context, false);
            auto pThis = std::dynamic_pointer_cast<user_impl_idp>(context->userImpl);

            context->result = pThis->internal_get_token_and_signature_helper(
                context->httpMethod,
                context->url,
                context->headers,
                context->bytes,
                context->promptForCredentialsIfNeeded,
                context->forceRefresh
                );

            // Handle UserInteractionRequired
            if (context->result.payload().token_request_result() != nullptr
                && context->result.payload().token_request_result()->ResponseStatus == WebTokenRequestStatus::UserInteractionRequired)
            {
                // Failed to get 'xboxlive.com' token, sign out if already sign in (SPOP or user banned).
                // But for sign in path, it's expected.
                if (context->url == pThis->get_auth_config()->xbox_live_endpoint() && pThis->is_signed_in())
                {
                    pThis->user_signed_out();
                }
                else if (context->url != pThis->get_auth_config()->xbox_live_endpoint()) // If it's not asking for xboxlive.com's token, we treat UserInteractionRequired as an error
                {
                    std::string errorMsg = "Failed to get token for endpoint: " + utility::conversions::to_utf8string(context->url);
                    context->result = xbox_live_result<token_and_signature_result>(xbox_live_error_code::runtime_error, errorMsg);
                }
            }
            return HCTaskSetCompleted(taskHandle);
        },
        context,
        [](_In_opt_ void *_context, _In_ HC_TASK_HANDLE taskHandle, void *completionRoutine, void *completionRoutineContext)
        {
            UNREFERENCED_PARAMETER(taskHandle);
            UNREFERENCED_PARAMETER(completionRoutine);
            UNREFERENCED_PARAMETER(completionRoutineContext);
            auto context = utils::remove_shared_ptr<get_token_and_signature_context>(_context);
            context->callback(context->result);
            return HC_OK;
        },
        context,
        nullptr,
        nullptr,
        nullptr
        );
}

xbox::services::xbox_live_result<token_and_signature_result>
user_impl_idp::internal_get_token_and_signature_helper(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const std::vector<byte>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh
    )
{
    if (m_provider == nullptr)
    {
        return xbox_live_result<token_and_signature_result>(xbox_live_error_code::runtime_error, "Xbox Live identity provider is not initialized");
    }

    auto request = ref new WebTokenRequest(m_provider);
    request->Properties->Insert("HttpMethod", PLATFORM_STRING_FROM_STRING_T(httpMethod));
    request->Properties->Insert("Url", PLATFORM_STRING_FROM_STRING_T(url));
    request->Properties->Insert("RequestHeaders", PLATFORM_STRING_FROM_STRING_T(headers));
    if (forceRefresh)
    {
        request->Properties->Insert("ForceRefresh", _T("true"));
    }
    if (bytes.size() > 0)
    {
        auto requestBody = bytes;
        request->Properties->Insert("RequestBody", PLATFORM_STRING_FROM_STRING_T(utility::conversions::to_base64(requestBody)));
    }

    request->Properties->Insert("Target", PLATFORM_STRING_FROM_STRING_T(m_authConfig->rps_ticket_service()));
    request->Properties->Insert("Policy", PLATFORM_STRING_FROM_STRING_T(m_authConfig->rps_ticket_policy()));

    if (promptForCredentialsIfNeeded)
    {
        // Sign in UI settings
        String^ pfn = Windows::ApplicationModel::Package::Current->Id->FamilyName;
        request->Properties->Insert("PackageFamilyName", pfn);
    }

    WebTokenRequestResult^ tokenResult = request_token_from_idp(
        xbox_live_context_settings::_s_dispatcher,
        promptForCredentialsIfNeeded,
        request,
        nullptr
        );

    xbox_live_result<token_and_signature_result> result = convert_web_token_request_result(tokenResult);

    return result;
}

sign_in_result
user_impl_idp::convert_web_token_request_status(
    _In_ WebTokenRequestResult^ tokenResult
    )
{
    if (tokenResult->ResponseStatus == WebTokenRequestStatus::UserCancel)
    {
        return sign_in_result(sign_in_status::user_cancel);
    }
    else
    {
        return sign_in_result(sign_in_status::user_interaction_required);
    }
}

xbox_live_result<token_and_signature_result> 
user_impl_idp::convert_web_token_request_result(
    _In_ WebTokenRequestResult^ tokenResult
    )
{
    if (tokenResult->ResponseStatus == WebTokenRequestStatus::Success)
    {
        if (tokenResult->ResponseData == nullptr || tokenResult->ResponseData->Size == 0)
        {
            return xbox_live_result<token_and_signature_result>(xbox_live_error_code::runtime_error, "invalid idp token response");
        }

        auto response = tokenResult->ResponseData->GetAt(0);

        LOG_DEBUG("Token result properties:");
        // print out the whole property bag returned from idp
        for (auto property : response->Properties)
        {
            LOGS_DEBUG << property->Key->Data() << " : " << property->Value->Data();
        }

        // Get the guaranteed properties
        string_t xuid = response->Properties->Lookup("XboxUserId")->Data();
        string_t gamertag = response->Properties->Lookup("Gamertag")->Data();
        string_t ageGroup = response->Properties->Lookup("AgeGroup")->Data();
        string_t environment = response->Properties->Lookup("Environment")->Data();
        string_t sandbox = response->Properties->Lookup("Sandbox")->Data();
        string_t webAccountId = response->WebAccount->Id->Data();
        string_t token = response->Token->Data();

        // Check optional properties, which doesn't exist if offline.
        string_t signature;
        if (response->Properties->HasKey("Signature"))
        {
            signature = response->Properties->Lookup("Signature")->Data();
        }

        string_t privilege;
        if (response->Properties->HasKey("Privileges"))
        {
            privilege = response->Properties->Lookup("Privileges")->Data();
        }

        // if it's prod, make it empty.
        if (utils::str_icmp(environment, L"prod") == 0)
        {
            environment.clear();
        }
        else if (environment[0] != _T('.'))
        {
            environment = _T(".") + environment;
        }

        auto app_config = xbox_live_app_config::get_app_config_singleton();
        app_config->set_environment(environment);
        app_config->set_sandbox(sandbox);

        return xbox_live_result<token_and_signature_result>(
            token_and_signature_result(
            std::move(token),
            std::move(signature),
            std::move(xuid),
            std::move(gamertag),
            string_t(),
            std::move(ageGroup),
            std::move(privilege),
            std::move(webAccountId),
            string_t()));
    }
    else if (tokenResult->ResponseStatus == WebTokenRequestStatus::AccountSwitch)
    {
        user_signed_out();
        return xbox_live_result<token_and_signature_result>(xbox_live_error_code::auth_user_switched, "User has switched");
    }
    else if (tokenResult->ResponseStatus == WebTokenRequestStatus::ProviderError)
    {
        std::string providerErrorMsg = utility::conversions::to_utf8string(tokenResult->ResponseError->ErrorMessage->Data());
        std::stringstream msg;
        msg << "Provider error: " << providerErrorMsg << ", Error Code: 0x" << std::hex << tokenResult->ResponseError->ErrorCode;

        std::error_code error = xbox_live_error_code(tokenResult->ResponseError->ErrorCode);
        return xbox_live_result<token_and_signature_result>(error, msg.str());
    }
    else
    {
        LOGS_ERROR << "Get token from IDP failed with ResponseStatus:" << (int)tokenResult->ResponseStatus;

        return xbox_live_result<token_and_signature_result>(tokenResult);
    }

}

WebTokenRequestResult^
user_impl_idp::request_token_from_idp(
    _In_opt_ Windows::UI::Core::CoreDispatcher^ coreDispatcher,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ WebTokenRequest^ request,
    _In_ WebAccount^ webAccount
    )
{
    WebTokenRequestResult^ tokenResult;
    if (coreDispatcher != nullptr && promptForCredentialsIfNeeded)
    {
        Event^ completeEvent = ref new Event();
        Platform::Exception^ retException = nullptr;

        coreDispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Normal,
            ref new Windows::UI::Core::DispatchedHandler([&tokenResult, &retException, request, completeEvent, webAccount]()
        {
            try
            {
                task<WebTokenRequestResult^> tokenTask = 
                    webAccount == nullptr ?
                    create_task(WebAuthenticationCoreManager::RequestTokenAsync(request)):
                    create_task(WebAuthenticationCoreManager::RequestTokenAsync(request, webAccount));

                tokenTask.then([&tokenResult, &retException, completeEvent](task<WebTokenRequestResult^> t)
                {
                    try
                    {
                        tokenResult = t.get();
                        completeEvent->Set();
                    }
                    catch (Exception^ ex)
                    {
                        retException = ex;
                        completeEvent->Set();
                    }
                });

            }
            catch (Exception^ ex)
            {
                retException = ex;
                completeEvent->Set();
            }
        }));

        completeEvent->Wait();
        if (retException != nullptr)
        {
            throw retException;
        }
    }
    else
    {
        if (promptForCredentialsIfNeeded)
        {
            tokenResult = webAccount == nullptr? 
                create_task(WebAuthenticationCoreManager::RequestTokenAsync(request)).get() :
                create_task(WebAuthenticationCoreManager::RequestTokenAsync(request, webAccount)).get();
        }
        else
        {
            tokenResult = webAccount == nullptr ? 
                create_task(WebAuthenticationCoreManager::GetTokenSilentlyAsync(request)).get() :
                create_task(WebAuthenticationCoreManager::GetTokenSilentlyAsync(request, webAccount)).get();
        }
        
    }

    return tokenResult;
}

// Not supported for user_impl_idp
pplx::task<xbox_live_result<void>>
user_impl_idp::sign_in_impl(
    _In_ const string_t& userDelegationTicket,
    _In_ bool forceRefresh
    )
{
    UNREFERENCED_PARAMETER(userDelegationTicket);
    UNREFERENCED_PARAMETER(forceRefresh);

    throw std::exception("not supported.");
}

void
user_impl_idp::user_signed_in(
    _In_ string_t xboxUserId,
    _In_ string_t gamertag,
    _In_ string_t ageGroup,
    _In_ string_t privileges,
    _In_ string_t webAccountId
    )
{
    user_impl::user_signed_in(xboxUserId, gamertag, ageGroup, privileges, webAccountId);

    // We use user watcher in MUA
    if (!is_multi_user_application())
    {
        std::weak_ptr<user_impl_idp> thisWeakPtr = std::dynamic_pointer_cast<user_impl_idp>(shared_from_this());
        //Start periodic timer for user signed out check
        Windows::Foundation::TimeSpan delay = UtilsWinRT::ConvertSecondsToTimeSpan(std::chrono::seconds(10));
        m_timer = ThreadPoolTimer::CreatePeriodicTimer(
            ref new TimerElapsedHandler([thisWeakPtr](ThreadPoolTimer^ source)
            {
                std::shared_ptr<user_impl_idp> pThis(thisWeakPtr.lock());
                if (pThis != nullptr)
                {
                    pThis->check_user_signed_out();
                }
            }),
            delay
            );
    }
    else
    {
        if (m_creationContext != nullptr)
        {
            auto xsapiSingleton = get_xsapi_singleton();
            std::lock_guard<std::mutex> lock(xsapiSingleton->m_trackingUsersLock);
            xsapiSingleton->m_trackingUsers[m_creationContext->NonRoamableId->Data()] = std::dynamic_pointer_cast<user_impl_idp>(shared_from_this());
        }
    }

}

void user_impl_idp::user_signed_out()
{
    user_impl::user_signed_out();

    if (m_creationContext != nullptr)
    {
        auto xsapiSingleton = get_xsapi_singleton();
        std::lock_guard<std::mutex> lock(xsapiSingleton->m_trackingUsersLock);
        xsapiSingleton->m_trackingUsers.erase(m_creationContext->NonRoamableId->Data());
    }

    if (m_timer != nullptr)
    {
        m_timer->Cancel();
        m_timer = nullptr;
    }
}

void user_impl_idp::check_user_signed_out()
{
    try
    {
        if (is_signed_in())
        {
            auto signedInAccount = pplx::create_task(WebAuthenticationCoreManager::FindAccountAsync(m_provider, ref new Platform::String(m_webAccountId.c_str()))).get();

            if (signedInAccount == nullptr)
            {
                user_signed_out();
            }
        }
    }
    catch (...) // If any exception happens during finding account, sign user out as well.
    {
        user_signed_out();
    }
}

void user_impl_idp::on_system_user_removed(Windows::System::UserWatcher ^sender, Windows::System::UserChangedEventArgs ^args)
{
    std::shared_ptr<user_impl_idp> signOutUser;
    {
        auto xsapiSingleton = get_xsapi_singleton();
        std::lock_guard<std::mutex> lock(xsapiSingleton->m_trackingUsersLock);
        auto user = xsapiSingleton->m_trackingUsers.find(args->User->NonRoamableId->Data());
        if (user != xsapiSingleton->m_trackingUsers.end())
        {
            signOutUser = user->second;
        }
    }

    if (signOutUser != nullptr)
    {
        signOutUser->user_signed_out();
    }
}

bool user_impl_idp::is_multi_user_application()
{
    // Save the result in memory, as we only need to check once;
    static int isSupported = -1;

    // Only RS1 sdk will have this check.
#ifdef NTDDI_WIN10_RS1
    if (isSupported == -1)
    {
        try
        {
            bool APIExist = Windows::Foundation::Metadata::ApiInformation::IsMethodPresent("Windows.System.UserPicker", "IsSupported");
            isSupported = (APIExist && Windows::System::UserPicker::IsSupported()) ? 1 : 0;
        }
        catch (...)
        {
            isSupported = 0;
        }
    }
#endif
    return isSupported == 1;
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

#endif



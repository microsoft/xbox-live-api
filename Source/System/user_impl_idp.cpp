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
#include "xbox_live_app_config_internal.h"

using namespace Platform;
using namespace Windows::Security::Authentication::Web::Core;
using namespace Windows::Security::Credentials;
using namespace Windows::System::Threading;
using namespace Windows::Foundation;
using namespace xbox::services;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::presence;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

void user_impl_idp::sign_in_impl(
    _In_ bool showUI, 
    _In_ bool forceRefresh,
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<sign_in_result>> callback
    )
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

    initialize_provider(
        queue,
        [thisWeakPtr, showUI, queue, callback](void)
    {
        std::shared_ptr<user_impl_idp> pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            callback(xbox_live_result<sign_in_result>(xbox_live_error_code::runtime_error, "user_impl shutting down"));
            return;
        }

        auto result =  pThis->internal_get_token_and_signature_helper(
            "GET",
            pThis->m_authConfig->xbox_live_endpoint(),
            xsapi_internal_string(),
            xsapi_internal_vector<unsigned char>(),
            showUI,
            false
            );

        if (result.err())
        {
            callback(xbox_live_result<sign_in_result>(result.err(), result.err_message()));
        }
        else
        {
            const auto& payload = *result.payload();

            if (!payload.xbox_user_id().empty())
            {
                //Call MSA IDP once getting first xtoken succeeded to show partner token consent.
                auto localConfig = xbox_system_factory::get_factory()->create_local_config();
                if (!localConfig->msa_sub_target().empty())
                {
                    Event^ completeEvent = ref new Event();
                    WebTokenRequest^ msaRequest;
                    WebAccount^ account = nullptr;

                    auto findProviderAsyncOp = WebAuthenticationCoreManager::FindAccountProviderAsync("https://login.microsoft.com", "consumers");
                    findProviderAsyncOp->Completed = ref new AsyncOperationCompletedHandler<WebAccountProvider^>(
                    [&msaRequest, payload, localConfig, showUI, completeEvent, &account](IAsyncOperation<WebAccountProvider^>^ asyncOp, Windows::Foundation::AsyncStatus status)
                    {
                        UNREFERENCED_PARAMETER(status);
                        auto provider = asyncOp->GetResults();

                        msaRequest = ref new WebTokenRequest(provider, "service::XboxLivePartner.Signin::DELEGATION");
                        msaRequest->Properties->Insert("subpolicy", "JWT");
                        msaRequest->Properties->Insert("subresource", ref new Platform::String(utils::string_t_from_internal_string(localConfig->msa_sub_target()).c_str()));

                        auto findAccountAsyncOp = WebAuthenticationCoreManager::FindAccountAsync(provider, PLATFORM_STRING_FROM_INTERNAL_STRING(payload.web_account_id()));
                        findAccountAsyncOp->Completed = ref new AsyncOperationCompletedHandler<WebAccount^>(
                            [msaRequest, showUI, completeEvent, &account](IAsyncOperation<WebAccount^>^ asyncOp, Windows::Foundation::AsyncStatus status)
                        {
                            UNREFERENCED_PARAMETER(status);
                            account = asyncOp->GetResults();
                            completeEvent->Set();
                        });
                    });

                    completeEvent->Wait();

                    LOG_DEBUG("webaccount:");
                    LOGS_DEBUG << account->Id->Data();
                    LOGS_DEBUG << account->UserName->Data();
                    for (auto property : account->Properties)
                    {
                        LOGS_DEBUG << property->Key->Data() << " : " << property->Value->Data();
                    }

                    WebTokenRequestResult^ partnerTokenResult = request_token_from_idp(
                        xbox_live_context_settings::_s_dispatcher,
                        showUI,
                        msaRequest,
                        account);

                    if (partnerTokenResult != nullptr && partnerTokenResult->ResponseStatus == WebTokenRequestStatus::ProviderError)
                    {
                        std::string providerErrorMsg = utility::conversions::to_utf8string(partnerTokenResult->ResponseError->ErrorMessage->Data());
                        std::stringstream msg;
                        msg << " MSA Provider error: " << providerErrorMsg << ", Error Code: 0x" << std::hex << partnerTokenResult->ResponseError->ErrorCode;

                        std::error_code error = xbox_live_error_code(partnerTokenResult->ResponseError->ErrorCode);
                        callback(xbox_live_result<sign_in_result>(error, msg.str()));
                        return;
                    }
                    else if (partnerTokenResult == nullptr || partnerTokenResult->ResponseStatus != WebTokenRequestStatus::Success) //other error 
                    {
                        callback(xbox_live_result<sign_in_result>(convert_web_token_request_status(partnerTokenResult)));
                        return;
                    }
                }

                // Hit presence service to validate the token.
                if (!payload.token().empty())
                {
                    auto xboxUserId = payload.xbox_user_id();
                    auto pathAndQuery = presence_service_internal::set_presence_sub_path(xboxUserId);

                    std::shared_ptr<http_call_internal> httpCall = xbox_system_factory::get_factory()->create_http_call(
                        std::make_shared<xbox_live_context_settings>(),
                        "POST",
                        utils::create_xboxlive_endpoint("userpresence", xbox_live_app_config_internal::get_app_config_singleton()),
                        utils::string_t_from_internal_string(pathAndQuery),
                        xbox_live_api::set_presence_helper
                    );

                    presence_title_request request(true,presence_data());

                    httpCall->set_retry_allowed(false);
                    httpCall->set_request_body(utils::internal_string_from_string_t(request.serialize().serialize()));
                    httpCall->set_xbox_contract_version_header_value(_T("3"));
                    httpCall->set_custom_header("Authorization", payload.token(), false);
                    if (!payload.signature().empty())
                    {
                        httpCall->set_custom_header("Signature", payload.signature(), false);
                    }

                    httpCall->get_response(http_call_response_body_type::json_body, queue,
                        [pThis, showUI, payload, queue, callback](std::shared_ptr<http_call_response_internal> response)
                    {
                        // If gettig 401, try to refresh token. If we succeeded or failed or any other reason, ignore the result and move on.
                        if (response->err_code().value() == (int)xbox_live_error_code::http_status_401_unauthorized)
                        {
                            auto refreshResult = pThis->internal_get_token_and_signature_helper(
                                "GET",
                                pThis->m_authConfig->xbox_live_endpoint(),
                                xsapi_internal_string(),
                                xsapi_internal_vector<unsigned char>(),
                                showUI,
                                true
                            );

                            // if refresh fails, return the error.
                            if (refreshResult.err())
                            {
                                //if it's silent pass, give user interaction required.
                                if (!showUI)
                                {
                                    callback( xbox_live_result<sign_in_result>(sign_in_status::user_interaction_required));
                                    return;
                                }
                                else
                                {
                                    callback(xbox_live_result<sign_in_result>(refreshResult.err(), refreshResult.err_message()));
                                    return;
                                }
                            }
                            else if (refreshResult.payload()->xbox_user_id().empty())
                            {
                                callback(xbox_live_result<sign_in_result>(convert_web_token_request_status(refreshResult.payload()->token_request_result())));
                                return;
                            }
                        }

                        pThis->user_signed_in(payload.xbox_user_id(), payload.gamertag(), payload.age_group(), payload.privileges(), payload.web_account_id());
                        callback(xbox_live_result<sign_in_result>(sign_in_status::success));
                    });
                }
                else
                {
                    pThis->user_signed_in(payload.xbox_user_id(), payload.gamertag(), payload.age_group(), payload.privileges(), payload.web_account_id());

                    callback(xbox_live_result<sign_in_result>(sign_in_status::success));
                }
            }
            else
            {
                callback(xbox_live_result<sign_in_result>(convert_web_token_request_status(payload.token_request_result())));
            }
        }
    });
}


void user_impl_idp::initialize_provider(
    _In_opt_ async_queue_handle_t queue,
    _In_ xbox_live_callback<void> callback
    )
{
    std::weak_ptr<user_impl_idp> thisWeakPtr = std::dynamic_pointer_cast<user_impl_idp>(shared_from_this());

    AsyncBlock* async = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    async->queue = queue;
    async->context = utils::store_shared_ptr(xsapi_allocate_shared<xbox_live_callback<void>>(callback));
    async->callback = [](AsyncBlock* async)
    {
        auto callbackPtr = utils::get_shared_ptr<xbox_live_callback<void>>(async->context);
        (*callbackPtr)();
        xsapi_memory::mem_free(async);
    };

    BeginAsync(async, utils::store_weak_ptr(thisWeakPtr), nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        if (op == AsyncOp_DoWork)
        {
            auto thisPtr = utils::get_shared_ptr<user_impl_idp>(data->context);
            if (thisPtr == nullptr || thisPtr->m_provider != nullptr)
            {
                CompleteAsync(data->async, S_OK, 0);
            }
            else
            {
                // First time initialization. 
                if (thisPtr->m_creationContext == nullptr && is_multi_user_application())
                {
                    std::string errorMsg = "Xbox Live User object is required to be constructed by a Windows::System::User object in the Multi-User environment.";
                    LOG_ERROR(errorMsg);
                    throw std::runtime_error(errorMsg);
                }

                IAsyncOperation<WebAccountProvider^>^ asyncOp;
                if (thisPtr->m_creationContext == nullptr || !is_multi_user_application())
                {
                    asyncOp = WebAuthenticationCoreManager::FindAccountProviderAsync("https://xsts.auth.xboxlive.com");
                }
                else
                {
                    asyncOp = WebAuthenticationCoreManager::FindAccountProviderAsync("https://xsts.auth.xboxlive.com", "", thisPtr->m_creationContext);
                }

                AsyncBlock* async = data->async;
                std::weak_ptr<user_impl_idp> thisWeakPtr(thisPtr);
                asyncOp->Completed = ref new AsyncOperationCompletedHandler<WebAccountProvider^>(
                    [thisWeakPtr, async](IAsyncOperation<WebAccountProvider^>^ asyncInfo, Windows::Foundation::AsyncStatus status)
                {
                    UNREFERENCED_PARAMETER(status);
                    std::shared_ptr<user_impl_idp> pThis(thisWeakPtr.lock());
                    if (pThis == nullptr)
                    {
                        throw std::runtime_error("user_impl shutting down");
                    }

                    auto provider = asyncInfo->GetResults();
                    if (provider == nullptr)
                    {
                        throw std::runtime_error("Xbox Live identity provider is not found");
                    }

                    pThis->m_provider = provider;
                    CompleteAsync(async, S_OK, 0);
                });
            }
            return E_PENDING;
        }
        else
        {
            return S_OK;
        }
    });
    ScheduleAsync(async, 0);
}

user_impl_idp::user_impl_idp(Windows::System::User^ systemUser) :
    user_impl(true, systemUser)
{
}

struct get_token_and_signature_context
{
    std::shared_ptr<user_impl> userImpl;
    xsapi_internal_string httpMethod;
    xsapi_internal_string url;
    xsapi_internal_string headers;
    xsapi_internal_vector<unsigned char> bytes;
    bool promptForCredentialsIfNeeded;
    bool forceRefresh;
    xbox_live_result<std::shared_ptr<token_and_signature_result_internal>> result;
    token_and_signature_callback callback;
};

void user_impl_idp::internal_get_token_and_signature(
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& url,
    _In_ const xsapi_internal_string& endpointForNsal,
    _In_ const xsapi_internal_string& headers,
    _In_ const xsapi_internal_vector<unsigned char>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh,
    _In_opt_ async_queue_handle_t queue,
    _In_ token_and_signature_callback callback
)
{
    UNREFERENCED_PARAMETER(endpointForNsal);

    auto context = xsapi_allocate_shared<get_token_and_signature_context>();
    context->userImpl = shared_from_this();
    context->httpMethod = httpMethod;
    context->url = url;
    context->headers = headers;
    context->bytes = bytes;
    context->promptForCredentialsIfNeeded = promptForCredentialsIfNeeded;
    context->forceRefresh = forceRefresh;
    context->callback = callback;

    AsyncBlock* internalAsyncBlock = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    internalAsyncBlock->context = utils::store_shared_ptr(context);
    internalAsyncBlock->queue = queue;
    internalAsyncBlock->callback = [](_In_ struct AsyncBlock* asyncBlock)
    {
        auto context = utils::get_shared_ptr<get_token_and_signature_context>(asyncBlock->context, true);
        context->callback(context->result);
        xsapi_memory::mem_free(asyncBlock);
    };

    auto hresult = BeginAsync(internalAsyncBlock, internalAsyncBlock->context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        std::shared_ptr<get_token_and_signature_context> context;
        std::shared_ptr<user_impl_idp> pThis;

        switch (op)
        {
        case AsyncOp_DoWork:
            context = utils::get_shared_ptr<get_token_and_signature_context>(data->context, false);
            pThis = std::dynamic_pointer_cast<user_impl_idp>(context->userImpl);

            context->result = pThis->internal_get_token_and_signature_helper(
                context->httpMethod,
                context->url,
                context->headers,
                context->bytes,
                context->promptForCredentialsIfNeeded,
                context->forceRefresh
            );

            // Handle UserInteractionRequired
            if (context->result.payload()->token_request_result() != nullptr
                && context->result.payload()->token_request_result()->ResponseStatus == WebTokenRequestStatus::UserInteractionRequired)
            {
                // Failed to get 'xboxlive.com' token, sign out if already sign in (SPOP or user banned).
                // But for sign in path, it's expected.
                if (context->url == pThis->get_auth_config()->xbox_live_endpoint() && pThis->is_signed_in())
                {
                    pThis->user_signed_out();
                }
                else if (context->url != pThis->get_auth_config()->xbox_live_endpoint()) // If it's not asking for xboxlive.com's token, we treat UserInteractionRequired as an error
                {
                    std::stringstream ss;
                    ss << "Failed to get token for endpoint: " << context;
                    context->result = xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>(xbox_live_error_code::runtime_error, ss.str());
                }
            }
            CompleteAsync(data->async, S_OK, 0);
            break;

        case AsyncOp_Cleanup:
            // Since the context is also shared by the caller, it is cleaned up in the callback
            return S_OK;
        }
        return S_OK;
    });

    if (SUCCEEDED(hresult))
    {
        ScheduleAsync(internalAsyncBlock, 0);
    }
}

xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>
user_impl_idp::internal_get_token_and_signature_helper(
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& url,
    _In_ const xsapi_internal_string& headers,
    _In_ const xsapi_internal_vector<byte>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh
    )
{
    if (m_provider == nullptr)
    {
        return xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>(xbox_live_error_code::runtime_error, "Xbox Live identity provider is not initialized");
    }

    auto request = ref new WebTokenRequest(m_provider);
    request->Properties->Insert("HttpMethod", PLATFORM_STRING_FROM_INTERNAL_STRING(httpMethod));
    request->Properties->Insert("Url", PLATFORM_STRING_FROM_INTERNAL_STRING(url));
    request->Properties->Insert("RequestHeaders", PLATFORM_STRING_FROM_INTERNAL_STRING(headers));
    if (forceRefresh)
    {
        request->Properties->Insert("ForceRefresh", _T("true"));
    }
    if (bytes.size() > 0)
    {
        std::vector<byte> stdVector(bytes.begin(), bytes.end());
        request->Properties->Insert("RequestBody", PLATFORM_STRING_FROM_STRING_T(utility::conversions::to_base64(stdVector)));
    }

    request->Properties->Insert("Target", PLATFORM_STRING_FROM_INTERNAL_STRING(m_authConfig->rps_ticket_service()));
    request->Properties->Insert("Policy", PLATFORM_STRING_FROM_INTERNAL_STRING(m_authConfig->rps_ticket_policy()));

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

    auto result = convert_web_token_request_result(tokenResult);

    return result;
}

sign_in_result
user_impl_idp::convert_web_token_request_status(
    _In_opt_ WebTokenRequestResult^ tokenResult
    )
{
    if (tokenResult != nullptr && tokenResult->ResponseStatus == WebTokenRequestStatus::UserCancel)
    {
        return sign_in_result(sign_in_status::user_cancel);
    }
    else
    {
        return sign_in_result(sign_in_status::user_interaction_required);
    }
}

xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>
user_impl_idp::convert_web_token_request_result(
    _In_ WebTokenRequestResult^ tokenResult
    )
{
    if (tokenResult->ResponseStatus == WebTokenRequestStatus::Success)
    {
        if (tokenResult->ResponseData == nullptr || tokenResult->ResponseData->Size == 0)
        {
            return xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>(xbox_live_error_code::runtime_error, "invalid idp token response");
        }

        auto response = tokenResult->ResponseData->GetAt(0);

        // Get the guaranteed properties
        xsapi_internal_string xuid = utils::internal_string_from_utf16(response->Properties->Lookup("XboxUserId")->Data());
        xsapi_internal_string gamertag = utils::internal_string_from_utf16(response->Properties->Lookup("Gamertag")->Data());
        xsapi_internal_string ageGroup = utils::internal_string_from_utf16(response->Properties->Lookup("AgeGroup")->Data());
        xsapi_internal_string environment = utils::internal_string_from_utf16(response->Properties->Lookup("Environment")->Data());
        xsapi_internal_string sandbox = utils::internal_string_from_utf16(response->Properties->Lookup("Sandbox")->Data());
        xsapi_internal_string webAccountId = utils::internal_string_from_utf16(response->WebAccount->Id->Data());
        xsapi_internal_string token = utils::internal_string_from_utf16(response->Token->Data());

        // Check optional properties, which doesn't exist if offline.
        xsapi_internal_string signature;
        if (response->Properties->HasKey("Signature"))
        {
            signature = utils::internal_string_from_utf16(response->Properties->Lookup("Signature")->Data());
        }

        xsapi_internal_string privilege;
        if (response->Properties->HasKey("Privileges"))
        {
            privilege = utils::internal_string_from_utf16(response->Properties->Lookup("Privileges")->Data());
        }

        // if it's prod, make it empty.
        if (utils::str_icmp(environment, "prod") == 0)
        {
            environment.clear();
        }
        else if (environment[0] != '.')
        {
            environment = "." + environment;
        }

        auto app_config = xbox_live_app_config_internal::get_app_config_singleton();
        app_config->set_environment(environment);
        app_config->set_sandbox(sandbox);

        return xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>(
            xsapi_allocate_shared<token_and_signature_result_internal>(
            std::move(token),
            std::move(signature),
            std::move(xuid),
            std::move(gamertag),
            xsapi_internal_string(),
            std::move(ageGroup),
            std::move(privilege),
            std::move(webAccountId),
            xsapi_internal_string()));
    }
    else if (tokenResult->ResponseStatus == WebTokenRequestStatus::AccountSwitch)
    {
        user_signed_out();
        return xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>(xbox_live_error_code::auth_user_switched, "User has switched");
    }
    else if (tokenResult->ResponseStatus == WebTokenRequestStatus::ProviderError)
    {
        std::string providerErrorMsg = utility::conversions::to_utf8string(tokenResult->ResponseError->ErrorMessage->Data());
        std::stringstream msg;
        msg << "Provider error: " << providerErrorMsg << ", Error Code: 0x" << std::hex << tokenResult->ResponseError->ErrorCode;

        std::error_code error = xbox_live_error_code(tokenResult->ResponseError->ErrorCode);
        return xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>(error, msg.str());
    }
    else
    {
        LOGS_ERROR << "Get token from IDP failed with ResponseStatus:" << (int)tokenResult->ResponseStatus;

        return xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>(
            xsapi_allocate_shared<token_and_signature_result_internal>(tokenResult)
            );
    }

}

WebTokenRequestResult^
user_impl_idp::request_token_from_idp(
    _In_opt_ Windows::UI::Core::CoreDispatcher^ coreDispatcher,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ WebTokenRequest^ request,
    _In_opt_ WebAccount^ webAccount
    )
{
    WebTokenRequestResult^ tokenResult;
    Event^ completeEvent = ref new Event();
    Platform::Exception^ retException = nullptr;

    auto asyncOpCompletedHandler = ref new AsyncOperationCompletedHandler<WebTokenRequestResult^>(
        [&tokenResult, &retException, completeEvent](IAsyncOperation<WebTokenRequestResult^>^ asyncOp, Windows::Foundation::AsyncStatus status)
    {
        UNREFERENCED_PARAMETER(status);
        try
        {
            tokenResult = asyncOp->GetResults();
            completeEvent->Set();
        }
        catch (Exception^ ex)
        {
            retException = ex;
            completeEvent->Set();
        }
    });

    if (coreDispatcher != nullptr && promptForCredentialsIfNeeded)
    {
        coreDispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Normal,
            ref new Windows::UI::Core::DispatchedHandler([&tokenResult, &retException, request, completeEvent, webAccount, asyncOpCompletedHandler]()
        {
            try
            {
                IAsyncOperation<WebTokenRequestResult^>^ tokenAsyncOp =
                    webAccount == nullptr ?
                    WebAuthenticationCoreManager::RequestTokenAsync(request):
                    WebAuthenticationCoreManager::RequestTokenAsync(request, webAccount);

                tokenAsyncOp->Completed = asyncOpCompletedHandler;
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
        IAsyncOperation<WebTokenRequestResult^>^ tokenAsyncOp;

        if (promptForCredentialsIfNeeded)
        {
            tokenAsyncOp = webAccount == nullptr?
                WebAuthenticationCoreManager::RequestTokenAsync(request) :
                WebAuthenticationCoreManager::RequestTokenAsync(request, webAccount);
        }
        else
        {
            tokenAsyncOp = webAccount == nullptr ?
                WebAuthenticationCoreManager::GetTokenSilentlyAsync(request) :
                WebAuthenticationCoreManager::GetTokenSilentlyAsync(request, webAccount);
        }
        
        tokenAsyncOp->Completed = asyncOpCompletedHandler;
        completeEvent->Wait();
    }

    return tokenResult;
}

void
user_impl_idp::user_signed_in(
    _In_ xsapi_internal_string xboxUserId,
    _In_ xsapi_internal_string gamertag,
    _In_ xsapi_internal_string ageGroup,
    _In_ xsapi_internal_string privileges,
    _In_ xsapi_internal_string webAccountId
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
            auto asyncOp = WebAuthenticationCoreManager::FindAccountAsync(m_provider, PLATFORM_STRING_FROM_INTERNAL_STRING(m_webAccountId));
            auto pThis = std::dynamic_pointer_cast<user_impl_idp>(shared_from_this());

            asyncOp->Completed = ref new AsyncOperationCompletedHandler<WebAccount^>(
                [pThis](IAsyncOperation<WebAccount^>^ asyncOp, Windows::Foundation::AsyncStatus status)
            {
                UNREFERENCED_PARAMETER(status);
                auto signedInAccount = asyncOp->GetResults();
                if (signedInAccount == nullptr)
                {
                    pThis->user_signed_out();
                }
            });
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



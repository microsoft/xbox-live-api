// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_system_factory.h"
#include "user_context.h"
#include "shared_macros.h"

#if defined __cplusplus_winrt
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
#if !XSAPI_CPP
using namespace Microsoft::Xbox::Services::System;
#endif
#endif

using namespace xbox::services::system;

#if !TV_API && !XSAPI_CPP // Non-XDK WinRT

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// XDK's Windows.* user object

user_context::user_context(_In_ XboxLiveUser^ user) :
    m_user(user),
    m_callerContextType(xbox::services::caller_context_type::title)
{
    XSAPI_ASSERT(m_user != nullptr);
    m_xboxUserId = utils::internal_string_from_utf16(m_user->XboxUserId->Data());
}

user_context::user_context(_In_ std::shared_ptr<xbox_live_user> user) :
    m_user(user_context::user_convert(user)),
    m_callerContextType(xbox::services::caller_context_type::title)
{
    XSAPI_ASSERT(m_user != nullptr);
    m_xboxUserId = utils::internal_string_from_utf16(m_user->XboxUserId->Data());
}

XboxLiveUser^ user_context::user() const
{
    return m_user;
}

struct auth_context
{
    XboxLiveUser^ user;
    String^ httpMethod;
    String^ url;
    String^ headers;
    String^ requestBodyString;
    Array<byte>^ byteArray;
    xbox_live_result<user_context_auth_result> result;
    xbox_live_callback<xbox_live_result<user_context_auth_result>> callback;

    auth_context() : requestBodyString(nullptr), byteArray(nullptr) {}
};

void get_auth_result(
    std::shared_ptr<auth_context> context,
    async_queue_handle_t queue
    )
{
    AsyncBlock* async = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    async->queue = queue;
    async->context = utils::store_shared_ptr(context);
    async->callback = [](AsyncBlock* async)
    {
        auto context = utils::get_shared_ptr<auth_context>(async->context);
        context->callback(context->result);
        xsapi_memory::mem_free(async);
    };

    auto hr = BeginAsync(async, async->context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        std::shared_ptr<auth_context> context;
        IAsyncOperation<GetTokenAndSignatureResult^>^ asyncOp = nullptr;

        switch (op)
        {
        case AsyncOp_DoWork:
            context = utils::get_shared_ptr<auth_context>(data->context, false);
            if (context->requestBodyString != nullptr)
            {
                asyncOp = context->user->GetTokenAndSignatureAsync(
                    context->httpMethod,
                    context->url,
                    context->headers,
                    context->requestBodyString
                );
            }
            else if (context->byteArray != nullptr)
            {
                asyncOp = context->user->GetTokenAndSignatureArrayAsync(
                    context->httpMethod,
                    context->url,
                    context->headers,
                    context->byteArray
                );
            }
            else
            {
                asyncOp = context->user->GetTokenAndSignatureAsync(
                    context->httpMethod,
                    context->url,
                    context->headers,
                    ""
                );
            }

            XSAPI_ASSERT(asyncOp != nullptr);
            asyncOp->Completed = ref new AsyncOperationCompletedHandler<GetTokenAndSignatureResult^>(
                [data, context](IAsyncOperation<GetTokenAndSignatureResult^>^ asyncOp, AsyncStatus status)
            {
                UNREFERENCED_PARAMETER(status);
                try
                {
                    auto result = asyncOp->GetResults();
                    user_context_auth_result userContextResult(
                        utils::internal_string_from_utf16(result->Token->Data()), 
                        utils::internal_string_from_utf16(result->Signature->Data())
                        );
                    context->result = xbox_live_result<user_context_auth_result>(userContextResult);
                    CompleteAsync(data->async, S_OK, 0);
                }
                catch (Exception^ ex)
                {
                    xbox_live_error_code err = xbox::services::utils::convert_exception_to_xbox_live_error_code();
                    CompleteAsync(data->async, utils::convert_xbox_live_error_code_to_hresult(err), 0);
                }
            });
            return E_PENDING;
        }
        return S_OK;
    });

    if (SUCCEEDED(hr))
    {
        ScheduleAsync(async, 0);
    }
}

void user_context::get_auth_result(
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& url,
    _In_ const xsapi_internal_string& headers,
    _In_ const xsapi_internal_string& requestBodyString,
    _In_ bool allUsersAuthRequired,
    _In_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<user_context_auth_result>> callback
)
{
    UNREFERENCED_PARAMETER(allUsersAuthRequired);

    auto context = xsapi_allocate_shared<auth_context>();
    context->user = m_user;
    context->httpMethod = UtilsWinRT::StringFromInternalString(httpMethod);
    context->url = UtilsWinRT::StringFromInternalString(url);
    context->headers = UtilsWinRT::StringFromInternalString(headers);
    context->requestBodyString = UtilsWinRT::StringFromInternalString(requestBodyString);
    context->callback = std::move(callback);

    xbox::services::get_auth_result(context, queue);
}

void user_context::get_auth_result(
    _In_ const xsapi_internal_string& httpMethod,
    _In_ const xsapi_internal_string& url,
    _In_ const xsapi_internal_string& headers,
    _In_ const xsapi_internal_vector<unsigned char>& requestBody,
    _In_ bool allUsersAuthRequired,
    _In_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<user_context_auth_result>> callback
    )
{
    UNREFERENCED_PARAMETER(allUsersAuthRequired);

    auto context = xsapi_allocate_shared<auth_context>();
    context->user = m_user;
    context->httpMethod = UtilsWinRT::StringFromInternalString(httpMethod);
    context->url = UtilsWinRT::StringFromInternalString(url);
    context->headers = UtilsWinRT::StringFromInternalString(headers);
    context->byteArray = ref new Array<byte>(static_cast<uint32_t>(requestBody.size()));
    memcpy(&context->byteArray->Data[0], &requestBody[0], requestBody.size());
    context->callback = std::move(callback);

    xbox::services::get_auth_result(context, queue);
}

void user_context::refresh_token(
    _In_ async_queue_handle_t queue,
    _In_ xbox_live_callback<xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>> callback
    )
{
    auto authConfig = m_user->_User_impl()->get_auth_config();

    m_user->_User_impl()->internal_get_token_and_signature(
        "GET",
        authConfig->xbox_live_endpoint(),
        xsapi_internal_string(),
        xsapi_internal_string(),
        xsapi_internal_vector<unsigned char>(),
        false,
        true,
        queue,
        [callback](xbox_live_result<std::shared_ptr<token_and_signature_result_internal>> result)
    {
        callback(xbox_live_result<std::shared_ptr<token_and_signature_result_internal>>(result.err(), result.err_message()));
    });
}

bool user_context::is_signed_in() const
{
    if (!user()) return false;
    return user()->IsSignedIn;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#endif
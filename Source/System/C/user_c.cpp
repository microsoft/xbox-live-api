// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "system_c.h"
#include "user_impl.h"
#include "user_internal_c.h"

using namespace xbox::services;
using namespace xbox::services::system;

#if !XDK_API
STDAPI
XblUserCreateHandleFromSystemUser(
    _In_opt_ Windows::System::User^ creationContext,
    _Out_ xbl_user_handle* user
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF_NULL(user);
    *user = new (xsapi_memory::mem_alloc(sizeof(xbl_xbox_live_user))) xbl_xbox_live_user(creationContext);
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblUserCreateHandle(
    _Out_ xbl_user_handle* user
    ) XBL_NOEXCEPT
try
{
    return XblUserCreateHandleFromSystemUser(nullptr, user);
}
CATCH_RETURN()

STDAPI_(xbl_user_handle)
XblUserDuplicateHandle(
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT
try
{
    if (user == nullptr)
    {
        return nullptr;
    }
    user->refCount++;
    return user;
}
CATCH_RETURN_WITH(nullptr)

STDAPI
XblUserCloseHandle(
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF_NULL(user);

    int refCount = --user->refCount;
    if (refCount <= 0)
    {
        assert(refCount == 0);
        user->~xbl_xbox_live_user();
        xsapi_memory::mem_free(user);
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblUserGetXboxUserId(
    _In_ xbl_user_handle user,
    _Out_ uint64_t* xboxUserId
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(user == nullptr || xboxUserId == nullptr);
    *xboxUserId = utils::internal_string_to_uint64(user->userImpl->xbox_user_id());
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblUserGetGamertag(
    _In_ xbl_user_handle user,
    _In_ size_t gamertagBufferSize,
    _Out_writes_to_(gamertagBufferSize, *written) char* gamertagBuffer,
    _Out_opt_ size_t* written
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(user == nullptr || gamertagBuffer == nullptr);
    size_t requiredSize = user->userImpl->gamertag().size() + 1;
    if (gamertagBufferSize < requiredSize)
    {
        return E_NOT_SUFFICIENT_BUFFER;
    }

    gamertagBuffer[0] = '\0';
    CopyMemory(gamertagBuffer, user->userImpl->gamertag().c_str(), requiredSize);
    if (written != nullptr)
    {
        *written = requiredSize;
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblUserGetAgeGroup(
    _In_ xbl_user_handle user,
    _Out_ XblAgeGroup* ageGroup
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(user == nullptr || ageGroup == nullptr);

    static auto const ageLookup = xsapi_internal_unordered_map<xsapi_internal_string, XblAgeGroup>
    {
        std::make_pair("Child", XblAgeGroup_Child),
        std::make_pair("Teen", XblAgeGroup_Teen),
        std::make_pair("Adult", XblAgeGroup_Adult)
    };

    auto ageIt = ageLookup.find(user->userImpl->age_group());
    if (ageIt == ageLookup.end())
    {
        *ageGroup = XblAgeGroup_Unknown;
    }
    else
    {
        *ageGroup = ageIt->second;
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblUserGetPrivilegesSize(
    _In_ xbl_user_handle user,
    _Out_ size_t* size
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(user == nullptr || size == nullptr);
    *size = user->userImpl->privileges().size() + 1;
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblUserGetPrivileges(
    _In_ xbl_user_handle user,
    _In_ size_t privilegesSize,
    _Out_writes_to_(privilegesSize, *written) char* privileges,
    _Out_opt_ size_t* written
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(user == nullptr || privileges == nullptr);

    size_t requiredSize = user->userImpl->privileges().size() + 1;
    if (privilegesSize < requiredSize)
    {
        return E_NOT_SUFFICIENT_BUFFER;
    }

    privileges[0] = '\0';
    CopyMemory(privileges, user->userImpl->privileges().c_str(), requiredSize);
    if (written != nullptr)
    {
        *written = requiredSize;
    }
    return S_OK;
}
CATCH_RETURN()

STDAPI
XblUserIsSignedIn(
    _In_ xbl_user_handle user,
    _Out_ bool *isSignedIn
    ) XBL_NOEXCEPT
try
{
    RETURN_C_INVALIDARGUMENT_IF(user == nullptr || isSignedIn == nullptr);
    *isSignedIn = user->userImpl->is_signed_in();
    return S_OK;
}
CATCH_RETURN()

struct SignInContext
{
    xbl_user_handle user;
    bool showUi;
    xbox_live_result<sign_in_result> result;
};

HRESULT SignInHelper(
    _Inout_ AsyncBlock* asyncBlock,
    _In_ xbl_user_handle user,
    _In_opt_ Platform::Object^ coreDispatcher,
    _In_ bool showUi
    )
{
    RETURN_C_INVALIDARGUMENT_IF(user == nullptr || asyncBlock == nullptr);

    verify_global_init();

    if (coreDispatcher != nullptr)
    {
        xbox_live_context_settings::_Set_dispatcher(coreDispatcher);
    }

    auto context = xsapi_allocate_shared<SignInContext>();
    context->showUi = showUi;
    context->user = user;

    auto result = BeginAsync(asyncBlock, utils::store_shared_ptr<SignInContext>(context), SignInHelper, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        std::shared_ptr<SignInContext> context;
        XblSignInResult result = {};

        switch (op)
        {
        case AsyncOp_DoWork:
            context = utils::get_shared_ptr<SignInContext>(data->context, false);

            context->user->userImpl->sign_in_impl(context->showUi, false, data->async->queue,
                [data, context](xbox_live_result<sign_in_result> result)
            {
                context->result = result;
                auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                CompleteAsync(data->async, hr, sizeof(XblSignInResult));
            });
            return E_PENDING;

        case AsyncOp_GetResult:
            context = utils::get_shared_ptr<SignInContext>(data->context, false);
            result.status = static_cast<XblSignInStatus>(context->result.payload().status());
            CopyMemory(data->buffer, &result, sizeof(XblSignInResult));
            break;

        case AsyncOp_Cleanup:
            utils::get_shared_ptr<SignInContext>(data->context);
            break;
        }
        return S_OK;
    });

    if (FAILED(result))
    {
        return result;
    }
    return ScheduleAsync(asyncBlock, 0);
}

STDAPI
XblUserSignIn(
    _Inout_ AsyncBlock* async,
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT
try
{
    return SignInHelper(async, user, nullptr, true);
}
CATCH_RETURN()

STDAPI
XblUserSignInSilently(
    _Inout_ AsyncBlock* async,
    _In_ xbl_user_handle user
    ) XBL_NOEXCEPT
try
{
    return SignInHelper(async, user, nullptr, false);
}
CATCH_RETURN()

STDAPI
XblUserSignInWithCoreDispatcher(
    _Inout_ AsyncBlock* async,
    _In_ xbl_user_handle user,
    _In_ Platform::Object^ coreDispatcherObj
    ) XBL_NOEXCEPT
try
{
    return SignInHelper(async, user, coreDispatcherObj, true);
}
CATCH_RETURN()

STDAPI
XblUserSignInSilentlyWithCoreDispatcher(
    _Inout_ AsyncBlock* async,
    _In_ xbl_user_handle user,
    _In_ Platform::Object^ coreDispatcherObj
    ) XBL_NOEXCEPT
try
{
    return SignInHelper(async, user, coreDispatcherObj, false);
}
CATCH_RETURN()

STDAPI
XblUserGetSignInResult(
    _Inout_ AsyncBlock* async,
    _Out_ XblSignInResult* signInResult
    ) XBL_NOEXCEPT
{
    return GetAsyncResult(async, SignInHelper, sizeof(XblSignInResult), signInResult, nullptr);
}

struct GetTokenAndSignatureContext
{
    xbl_user_handle user;
    xsapi_internal_string httpMethod;
    xsapi_internal_string url;
    xsapi_internal_string headers;
    xsapi_internal_string requestBodyString;
    XblGetTokenAndSignatureCallback callback;
    xbox_live_result<std::shared_ptr<token_and_signature_result_internal>> result;
};

STDAPI
XblUserGetTokenAndSignature(
    _Inout_ AsyncBlock* async,
    _In_ xbl_user_handle user,
    _In_z_ const char* httpMethod,
    _In_z_ const char* url,
    _In_z_ const char* headers,
    _In_z_ const char* requestBodyString,
    _In_ XblGetTokenAndSignatureCallback callback
    ) XBL_NOEXCEPT
try
{
    auto context = xsapi_allocate_shared<GetTokenAndSignatureContext>();
    context->user = user;
    context->httpMethod = httpMethod;
    context->url = url;
    context->headers = headers;
    context->requestBodyString = requestBodyString;
    context->callback = callback;

    auto result = BeginAsync(async, utils::store_shared_ptr<GetTokenAndSignatureContext>(context), nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        switch (op)
        {
        case AsyncOp_DoWork:
            std::shared_ptr<GetTokenAndSignatureContext> context = utils::get_shared_ptr<GetTokenAndSignatureContext>(data->context);

            context->user->userImpl->get_token_and_signature(context->httpMethod, 
                context->url, 
                context->headers, 
                context->requestBodyString,
                data->async->queue,
                [data, context](xbox_live_result<std::shared_ptr<token_and_signature_result_internal>> result)
            {
                auto hr = utils::convert_xbox_live_error_code_to_hresult(result.err());
                CompleteAsync(data->async, hr, 0);

                // Only calling the typed callback on success
                if (SUCCEEDED(hr))
                {
                    context->callback(data->async->context,
                        result.payload()->token().data(),
                        result.payload()->signature().data()
                        );
                }
            });
            return E_PENDING;
        }
        return S_OK;
    });

    if (FAILED(result))
    {
        return result;
    }
    return ScheduleAsync(async, 0);
}
CATCH_RETURN()

STDAPI_(function_context)
XblUserAddSignOutCompletedHandler(
    _In_ XblSignOutCompletedHandler handler
    ) XBL_NOEXCEPT
try
{
    verify_global_init();

    return xbox_live_user::add_sign_out_completed_handler(
        [handler](const xbox::services::system::sign_out_completed_event_args& args)
    {
        auto singleton = get_xsapi_singleton();
        std::lock_guard<std::mutex> lock(singleton->m_trackingUsersLock);

        auto iter = singleton->m_userHandlesMap.find(args.user());
        if (iter != singleton->m_userHandlesMap.end())
        {
            handler(iter->second);
        }
    });
}
CATCH_RETURN_WITH(-1)

XBL_API void XBL_CALLING_CONV
RemoveSignOutCompletedHandler(
    _In_ function_context context
    ) XBL_NOEXCEPT
try
{
    verify_global_init();
    xbox_live_user::remove_sign_out_completed_handler(context);
}
CATCH_RETURN_WITH(;)
#endif // !XDK_API
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GetTokenAndSignatureResult_WinRT.h"
#include "xsapi/system.h"
#include "User_WinRT.h"
#include "Macros_WinRT.h"
#include "user_impl.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::system;
using namespace xbox::services;
using namespace pplx;
using namespace concurrency;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_BEGIN

std::shared_ptr<user_impl> CreateUserImpl(Windows::System::User^ systemuser)
{
    return user_factory::create_user_impl(systemuser);
}

SignOutCompletedEventArgs::SignOutCompletedEventArgs(
    _In_ std::shared_ptr<xbox::services::system::user_impl> user_impl
    ):
    m_user(ref new XboxLiveUser(user_impl))
{
}

XboxLiveUser^ SignOutCompletedEventArgs::User::get()
{
    return m_user;
}

UserEventBind::UserEventBind()
{
    // Using std::bind to pass a handle to a non-static member, and using C++ class instead ref class for std::bind to work
    auto handler = std::bind(
        &UserEventBind::UserSignOutCompletedHandler,
        this,
        std::placeholders::_1
        );

    m_functionContext = user_impl::add_sign_out_completed_handler(handler);
}

UserEventBind::~UserEventBind()
{
    user_impl::remove_sign_out_completed_handler(m_functionContext);
}

void UserEventBind::UserSignOutCompletedHandler(_In_ xbox::services::system::sign_out_completed_event_args args)
{
    auto internalUser = args.user()->_User_impl();
    if (Xbox::Services::XboxLiveContextSettings::Dispatcher != nullptr)
    {
        Xbox::Services::XboxLiveContextSettings::Dispatcher->RunAsync(
            Windows::UI::Core::CoreDispatcherPriority::Normal,
            ref new Windows::UI::Core::DispatchedHandler([internalUser]()
        {
            XboxLiveUser::RaiseSignOutCompleted(ref new SignOutCompletedEventArgs(internalUser));
        }));
    }
    else
    {
        XboxLiveUser::RaiseSignOutCompleted(ref new SignOutCompletedEventArgs(args.user()->_User_impl()));
    }
}

XboxLiveUser::XboxLiveUser():
    m_cppObj(CreateUserImpl(nullptr))
{
}

XboxLiveUser::XboxLiveUser(Windows::System::User^ systemUser) :
    m_cppObj(CreateUserImpl(systemUser))
{
}

XboxLiveUser::XboxLiveUser(std::shared_ptr<xbox::services::system::user_impl> user_impl) :
    m_cppObj(user_impl)
{
}

IAsyncOperation<SignInResult^>^
XboxLiveUser::SignInAsync(
    _In_opt_ Platform::Object^ coreDispatcherObj
    )
{
    xbox_live_context_settings::_Set_dispatcher(coreDispatcherObj);
    task_completion_event<SignInResult^> tce;

    _User_impl()->sign_in_impl(true, false, get_xsapi_singleton()->m_asyncQueue, 
        [tce](xbox_live_result<sign_in_result> t)
    {
        THROW_IF_ERR(t);
        tce.set(ref new SignInResult(t.payload()));
    });

    auto t = task<SignInResult^>(tce);
    return ASYNC_FROM_TASK(t);
}

IAsyncOperation<SignInResult^>^
XboxLiveUser::SignInSilentlyAsync(
    _In_opt_ Platform::Object^ coreDispatcherObj
    )
{
    xbox_live_context_settings::_Set_dispatcher(coreDispatcherObj);
    task_completion_event<SignInResult^> tce;

    _User_impl()->sign_in_impl(false, false, get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<sign_in_result> t)
    {
        THROW_IF_ERR(t);
        tce.set(ref new SignInResult(t.payload()));
    });

    auto t = task<SignInResult^>(tce);
    return ASYNC_FROM_TASK(t);
}

IAsyncOperation<GetTokenAndSignatureResult^>^
XboxLiveUser::GetTokenAndSignatureAsync(
    _In_ Platform::String^ httpMethod,
    _In_ Platform::String^ url,
    _In_ Platform::String^ headers
    )
{
    task_completion_event<GetTokenAndSignatureResult^> tce;

    m_cppObj->internal_get_token_and_signature(
        INTERNAL_STRING_FROM_PLATFORM_STRING(httpMethod),
        INTERNAL_STRING_FROM_PLATFORM_STRING(url),
        INTERNAL_STRING_FROM_PLATFORM_STRING(url),
        INTERNAL_STRING_FROM_PLATFORM_STRING(headers),
        xsapi_internal_vector<unsigned char>(),
        false,
        false,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<token_and_signature_result_internal>> result)
    {
        THROW_IF_ERR(result);
        tce.set(ref new GetTokenAndSignatureResult(result.payload()));
    });
    
    auto t = task<GetTokenAndSignatureResult^>(tce);
    return ASYNC_FROM_TASK(t);
}

IAsyncOperation<GetTokenAndSignatureResult^>^
XboxLiveUser::GetTokenAndSignatureArrayAsync(
    _In_ String^ httpMethod,
    _In_ String^ url,
    _In_ String^ headers,
    _In_opt_ const Array<byte>^ requestBodyArray
    )
{
    xsapi_internal_vector<unsigned char> vec;
    if (requestBodyArray != nullptr && requestBodyArray->Length > 0)
    {
        vec = xsapi_internal_vector<unsigned char>(requestBodyArray->begin(), requestBodyArray->end());
    }

    task_completion_event<GetTokenAndSignatureResult^> tce;
    m_cppObj->internal_get_token_and_signature(
        INTERNAL_STRING_FROM_PLATFORM_STRING(httpMethod),
        INTERNAL_STRING_FROM_PLATFORM_STRING(url),
        INTERNAL_STRING_FROM_PLATFORM_STRING(url),
        INTERNAL_STRING_FROM_PLATFORM_STRING(headers),
        vec,
        false,
        false,
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<token_and_signature_result_internal>> result)
    {
        THROW_IF_ERR(result);
        tce.set(ref new GetTokenAndSignatureResult(result.payload()));
    });

    auto t = task<GetTokenAndSignatureResult^>(tce);
    return ASYNC_FROM_TASK(t);
}

IAsyncOperation<GetTokenAndSignatureResult^>^
XboxLiveUser::GetTokenAndSignatureAsync(
    _In_ String^ httpMethod,
    _In_ String^ url,
    _In_ String^ headers,
    _In_ String^ body
    )
{
    task_completion_event<GetTokenAndSignatureResult^> tce;
    m_cppObj->get_token_and_signature(
        INTERNAL_STRING_FROM_PLATFORM_STRING(httpMethod),
        INTERNAL_STRING_FROM_PLATFORM_STRING(url),
        INTERNAL_STRING_FROM_PLATFORM_STRING(headers),
        INTERNAL_STRING_FROM_PLATFORM_STRING(body),
        get_xsapi_singleton()->m_asyncQueue,
        [tce](xbox_live_result<std::shared_ptr<token_and_signature_result_internal>> result)
    {
        THROW_IF_ERR(result);
        tce.set(ref new GetTokenAndSignatureResult(result.payload()));
    });

    auto t = task<GetTokenAndSignatureResult^>(tce);
    return ASYNC_FROM_TASK(t);
}

void 
XboxLiveUser::RaiseSignOutCompleted(SignOutCompletedEventArgs^ args)
{
    SignOutCompleted(nullptr, args);
}

Windows::System::User^
XboxLiveUser::WindowsSystemUser::get()
{
    return m_cppObj->creation_context();
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_END

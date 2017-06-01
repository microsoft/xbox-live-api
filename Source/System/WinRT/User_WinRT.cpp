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

    auto task = GetUserImpl()->sign_in_impl(true, false)
    .then([](xbox_live_result<sign_in_result> t)
    {
        THROW_IF_ERR(t);
        return ref new SignInResult(t.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<SignInResult^>^
XboxLiveUser::SignInSilentlyAsync(
    _In_opt_ Platform::Object^ coreDispatcherObj
    )
{
    xbox_live_context_settings::_Set_dispatcher(coreDispatcherObj);

    auto task = GetUserImpl()->sign_in_impl(false, false)
    .then([](xbox_live_result<sign_in_result> t)
    {
        THROW_IF_ERR(t);
        return ref new SignInResult(t.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<GetTokenAndSignatureResult^>^
wrap_result(_In_ pplx::task<xbox::services::xbox_live_result<token_and_signature_result>> inner)
{
    auto task = inner
    .then([](xbox::services::xbox_live_result<token_and_signature_result> result)
    {
        THROW_IF_ERR(result);
        return ref new GetTokenAndSignatureResult(result.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<GetTokenAndSignatureResult^>^
XboxLiveUser::GetTokenAndSignatureAsync(
    _In_ Platform::String^ httpMethod,
    _In_ Platform::String^ url,
    _In_ Platform::String^ headers
    )
{
    return wrap_result(m_cppObj->internal_get_token_and_signature(
        STRING_T_FROM_PLATFORM_STRING(httpMethod),
        STRING_T_FROM_PLATFORM_STRING(url),
        STRING_T_FROM_PLATFORM_STRING(url),
        STRING_T_FROM_PLATFORM_STRING(headers),
        std::vector<unsigned char>(),
        false,
        false));
}

IAsyncOperation<GetTokenAndSignatureResult^>^
XboxLiveUser::GetTokenAndSignatureArrayAsync(
    _In_ String^ httpMethod,
    _In_ String^ url,
    _In_ String^ headers,
    _In_opt_ const Array<byte>^ requestBodyArray
    )
{
    std::vector<unsigned char> vec;
    if (requestBodyArray != nullptr && requestBodyArray->Length > 0)
    {
        vec = std::vector<unsigned char>(requestBodyArray->begin(), requestBodyArray->end());
    }

    return wrap_result(m_cppObj->internal_get_token_and_signature(
        STRING_T_FROM_PLATFORM_STRING(httpMethod),
        STRING_T_FROM_PLATFORM_STRING(url),
        STRING_T_FROM_PLATFORM_STRING(url),
        STRING_T_FROM_PLATFORM_STRING(headers),
        vec,
        false,
        false));
}

IAsyncOperation<GetTokenAndSignatureResult^>^
XboxLiveUser::GetTokenAndSignatureAsync(
    _In_ String^ httpMethod,
    _In_ String^ url,
    _In_ String^ headers,
    _In_ String^ body
    )
{
    return wrap_result(m_cppObj->get_token_and_signature(
        STRING_T_FROM_PLATFORM_STRING(httpMethod),
        STRING_T_FROM_PLATFORM_STRING(url),
        STRING_T_FROM_PLATFORM_STRING(headers),
        STRING_T_FROM_PLATFORM_STRING(body)
        ));
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

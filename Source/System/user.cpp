// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "utils.h"
#include "user_impl.h"
#include "xsapi/system.h"

using namespace concurrency;
using namespace std;
using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

sign_out_completed_event_args::sign_out_completed_event_args(
    _In_ std::weak_ptr<system::xbox_live_user> weakUser,
    _In_ std::shared_ptr<user_impl> user_impl
    )
{
    m_user = weakUser.lock();
    m_user_impl = user_impl;

    if (m_user == nullptr)
    {
        m_user = std::make_shared<xbox::services::system::xbox_live_user>(user_factory::create_user(m_user_impl));
    }
}

std::shared_ptr<system::xbox_live_user> sign_out_completed_event_args::user() const
{
    return m_user;
}

std::shared_ptr<user_impl> sign_out_completed_event_args::_Internal_user()
{
    return m_user_impl;
}

#if XSAPI_SERVER

pplx::task<xbox_live_result<void>> xbox_live_user::signin(
    _In_ std::shared_ptr<xbox_live_server> server,
    _In_ const string_t& user_delegation_ticket
    )
{
    if (!server->is_signed_in())
    {
        return pplx::task_from_result(xbox_live_result<void>(xbox_live_error_code::runtime_error, "xbox_live_server need to sign in first"));
    }

    return m_user_impl->sign_in_impl(user_delegation_ticket, false);
}

#elif defined(_WIN32)

pplx::task<xbox_live_result<sign_in_result>> xbox_live_user::signin()
{
    return m_user_impl->sign_in_impl(true, false);
}

pplx::task<xbox_live_result<sign_in_result>> xbox_live_user::signin_silently()
{
    return m_user_impl->sign_in_impl(false, false);
}

#endif

void xbox_live_user::set_title_telemetry_session_id(_In_ const string_t& sessionId)
{
    m_user_impl->set_title_telemetry_session_id(sessionId);
}

const string_t& xbox_live_user::_Title_telemetry_session_id()
{
    return m_user_impl->title_telemetry_session_id();
}

#if defined(WINAPI_FAMILY) && WINAPI_FAMILY==WINAPI_FAMILY_APP
pplx::task<xbox_live_result<sign_in_result>>
xbox_live_user::signin(_In_opt_ Platform::Object^ coreDispatcherObj)
{
    xbox_live_context_settings::_Set_dispatcher(coreDispatcherObj);
    return m_user_impl->sign_in_impl(true, false);
}

pplx::task<xbox_live_result<sign_in_result>> 
xbox_live_user::signin_silently(_In_opt_ Platform::Object^ coreDispatcherObj)
{
    xbox_live_context_settings::_Set_dispatcher(coreDispatcherObj);
    return m_user_impl->sign_in_impl(false, false);
}

xbox_live_user::xbox_live_user(Windows::System::User^ systemUser)
{
    m_user_impl = user_factory::create_user_impl(systemUser);
}

#endif

xbox_live_user::xbox_live_user()
{
    m_user_impl = user_factory::create_user_impl(nullptr);
}

const string_t& xbox_live_user::xbox_user_id() const
{
    return m_user_impl->xbox_user_id();
}

const string_t& xbox_live_user::gamertag() const
{
    return m_user_impl->gamertag();
}

const string_t& xbox_live_user::age_group() const
{
    return m_user_impl->age_group();
}

const string_t& xbox_live_user::privileges() const
{
    return m_user_impl->privileges();
}

std::shared_ptr<auth_config> xbox_live_user::auth_config()
{
    return m_user_impl->get_auth_config();
}

bool xbox_live_user::is_signed_in() const
{
    return m_user_impl->is_signed_in();
}

#if WINAPI_FAMILY && WINAPI_FAMILY==WINAPI_FAMILY_APP
const string_t& xbox_live_user::web_account_id() const
{
    return m_user_impl->web_account_id();
}

Windows::System::User^ xbox_live_user::windows_system_user() const
{
    return m_user_impl->creation_context();
}
#endif

pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
xbox_live_user::get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers
    )
{
    return m_user_impl->get_token_and_signature(httpMethod, url, headers);
}

pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
xbox_live_user::get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_opt_ const string_t& requestBodyString
    )
{
    return m_user_impl->get_token_and_signature(httpMethod, url, headers, requestBodyString);
}

pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
xbox_live_user::get_token_and_signature_array(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& requestBodyArray)
{
    return m_user_impl->get_token_and_signature_array(httpMethod, url, headers, requestBodyArray);
}

function_context
xbox_live_user::add_sign_out_completed_handler(_In_ std::function<void(const sign_out_completed_event_args&)> handler)
{
    return user_impl::add_sign_out_completed_handler(handler);
}

void
xbox_live_user::remove_sign_out_completed_handler(_In_ function_context context)
{
    return user_impl::remove_sign_out_completed_handler(context);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

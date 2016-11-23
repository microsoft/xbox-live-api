//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "user_impl_a.h"
#include "user_auth_android.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

std::shared_ptr<user_impl_android> user_impl_android::m_singletonInstance;

std::shared_ptr<user_impl_android> user_impl_android::get_instance()
{
    return m_singletonInstance;
}

user_impl_android::user_impl_android() :
    user_impl(false)
{
    m_auth = std::make_shared<user_auth_android>();
    m_authConfig = m_auth->auth_config();
}

user_impl_android::~user_impl_android()
{
}

pplx::task<xbox_live_result<sign_in_result>>
user_impl_android::sign_in_impl(_In_ bool showUI, _In_ bool forceRefresh)
{
    set_signed_out_callback();
    auto sharedPtr = shared_from_this();
    m_singletonInstance = std::dynamic_pointer_cast<user_impl_android>(sharedPtr);
    std::weak_ptr<user_impl_android> pThisWeak = std::dynamic_pointer_cast<user_impl_android>(sharedPtr);
    return m_auth->sign_in_impl(showUI, forceRefresh).then([pThisWeak](xbox_live_result<sign_in_result> result)
    {
        auto pThis = pThisWeak.lock();
        if (pThis)
        {
            pThis->m_xboxUserId = pThis->m_auth->xbox_user_id();
            pThis->m_gamertag = pThis->m_auth->gamertag();
            pThis->m_ageGroup = pThis->m_auth->age_group();
            pThis->m_privileges = pThis->m_auth->privileges();
            pThis->m_isSignedIn = pThis->m_auth->is_signed_in();
            return result;
        }
        else
        {
            return xbox_live_result<sign_in_result>(xbox_live_error_code::runtime_error, "user_impl_android is invalid");
        }
    });
}

pplx::task<xbox_live_result<void>>
user_impl_android::signout()
{
    set_signed_out_callback();
    std::weak_ptr<user_impl_android> pThisWeak = std::dynamic_pointer_cast<user_impl_android>(shared_from_this());
    return m_auth->signout().then([pThisWeak](xbox_live_result<void> result)
    {
        auto pThis = pThisWeak.lock();
        if (pThis)
        {
            pThis->m_xboxUserId = pThis->m_auth->xbox_user_id();
            pThis->m_gamertag = pThis->m_auth->gamertag();
            pThis->m_ageGroup = pThis->m_auth->age_group();
            pThis->m_privileges = pThis->m_auth->privileges();
            pThis->m_isSignedIn = pThis->m_auth->is_signed_in();
            return result;
        }
        else
        {
            return xbox_live_result<void>(xbox_live_error_code::runtime_error, "user_impl_android is invalid");
        }
    });
}

pplx::task<xbox_live_result<token_and_signature_result> >
user_impl_android::internal_get_token_and_signature(
    _In_ const string_t& httpMethod,
    _In_ const string_t& url,
    _In_ const string_t& endpointForNsal,
    _In_ const string_t& headers,
    _In_ const std::vector<unsigned char>& bytes,
    _In_ bool promptForCredentialsIfNeeded,
    _In_ bool forceRefresh
    )
{
    set_signed_out_callback();
    std::weak_ptr<user_impl_android> pThisWeak = std::dynamic_pointer_cast<user_impl_android>(shared_from_this());
    return m_auth->internal_get_token_and_signature(httpMethod, url, endpointForNsal, headers, bytes, promptForCredentialsIfNeeded, forceRefresh)
    .then([pThisWeak](xbox_live_result<token_and_signature_result> result)
    {
        auto pThis = pThisWeak.lock();
        if (pThis)
        {
            pThis->m_xboxUserId = pThis->m_auth->xbox_user_id();
            pThis->m_gamertag = pThis->m_auth->gamertag();
            pThis->m_ageGroup = pThis->m_auth->age_group();
            pThis->m_privileges = pThis->m_auth->privileges();
            pThis->m_isSignedIn = pThis->m_auth->is_signed_in();
            return result;
        }
        else
        {
            return xbox_live_result<token_and_signature_result>(xbox_live_error_code::runtime_error, "user_impl_android is invalid");
        }
    });
}

const std::weak_ptr<xbox_live_user>&
user_impl_android::user()
{
    return m_user;
}

void user_impl_android::set_user(_In_ const std::weak_ptr<xbox_live_user>& user)
{
    m_user = user;
}

pplx::task<xbox_live_result<void>>
user_impl_android::sign_in_impl(
    _In_ const string_t& userDelegationTicket,
    _In_ bool forceRefresh
    )
{
    throw std::exception();
}

void user_impl_android::set_signed_out_callback()
{
    if (!m_hasSetSignedOutCallback)
    {
        std::weak_ptr<user_impl_android> pThisWeak = std::dynamic_pointer_cast<user_impl_android>(shared_from_this());
        m_auth->set_signed_out_callback([pThisWeak]()
        {
            auto pThis = pThisWeak.lock();
            if (pThis)
            {
                pThis->user_signed_out();
            }
        });
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

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

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

#pragma mark - Sign In

pplx::task<xbox_live_result<sign_in_result>>
user_impl_ios::sign_in_impl(_In_ bool showUI, _In_ bool forceRefresh)
{
    m_auth->set_title_telemetry_session_id(m_titleTelemetrySessionId);
    set_signed_out_callback();
    auto sharedPtr = shared_from_this();
    std::weak_ptr<user_impl_ios> pThisWeak = std::dynamic_pointer_cast<user_impl_ios>(sharedPtr);
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
user_impl_ios::signout()
{
    m_auth->set_title_telemetry_session_id(m_titleTelemetrySessionId);
    set_signed_out_callback();
    std::weak_ptr<user_impl_ios> pThisWeak = std::dynamic_pointer_cast<user_impl_ios>(shared_from_this());
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

#pragma mark - Initialization

user_impl_ios::user_impl_ios() :
user_impl()
{
    m_auth = user_auth_ios::get_instance();
}

pplx::task<xbox_live_result<token_and_signature_result>>
user_impl_ios::internal_get_token_and_signature(
                                                _In_ const string_t& httpMethod,
                                                _In_ const string_t& url,
                                                _In_ const string_t& endpointForNsal,
                                                _In_ const string_t& headers,
                                                _In_ const std::vector<unsigned char>& bytes,
                                                _In_ bool promptForCredentialsIfNeeded,
                                                _In_ bool forceRefresh
                                                )
{
    m_auth->set_title_telemetry_session_id(m_titleTelemetrySessionId);
    set_signed_out_callback();
    std::weak_ptr<user_impl_ios> pThisWeak = std::dynamic_pointer_cast<user_impl_ios>(shared_from_this());
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

void user_impl_ios::set_signed_out_callback()
{
    if (!m_hasSetSignedOutCallback)
    {
        std::weak_ptr<user_impl_ios> pThisWeak = std::dynamic_pointer_cast<user_impl_ios>(shared_from_this());
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

#ifdef BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

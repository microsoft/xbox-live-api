//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "user_impl.h"

#ifdef XSAPI_I
#if defined(__OBJC__)
#import <UIKit/UIKit.h>
#import "XBLSignInHandler.h"
#import "MSAAuthentication/MSAAccountManager.h"
#import "sign_in_delegate_ios.h"

#endif
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class user_auth_ios;

class user_impl_ios : public user_impl
{
public:
    
    user_impl_ios();
    
    pplx::task<xbox::services::xbox_live_result<sign_in_result>> sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh
        ) override;
    
    pplx::task<xbox::services::xbox_live_result<void>> signout() override;
    
    pplx::task<xbox::services::xbox_live_result<token_and_signature_result>>
    internal_get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& endpointForNsal,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh
        ) override;
    
private:
    void set_signed_out_callback();
    
    bool m_hasSetSignedOutCallback;
    std::shared_ptr<user_auth_ios> m_auth;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

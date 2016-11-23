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

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class user_auth_android;

class user_impl_android : public user_impl
{
public:
    pplx::task<xbox::services::xbox_live_result<sign_in_result>> sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh
        ) override;

    pplx::task<xbox::services::xbox_live_result<void>> signout() override;

    pplx::task<xbox::services::xbox_live_result<void>> sign_in_impl(
        _In_ const string_t& userDelegationTicket,
        _In_ bool forceRefresh
        ) override;

    user_impl_android();

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

    static std::shared_ptr<user_impl_android> get_instance();

    const std::weak_ptr<xbox_live_user>& user();
    void set_user(_In_ const std::weak_ptr<xbox_live_user>& user) override;

    ~user_impl_android();
private:

    void set_signed_out_callback();

    bool m_hasSetSignedOutCallback;

    static std::shared_ptr<user_impl_android> m_singletonInstance;
    std::weak_ptr<xbox_live_user> m_user;
    std::shared_ptr<user_auth_android> m_auth;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
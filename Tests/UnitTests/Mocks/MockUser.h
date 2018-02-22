// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "pch.h"
#include "xbox_system_factory.h"
#include "xsapi/system.h"
#include "user_impl.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockUser :public user_impl
{
public:

    virtual pplx::task<xbox_live_result<sign_in_result>> sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh
        ) override;

    virtual pplx::task<xbox_live_result<void>> sign_in_impl(
        _In_ const string_t& userDelegationTicket,
        _In_ bool forceRefresh
        ) override;

    std::shared_ptr<xbox::services::system::auth_config> auth_config();
    std::shared_ptr<xbox::services::xbox_live_context_settings> xbox_live_context_settings();

    virtual pplx::task<xbox::services::xbox_live_result<token_and_signature_result> >
    internal_get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& endpointForNsal,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh
        ) override;

    virtual void internal_get_token_and_signature(
        _In_ const string_t& httpMethod,
        _In_ const string_t& url,
        _In_ const string_t& endpointForNsal,
        _In_ const string_t& headers,
        _In_ const std::vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh,
        _In_ uint64_t taskGroupId,
        _In_ xbox_live_callback<xbox::services::xbox_live_result<token_and_signature_result>> callback
        ) override;

    MockUser();

    HRESULT ResultHR;

};


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

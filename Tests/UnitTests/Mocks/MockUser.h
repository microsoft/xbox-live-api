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

    virtual void sign_in_impl(
        _In_ bool showUI,
        _In_ bool forceRefresh,
        _In_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<sign_in_result>> callback
        ) override;

    std::shared_ptr<xbox::services::system::auth_config> auth_config();
    std::shared_ptr<xbox::services::xbox_live_context_settings> xbox_live_context_settings();

    virtual void internal_get_token_and_signature(
        _In_ const xsapi_internal_string& httpMethod,
        _In_ const xsapi_internal_string& url,
        _In_ const xsapi_internal_string& endpointForNsal,
        _In_ const xsapi_internal_string& headers,
        _In_ const xsapi_internal_vector<unsigned char>& bytes,
        _In_ bool promptForCredentialsIfNeeded,
        _In_ bool forceRefresh,
        _In_ async_queue_handle_t queue,
        _In_ token_and_signature_callback callback
        ) override;

    MockUser();

    HRESULT ResultHR;

};


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

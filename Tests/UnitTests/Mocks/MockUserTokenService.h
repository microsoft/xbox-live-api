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
#include "pch.h"
#include "user_token_service.h"
#include "title_token_service.h"
#include "service_token_service.h"
#include "Ecdsa.h"
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockTitleTokenService : public title_token_service
{
public:
    virtual pplx::task<xbox::services::xbox_live_result<token_result>>
    get_t_token_from_service(
        _In_ const string_t& rpsTicket,
        _In_ std::shared_ptr<ecdsa> proofKey,
        _In_ std::shared_ptr<auth_config> authenticationConfiguration,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
        _In_ const string_t& deviceToken
        ) override;

    MockTitleTokenService();

    HRESULT ResultHR;
    token_result ResultValue;
};

class MockUserTokenService : public user_token_service
{
public:
    virtual pplx::task<xbox::services::xbox_live_result<token_result>>
    get_u_token_from_service(
        _In_ const string_t& rpsTicket,
        _In_ std::shared_ptr<ecdsa> proofKey,
        _In_ std::shared_ptr<auth_config> authenticationConfiguration,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings
        ) override;

    MockUserTokenService();

    HRESULT ResultHR;
    token_result ResultValue;
};

class MockServiceTokenService : public service_token_service
{
public:
    virtual pplx::task<xbox::services::xbox_live_result<token_result>>
        get_s_token_from_service(
            _In_ std::shared_ptr<ecdsa> proofKey,
            _In_ std::shared_ptr<auth_config> authenticationConfiguration,
            _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings
            ) override;

    MockServiceTokenService();

    HRESULT ResultHR;
    token_result ResultValue;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


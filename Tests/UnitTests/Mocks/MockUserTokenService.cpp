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
#include "MockUserTokenService.h"
#include "StockMocks.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

MockUserTokenService::MockUserTokenService() :
    ResultHR(S_OK)
{
    auto token = token_result::deserialize(
        StockMocks::GetStockJsonMockUserTokenResult()
        );

    ResultValue = token;
    //assert(!token.err());
    //ResultValue = token.payload();
}

pplx::task<xbox::services::xbox_live_result<token_result>>
MockUserTokenService::get_u_token_from_service(
    _In_ const std::wstring& rpsTicket,
    _In_ std::shared_ptr<ecdsa> proofKey,
    _In_ std::shared_ptr<auth_config> authenticationConfiguration,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings
    )
{
    HRESULT hr = ResultHR;
    if (FAILED(hr))
    {
        throw hr;
    }

    return pplx::task_from_result(xbox::services::xbox_live_result<token_result>(ResultValue));
}


MockTitleTokenService::MockTitleTokenService() :
    ResultHR(S_OK)
{
    auto token = token_result::deserialize(
        StockMocks::GetStockJsonMockUserTokenResult()
        );

    ResultValue = token;

    //assert(!token.err());
    //ResultValue = token.payload();
}

pplx::task<xbox::services::xbox_live_result<token_result>>
MockTitleTokenService::get_t_token_from_service(
    _In_ const string_t& rpsTicket,
    _In_ std::shared_ptr<ecdsa> proofKey,
    _In_ std::shared_ptr<auth_config> authenticationConfiguration,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
    _In_ const string_t& deviceToken
    )
{
    HRESULT hr = ResultHR;
    if (FAILED(hr))
    {
        throw hr;
    }

    return pplx::task_from_result(xbox::services::xbox_live_result<token_result>(ResultValue));
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


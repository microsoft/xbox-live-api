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
#include "MockXstsTokenService.h"
#include "StockMocks.h"
#include "token_result.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

MockXstsTokenService::MockXstsTokenService() :
    ResultHR(S_OK)
{
    auto cppObj = token_result::deserialize(
        StockMocks::GetStockJsonMockXTokenResult()
        );

    ResultValue = cppObj;
/*
    assert(!cppObj.err());
    ResultValue = cppObj.payload();*/
}

pplx::task< xbox::services::xbox_live_result<token_result> >
MockXstsTokenService::get_x_token_from_service(
    _In_ std::shared_ptr<xbox::services::system::ecdsa> proofKey,
    _In_ string_t deviceToken,
    _In_ string_t titleToken,
    _In_ string_t userToken,
    _In_ string_t serviceToken,
    _In_ string_t relyingParty,
    _In_ string_t tokenType,
    _In_ std::shared_ptr<xbox::services::system::auth_config> authenticationConfiguration,
    _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
    _In_ string_t titleId
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


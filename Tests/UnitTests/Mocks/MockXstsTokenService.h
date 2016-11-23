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
#include "xsapi/system.h"
#include "xsts_token_service.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class MockXstsTokenService : public xsts_token_service
{
public:
    virtual pplx::task< xbox::services::xbox_live_result<token_result> >
    get_x_token_from_service(
        _In_ std::shared_ptr<xbox::services::system::ecdsa> proofKey,
        _In_ string_t deviceToken,
        _In_ string_t titleToken,
        _In_ string_t userToken,
        _In_ string_t serviceToken,
        _In_ string_t relyingParty,
        _In_ string_t tokenType,
        _In_ std::shared_ptr<xbox::services::system::auth_config> authenticationConfiguration,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ string_t titleId
        ) override;

    MockXstsTokenService();

    HRESULT ResultHR;
    token_result ResultValue;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


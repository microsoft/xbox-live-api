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
#include "xsapi/system.h"
#include "shared_macros.h"
#include "token_result.h"
#include "Ecdsa.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

class xsts_token_service
{
public:
    virtual pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result> >
    get_x_token_from_service(
        _In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::system::ecdsa> proofKey,
        _In_ string_t deviceToken,
        _In_ string_t titleToken,
        _In_ string_t userToken,
        _In_ string_t serviceToken,
        _In_ string_t relyingParty,
        _In_ string_t tokenType,
        _In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::system::auth_config> authenticationConfiguration,
        _In_ std::shared_ptr<XBOX_LIVE_NAMESPACE::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ string_t titleId
        ) = 0;
};

class xsts_token_service_impl : public xsts_token_service
{
public:
    /// <summary>
    /// Returns a XToken obtained from XSTS
    /// </summary>
    virtual pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>>
    get_x_token_from_service(
        _In_ std::shared_ptr<ecdsa> proofKey,
        _In_ string_t deviceToken,
        _In_ string_t titleToken,
        _In_ string_t userToken,
        _In_ string_t serviceToken,
        _In_ string_t relyingParty,
        _In_ string_t tokenType,
        _In_ std::shared_ptr<auth_config> authenticationConfiguration,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings,
        _In_ string_t titleId
        ) override;
};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif


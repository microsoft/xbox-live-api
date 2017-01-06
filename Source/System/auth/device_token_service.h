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
#include "token_result.h"
#include "Ecdsa.h"
#include "local_config.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

class device_token_service
{
public:
    virtual pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>>
    get_d_token_from_service(
        _In_ const string_t& rpsTicket,
        _In_ std::shared_ptr<ecdsa> proofKey,
        _In_ std::shared_ptr<auth_config> authenticationConfiguration,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings
        ) = 0;
};

class device_token_service_impl : public device_token_service
{
public:
    virtual pplx::task<XBOX_LIVE_NAMESPACE::xbox_live_result<token_result>>
    get_d_token_from_service(
        _In_ const string_t& rpsTicket,
        _In_ std::shared_ptr<ecdsa> proofKey,
        _In_ std::shared_ptr<auth_config> authenticationConfiguration,
        _In_ std::shared_ptr<xbox_live_context_settings> xboxLiveContextSettings
        ) override;
};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

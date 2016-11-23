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
#include "shared_macros.h"
#include "Ecdsa.h"
#include "signature_policy.h"
#include "cpprest/json.h"

#include <string>
#include <stdint.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class token_request
{
public:
    static web::json::value create_device_token_request(
        _In_ string_t relyingParty,
        _In_ string_t tokenType,
        _In_ std::shared_ptr<ecdsa> proofKey,
        _In_ string_t authMethod,
        _In_ string_t siteName,
        _In_ string_t rpsTicket
        );

    static web::json::value create_title_token_request(
        _In_ string_t relyingParty,
        _In_ string_t tokenType,
        _In_ string_t deviceToken,
        _In_ std::shared_ptr<ecdsa> proofKey,
        _In_ string_t authMethod,
        _In_ string_t siteName,
        _In_ string_t rpsTicket
        );

    static web::json::value create_user_token_request(
        _In_ string_t relyingParty,
        _In_ string_t tokenType,
        _In_ string_t authMethod,
        _In_ string_t siteName,
        _In_ string_t rpsTicket,
        _In_ std::shared_ptr<ecdsa> proofKey
        );

    static web::json::value create_service_token_request(
        _In_ string_t relyingParty,
        _In_ string_t tokenType,
        _In_ std::shared_ptr<ecdsa> proofKey
        );

    static web::json::value create_xtoken_request(
        _In_ string_t relyingParty,
        _In_ string_t tokenType,
        _In_ string_t deviceToken,
        _In_ string_t titleToken,
        _In_ string_t userToken,
        _In_ string_t serviceToken,
        _In_ string_t sandbox);

    static const signature_policy auth_signature_policy;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


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
#include "signature_policy.h"
#include "ecdsa.h"
#include "utils.h"
#include <cpprest/http_msg.h>
#ifdef _WIN32
#include <bcrypt.h>
#endif

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

class request_signer
{
public:

    /// <summary>
    /// Hashes the request elements according to the signature policy. Used for testing.
    /// </summary>
    static std::vector<unsigned char> hash_request(
        _In_ const signature_policy& signaturePolicy,
        _In_ int64_t timestamp,
        _In_ const string_t& httpMethod,
        _In_ const string_t& urlPathAndQuery,
        _In_ const web::http::http_headers& headers,
        _In_ const std::vector<unsigned char>& body
        );

    /// <summary>
    /// Signs the request elements according to the signature policy using the Ecdsa key.
    /// </summary>
    static string_t sign_request(
        _In_ ecdsa& ecdsaValue,
        _In_ const signature_policy& signaturePolicy,
        _In_ int64_t timestamp,
        _In_ const string_t& httpMethod,
        _In_ const string_t& urlPathAndQuery,
        _In_ const web::http::http_headers& headers,
        _In_ const std::vector<unsigned char>& body
        );
};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

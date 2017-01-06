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
#include "shared_macros.h"
#include "Ecdsa.h"
#include "json_web_key.h"
#include "utils.h"
#include "cpprest/json.h"

#include <string>
#include <vector>
#include <stdint.h>

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

web::json::value json_web_key::serialize_json_web_key(_In_ std::shared_ptr<ecdsa> eccKey)
{
#if 0
    std::vector<unsigned char> publicBlob = eccKey.export_public_blob();

    // The public blob is a pair of 32 bit uints followed by the X and
    // Y parameters which are each 256 bits.

    if (publicBlob.size() != 4 + 4 + 32 + 32)
    {
        throw std::invalid_argument("Invalid ECC public blob");
    }

    std::vector<unsigned char> xparam(publicBlob.begin() + 8, publicBlob.begin() + 8 + 32);
    std::vector<unsigned char> yparam(publicBlob.begin() + 8 + 32, publicBlob.end());
#endif
    
    const ecc_pub_key& pubKey = eccKey->pub_key();
    
    web::json::value jwk;

    jwk[_T("crv")] = web::json::value(_T("P-256"));
    jwk[_T("alg")] = web::json::value(_T("ES256"));
    jwk[_T("use")] = web::json::value(_T("sig"));
    jwk[_T("kty")] = web::json::value(_T("EC"));
    jwk[_T("x")] = web::json::value(utils::base64_url_encode(pubKey.x));
    jwk[_T("y")] = web::json::value(utils::base64_url_encode(pubKey.y));

    return jwk;
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif


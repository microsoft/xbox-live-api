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
#include "signature_policy.h"
#include "request_signer.h"
#include "Utils.h"
#include "request_signer_helpers.h"
#include "sha256.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

string_t request_signer::sign_request(
    _In_ ecdsa& ecdsaValue,
    _In_ const signature_policy& signaturePolicy,
    _In_ int64_t timestamp,
    _In_ const string_t& httpMethod,
    _In_ const string_t& urlPathAndQuery,
    _In_ const web::http::http_headers& headers,
    _In_ const std::vector<unsigned char>& bytes
    )
{
    std::vector<unsigned char> hash = hash_request(
        signaturePolicy,
        timestamp,
        httpMethod,
        urlPathAndQuery,
        headers,
        bytes
        );

    std::vector<unsigned char> signature = ecdsaValue.sign_hash(hash);
    if (signature.size() > 0)
    {
        unsigned char buffer[12];
        request_signer_helpers::insert_version(buffer, signaturePolicy.version());
        request_signer_helpers::insert_timestamp(buffer + 4, timestamp);

        signature.reserve(signature.size() + 12);
        signature.insert(signature.begin(), buffer, buffer + 12);

        return utility::conversions::to_base64(signature);
    }

    return string_t();
}

static void add_ascii_string_to_hash(
    _In_ sha256& sha256,
    _In_ const utility::string_t& str)
{
    std::string ascii = utility::conversions::to_utf8string(str);
    sha256.add_bytes(reinterpret_cast<const unsigned char*>(ascii.c_str()), ascii.length() + 1); // Note the length+1 so we add the string and NULL byte in one step
}


std::vector<unsigned char> request_signer::hash_request(
    _In_ const signature_policy& signaturePolicy,
    _In_ int64_t timestamp,
    _In_ const string_t& httpMethod,
    _In_ const string_t& urlPathAndQuery,
    _In_ const web::http::http_headers& headers,
    _In_ const std::vector<unsigned char>& bytes
    )
{
    sha256 sha256;
    unsigned char nullByte[] = { 0 };
    
    unsigned int version = signaturePolicy.version();
    
    // create buffer for policy version and timestamp
    unsigned char buffer[14];
    
    request_signer_helpers::insert_version(buffer, version);
    buffer[4] = 0; // null byte after the version
    
    request_signer_helpers::insert_timestamp(buffer + 5, timestamp);
    buffer[13] = 0; // null byte after the timestamp
    
    sha256.add_bytes(buffer, 14);
    
    add_ascii_string_to_hash(sha256, httpMethod);
    add_ascii_string_to_hash(sha256, urlPathAndQuery);
    
    // add the headers. http_headers[] returns an empty string if the header is not present
    // which is perfect since we need to add a null byte anyways.
    add_ascii_string_to_hash(sha256, request_signer_helpers::get_header_or_empty_string(headers, _T("Authorization")));
    auto extraHeaders = signaturePolicy.extra_headers();
    for (auto it = extraHeaders.cbegin(); it != extraHeaders.cend(); it++)
    {
        add_ascii_string_to_hash(sha256, request_signer_helpers::get_header_or_empty_string(headers, *it));
    }
    
    // hash up to max body bytes of the body
    std::size_t numBytesToHash = min(static_cast<size_t>(signaturePolicy.max_body_bytes()), bytes.size());

    sha256.add_bytes(bytes.data(), numBytesToHash);
    sha256.add_bytes(nullByte, 1);
    
    return sha256.get_hash();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
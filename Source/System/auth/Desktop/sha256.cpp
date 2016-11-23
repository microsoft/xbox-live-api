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

sha256::sha256() :
    m_provider(BCRYPT_SHA256_ALGORITHM),
    m_hashHandle(nullptr)
{
    NTSTATUS status;
    DWORD cbData;
    DWORD pbHashObj;

    // Figure out how much memory we need for the hash object
    status = BCryptGetProperty(
        m_provider.GetProviderHandle(),
        BCRYPT_OBJECT_LENGTH,
        (PBYTE)&pbHashObj,
        sizeof(DWORD),
        &cbData,
        0);

    if (!BCRYPT_SUCCESS(status))
    {
        throw std::runtime_error("Error getting sha256 object length");
    }

    m_hashObject = std::unique_ptr<unsigned char[]>(new unsigned char[pbHashObj]);

    // Get hash handle
    status = BCryptCreateHash(
        m_provider.GetProviderHandle(),
        &m_hashHandle,
        m_hashObject.get(),
        pbHashObj,
        NULL,
        0,
        0);

    if (!BCRYPT_SUCCESS(status))
    {
        throw std::runtime_error("Error creating hash");
    }
}

sha256::~sha256()
{
    if (m_hashHandle != nullptr)
    {
        BCryptDestroyHash(m_hashHandle);
    }
}

void sha256::add_bytes(const unsigned char* bytes, std::size_t cbBytes)
{
    NTSTATUS status = BCryptHashData(
        m_hashHandle,
        const_cast<unsigned char*>(bytes),
        (ULONG)cbBytes,
        0);

    if (!BCRYPT_SUCCESS(status))
    {
        throw std::runtime_error("Error SHA256 hashing data");
    }
}

void sha256::add_bytes(const std::vector<unsigned char>& bytes)
{
    add_bytes(bytes.data(), bytes.size());
}

std::vector<unsigned char> sha256::get_hash()
{
    DWORD cbData = 0;
    DWORD cbHash = 0; // size of hash value

    // Get size of hash value
    NTSTATUS status = BCryptGetProperty(
        m_provider.GetProviderHandle(),
        BCRYPT_HASH_LENGTH,
        (PUCHAR)&cbHash,
        sizeof(DWORD),
        &cbData,
        0);
    if (!BCRYPT_SUCCESS(status))
    {
        throw std::runtime_error("Error getting hash value size");
    }

    std::vector<unsigned char> hash(cbHash);

    status = BCryptFinishHash(
        m_hashHandle,
        hash.data(),
        cbHash,
        0);
    if (!BCRYPT_SUCCESS(status))
    {
        throw std::runtime_error("Error finishing SHA256 hash");
    }

    return hash;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

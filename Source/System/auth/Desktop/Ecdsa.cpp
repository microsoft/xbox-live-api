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
#include "Ecdsa.h"
#include <string>
#include <vector>
#include <Windows.h>
#include <bcrypt.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

ecc_pub_key::ecc_pub_key(std::vector<unsigned char> x0, std::vector<unsigned char> y0) :
    x(std::move(x0)),
    y(std::move(y0))
{
}

ecdsa::ecdsa() :
    m_algorithmProvider(BCRYPT_ECDSA_P256_ALGORITHM)
{
    NTSTATUS status = BCryptGenerateKeyPair(
        m_algorithmProvider.GetProviderHandle(),
        &m_keyHandle,
        256,
        0);

    if (!BCRYPT_SUCCESS(status))
    {
        throw std::runtime_error("Error creating ECC key");
    }

    status = BCryptFinalizeKeyPair(m_keyHandle, 0);

    if (!BCRYPT_SUCCESS(status))
    {
        BCryptDestroyKey(m_keyHandle);
        throw std::runtime_error("Error finalizing ECC key"); 
    }
}

ecdsa::ecdsa(_In_ const ecc_pub_key& pub_key) :
    m_algorithmProvider(BCRYPT_ECDSA_P256_ALGORITHM)
{
    // The public blob is a pair of 32 bit uints followed by the X and
    // Y parameters which are each 256 bits. 
    // It has the following format in contiguous memory.
    //
    //      ULONG Magic; // Specifies the type of key this BLOB represents. 
    //      ULONG cbKey; // The length, in bytes, of the key.
    //      BYTE X[cbKey] // Big-endian.
    //      BYTE Y[cbKey] // Big-endian.

    uint32_t xSize = static_cast<uint32_t>(pub_key.x.size());
    uint32_t ySize = static_cast<uint32_t>(pub_key.y.size());
    XSAPI_ASSERT(xSize == ySize);

    BCRYPT_ECCKEY_BLOB header;
    header.dwMagic = BCRYPT_ECDSA_PUBLIC_P256_MAGIC;
    header.cbKey = xSize;

    std::vector<unsigned char> publicBlob(sizeof(BCRYPT_ECCKEY_BLOB) + 32 + 32);
    std::memcpy(&publicBlob[0], &header, sizeof(BCRYPT_ECCKEY_BLOB));
    publicBlob.insert(publicBlob.end(), pub_key.x.begin(), pub_key.x.end());
    publicBlob.insert(publicBlob.end(), pub_key.y.begin(), pub_key.y.end());

    NTSTATUS status = BCryptImportKeyPair(
        m_algorithmProvider.GetProviderHandle(),
        NULL,
        BCRYPT_ECCPUBLIC_BLOB,
        &m_keyHandle,
        const_cast<unsigned char*>(publicBlob.data()),
        (ULONG)publicBlob.size(),
        0);

    if (!BCRYPT_SUCCESS(status))
    {
        throw std::runtime_error("Error importing ECC public key blob");
    }
}

ecdsa::~ecdsa()
{
    BCryptDestroyKey(m_keyHandle);
}

std::vector<unsigned char> ecdsa::sign_hash(const std::vector<unsigned char>& hash) const
{
    DWORD signatureSize = 0;
    std::vector<unsigned char> signature;

    PBYTE pbHashValue = (PBYTE)hash.data();
    DWORD cbHashValue = (DWORD)hash.size();

    // Figure out how big the signature value is
    NTSTATUS status = BCryptSignHash(
        m_keyHandle,
        NULL,
        pbHashValue,
        cbHashValue,
        NULL,
        0,
        &signatureSize,
        0);

    if (BCRYPT_SUCCESS(status))
    {
        signature.reserve(signatureSize);

        status = BCryptSignHash(
            m_keyHandle,
            NULL,
            pbHashValue,
            cbHashValue,
            signature.data(),
            signatureSize,
            &signatureSize,
            0);

        if (!BCRYPT_SUCCESS(status))
        {
            signature.clear();
        }
    }

    return signature;
}

bool ecdsa::verify_signature(_In_ const std::vector<unsigned char>& hash, _In_ const std::vector<unsigned char>& signature) const
{
    NTSTATUS status = BCryptVerifySignature(
        m_keyHandle,
        NULL,
        const_cast<unsigned char*>(hash.data()),
        (DWORD)hash.size(),
        const_cast<unsigned char*>(signature.data()),
        (DWORD)signature.size(),
        0);

    return BCRYPT_SUCCESS(status);
}

const ecc_pub_key& ecdsa::pub_key()
{
    if (m_pubKey.x.size() == 0 || m_pubKey.y.size() == 0)
    {
        DWORD blobSize = 0;

        // Get size of blob
        NTSTATUS status = BCryptExportKey(
            m_keyHandle,
            NULL,
            BCRYPT_ECCPUBLIC_BLOB,
            NULL,
            0,
            &blobSize,
            0);

        if (BCRYPT_SUCCESS(status))
        {
            std::vector<unsigned char> publicBlob(blobSize);

            status = BCryptExportKey(
                m_keyHandle,
                NULL,
                BCRYPT_ECCPUBLIC_BLOB,
                publicBlob.data(),
                blobSize,
                &blobSize,
                0);

            if (BCRYPT_SUCCESS(status))
            {
                // Extract the key size
                std::size_t keySize = 0;
                for (int i = 0; i < sizeof(ULONG); i++)
                {
                    keySize |= publicBlob[sizeof(ULONG) + i] << (8 * (sizeof(ULONG) - i));
                    char buf[32] = { 0 };
                    sprintf_s(buf, sizeof(buf), "%zd", keySize);
                }

                // Extract X
                auto x = std::vector<unsigned char>(
                    publicBlob.begin() + (sizeof(ULONG) * 2),
                    publicBlob.begin() + (sizeof(ULONG) * 2) + keySize);

                // Extract Y
                auto y = std::vector<unsigned char>(
                    publicBlob.begin() + (sizeof(ULONG) * 2) + keySize,
                    publicBlob.end());

                m_pubKey = ecc_pub_key(x, y);
            }
        }
    }

    return m_pubKey;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
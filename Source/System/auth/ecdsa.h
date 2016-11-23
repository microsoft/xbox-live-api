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

#include <vector>
#include <string>

#include "shared_macros.h"

#if defined _WIN32 && !defined __cplusplus_winrt
#include <mutex>
#include "algorithm_provider.h"
#elif defined __cplusplus_winrt
#include <mutex>
#else
#include <memory>
#include <openssl/ecdsa.h>
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

struct ecc_pub_key
{
    std::vector<unsigned char> x;
    std::vector<unsigned char> y;

    ecc_pub_key() {};
    ecc_pub_key(std::vector<unsigned char> x0, std::vector<unsigned char> y0);
};

/// <summary>
/// Simple RAII wrapper around OpenSSL's EC_KEY with methods
/// for exporting public key and signing hashes. This class
/// is thread safe if you set the locking callbacks for
/// OpenSSL. Boost does this for us so we don't have to.
/// This object has shared_ptr semantics.
/// </summary>
class ecdsa
{
public:
    /// <summary>
    /// Creates and generates an EC key with a NIST P-256 curve.
    /// </summary>
    ecdsa();

    ~ecdsa();

#ifdef _WIN32
    ecdsa(_In_ const ecc_pub_key& pub_key);

    /// <summary>
    /// Verifies the signature against the supplied hash.
    /// </summary>
    bool verify_signature(_In_ const std::vector<unsigned char>& hash, _In_ const std::vector<unsigned char>& signature) const;
#endif

    /// <summary>
    /// Signs the hash using the EC key.
    /// </summary>
    std::vector<unsigned char> sign_hash(const std::vector<unsigned char>& hash) const;

#ifdef __cplusplus_winrt
    /// <summary>
    /// WinRT specific overload that takes IBuffers for performance.
    /// </summary>
    Windows::Storage::Streams::IBuffer^ sign_hash(_In_ Windows::Storage::Streams::IBuffer^ hash) const;
#endif

    /// <summary>
    /// Exports the X and Y parameters of the public key. X and
    /// Y are big-endian, unsigned, 32 byte integers.
    /// </summary>
    const ecc_pub_key& pub_key();

private:
    ecc_pub_key m_pubKey;

#if defined _WIN32 && !defined __cplusplus_winrt
    algorithm_provider m_algorithmProvider;
    BCRYPT_KEY_HANDLE m_keyHandle;
#elif defined __cplusplus_winrt
    // The WinRT crypto APIs are not thread safe so we need mutex. sign_hash() is
    // still conceptually const. We mark the mutex mutable so that we can use it
    // in const scenarios.
    mutable std::mutex m_keyLock;

    Windows::Security::Cryptography::Core::CryptographicKey^ m_key;
#else
    std::shared_ptr<EC_KEY> m_key;
#endif
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

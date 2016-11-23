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

#include <vector>
#include <openssl/obj_mac.h>
#include "ecdsa.h"
#include "big_num.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

ecc_pub_key::ecc_pub_key(std::vector<unsigned char> x0, std::vector<unsigned char> y0) :
    x(std::move(x0)),
    y(std::move(y0))
{
}

ecdsa::ecdsa()
{
    m_key = std::shared_ptr<EC_KEY>(
        EC_KEY_new_by_curve_name(NID_X9_62_prime256v1),
        EC_KEY_free);

    if (m_key == nullptr)
    {
        throw std::runtime_error("Failed to create prime256v1 EC key");
    }

    if (!EC_KEY_generate_key(m_key.get()))
    {
        throw std::runtime_error("Failed to generate EC key");
    }
}

ecdsa::~ecdsa()
{
}

const ecc_pub_key& ecdsa::pub_key()
{
    if (m_pubKey.x.size() == 0 || m_pubKey.y.size() == 0)
    {
        const EC_GROUP *group = EC_KEY_get0_group(m_key.get());
        const EC_POINT *point = EC_KEY_get0_public_key(m_key.get());

        big_num x, y;

        EC_POINT_get_affine_coordinates_GFp(group, point, &x.bn(), &y.bn(), NULL);

        m_pubKey = ecc_pub_key(x.get_bytes(32), y.get_bytes(32));
    }

    return m_pubKey;
}

std::vector<unsigned char>
ecdsa::sign_hash(const std::vector<unsigned char>& hash) const
{
    if (hash.size() > INT_MAX)
        throw std::invalid_argument("hash size is too large");

    std::unique_ptr<ECDSA_SIG, void(*)(ECDSA_SIG*)> sig(
        ECDSA_do_sign(hash.data(), (int)hash.size(), m_key.get()),
        ECDSA_SIG_free);

    std::vector<unsigned char> sig_bytes(64);

    int r_len = BN_num_bytes(sig->r);
    int s_len = BN_num_bytes(sig->s);

    BN_bn2bin(sig->r, sig_bytes.data() + 32 - r_len);
    BN_bn2bin(sig->s, sig_bytes.data() + 32 + 32 - s_len);

    return sig_bytes;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
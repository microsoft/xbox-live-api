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
#include "sha256.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

sha256::sha256()
{
    SHA256_Init(&m_ctx);
}

void
sha256::add_bytes(const std::vector<unsigned char> data)
{
    add_bytes(data.data(), data.size());
}

void
sha256::add_bytes(const unsigned char *data, std::size_t length)
{
    SHA256_Update(&m_ctx, data, length);
}

std::vector<unsigned char>
sha256::get_hash()
{
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    SHA256_Final(hash.data(), &m_ctx);

    return hash;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
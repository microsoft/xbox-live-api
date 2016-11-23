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
#include <openssl/sha.h>

#include "shared_macros.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class sha256
{
public:
    sha256();

    // sha256 is stateful and must never be copied.
    sha256(const sha256& other) = delete;

    void add_bytes(const std::vector<unsigned char> data);
    void add_bytes(const unsigned char *data, std::size_t length);
    std::vector<unsigned char> get_hash();

private:
    SHA256_CTX m_ctx;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
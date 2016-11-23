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
#include <openssl/bn.h>

#include "shared_macros.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

// simple RAII wrapper around OpenSSL's BIGNUM
class big_num
{
public:
    big_num();
    ~big_num();

    BIGNUM& bn();
    std::vector<unsigned char> get_bytes() const;
    std::vector<unsigned char> get_bytes(int bufferSize) const;

private:
    BIGNUM m_bn;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
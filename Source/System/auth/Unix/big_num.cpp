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
#include "big_num.h"

#include <vector>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

big_num::big_num()
{
    BN_init(&m_bn);
}

big_num::~big_num()
{
    BN_free(&m_bn);
}

BIGNUM&
big_num::bn()
{
    return m_bn;
}

std::vector<unsigned char>
big_num::get_bytes() const
{
    int len = BN_num_bytes(&m_bn);
    return get_bytes(len);
}

std::vector<unsigned char>
big_num::get_bytes(int bufferSize) const
{
    int len = BN_num_bytes(&m_bn);
    if (len > bufferSize) throw std::invalid_argument("bufferSize is not large enough");

    std::vector<unsigned char> bytes(bufferSize);
    BN_bn2bin(&m_bn, bytes.data() + bufferSize - len);
    return bytes;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
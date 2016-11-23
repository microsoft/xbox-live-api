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
#include "xsapi/system.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

class sha256
{
public:
    sha256(void);
    ~sha256(void);

    void add_bytes(const unsigned char* bytes, std::size_t cbBytes);
    void add_bytes(const std::vector<unsigned char>& bytes);
    std::vector<unsigned char> get_hash();

private:
    algorithm_provider m_provider;
    BCRYPT_HASH_HANDLE m_hashHandle; 
    std::unique_ptr<unsigned char[]> m_hashObject;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

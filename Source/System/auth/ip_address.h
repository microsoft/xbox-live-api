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

#include <string>

#include "shared_macros.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

enum class ip_address_type
{
    unknown,
    ipv4,
    ipv6
};

class ip_address
{
public:

    ip_address();
    ip_address(_In_ const string_t& ipStr);

    static bool try_parse(_In_ const string_t& ipStr, _In_ ip_address& ipAddr);

    ip_address_type get_type() const;
    const std::vector<unsigned char>& bytes() const;

    bool operator==(_In_ const ip_address& rhs) const;

private:

    ip_address(_In_ const std::vector<unsigned char>& bytes, _In_ ip_address_type type);

    std::vector<unsigned char> m_bytes;
    ip_address_type m_type;
};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

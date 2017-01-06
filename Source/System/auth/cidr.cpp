//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include <sstream>
#include <string>

#include "pch.h"
#include "ip_address.h"
#include "cidr.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

bool parse_helper(_In_ const string_t& cidrStr, _Out_ ip_address& addr, _Out_ int& prefix)
{
    std::size_t pos = cidrStr.find('/');
    if (string_t::npos == pos)
    {
        return false;
    }

    if (!ip_address::try_parse(cidrStr.substr(0, pos), addr))
    {
        return false;
    }

    stringstream_t(cidrStr.substr(pos + 1)) >> prefix;

    if (addr.get_type() == ip_address_type::ipv4) return prefix >= 0 && prefix <= 32;
    if (addr.get_type() == ip_address_type::ipv6) return prefix >= 0 && prefix <= 128;

    return true;
}

cidr::cidr()
{
}

cidr::cidr(_In_ const string_t& cidrStr)
{
    if (!parse_helper(cidrStr, m_addr, m_prefix))
    {
        throw std::invalid_argument("Invalid CIDR string");
    }
}

const ip_address& cidr::address() const
{
    return m_addr;
}

const int cidr::prefix_size() const
{
    return m_prefix;
}

bool cidr::try_parse(_In_ string_t cidrStr, _Out_ cidr& result)
{
    ip_address addr;
    int prefix;

    if (!parse_helper(cidrStr, addr, prefix))
    {
        return false;
    }

    result = cidr(addr, prefix);
    return true;
}

cidr::cidr(_In_ const ip_address& addr, _In_ int prefix) :
    m_addr(addr), m_prefix(prefix)
{
}

bool cidr::contains(_In_ const ip_address& ipAddr) const
{
    if (ipAddr.get_type() != m_addr.get_type())
    {
        return false;
    }

    const std::vector<unsigned char>& mask(m_addr.bytes());
    const std::vector<unsigned char>& ip(ipAddr.bytes());

    for (int i = 0; i < m_prefix / 8; ++i)
    {
        if (mask[i] != ip[i])
        {
            return false;
        }
    }

    int mod = m_prefix % 8;
    if (mod != 0)
    {
        int last = m_prefix / 8;
        unsigned char temp = (0xFF << (8 - mod));
        if ((mask[last] & temp) != (ip[last] & temp))
        {
            return false;
        }
    }

    return true;
}

bool cidr::operator==(_In_ const cidr& rhs) const
{
    return m_addr == rhs.m_addr && m_prefix == rhs.m_prefix;
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

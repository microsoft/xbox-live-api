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
#include "ip_address.h"
#include "shared_macros.h"

#include <string>
#include <sstream>
#include <vector>

#include <arpa/inet.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

bool parse_helper_unix(
    _In_ const string_t& ipStr,
    _In_ std::vector<unsigned char>& bytes, 
    ip_address_type& type
    )
{
    if (ipStr.find(':') != string_t::npos)
    {
        // IPv6
        struct in6_addr ipv6;
        if (inet_pton(AF_INET6, ipStr.c_str(), &ipv6) == 1)
        {
            bytes = std::vector<unsigned char>(
                (unsigned char*)&ipv6,
                ((unsigned char*)&ipv6) + sizeof(in6_addr));
            type = ip_address_type::ipv6;
            return true;
        }
    }
    else
    {
        // IPv4
        struct in_addr ipv4;
        if (inet_pton(AF_INET, ipStr.c_str(), &ipv4) == 1)
        {
            bytes = std::vector<unsigned char>(
                (unsigned char*)&ipv4,
                ((unsigned char*)&ipv4) + sizeof(in_addr));
            type = ip_address_type::ipv4;
            return true;
        }
    }

    return false;
}

ip_address::ip_address()
{
}

ip_address::ip_address(const string_t& ipStr)
{
    if (!parse_helper_unix(ipStr, m_bytes, m_type))
    {
        throw new std::runtime_error("Invalid IP address");
    }
}

ip_address::ip_address(const std::vector<unsigned char>& bytes, ip_address_type type) :
    m_bytes(bytes),
    m_type(type)
{
}

bool ip_address::try_parse(const string_t& ipStr, ip_address& ipAddr)
{
    std::vector<unsigned char> bytes;
    ip_address_type type;

    if (!parse_helper_unix(ipStr, bytes, type))
    {
        return false;
    }

    ipAddr = ip_address(bytes, type);
    return true;
}

ip_address_type ip_address::get_type() const
{
    return m_type;
}

const std::vector<unsigned char>& ip_address::bytes() const
{
    return m_bytes;
}

bool ip_address::operator==(const ip_address& rhs) const
{
    return
        m_type == rhs.m_type &&
        m_bytes == rhs.m_bytes;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

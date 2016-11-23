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

#include <string>
#include <sstream>
#include <vector>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

ip_address_type get_host_name_type(
    _In_ const string_t& ipStr
    );

bool parse_ipv4(
    _In_ const string_t& ipStr, 
    _In_ std::vector<unsigned char>& bytes)
{
    bytes.reserve(4);
    stringstream_t ss(ipStr);
    unsigned short octet;

    ss >> octet;
    bytes.push_back((unsigned char)octet);

    for (int i = 0; i < 3; i++)
    {
        ss.get(); // eat the dots
        ss >> octet;
        bytes.push_back((unsigned char)octet);
    }

    return true;
}

bool parse_ipv6(
    _In_ const string_t& ipStr, 
    _In_ std::vector<unsigned char>& bytes)
{
    bytes.reserve(16);
    
    std::vector<string_t> hexPairs;
    stringstream_t sstream(ipStr);

    while (!sstream.eof())
    {
        string_t pair;
        std::getline(sstream, pair, _T(':'));
        if (pair.empty() && hexPairs.size() > 0 && hexPairs.back().empty())
        {
            continue;
        }

        hexPairs.push_back(pair);
    }

    for (auto it = hexPairs.begin(); it != hexPairs.end(); ++it)
    {
        if (it->empty())
        {
            // fill in
            std::size_t numMissing = 8 - hexPairs.size() + 1;
            for (unsigned int i = 0; i < numMissing; ++i)
            {
                bytes.push_back(0);
                bytes.push_back(0);
            }

            continue;
        }

        unsigned short hexPair;
        stringstream_t(*it) >> std::hex >> hexPair;
        bytes.push_back((unsigned char)(0xFF & (hexPair >> 8)));
        bytes.push_back((unsigned char)(0XFF & hexPair));
    }

    return true;
}

bool parse_helper(
    _In_ const string_t& ipStr,
    _In_ std::vector<unsigned char>& bytes, 
    ip_address_type& type
    )
{
    type = get_host_name_type(ipStr);
    if (type == ip_address_type::ipv4)
    {
        return parse_ipv4(ipStr, bytes);
    }
    else if (type == ip_address_type::ipv6)
    {
        return parse_ipv6(ipStr, bytes);
    }

    return false;
}

ip_address::ip_address()
{
}

ip_address::ip_address(_In_ const string_t& ipStr)
{
    if (!parse_helper(ipStr, m_bytes, m_type))
    {
        throw std::runtime_error("Invalid IP address");
    }
}

ip_address::ip_address(_In_ const std::vector<unsigned char>& bytes, _In_ ip_address_type type) :
    m_bytes(bytes),
    m_type(type)
{
}

bool ip_address::try_parse(_In_ const string_t& ipStr, _In_ ip_address& ipAddr)
{
    std::vector<unsigned char> bytes;
    ip_address_type type;

    if (!parse_helper(ipStr, bytes, type))
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

bool ip_address::operator==(_In_ const ip_address& rhs) const
{
    return
        m_type == rhs.m_type &&
        m_bytes == rhs.m_bytes;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

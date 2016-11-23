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
#include "UnitTestBase.h"
#include "DefineTestMacros.h"
#include "Cidr.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#define TEST_CLASS_OWNER L"jameslao"

class CidrTests
{
public:
    TEST_CLASS(CidrTests);

    DEFINE_TEST_CASE_WITH_DESC(CidrIpv4ByteBoundaryPrefixContains, L"IPv4 CIDR range test where prefix is on a byte boundary")
    DEFINE_TEST_CASE_WITH_DESC(CidrIpv4NonByteBoundaryPrefixContains, L"IPv4 CIDR range test where prefix is not on a byte boundary")

    DEFINE_TEST_CASE_WITH_DESC(CidrIpv6ByteBoundaryPrefixContains, L"IPv6 CIDR range test where prefix is on a byte boundary")
    DEFINE_TEST_CASE_WITH_DESC(CidrIpv6NonByteBoundaryPrefixContains, L"IPv6 CIDR range test where prefix is not on a byte boundary")

    DEFINE_TEST_CASE_WITH_DESC(Ipv4CidrDoesNotContainIpv6, L"IPv4 CIDR range cannot contain an IPv6 address")
    DEFINE_TEST_CASE_WITH_DESC(Ipv6CidrDoesNotContainIpv4, L"IPv6 CIDR range cannot contain an IPv4 address")

    DEFINE_TEST_CASE_WITH_DESC(Ipv4CidrPrefixLength, L"IPv4 CIDR prefix size is 32 bits or less")
    DEFINE_TEST_CASE_WITH_DESC(Ipv6CidrPrefixLength, L"IPv6 CIDR prefix size is 128 bits or less")
};

#define VERIFY_CIDR_CONTAINS(cidrValue, ip) \
    VERIFY_IS_TRUE(cidr(cidrValue).contains(ip_address(ip)), L"CIDR range does not contain IP");

#define VERIFY_CIDR_NOT_CONTAINS(cidrValue, ip) \
    VERIFY_IS_FALSE(cidr(cidrValue).contains(ip_address(ip)), L"CIDR range contains IP");

void CidrTests::CidrIpv4ByteBoundaryPrefixContains()
{
    VERIFY_CIDR_CONTAINS(L"192.168.100.0/22", L"192.168.100.0");
    VERIFY_CIDR_CONTAINS(L"192.168.100.0/22", L"192.168.101.0");
    VERIFY_CIDR_CONTAINS(L"192.168.100.0/22", L"192.168.102.0");
    VERIFY_CIDR_CONTAINS(L"192.168.100.0/22", L"192.168.100.1");
    VERIFY_CIDR_CONTAINS(L"192.168.100.0/22", L"192.168.103.254");
    VERIFY_CIDR_CONTAINS(L"192.168.100.0/22", L"192.168.103.255");

    VERIFY_CIDR_NOT_CONTAINS(L"192.168.100.0/22", L"192.40.100.0");
    VERIFY_CIDR_NOT_CONTAINS(L"192.168.100.0/22", L"193.168.100.0");
    VERIFY_CIDR_NOT_CONTAINS(L"192.168.100.0/22", L"192.168.96.0");
    VERIFY_CIDR_NOT_CONTAINS(L"192.168.100.0/22", L"192.168.104.0");
}

void CidrTests::CidrIpv4NonByteBoundaryPrefixContains()
{
    VERIFY_CIDR_CONTAINS(L"192.168.0.0/16", L"192.168.0.0");
    VERIFY_CIDR_CONTAINS(L"192.168.0.0/16", L"192.168.1.0");
    VERIFY_CIDR_CONTAINS(L"192.168.0.0/16", L"192.168.0.1");

    VERIFY_CIDR_NOT_CONTAINS(L"192.168.0.0/16", L"192.167.0.0");
    VERIFY_CIDR_NOT_CONTAINS(L"192.168.0.0/16", L"193.168.0.0");
}

void CidrTests::CidrIpv6ByteBoundaryPrefixContains()
{
    VERIFY_CIDR_CONTAINS(L"2001:db8::/48", L"2001:db8:0:0:0:0:0:0");
    VERIFY_CIDR_CONTAINS(L"2001:db8::/48", L"2001:db8:0:1:0:0:0:0");
    VERIFY_CIDR_CONTAINS(L"2001:db8::/48", L"2001:db8:0:0:1:0:0:0");
    VERIFY_CIDR_CONTAINS(L"2001:db8::/48", L"2001:db8:0:0:0:1:0:0");
    VERIFY_CIDR_CONTAINS(L"2001:db8::/48", L"2001:db8:0:0:0:0:1:0");
    VERIFY_CIDR_CONTAINS(L"2001:db8::/48", L"2001:db8:0:0:0:0:0:1");
    VERIFY_CIDR_CONTAINS(L"2001:db8::/48", L"2001:db8:0:ffff:ffff:ffff:ffff:ffff");

    VERIFY_CIDR_NOT_CONTAINS(L"2001:db8::/48", L"2001:db8:1:0:0:0:0:0");
    VERIFY_CIDR_NOT_CONTAINS(L"2001:db8::/48", L"2001:1db8::");
    VERIFY_CIDR_NOT_CONTAINS(L"2001:db8::/48", L"2002:db8::");
}

void CidrTests::CidrIpv6NonByteBoundaryPrefixContains()
{
    // 0xa0 = 0b1010 0000
    VERIFY_CIDR_CONTAINS(L"2001:a000::/19", L"2001:a000:0:0:0:0:0:0");
    VERIFY_CIDR_CONTAINS(L"2001:a000::/19", L"2001:b000:0:0:0:0:0:0"); // 0xb0 = 0b1011 0000
    VERIFY_CIDR_CONTAINS(L"2001:a000::/19", L"2001:a800:0:0:0:0:0:0"); // 0xa8 = 0b1010 1000

    VERIFY_CIDR_NOT_CONTAINS(L"2001:a000::/19", L"2001:8000:0:0:0:0:0:0"); // 0x80 = 0b1000 0000
    VERIFY_CIDR_NOT_CONTAINS(L"2001:a000::/19", L"2001:e000:0:0:0:0:0:0"); // 0x80 = 0b1110 0000
    VERIFY_CIDR_NOT_CONTAINS(L"2001:a000::/19", L"2001:2000:0:0:0:0:0:0"); // 0x80 = 0b0010 0000

    VERIFY_CIDR_NOT_CONTAINS(L"2001:a000::/19", L"2002:a000:0:0:0:0:0:0");
}

void CidrTests::Ipv4CidrDoesNotContainIpv6()
{
    VERIFY_CIDR_NOT_CONTAINS(L"192.168.0.0/16", L"c0a8::");
}

void CidrTests::Ipv6CidrDoesNotContainIpv4()
{
    VERIFY_CIDR_NOT_CONTAINS(L"2001::/16", L"32.1.0.0");
}

void CidrTests::Ipv4CidrPrefixLength()
{
    ip_address expectedAddr(L"192.168.0.0");

    for (int i = 0; i <= 32; ++i)
    {
        std::wstringstream ss;
        ss << L"192.168.0.0/";
        ss << i;
        cidr cidr(ss.str());
        VERIFY_ARE_EQUAL(i, cidr.prefix_size());
        VERIFY_ARE_EQUAL(expectedAddr, cidr.address());
    }

    VERIFY_THROWS(cidr(L"192.168.0.0/33"), std::invalid_argument);
    VERIFY_THROWS(cidr(L"192.168.0.0/-1"), std::invalid_argument);
}

void CidrTests::Ipv6CidrPrefixLength()
{
    ip_address expectedAddr(L"2001:a000::");

    for (int i = 0; i <= 128; ++i)
    {
        std::wstringstream ss;
        ss << L"2001:a000::/";
        ss << i;
        cidr cidr(ss.str());
        VERIFY_ARE_EQUAL(i, cidr.prefix_size());
        VERIFY_ARE_EQUAL(expectedAddr, cidr.address());
    }

    VERIFY_THROWS(cidr(L"2001:a000::/129"), std::invalid_argument);
    VERIFY_THROWS(cidr(L"2001:a000::/-1"), std::invalid_argument);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


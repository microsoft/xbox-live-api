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
#include "ip_address.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#define TEST_CLASS_OWNER L"jameslao"

class IpAddressTests
{
public:
    TEST_CLASS(IpAddressTests);

    DEFINE_TEST_CASE_WITH_DESC(Ipv4Test, L"IPv4 address parsing test")
    DEFINE_TEST_CASE_WITH_DESC(Ipv6Test, L"IPv6 address parsing test")
};

#define VERIFY_IP_BYTES(expected, ipStr) \
    VERIFY_ARE_EQUAL(expected, xbox::services::system::ip_address(ipStr).bytes());

void IpAddressTests::Ipv4Test()
{
    std::vector<unsigned char> expected{ 192, 168, 1, 1 };
    VERIFY_IP_BYTES(expected, L"192.168.1.1");

    expected = std::vector<unsigned char> { 0, 0, 0, 0 };
    VERIFY_IP_BYTES(expected, L"0.0.0.0");
}

void IpAddressTests::Ipv6Test()
{
    std::vector<unsigned char> expected{ 32, 1, 13, 184, 133, 163, 0, 0, 0, 0, 138, 46, 3, 112, 115, 52 };

    VERIFY_IP_BYTES(expected, L"2001:0db8:85a3:0000:0000:8a2e:0370:7334");
    VERIFY_IP_BYTES(expected, L"2001:db8:85a3:0:0:8a2e:370:7334");
    VERIFY_IP_BYTES(expected, L"2001:db8:85a3::8a2e:370:7334");

    expected = std::vector<unsigned char> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    VERIFY_IP_BYTES(expected, L"::");
    VERIFY_IP_BYTES(expected, L"0:0:0:0:0:0:0:0");

    expected = std::vector<unsigned char> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

    VERIFY_IP_BYTES(expected, L"::1");
    VERIFY_IP_BYTES(expected, L"0:0:0:0:0:0:0:1");

    expected = std::vector<unsigned char> { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1 };

    VERIFY_IP_BYTES(expected, L"::1:1");
    VERIFY_IP_BYTES(expected, L"0:0:0:0:0:0:1:1");

    expected = std::vector<unsigned char> { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    VERIFY_IP_BYTES(expected, L"1::");
    VERIFY_IP_BYTES(expected, L"1:0:0:0:0:0:0:0");

    expected = std::vector<unsigned char> { 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    VERIFY_IP_BYTES(expected, L"1:1::");
    VERIFY_IP_BYTES(expected, L"1:1:0:0:0:0:0:0");
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


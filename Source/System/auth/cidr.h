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
#include "ip_address.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN


/// <summary>
/// This represents an IP address and it's routing prefix in CIDR notation.
/// It is used in the NSAL for specifying IP ranges.
/// http://en.wikipedia.org/wiki/Classless_Inter-Domain_Routing
/// </summary>
class cidr
{
public:

    cidr();
    cidr(_In_ const string_t& cidrStr);

    static bool try_parse(_In_ string_t cidrStr, _Out_ cidr& result);

    const ip_address& address() const;
    const int prefix_size() const;

    /// <summary>
    /// Tests whether ipAddr is contained in the IP range.
    /// </summary>
    /// <param name="ipAddr">The IP address to test</param>
    /// <returns>true if the address is contained in the range and false otherwise</returns>
    bool contains(_In_ const ip_address& ipAddr) const;

    bool operator==(_In_ const cidr& rhs) const;

private:

    cidr(_In_ const ip_address& addr, _In_ int prefix);

    ip_address m_addr;
    int m_prefix;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include <string>
#include <sstream>
#include "pch.h"
#if !XSAPI_I
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include "ip_address.h"
#include "Utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

ip_address_type get_host_name_type(
    _In_ const string_t& ipStrT
    )
{
#ifdef _WIN32
    std::string ipStr = utility::conversions::utf16_to_utf8( ipStrT );
#else
    std::string ipStr = ipStrT;
#endif

    struct in_addr addr;
    addr.s_addr = inet_addr(ipStr.c_str());
    if (addr.s_addr == INADDR_NONE) 
    {
        return ip_address_type::unknown;
    }
    else
    {
        struct hostent* remoteHost;
        remoteHost = gethostbyaddr((char*)&addr, 4, AF_INET);
        if (remoteHost == NULL)
        {
            return ip_address_type::unknown;
        }
        else
        {
            switch (remoteHost->h_addrtype)
            {
                case AF_INET: return ip_address_type::ipv4;
                case AF_INET6: return ip_address_type::ipv6;
                default: return ip_address_type::unknown;
            }
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
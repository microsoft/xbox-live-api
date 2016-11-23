//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include <algorithm>
#include <string>

#include "pch.h"
#include "nsal_endpoint.h"
#include "utils.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

nsal_endpoint_info::nsal_endpoint_info()
{
}

nsal_endpoint_info::nsal_endpoint_info(
    _In_ const string_t relyingParty,
    _In_ const string_t subRelyingParty,
    _In_ const string_t tokenType, 
    _In_ int signaturePolicyIndex) :
    m_relyingParty(std::move(relyingParty)),
    m_subRelyingParty(std::move(subRelyingParty)),
    m_tokenType(std::move(tokenType)),
    m_signaturePolicyIndex(signaturePolicyIndex)
{
}

const string_t& nsal_endpoint_info::relying_party() const
{
    return m_relyingParty;
}

const string_t& nsal_endpoint_info::sub_relying_party() const
{
    return m_subRelyingParty;
}

const string_t& nsal_endpoint_info::token_type() const
{
    return m_tokenType;
}

int nsal_endpoint_info::signature_policy_index() const
{
    return m_signaturePolicyIndex;
}

bool nsal_endpoint_info::operator==(
    _In_ const nsal_endpoint_info& rhs) const
{
    return
        m_relyingParty == rhs.m_relyingParty &&
        m_subRelyingParty == rhs.m_subRelyingParty &&
        m_tokenType == rhs.m_tokenType &&
        m_signaturePolicyIndex == rhs.m_signaturePolicyIndex;
}

nsal_endpoint::nsal_endpoint(
    _In_ nsal_protocol protocol,
    _In_ nsal_host_name_type hostNameType,
    _In_ int port) :
    m_protocol(protocol),
    m_hostNameType(hostNameType),
    m_port(port)
{
}

nsal_protocol nsal_endpoint::protocol() const
{
    return m_protocol;
}

nsal_host_name_type nsal_endpoint::host_name_type() const
{
    return m_hostNameType;
}

int nsal_endpoint::port() const
{
    return m_port;
}

void nsal_endpoint::add_info(
    _In_ const string_t& path,
    _In_ const nsal_endpoint_info& info
    )
{
    m_paths.add(path, info);
}

bool nsal_endpoint::get_info(
    _In_ const string_t& path, 
    _In_ nsal_endpoint_info& info ) const
{
    return m_paths.get(path, info);
}

bool nsal_endpoint::get_info_for_exact_path(
    _In_ const string_t& path, 
    _In_ nsal_endpoint_info& info ) const
{
    return m_paths.get_exact(path, info);
}

bool nsal_endpoint::is_match(
    _In_ nsal_protocol protocol, 
    _In_ int port) const
{
    return
        m_protocol == protocol &&
        m_port == port;
}

fqdn_nsal_endpoint::fqdn_nsal_endpoint(
    _In_ nsal_protocol protocol,
    _In_ const string_t& hostName,
    _In_ nsal_host_name_type hostNameType,
    _In_ int port) :
    nsal_endpoint(protocol, hostNameType, port), 
    m_hostName(std::move(hostName))
{
}

bool fqdn_nsal_endpoint::is_match(
    _In_ nsal_protocol protocol, 
    _In_ const string_t& hostName, 
    _In_ int port) const
{
    return
        nsal_endpoint::is_match(protocol, port) &&
        utils::str_icmp(m_hostName, hostName) == 0;
}

bool fqdn_nsal_endpoint::is_same(
    _In_ nsal_protocol protocol, 
    _In_ const string_t& hostName, 
    _In_ int port) const
{
    return is_match(protocol, hostName, port);
}

wildcard_nsal_endpoint::wildcard_nsal_endpoint(
    _In_ nsal_protocol protocol,
    _In_ const string_t& hostName,
    _In_ nsal_host_name_type hostNameType,
    _In_ int port) :
    nsal_endpoint(protocol, hostNameType, port)
{
    // regex: "\*\..+"
    if (hostName.length() < 3 || hostName[0] != '*' || hostName[1] != '.')
    {
        throw std::invalid_argument("Invalid wildcard host name");  
    }

    // We don't store the initial '*'
    m_hostName = hostName.substr(1);
}

bool wildcard_nsal_endpoint::is_match(
    _In_ nsal_protocol protocol, 
    _In_ const string_t& hostName, 
    _In_ int port) const
{
    if (!nsal_endpoint::is_match(protocol, port))
    {
        return false;
    }

    std::size_t pos = hostName.find(m_hostName);
    if (pos == string_t::npos)
    {
        return false;
    }

    return m_hostName.size() == (hostName.size() - pos);
}

bool wildcard_nsal_endpoint::is_same(
    _In_ nsal_protocol protocol, 
    _In_ const string_t& hostName, 
    _In_ int port) const
{
    return
        nsal_endpoint::is_match(protocol, port) &&
        utils::str_icmp(m_hostName, hostName) == 0;
}

uint64_t wildcard_nsal_endpoint::length() const
{
    return m_hostName.length();
}

ip_nsal_endpoint::ip_nsal_endpoint(
    _In_ nsal_protocol protocol,
    _In_ const string_t& hostName,
    _In_ nsal_host_name_type hostNameType,
    _In_ int port) :
    nsal_endpoint(protocol, hostNameType, port),
    m_ipAddress(hostName)
{
}

bool ip_nsal_endpoint::is_match(
    _In_ nsal_protocol protocol, 
    _In_ const ip_address& hostName, 
    _In_ int port) const
{
    return nsal_endpoint::is_match(protocol, port) && m_ipAddress == hostName;
}

bool ip_nsal_endpoint::is_same(
    _In_ nsal_protocol protocol, 
    _In_ const string_t& hostName, 
    _In_ int port) const
{
    return nsal_endpoint::is_match(protocol, port) && m_ipAddress == ip_address(hostName);
}

cidr_nsal_endpoint::cidr_nsal_endpoint(
    _In_ nsal_protocol protocol,
    _In_ const string_t& hostName,
    _In_ nsal_host_name_type hostNameType,
    _In_ int port) :
    nsal_endpoint(protocol, hostNameType, port),
    m_cidr(hostName)
{
}

bool cidr_nsal_endpoint::is_match(
    _In_ nsal_protocol protocol, 
    _In_ const ip_address& hostName, 
    _In_ int port) const
{
    return nsal_endpoint::is_match(protocol, port) && m_cidr.contains(hostName);
}

bool cidr_nsal_endpoint::is_same(
    _In_ nsal_protocol protocol, 
    _In_ const string_t& hostName, 
    _In_ int port) const
{
    return nsal_endpoint::is_match(protocol, port) && m_cidr == cidr(hostName);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

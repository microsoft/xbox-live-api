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
#include "trie.h"
#include "ip_address.h"
#include "Cidr.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

enum class nsal_protocol
{
    unknown,
    http,
    https,
    tcp,
    udp,
    wss
};

enum class nsal_host_name_type
{
    unknown,
    fqdn,
    wildcard,
    ip,
    cidr
};

class nsal_endpoint_info
{
public:
    nsal_endpoint_info();
    nsal_endpoint_info(
        _In_ string_t relyingParty,
        _In_ string_t subRelyingParty,
        _In_ string_t tokenType,
        _In_ int signaturePolicyIndex
        );

    const string_t& relying_party() const;
    const string_t& sub_relying_party() const;
    const string_t& token_type() const;
    int signature_policy_index() const;

    bool operator==(_In_ const nsal_endpoint_info& rhs) const;

private:

    string_t m_relyingParty;
    string_t m_subRelyingParty;
    string_t m_tokenType;
    int m_signaturePolicyIndex;
};

class nsal_endpoint
{
public:
    nsal_endpoint(
        _In_ nsal_protocol protocol,
        _In_ nsal_host_name_type hostNameType,
        _In_ int port
        );

    nsal_protocol protocol() const;
    nsal_host_name_type host_name_type() const;
    int port() const;

    void add_info(_In_ const string_t& path, _In_ const nsal_endpoint_info& info);
    bool get_info(_In_ const string_t& path, _In_ nsal_endpoint_info& info) const;
    bool get_info_for_exact_path(_In_ const string_t& path, _In_ nsal_endpoint_info& info) const;

    /// <summary>
    /// Tests whether protocol, hostName, and port identify this endpoint. Note
    /// that this is different from IsMatch() which matches using the semantics of
    /// the host name type. For instance, '*.contoso.com' wildcard will match any
    /// subdomain of contoso.com. Where as Same() will only return true if hostName
    /// is '*.contoso.com'. This is used only during the construction of the NSAL
    /// to resolve duplicates.
    /// </summary>
    virtual bool is_same(_In_ nsal_protocol protocol, _In_ const string_t& hostName, _In_ int port) const = 0;

protected:

    nsal_protocol m_protocol;
    nsal_host_name_type m_hostNameType;
    int m_port;
    trie<nsal_endpoint_info> m_paths;

    bool is_match(_In_ nsal_protocol protocol, _In_ int port) const;

};

class fqdn_nsal_endpoint : public nsal_endpoint
{
public:
    fqdn_nsal_endpoint(
        _In_ nsal_protocol protocol,
        _In_ const string_t& hostName,
        _In_ nsal_host_name_type hostNameType,
        _In_ int port);

    bool is_match(_In_ nsal_protocol protocol, _In_ const string_t& hostName, _In_ int port) const;
    bool is_same(_In_ nsal_protocol protocol, _In_ const string_t& hostName, _In_ int port) const override;

private:
    string_t m_hostName;
};

class wildcard_nsal_endpoint : public nsal_endpoint
{
public:
    wildcard_nsal_endpoint(
        _In_ nsal_protocol protocol,
        _In_ const string_t& hostName,
        _In_ nsal_host_name_type hostNameType,
        _In_ int port);

    bool is_match(_In_ nsal_protocol protocol, _In_ const string_t& hostName, _In_ int port) const;
    bool is_same(_In_ nsal_protocol protocol, _In_ const string_t& hostName, _In_ int port) const override;
    uint64_t length() const;
private:
    string_t m_hostName;
};

class ip_nsal_endpoint : public nsal_endpoint
{
public:
    ip_nsal_endpoint(
        _In_ nsal_protocol protocol,
        _In_ const string_t& hostName,
        _In_ nsal_host_name_type hostNameType,
        _In_ int port);

    bool is_match(_In_ nsal_protocol protocol, _In_ const ip_address& hostName, _In_ int port) const;
    bool is_same(_In_ nsal_protocol protocol, _In_ const string_t& hostName, _In_ int port) const override;

private:
    ip_address m_ipAddress;
};

class cidr_nsal_endpoint : public nsal_endpoint
{
public:
    cidr_nsal_endpoint(
        _In_ nsal_protocol protocol,
        _In_ const string_t& hostName,
        _In_ nsal_host_name_type hostNameType,
        _In_ int port);

    bool is_match(_In_ nsal_protocol protocol, _In_ const ip_address& hostName, _In_ int port) const;
    bool is_same(_In_ nsal_protocol protocol, _In_ const string_t& hostName, _In_ int port) const override;

private:
    cidr m_cidr;
};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif


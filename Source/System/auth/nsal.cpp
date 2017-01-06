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
#include "nsal.h"
#include "xtitle_service.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif


template<typename T>
void add_endpoint_helper(
    _In_ std::vector<T>& endpoints,
    _In_ nsal_protocol protocol,
    _In_ const string_t& hostName,
    _In_ nsal_host_name_type hostNameType,
    _In_ int port,
    _In_ const string_t&  path,
    _In_ const string_t& relyingParty,
    _In_ const string_t& subRelyingParty,
    _In_ const string_t& tokenType,
    _In_ int signaturePolicyIndex)
{
    // First check if there's already a match
    nsal_endpoint_info newInfo(relyingParty, subRelyingParty, tokenType, signaturePolicyIndex);

    for (auto endpoint = endpoints.begin(); endpoint != endpoints.end(); endpoint++)
    {
        if (endpoint->is_same(protocol, hostName, port))
        {
            nsal_endpoint_info existingInfo;
            if (endpoint->get_info_for_exact_path(path, existingInfo))
            {
                // The endpoint already exists, make sure all the info about it matches
                if (existingInfo == newInfo)
                {
                    return;
                }
                else
                {
                    throw std::runtime_error("endpoints conflict");
                }
            }
            else
            {
                // The specific path does not exist so add it
                endpoint->add_info(path, newInfo);
                return;
            }
        }
    }

    // No matching endpoints so we create a new one.
    endpoints.emplace_back(protocol, hostName, hostNameType, port);
    endpoints.back().add_info(path, newInfo);
}

void nsal::add_endpoint(
    _In_ nsal_protocol protocol,
    _In_ const string_t& hostName,
    _In_ nsal_host_name_type hostNameType,
    _In_ int port,
    _In_ const string_t& path,
    _In_ const string_t& relyingParty,
    _In_ const string_t& subRelyingParty,
    _In_ const string_t& tokenType,
    _In_ int signaturePolicyIndex)
{
    switch (hostNameType)
    {
    case nsal_host_name_type::fqdn:
        add_endpoint_helper<fqdn_nsal_endpoint>(
            m_fqdnEndpoints,
            protocol,
            hostName,
            hostNameType,
            port,
            path,
            relyingParty,
            subRelyingParty,
            tokenType,
            signaturePolicyIndex);
        break;

    case nsal_host_name_type::wildcard:
        add_endpoint_helper<wildcard_nsal_endpoint>(
            m_wildcardEndpoints,
            protocol,
            hostName,
            hostNameType,
            port,
            path,
            relyingParty,
            subRelyingParty,
            tokenType,
            signaturePolicyIndex);
        break;

    case nsal_host_name_type::ip:
        add_endpoint_helper<ip_nsal_endpoint>(
            m_ipEndpoints,
            protocol,
            hostName,
            hostNameType,
            port,
            path,
            relyingParty,
            subRelyingParty,
            tokenType,
            signaturePolicyIndex);
        break;

    case nsal_host_name_type::cidr:
        add_endpoint_helper<cidr_nsal_endpoint>(
            m_cidrEndpoints,
            protocol,
            hostName,
            hostNameType,
            port,
            path,
            relyingParty,
            subRelyingParty,
            tokenType,
            signaturePolicyIndex);
        break;

    default:
        throw std::runtime_error("unsupported host name type");
    }
}

template<typename TEndpoint, typename THostName>
bool get_helper(
    _In_ const std::vector<TEndpoint>& endpoints,
    _In_ nsal_protocol protocol,
    _In_ const THostName& hostName,
    _In_ int port,
    _In_ const string_t& path,
    _Out_ nsal_endpoint_info& info)
{
    for (auto endpoint = endpoints.begin(); endpoint != endpoints.end(); endpoint++)
    {
        if (endpoint->is_match(protocol, hostName, port))
        {
            return endpoint->get_info(path, info);
        }
    }

    return false;
}

bool nsal::get_endpoint(
    _In_ web::http::uri& uri,
    _Out_ nsal_endpoint_info& info) const
{
    nsal_protocol protocol = nsal::deserialize_protocol(uri.scheme());
    int port = nsal::get_port(protocol, uri.port());
    return get_endpoint(protocol, uri.host(), port, uri.path(), info);
}

bool nsal::get_endpoint(
    _In_ nsal_protocol protocol,
    _In_ const string_t& hostName,
    _In_ int port,
    _In_ const string_t& path,
    _Out_ nsal_endpoint_info& info) const
{
    string_t hostNameWithoutEnv = hostName;

    string_t dnet = _T(".dnet");
    size_t f = hostNameWithoutEnv.find(dnet);
    if ( f != string_t::npos )
    {
        hostNameWithoutEnv.replace(f, dnet.length(), _T(""));
    }

    ip_address ipAddr;
    if (ip_address::try_parse(hostNameWithoutEnv, ipAddr))
    {
        if (get_helper(m_ipEndpoints, protocol, ipAddr, port, path, info))
        {
            return true;
        }

        if (get_helper(m_cidrEndpoints, protocol, ipAddr, port, path, info))
        {
            return true;
        }
    }
    else
    {
        if (get_helper(m_fqdnEndpoints, protocol, hostNameWithoutEnv, port, path, info))
        {
            return true;
        }

        if (get_helper(m_wildcardEndpoints, protocol, hostNameWithoutEnv, port, path, info))
        {
            return true;
        }
    }

    return false;
}

void nsal::add_signature_policy(_In_ const signature_policy& signaturePolicy)
{
    m_signaturePolicies.push_back(signaturePolicy);
}

const signature_policy& nsal::get_signature_policy(_In_ int index) const
{
    return m_signaturePolicies[index];
}

nsal_protocol nsal::deserialize_protocol(_In_ const utility::string_t& str)
{
    if (_T("https") == str) return nsal_protocol::https;
    if (_T("http") == str) return nsal_protocol::http;
    if (_T("tcp") == str) return nsal_protocol::tcp;
    if (_T("udp") == str) return nsal_protocol::udp;
    if (_T("wss") == str) return nsal_protocol::wss;
    throw web::json::json_exception(_T("Invalid protocol for NSAL endpoint"));
}

nsal_host_name_type nsal::deserialize_host_name_type(_In_ const utility::string_t& str)
{
    if (_T("fqdn") == str) return nsal_host_name_type::fqdn;
    if (_T("wildcard") == str) return nsal_host_name_type::wildcard;
    if (_T("ip") == str) return nsal_host_name_type::ip;
    if (_T("cidr") == str) return nsal_host_name_type::cidr;
    throw web::json::json_exception(_T("Invalid NSAL host name type"));
}

int nsal::deserialize_port(_In_ nsal_protocol protocol, _In_ const web::json::value& json)
{
    int port = utils::extract_json_int(json, _T("Port"), false, 0);
    return nsal::get_port(protocol, port);
}

int nsal::get_port(
    _In_ nsal_protocol protocol,
    _In_ int port
    )
{
    if (port == 0)
    {
        switch (protocol)
        {
        case nsal_protocol::https:
            return 443;
        case nsal_protocol::http:
        case nsal_protocol::wss:
            return 80;
        default:
            throw web::json::json_exception(_T("Must specify port when protocol is not http or https"));
        }
    }

    return port;
}

void
nsal::deserialize_endpoint(
    _In_ nsal& nsal,
    _In_ const web::json::value& endpoint)
{
    utility::string_t relyingParty(utils::extract_json_string(endpoint, _T("RelyingParty")));
    if (relyingParty.empty())
    {
        // If there's no RP, we don't care since there's no token to attach
        return;
    }
    utility::string_t subRelyingParty(utils::extract_json_string(endpoint, _T("SubRelyingParty")));

    nsal_protocol protocol(nsal::deserialize_protocol(utils::extract_json_string(endpoint, _T("Protocol"), true)));
    nsal_host_name_type hostNameType(deserialize_host_name_type(utils::extract_json_string(endpoint, _T("HostType"), true)));
    int port = deserialize_port(protocol, endpoint);
    nsal.add_endpoint(
        protocol,
        utils::extract_json_string(endpoint, _T("Host"), true),
        hostNameType,
        port,
        utils::extract_json_string(endpoint, _T("Path")),
        relyingParty,
        subRelyingParty,
        utils::extract_json_string(endpoint, _T("TokenType"), true),
        utils::extract_json_int(endpoint, _T("SignaturePolicyIndex"), false, -1));
}

void 
nsal::deserialize_signature_policy(_In_ nsal& nsal, _In_ const web::json::value& json)
{
    std::error_code errc = xbox_live_error_code::no_error;
    int version = utils::extract_json_int(json, _T("Version"), true);
    int maxBodyBytes = utils::extract_json_int(json, _T("MaxBodyBytes"), true);
    std::vector<string_t> extraHeaders(
        utils::extract_json_vector<string_t>(utils::json_string_extractor, json, _T("ExtraHeaders"), errc, false));

    nsal.add_signature_policy(signature_policy(version, maxBodyBytes, extraHeaders));
}

nsal
nsal::deserialize(_In_ const web::json::value& json)
{
    nsal nsal;
    auto& jsonObj(json.as_object());

    auto it = jsonObj.find(_T("SignaturePolicies"));
    if (it != jsonObj.end())
    {
        web::json::array signaturePolicies(it->second.as_array());
        for (auto it2 = signaturePolicies.begin(); it2 != signaturePolicies.end(); ++it2)
        {
            deserialize_signature_policy(nsal, *it2);
        }
    }

    it = jsonObj.find(_T("EndPoints"));
    if (it != jsonObj.end())
    {
        web::json::array endpoints(it->second.as_array());
        for (auto it2 = endpoints.begin(); it2 != endpoints.end(); ++it2)
        {
            deserialize_endpoint(nsal, *it2);
        }
    }

    nsal.add_endpoint(
        nsal_protocol::wss,
        _T("*.xboxlive.com"),
        nsal_host_name_type::wildcard,
        80,
        string_t(),
        _T("http://xboxlive.com"),
        string_t(),
        _T("JWT"),
        0
        );
    
    nsal._Sort_wildcard_endpoints();
    return nsal;
}

void nsal::_Sort_wildcard_endpoints()
{
    std::sort(
        m_wildcardEndpoints.begin(),
        m_wildcardEndpoints.end(),
        [](_In_ const wildcard_nsal_endpoint& lhs, _In_ const wildcard_nsal_endpoint& rhs) -> bool
        {
            return lhs.length() > rhs.length();
        });
}

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

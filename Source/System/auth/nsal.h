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

#include <vector>

#include "nsal_endpoint.h"
#include "signature_policy.h"

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_BEGIN
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN
#endif

class nsal
{
public:

    /// <summary>
    /// Adds an endpoint to the NSAL.
    /// </summary>
    /// <param name="protocol">The protocol for the endpoint.</param>
    /// <param name="hostName">The host name for the endpoint.</param>
    /// <param name="hostNameType">The format of hostName (e.g. fqdn, wildcard, etc).</param>
    /// <param name="port">The port for the endpoint. 0 if no port specified.</param>
    /// <param name="path">The path to the resource when protocol is http or https.</param>
    /// <param name="relyingParty">The relying party the endpoint accepts. Only valid for https endpoints.</param>
    /// <param name="tokenType">The token type the endpoint accepts.</param>
    /// <param name="signaturePolicyIndex">The index of the signature policy to apply. Pass -1 for no policy.</param>
    void add_endpoint(
        _In_ nsal_protocol protocol,
        _In_ const string_t& hostName,
        _In_ nsal_host_name_type hostNameType,
        _In_ int port,
        _In_ const string_t& path,
        _In_ const string_t& relyingParty,
        _In_ const string_t& subRelyingParty,
        _In_ const string_t& tokenType,
        _In_ int signaturePolicyIndex);

    /// <summary>
    /// Gets the info for the endpoint.
    /// </summary>
    /// <param name="protocol">The protocol for the endpoint.</param>
    /// <param name="hostName">The host name for the endpoint.</param>
    /// <param name="port">The port for the endpoint. 0 if no port specified.</param>
    /// <param name="info">Reference to place to store the result if successful.</param>
    /// <returns>true if information about the endpoint is found. false otherwise.</returns>
    bool get_endpoint(
        _In_ nsal_protocol protocol,
        _In_ const string_t& hostName,
        _In_ int port,
        _In_ const string_t& path,
        _Out_ nsal_endpoint_info& info) const;

    bool get_endpoint(
        _In_ web::http::uri& uri,
        _Out_ nsal_endpoint_info& info) const;

    /// <summary>
    /// Adds a signature policy.
    /// </summary>
    /// <param name="signaturePolicy">The signature policy to add.</param>
    void add_signature_policy(
        _In_ const signature_policy& signaturePolicy);

    /// <summary>
    /// Gets the signature policy at index.
    /// </summary>
    /// <param name="index">The signature policy.</param>
    const signature_policy& get_signature_policy(
        _In_ int index) const;
    
    // deserialize helpers
    static nsal deserialize(_In_ const web::json::value& json);
    static void deserialize_endpoint(
        _In_ nsal& nsal,
        _In_ const web::json::value& endpoint
        );
    static void deserialize_signature_policy(
        _In_ nsal& nsal,
        _In_ const web::json::value& json
        );
    static nsal_protocol deserialize_protocol(_In_ const utility::string_t& str);
    static nsal_host_name_type deserialize_host_name_type(_In_ const utility::string_t& str);
    static int deserialize_port(_In_ nsal_protocol protocol, _In_ const web::json::value& json);
    static int get_port(_In_ nsal_protocol protocol, _In_ int port);

    /// <summary>
    /// Internal Function
    /// </summary>
    void _Sort_wildcard_endpoints();
    
private:
    std::vector<fqdn_nsal_endpoint> m_fqdnEndpoints;
    std::vector<wildcard_nsal_endpoint> m_wildcardEndpoints;
    std::vector<ip_nsal_endpoint> m_ipEndpoints;
    std::vector<cidr_nsal_endpoint> m_cidrEndpoints;
    std::vector<signature_policy> m_signaturePolicies;

};

#if BEAM_API
NAMESPACE_MICROSOFT_XBOX_SERVICES_BEAM_SYSTEM_CPP_END
#else
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
#endif

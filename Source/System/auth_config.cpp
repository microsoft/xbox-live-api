// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi_utils.h"
#include "auth_config.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN


xsapi_internal_string get_endpoint_path(
    _In_ const xsapi_internal_string& serviceName,
    _In_ const xsapi_internal_string& environmentPrefix,
    _In_ const xsapi_internal_string& environment,
    _In_ bool appendProtocol = true
    )
{
    xsapi_internal_stringstream source;
    if (appendProtocol)
    {
        source << "https://";
    }
    source << environmentPrefix;
    source << serviceName;
    source << environment;
    source << ".xboxlive.com";
    return source.str();
}

auth_config::auth_config() :
    m_useCompactTicket(false),
    m_detailError(0),
    m_httpStatusCode(0)
{
    m_xtokenComposition = { token_identity_type::u_token, token_identity_type::d_token, token_identity_type::t_token };
}

auth_config::auth_config(
    _In_ xsapi_internal_string sandbox,
    _In_ xsapi_internal_string environmentPrefix,
    _In_ xsapi_internal_string environment,
    _In_ bool useCompactTicket,
    _In_ bool isCreatorsTitle,
    _In_ xsapi_internal_string scope) :
    m_useCompactTicket(useCompactTicket),
    m_sandbox(std::move(sandbox)),
    m_detailError(0),
    m_rpsTicketPolicy(useCompactTicket? "MBI_SSL" : "DELEGATION"),
    m_xboxLiveRelyingParty("http://auth.xboxlive.com"),
    m_xboxLiveEndpoint("https://xboxlive.com"),
    m_environment(std::move(environment))
{
    m_xTitleEndpoint = get_endpoint_path("title.mgt", environmentPrefix, environment);
    m_deviceTokenEndpoint = get_endpoint_path("device.auth", environmentPrefix, environment);
    m_titleTokenEndpoint = get_endpoint_path("title.auth", environmentPrefix, environment);
    m_userTokenEndpoint = get_endpoint_path("user.auth", environmentPrefix, environment);
    m_serviceTokenEndpoint = get_endpoint_path("service.auth", environmentPrefix, environment);
    m_xTokenEndpoint = get_endpoint_path("xsts.auth", environmentPrefix, environment);
    m_userTokenSiteName = get_endpoint_path("open-user.auth", "", environment, false);
    m_rpsTicketService = isCreatorsTitle ? scope : (useCompactTicket ? m_userTokenSiteName : scope);
    m_xtokenComposition = { token_identity_type::u_token, token_identity_type::d_token, token_identity_type::t_token };
}

const xsapi_internal_string& auth_config::rps_ticket_service() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_rpsTicketService;
}

void auth_config::set_rps_ticket_service(
    _In_ xsapi_internal_string value
    )
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_rpsTicketService = std::move(value);
}

const xsapi_internal_string& auth_config::rps_ticket_policy() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_rpsTicketPolicy;
}

void auth_config::set_rps_ticket_policy(
    _In_ xsapi_internal_string value
    )
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_rpsTicketPolicy = std::move(value);
}

const xsapi_internal_string& auth_config::xbox_live_endpoint() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_xboxLiveEndpoint;
}

void auth_config::set_xbox_live_endpoint(
    _In_ xsapi_internal_string value
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_xboxLiveEndpoint = std::move(value);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

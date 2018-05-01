// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/system.h"
#include "utils.h"
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


#if XSAPI_U
bool auth_config::use_win10_auth() const
{
#if UWP_API || XSAPI_U
    return true;
#else
    return false;
#endif
}

bool auth_config::use_compact_ticket() const
{
    return m_useCompactTicket;
}

const string_t& auth_config::environment() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_environment;
}

const string_t& auth_config::device_token_endpoint() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_deviceTokenEndpoint;
}

void auth_config::set_device_token_endpoint(
    _In_ string_t value
    )
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_deviceTokenEndpoint = std::move(value);
}

const string_t& auth_config::title_token_endpoint() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_titleTokenEndpoint;
}

void auth_config::set_title_token_endpoint(
    _In_ string_t value
    )
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_titleTokenEndpoint = std::move(value);
}

const string_t& auth_config::user_token_site_name() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_userTokenSiteName;
}

void auth_config::set_user_token_site_name(
    _In_ string_t value
    )
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_userTokenSiteName = std::move(value);
}

const string_t& auth_config::user_token_endpoint() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_userTokenEndpoint;
}

void auth_config::set_user_token_endpoint(
    _In_ string_t value
    )
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_userTokenEndpoint = std::move(value);
}

const string_t& auth_config::service_token_endpoint() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_serviceTokenEndpoint;
}

void auth_config::set_service_token_endpoint(
    _In_ string_t value
    )
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_serviceTokenEndpoint = std::move(value);
}

const string_t& auth_config::xbox_live_relying_party() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_xboxLiveRelyingParty;
}

void auth_config::set_xbox_live_relying_party(
    _In_ string_t value
    )
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_xboxLiveRelyingParty = std::move(value);
}

const string_t& auth_config::x_token_endpoint() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_xTokenEndpoint;
}

void auth_config::set_x_token_endpoint(
    _In_ string_t value
    )
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_xTokenEndpoint = std::move(value);
}

const string_t& auth_config::x_title_endpoint() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_xTitleEndpoint;
}

void auth_config::set_x_title_endpoint(
    _In_ string_t value
    )
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_xTitleEndpoint = std::move(value);
}

void auth_config::set_app_id(string_t appId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_appId = std::move(appId);
}

const string_t& auth_config::app_id() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_appId;
}

void auth_config::set_microsoft_account_id(string_t accountId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_accountId = std::move(accountId);
}

const string_t& auth_config::microsoft_account_id() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_accountId;
}

void auth_config::set_detail_error(uint32_t detail_error)
{
    m_detailError = detail_error;
}

uint32_t auth_config::detail_error() const
{
    return m_detailError;
}

void auth_config::reset()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_redirect.clear();
    m_detailError = 0;
}

void auth_config::set_redirect(_In_ string_t value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_redirect = std::move(value);
}

const string_t& auth_config::redirect() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_redirect;
}

const std::vector<token_identity_type>& auth_config::xtoken_composition() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_xtokenComposition;
}

void auth_config::set_xtoken_composition(std::vector<token_identity_type> value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_xtokenComposition = value;
}

uint32_t auth_config::http_status_code() const
{
    return m_httpStatusCode;
}
void auth_config::set_http_status_code(
    _In_ uint32_t httpStatusCode
    )
{
    m_httpStatusCode = httpStatusCode;
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

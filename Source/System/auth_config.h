// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>
NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

enum class token_identity_type
{
    x_token = 0,
    d_token,
    t_token,
    u_token,
    s_token,
};


class auth_config
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    auth_config();

    /// <summary>
    /// Internal function
    /// </summary>
    auth_config(
        _In_ xsapi_internal_string sandbox,
        _In_ xsapi_internal_string environmentPrefix,
        _In_ xsapi_internal_string environment,
        _In_ bool useCompactTicket,
        _In_ bool isCreatorsTitle,
        _In_ xsapi_internal_string scope
        );

    const xsapi_internal_string& xbox_live_endpoint() const;
    void set_xbox_live_endpoint(_In_ xsapi_internal_string value);

    const xsapi_internal_string& rps_ticket_service() const;
    void set_rps_ticket_service(_In_ xsapi_internal_string value);
    const xsapi_internal_string& rps_ticket_policy() const;
    void set_rps_ticket_policy(_In_ xsapi_internal_string value);

#if XSAPI_U
    // TODO update mobile auth config
    /// <summary>
    /// Internal function
    /// </summary>
    bool use_win10_auth() const;
    bool use_compact_ticket() const;

    void set_app_id(string_t appId);
    const string_t& app_id() const;
    void set_microsoft_account_id(string_t accountId);
    const string_t& microsoft_account_id() const;

    void set_detail_error(uint32_t detail_error);
    uint32_t detail_error() const;

    const string_t& device_token_endpoint() const;
    void set_device_token_endpoint(_In_ string_t value);
    const string_t& title_token_endpoint() const;
    void set_title_token_endpoint(_In_ string_t value);
    const string_t& service_token_endpoint() const;
    void set_service_token_endpoint(_In_ string_t value);

    const string_t& user_token_site_name() const;
    void set_user_token_site_name(_In_ string_t value);
    const string_t& user_token_endpoint() const;
    void set_user_token_endpoint(_In_ string_t value);

    const string_t& xbox_live_relying_party() const;
    void set_xbox_live_relying_party(_In_ string_t value);
    const string_t& x_token_endpoint() const;
    void set_x_token_endpoint(_In_ string_t value);
    const string_t& x_title_endpoint() const;
    void set_x_title_endpoint(_In_ string_t value);

    const string_t& redirect() const;
    void set_redirect(_In_ string_t value);

    const std::vector<token_identity_type>& xtoken_composition() const;
    void set_xtoken_composition(std::vector<token_identity_type> value);

    uint32_t http_status_code() const;
    void set_http_status_code(_In_ uint32_t httpStatusCode);

    const string_t& environment() const;

#endif

    // reset the variables could change in each token fetching.
    void reset();

private:
    mutable std::mutex m_mutex;
    xsapi_internal_string m_sandbox;
    xsapi_internal_string m_rpsTicketService;
    xsapi_internal_string m_rpsTicketPolicy;
    xsapi_internal_string m_xTitleEndpoint;
    xsapi_internal_string m_deviceTokenEndpoint;
    xsapi_internal_string m_titleTokenEndpoint;
    xsapi_internal_string m_userTokenEndpoint;
    xsapi_internal_string m_serviceTokenEndpoint;
    xsapi_internal_string m_xTokenEndpoint;
    xsapi_internal_string m_userTokenSiteName;
    xsapi_internal_string m_xboxLiveRelyingParty;
    xsapi_internal_string m_xboxLiveEndpoint;
    xsapi_internal_string m_appId;
    xsapi_internal_string m_accountId;
    bool m_useCompactTicket;
    uint32_t m_detailError;
    xsapi_internal_string m_redirect;
    xsapi_internal_vector<token_identity_type> m_xtokenComposition;
    uint32_t m_httpStatusCode;
    xsapi_internal_string m_environment;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
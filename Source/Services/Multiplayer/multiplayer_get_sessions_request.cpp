// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/multiplayer.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_BEGIN

multiplayer_get_sessions_request::multiplayer_get_sessions_request(
    _In_ string_t serviceConfigurationId,
    _In_ uint32_t maxItems
    ) :
    m_serviceConfigurationId(std::move(serviceConfigurationId)),
    m_maxItems(maxItems),
    m_visibilityFilter(multiplayer_session_visibility::open),
    m_contractVersionFilter(0)
{
}

const string_t&
multiplayer_get_sessions_request::service_configuration_id()
{
    return m_serviceConfigurationId;
}

uint32_t
multiplayer_get_sessions_request::max_items()
{
    return m_maxItems;
}

bool
multiplayer_get_sessions_request::include_private_sessions()
{
    return m_includePrivateSessions;
}

void
multiplayer_get_sessions_request::set_include_private_sessions(
    _In_ bool includePrivateSessions
    )
{
    m_includePrivateSessions = includePrivateSessions;
}

bool
multiplayer_get_sessions_request::include_reservations()
{
    return m_includeReservations;
}

void
multiplayer_get_sessions_request::set_include_reservations(
    _In_ bool includeResevations
    )
{
    m_includeReservations = includeResevations;
}

bool
multiplayer_get_sessions_request::include_inactive_sessions()
{
    return m_includeInactiveSessions;
}

void
multiplayer_get_sessions_request::set_include_inactive_sessions(
    _In_ bool includeInactiveSessions
    )
{
    m_includeInactiveSessions = includeInactiveSessions;
}

const string_t&
multiplayer_get_sessions_request::xbox_user_id_filter()
{
    return m_xboxUserIdFilter;
}

void
multiplayer_get_sessions_request::set_xbox_user_id_filter(
    _In_ const string_t& filter
    )
{
    m_xboxUserIdFilter = std::move(filter);
}

const std::vector<string_t>&
multiplayer_get_sessions_request::xbox_user_ids_filter()
{
    return m_xboxUserIdsFilter;
}

void
multiplayer_get_sessions_request::set_xbox_user_ids_filter(
    _In_ std::vector<string_t> filter
    )
{
    m_xboxUserIdsFilter = std::move(filter);
}

const string_t&
multiplayer_get_sessions_request::keyword_filter()
{
    return m_keywordFilter;
}

void 
multiplayer_get_sessions_request::set_keyword_filter(
    _In_ const string_t& filter
    )
{
    m_keywordFilter = std::move(filter);
}

const string_t& 
multiplayer_get_sessions_request::session_template_name_filter()
{
    return m_sessionTemplateNameFilter;
}

void
multiplayer_get_sessions_request::set_session_template_name_filter(
    _In_ const string_t& filter
    )
{
    m_sessionTemplateNameFilter = std::move(filter);

}

multiplayer_session_visibility
multiplayer_get_sessions_request::visibility_filter()
{
    return m_visibilityFilter;
}

void
multiplayer_get_sessions_request::set_visibility_filter(
    _In_ multiplayer_session_visibility filter
    )
{
    m_visibilityFilter = filter;
}

uint32_t
multiplayer_get_sessions_request::contract_version_filter()
{
    return m_contractVersionFilter;
}

void
multiplayer_get_sessions_request::set_contract_version_filter(
    _In_ uint32_t filter
    )
{
    m_contractVersionFilter = filter;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_CPP_END
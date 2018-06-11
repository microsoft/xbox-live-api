// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

multiplayer_client_pending_request::multiplayer_client_pending_request() :
    m_joinability(joinability::none),
    m_requestType(pending_request_type::non_synchronized_changes),
    m_localUserLobbyState(multiplayer_local_user_lobby_state::unknown),
    m_context(nullptr)
{
    m_identifier = utils::interlocked_increment(get_xsapi_singleton()->m_multiplayerClientPendingRequestUniqueIdentifier);
}

pending_request_type
multiplayer_client_pending_request::request_type() const
{
    return m_requestType;
}

context_t
multiplayer_client_pending_request::context()
{
    return m_context;
}

uint32_t
multiplayer_client_pending_request::identifier() const
{
    return m_identifier;
}

// Local User properties
std::shared_ptr<multiplayer_local_user>
multiplayer_client_pending_request::local_user()
{
    return m_localUser;
}

void
multiplayer_client_pending_request::set_local_user(
    _In_ std::shared_ptr<multiplayer_local_user> user
    )
{
    m_localUser = user;
}

multiplayer_local_user_lobby_state
multiplayer_client_pending_request::lobby_state()
{
    return m_localUserLobbyState;
}

void
multiplayer_client_pending_request::set_lobby_state(
    _In_ multiplayer_local_user_lobby_state userState
    )
{
    m_localUserLobbyState = userState;
}

const string_t&
multiplayer_client_pending_request::lobby_handle_id() const
{
    return m_lobbyHandleId;
}

void
multiplayer_client_pending_request::set_lobby_handle_id(_In_ const string_t& handleId)
{
    m_lobbyHandleId = handleId;
}

const multiplayer_session_reference&
multiplayer_client_pending_request::team_session_reference() const
{
    return m_teamSessionRef;
}

void
multiplayer_client_pending_request::set_team_session_reference(_In_ const multiplayer_session_reference& sessionRef)
{
    m_teamSessionRef = sessionRef;
}

const string_t&
multiplayer_client_pending_request::local_user_connection_address() const
{
    return m_localUserConnectionAddress;
}

void
multiplayer_client_pending_request::set_local_user_connection_address(
    _In_ std::shared_ptr<multiplayer_local_user> localUser,
    _In_ string_t connectionAddress,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_localUser = localUser;

    // convert the address into a base64 string as the service will reject it otherwise.
    utf8string utf8String = utility::conversions::to_utf8string(connectionAddress);
    std::vector<unsigned char> input(utf8String.c_str(), utf8String.c_str() + utf8String.size());
    m_localUserConnectionAddress = utility::conversions::to_base64(input);
}

const std::map<string_t, web::json::value>&
multiplayer_client_pending_request::local_user_properties() const
{
    return m_localUserProperties;
}

void
multiplayer_client_pending_request::set_local_user_properties(
    _In_ std::shared_ptr<multiplayer_local_user> localUser,
    _In_ string_t name,
    _In_ web::json::value valueJson,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_localUser = localUser;
    m_localUserProperties[name] = valueJson;
}

// Session non-synchronized properties
multiplayer::manager::joinability
multiplayer_client_pending_request::joinability()
{
    return m_joinability;
}

void multiplayer_client_pending_request::set_joinability(
    _In_ xbox::services::multiplayer::manager::joinability value,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_joinability = value;
}

const std::map<string_t, web::json::value>&
multiplayer_client_pending_request::session_properties() const
{
    return m_sessionProperties;
}

void
multiplayer_client_pending_request::set_session_properties(
    _In_ string_t name,
    _In_ web::json::value valueJson,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_sessionProperties[name] = valueJson;
}

// Session synchronized properties
const string_t&
multiplayer_client_pending_request::synchronized_host_device_token() const
{
    return m_synchronizedHostDeviceToken;
}

void
multiplayer_client_pending_request::set_synchronized_host_device_token(
    _In_ const string_t& hostDeviceToken,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_requestType = pending_request_type::synchronized_changes;
    m_synchronizedHostDeviceToken = hostDeviceToken;
}

const std::map<string_t, web::json::value>&
multiplayer_client_pending_request::synchronized_session_properties() const
{
    return m_synchronizedSessionProperties;
}

void
multiplayer_client_pending_request::set_synchronized_session_properties(
    _In_ string_t name,
    _In_ web::json::value valueJson,
    _In_opt_ context_t context
    )
{
    m_context = context;
    m_requestType = pending_request_type::synchronized_changes;
    m_synchronizedSessionProperties[name] = valueJson;
}

void
multiplayer_client_pending_request::append_pending_changes(
    _In_ std::shared_ptr<multiplayer_session> sessionToCommit,
    _In_ std::shared_ptr<multiplayer_local_user> localUser,
    _In_ bool isGameInProgress
    )
{
    // Apply local user properties
    if (localUser != nullptr && m_localUser != nullptr &&
        utils::str_icmp(localUser->xbox_user_id(), m_localUser->xbox_user_id() ) == 0)
    {
        if (!m_localUserConnectionAddress.empty())
        {
            // Assign connection address to the localUser instance as it's used in oher places locally.
            localUser->set_connection_address(m_localUserConnectionAddress);
            sessionToCommit->set_current_user_secure_device_address_base64(m_localUserConnectionAddress);
        }

        if (m_localUserProperties.size() > 0)
        {
            for (const auto& prop : m_localUserProperties)
            {
                sessionToCommit->set_current_user_member_custom_property_json(prop.first, prop.second);
            }
        }

        localUser->set_write_changes_to_service(false);
    }

    // Apply session non-sync properties

    if (m_joinability != joinability::none)
    {
        multiplayer_manager_utils::set_joinability(m_joinability, sessionToCommit, isGameInProgress);
    }

    if (m_sessionProperties.size() > 0)
    {
        for (const auto& prop : m_sessionProperties)
        {
            sessionToCommit->set_session_custom_property_json(prop.first, prop.second);
        }
    }

    // Apply session sync properties

    if (!m_synchronizedHostDeviceToken.empty())
    {
        sessionToCommit->set_host_device_token(m_synchronizedHostDeviceToken);
    }

    if (m_synchronizedSessionProperties.size() > 0)
    {
        for (const auto& prop : m_synchronizedSessionProperties)
        {
            sessionToCommit->set_session_custom_property_json(prop.first, prop.second);
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
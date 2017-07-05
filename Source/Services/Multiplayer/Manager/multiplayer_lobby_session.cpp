// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

multiplayer_lobby_session::multiplayer_lobby_session(
    _In_ std::shared_ptr<multiplayer_client_manager> multiplayerClientManagerInstance
    ):
    m_multiplayerClientManager(std::move(multiplayerClientManagerInstance)),
    m_changeNumber(0)
{
}

multiplayer_lobby_session::multiplayer_lobby_session():
    m_changeNumber(0)
{
}

multiplayer_lobby_session::multiplayer_lobby_session(
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ std::shared_ptr<multiplayer_member> host,
    _In_ std::vector<std::shared_ptr<multiplayer_member>> members,
    _In_ std::vector<std::shared_ptr<multiplayer_member>> localMembers
    ) :
    m_sessionReference(session->session_reference()),
    m_host(std::move(host)),
    m_correlationId(session->multiplayer_correlation_id()),
    m_changeNumber(session->change_number()),
    m_members(std::move(members)),
    m_localMembers(std::move(localMembers)),
    m_properties(session->session_properties()->session_custom_properties_json()),
    m_sessionConstants(session->session_constants()),
    m_lastTournamentTeamResult(session->tournaments_server().last_team_result())
{
}

std::shared_ptr<multiplayer_lobby_session> multiplayer_lobby_session::_Create_deep_copy()
{
    auto copy = std::make_shared<multiplayer_lobby_session>();
    copy->deep_copy_from(*this);
    return copy;
}

void multiplayer_lobby_session::deep_copy_from(
    _In_ const multiplayer_lobby_session& other
    )
{
    m_sessionReference = other.m_sessionReference;
    m_host = other.m_host;
    m_correlationId = other.m_correlationId;
    m_changeNumber = other.m_changeNumber;
    m_members = other.m_members;
    m_localMembers = other.m_localMembers;
    m_properties = other.m_properties;
    m_sessionConstants = other.m_sessionConstants;
    m_lastTournamentTeamResult = other.m_lastTournamentTeamResult;
    m_multiplayerClientManager = other.m_multiplayerClientManager;
}

xbox_live_result<void>
multiplayer_lobby_session::add_local_user(
    _In_ xbox_live_user_t user
    )
{
    if (m_multiplayerClientManager->latest_pending_read() == nullptr)
    {
        m_multiplayerClientManager->initialize();
    }

    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->lobby_client()->add_local_user(
        user,
        multiplayer_local_user_lobby_state::add
        ), void);
}

xbox_live_result<void>
multiplayer_lobby_session::remove_local_user(
    _In_ xbox_live_user_t user
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager->latest_pending_read() == nullptr, void, xbox_live_error_code::logic_error, "No user added. Call add_local_user() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->lobby_client()->remove_local_user(user), void);
}

xbox_live_result<void>
multiplayer_lobby_session::set_local_member_properties(
    _In_ xbox_live_user_t user,
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager->latest_pending_read() == nullptr, void, xbox_live_error_code::logic_error, "No user added. Call add_local_user() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->lobby_client()->set_local_member_properties(user, std::move(name), std::move(valueJson), context), void);
}

xbox_live_result<void>
multiplayer_lobby_session::delete_local_member_properties(
    _In_ xbox_live_user_t user,
    _In_ const string_t& name,
    _In_opt_ context_t context
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager->latest_pending_read() == nullptr, void, xbox_live_error_code::logic_error, "No user added. Call add_local_user() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->lobby_client()->delete_local_member_properties(user, name, context), void);
}

xbox_live_result<void>
multiplayer_lobby_session::set_local_member_connection_address(
    _In_ xbox_live_user_t user,
    _In_ const string_t& connectionAddress,
    _In_opt_ context_t context
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager->latest_pending_read() == nullptr, void, xbox_live_error_code::logic_error, "No user added. Call add_local_user() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->lobby_client()->set_local_member_connection_address(user, connectionAddress, context), void);
}

const multiplayer_session_reference&
multiplayer_lobby_session::session_reference() const
{
    return m_sessionReference;
}

const string_t&
multiplayer_lobby_session::correlation_id() const
{
    return m_correlationId;
}

uint64_t
multiplayer_lobby_session::_Change_number() const
{
    return m_changeNumber;
}

std::shared_ptr<multiplayer_member>
multiplayer_lobby_session::host() const
{
    return m_host;
}

void
multiplayer_lobby_session::_Set_host(
    _In_ std::shared_ptr<multiplayer_member> hostMember
    )
{
    m_host = hostMember;
}

const std::vector<std::shared_ptr<multiplayer_member>>&
multiplayer_lobby_session::members() const
{
    return m_members;
}

std::shared_ptr<multiplayer_session_constants>
multiplayer_lobby_session::session_constants() const
{
    return m_sessionConstants;
}

const tournament_team_result&
multiplayer_lobby_session::last_tournament_team_result() const
{
    return m_lastTournamentTeamResult;
}

const std::vector<std::shared_ptr<multiplayer_member>>&
multiplayer_lobby_session::local_members() const
{
    return m_localMembers;
}

const web::json::value&
multiplayer_lobby_session::properties() const
{
    return m_properties;
}

xbox_live_result<void>
multiplayer_lobby_session::set_properties(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager->latest_pending_read() == nullptr, void, xbox_live_error_code::logic_error, "No user added. Call add_local_user() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->set_properties(m_sessionReference, name, valueJson, context), void);
}

bool
multiplayer_lobby_session::is_host(
    _In_ const string_t& xboxUserId
    )
{
    if (m_host == nullptr || m_members.size() == 0)
    {
        return false;
    }

    return utils::str_icmp(xboxUserId, m_host->xbox_user_id()) == 0;
}

xbox_live_result<void>
multiplayer_lobby_session::set_synchronized_host(
    _In_ std::shared_ptr<multiplayer_member> gameHost,
    _In_opt_ context_t context
    )
{
    RETURN_CPP_IF(gameHost == nullptr, void, xbox_live_error_code::invalid_argument, "GameHost was null.");

    RETURN_CPP_IF(m_multiplayerClientManager->latest_pending_read() == nullptr, void, xbox_live_error_code::logic_error, "No user added. Call add_local_user() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->set_synchronized_host(m_sessionReference, gameHost->_Device_token(), context), void);
}

xbox_live_result<void>
multiplayer_lobby_session::set_synchronized_properties(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager->latest_pending_read() == nullptr, void, xbox_live_error_code::logic_error, "No user added. Call add_local_user() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->set_synchronized_properties(m_sessionReference, name, valueJson, context), void);
}

void
multiplayer_lobby_session::_Set_multiplayer_client_manager(
    _In_ std::shared_ptr<multiplayer_client_manager> clientManager
    )
{
    m_multiplayerClientManager = clientManager;
}

#if !XSAPI_U
xbox_live_result<void>
multiplayer_lobby_session::invite_friends(
    _In_ xbox_live_user_t user,
    _In_ const string_t& contextStringId,
    _In_ const string_t& customActivationContext
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager->latest_pending_read() == nullptr, void, xbox_live_error_code::logic_error, "No user added. Call add_local_user() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->invite_friends(user, contextStringId, customActivationContext), void);
}
#endif

xbox_live_result<void>
multiplayer_lobby_session::invite_users(
    _In_ xbox_live_user_t user,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const string_t& contextStringId,
    _In_ const string_t& customActivationContext
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager->latest_pending_read() == nullptr, void, xbox_live_error_code::logic_error, "No user added. Call add_local_user() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->invite_users(user, xboxUserIds, contextStringId, customActivationContext), void);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
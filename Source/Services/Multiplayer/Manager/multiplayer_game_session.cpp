// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

std::shared_ptr<multiplayer_game_session> multiplayer_game_session::_Create_deep_copy()
{
    auto copy = std::make_shared<multiplayer_game_session>();
    copy->deep_copy_from(*this);
    return copy;
}

void multiplayer_game_session::deep_copy_from(
    _In_ const multiplayer_game_session& other
    )
{
    m_sessionReference = other.m_sessionReference;
    m_host = other.m_host;
    m_correlationId = other.m_correlationId;
    m_changeNumber = other.m_changeNumber;
    m_members = other.m_members;
    m_properties = other.m_properties;
    m_sessionConstants = other.m_sessionConstants;
    m_multiplayerClientManager = other.m_multiplayerClientManager;
    m_teams = other.m_teams;
    m_tournamentTeamResults = other.m_tournamentTeamResults;
}

multiplayer_game_session::multiplayer_game_session():
    m_changeNumber(0)
{
}

multiplayer_game_session::multiplayer_game_session(
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ std::shared_ptr<multiplayer_member> host,
    _In_ std::vector<std::shared_ptr<multiplayer_member>> members
    ):
    m_sessionReference(session->session_reference()),
    m_host(std::move(host)),
    m_correlationId(session->multiplayer_correlation_id()),
    m_changeNumber(session->change_number()),
    m_members(std::move(members)),
    m_properties(session->session_properties()->session_custom_properties_json()),
    m_sessionConstants(session->session_constants()),
    m_teams(session->tournaments_server().teams()),
    m_tournamentTeamResults(session->arbitration_server().results())
{
}

const multiplayer_session_reference&
multiplayer_game_session::session_reference() const
{
    return m_sessionReference;
}

const string_t&
multiplayer_game_session::correlation_id() const
{
    return m_correlationId;
}

uint64_t
multiplayer_game_session::_Change_number() const
{
    return m_changeNumber;
}

std::shared_ptr<multiplayer_member>
multiplayer_game_session::host() const
{
    return m_host;
}

void
multiplayer_game_session::_Set_host(
    _In_ std::shared_ptr<multiplayer_member> hostMember
    )
{
    m_host = hostMember;
}

const std::vector<std::shared_ptr<multiplayer_member>>&
multiplayer_game_session::members() const
{
    return m_members;
}

std::shared_ptr<multiplayer_session_constants>
multiplayer_game_session::session_constants() const
{
    return m_sessionConstants;
}

const std::unordered_map<string_t, multiplayer_session_reference>& 
multiplayer_game_session::tournament_teams() const
{
    return m_teams;
}

const std::unordered_map<string_t, tournament_team_result>&
multiplayer_game_session::tournament_team_results() const
{
    return m_tournamentTeamResults;
}

const web::json::value&
multiplayer_game_session::properties() const
{
    return m_properties;
}

xbox_live_result<void>
multiplayer_game_session::set_properties(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
    )
{
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->set_properties(m_sessionReference, name, valueJson, context), void);
}

bool
multiplayer_game_session::is_host( 
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
multiplayer_game_session::set_synchronized_host(
    _In_ std::shared_ptr<multiplayer_member> gameHost,
    _In_opt_ context_t context
    )
{
    RETURN_CPP_IF(gameHost == nullptr, void, xbox_live_error_code::invalid_argument, "GameHost was null.");

    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->set_synchronized_host(m_sessionReference, gameHost->_Device_token(), context), void);
}

xbox_live_result<void>
multiplayer_game_session::set_synchronized_properties(
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
    )
{
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->set_synchronized_properties(m_sessionReference, name, valueJson, context), void);
}

void
multiplayer_game_session::_Set_multiplayer_client_manager(
    _In_ std::shared_ptr<multiplayer_client_manager> clientManager
    )
{
    m_multiplayerClientManager = clientManager;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
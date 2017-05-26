// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/services.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

multiplayer_local_user::multiplayer_local_user(
    _In_ xbox_live_user_t user,
    _In_ string_t xboxUserId,
    _In_ bool isPrimary
    ) :
    m_xboxLiveContextImpl(std::make_shared<xbox_live_context_impl>(user)),
    m_xboxUserId(std::move(xboxUserId)),
    m_isPrimaryXboxLiveContext(isPrimary),
    m_writeChangesToService(false),
    m_writeConnectionAddress(false),
    m_lobbyState(multiplayer_local_user_lobby_state::unknown),
    m_gameState(multiplayer_local_user_game_state::unknown),
    m_rtaStateChangedContext(0),
    m_sessionChangedContext(0),
    m_subscriptionLostContext(0),
    m_rtaResyncContext(0)
{
    m_xboxLiveContextImpl->user_context()->set_caller_context_type(caller_context_type::multiplayer_manager);
    m_xboxLiveContextImpl->init();
}

multiplayer_local_user::~multiplayer_local_user()
{
    m_xboxLiveContextImpl.reset();
}

string_t
multiplayer_local_user::xbox_user_id() const
{
    return m_xboxUserId;
}

multiplayer_local_user_lobby_state
multiplayer_local_user::lobby_state() const
{
    return m_lobbyState;
}

void
multiplayer_local_user::set_lobby_state(
    _In_ multiplayer_local_user_lobby_state userState
    )
{
    m_lobbyState = userState;

    if (userState == multiplayer_local_user_lobby_state::add ||
        userState == multiplayer_local_user_lobby_state::join ||
        userState == multiplayer_local_user_lobby_state::leave )
    {
        m_writeChangesToService = true;
    }
}

multiplayer_local_user_game_state
multiplayer_local_user::game_state() const
{
    return m_gameState;
}

void
multiplayer_local_user::set_game_state(
    _In_ multiplayer_local_user_game_state userState
    )
{
    m_gameState = userState;
}

const string_t&
multiplayer_local_user::lobby_handle_id() const
{
    return m_lobbyHandleId;
}

void
multiplayer_local_user::set_lobby_handle_id(_In_ const string_t& handleId)
{
    m_lobbyHandleId = handleId;
}

string_t
multiplayer_local_user::connection_address() const
{
    return m_connectionAddress;
}

void
multiplayer_local_user::set_connection_address(_In_ string_t address)
{
    m_connectionAddress = address;
}

bool
multiplayer_local_user::is_primary_xbox_live_context() const
{
    return m_isPrimaryXboxLiveContext;
}

void
multiplayer_local_user::set_is_primary_xbox_live_context(
    _In_ bool isPrimary
    )
{
    m_isPrimaryXboxLiveContext = isPrimary;
}

bool
multiplayer_local_user::write_changes_to_service() const
{
    return m_writeChangesToService;
}

void
multiplayer_local_user::set_write_changes_to_service(_In_ bool value)
{
    m_writeChangesToService = value;
}

std::shared_ptr<xbox::services::xbox_live_context_impl>
multiplayer_local_user::context() const
{
    return m_xboxLiveContextImpl;
}

function_context
multiplayer_local_user::rta_state_changed_context() const
{
    return m_rtaStateChangedContext;
}

void
multiplayer_local_user::set_rta_state_changed_context(
    _In_ function_context functionContext
    )
{
    m_rtaStateChangedContext = functionContext;
}

function_context
multiplayer_local_user::session_changed_context() const
{
    return m_sessionChangedContext;
}

void
multiplayer_local_user::set_session_changed_context(
    _In_ function_context functionContext
    )
{
    m_sessionChangedContext = functionContext;
}

function_context
multiplayer_local_user::subscription_lost_context() const
{
    return m_subscriptionLostContext;
}

void
multiplayer_local_user::set_subscription_lost_context(
    _In_ function_context functionContext
    )
{
    m_subscriptionLostContext = functionContext;
}

function_context
multiplayer_local_user::rta_resync_context() const
{
    return m_rtaResyncContext;
}

void
multiplayer_local_user::set_rta_resync_context(
    _In_ function_context functionContext
    )
{
    m_rtaResyncContext = functionContext;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
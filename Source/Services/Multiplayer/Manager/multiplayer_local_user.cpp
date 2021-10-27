// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerLocalUser::MultiplayerLocalUser(
    _In_ User&& user,
    _In_ uint64_t xuid,
    _In_ bool isPrimary
) :
    m_sessionChangedContext(0),
    m_subscriptionLostContext(0),
    m_rtaResyncContext(0),
    m_writeChangesToService(false),
    m_xuid(xuid),
    m_lobbyState(MultiplayerLocalUserLobbyState::Unknown),
    m_gameState(MultiplayerLocalUserGameState::Unknown),
    m_isPrimaryXboxLiveContext(isPrimary)
{
    // TODO parameterize RTAManager
    m_xboxLiveContextImpl = XblContext::Make(std::move(user));

    m_xboxLiveContextImpl->Initialize(GlobalState::Get()->RTAManager());
    m_xboxLiveContextImpl->Settings()->SetHttpUserAgent(HttpCallAgent::MultiplayerManager);
}

MultiplayerLocalUser::~MultiplayerLocalUser()
{
    m_xboxLiveContextImpl.reset();
}

uint64_t
MultiplayerLocalUser::Xuid() const
{
    return m_xuid;
}

MultiplayerLocalUserLobbyState
MultiplayerLocalUser::LobbyState() const
{
    return m_lobbyState;
}

void
MultiplayerLocalUser::SetLobbyState(
    _In_ MultiplayerLocalUserLobbyState userState
    )
{
    m_lobbyState = userState;

    if (userState == MultiplayerLocalUserLobbyState::Add ||
        userState == MultiplayerLocalUserLobbyState::Join ||
        userState == MultiplayerLocalUserLobbyState::Leave )
    {
        m_writeChangesToService = true;
    }
}

MultiplayerLocalUserGameState
MultiplayerLocalUser::GameState() const
{
    return m_gameState;
}

void
MultiplayerLocalUser::SetGameState(
    _In_ MultiplayerLocalUserGameState userState
    )
{
    m_gameState = userState;
}

const xsapi_internal_string&
MultiplayerLocalUser::LobbyHandleId() const
{
    return m_lobbyHandleId;
}

void
MultiplayerLocalUser::SetLobbyHandleId(_In_ const xsapi_internal_string& handleId)
{
    m_lobbyHandleId = handleId;
}

const xsapi_internal_string&
MultiplayerLocalUser::ConnectionAddress() const
{
    return m_connectionAddress;
}

void
MultiplayerLocalUser::SetConnectionAddress(_In_ const xsapi_internal_string& address)
{
    m_connectionAddress = address;
}

bool
MultiplayerLocalUser::IsPrimaryXboxLiveContext() const
{
    return m_isPrimaryXboxLiveContext;
}

void
MultiplayerLocalUser::SetIsPrimaryXboxLiveContext(
    _In_ bool isPrimary
    )
{
    m_isPrimaryXboxLiveContext = isPrimary;
}

bool
MultiplayerLocalUser::WriteChangesToService() const
{
    return m_writeChangesToService;
}

void
MultiplayerLocalUser::SetWriteChangesToService(_In_ bool value)
{
    m_writeChangesToService = value;
}

std::shared_ptr<XblContext>
MultiplayerLocalUser::Context() const
{
    return m_xboxLiveContextImpl;
}

XblFunctionContext
MultiplayerLocalUser::SessionChangedContext() const
{
    return m_sessionChangedContext;
}

void
MultiplayerLocalUser::SetSessionChangedContext(
    _In_ XblFunctionContext functionContext
    )
{
    m_sessionChangedContext = functionContext;
}

XblFunctionContext
MultiplayerLocalUser::ConnectionIdChangedContext() const
{
    return m_connectionIdChangedContext;
}

void
MultiplayerLocalUser::SetConnectionIdChangedContext(
    _In_ XblFunctionContext functionContext
)
{
    m_connectionIdChangedContext = functionContext;
}

XblFunctionContext
MultiplayerLocalUser::SubscriptionLostContext() const
{
    return m_subscriptionLostContext;
}

void
MultiplayerLocalUser::SetSubscriptionLostContext(
    _In_ XblFunctionContext functionContext
    )
{
    m_subscriptionLostContext = functionContext;
}

XblFunctionContext
MultiplayerLocalUser::RtaResyncContext() const
{
    return m_rtaResyncContext;
}

void
MultiplayerLocalUser::SetRtaResyncContext(
    _In_ XblFunctionContext functionContext
    )
{
    m_rtaResyncContext = functionContext;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;

STDAPI_(bool) XblMultiplayerManagerMemberAreMembersOnSameDevice(
    _In_ const XblMultiplayerManagerMember* first,
    _In_ const XblMultiplayerManagerMember* second
) XBL_NOEXCEPT
try
{
    if (first != nullptr && second != nullptr)
    {
        return utils::str_icmp(first->DeviceToken, second->DeviceToken) == 0;
    }
    return false;
}
CATCH_RETURN()

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerMember::MultiplayerMember() :
    m_memberId(0),
    m_xuid(0),
    m_isLocal(false),
    m_isInLobby(false),
    m_isInGame(false),
    m_status(XblMultiplayerSessionMemberStatus::Inactive)
{
}

MultiplayerMember::MultiplayerMember(
    _In_ const XblMultiplayerSessionMember* member,
    _In_ bool isLocal,
    _In_ bool /*isGameHost*/,
    _In_ bool /*isLobbyHost*/,
    _In_ bool isInLobby,
    _In_ bool isInGame
    ):
    m_memberId(member->MemberId),
    m_xuid(member->Xuid),
    m_gamertag(member->Gamertag),
    m_deviceToken(member->DeviceToken.Value),
    m_isLocal(isLocal),
    m_isInLobby(isInLobby),
    m_isInGame(isInGame),
    m_status(member->Status)
{
    if (member->InitialTeam)
    {
        m_initialTeam = member->InitialTeam;
    }
    if (member->CustomPropertiesJson)
    {
        m_jsonProperties = member->CustomPropertiesJson;
    }
    if (member->SecureDeviceBaseAddress64)
    {
        m_connectionAddress = utils::parse_secure_device_address(member->SecureDeviceBaseAddress64);
    }
}

uint32_t
MultiplayerMember::MemberId() const
{
    return m_memberId;
}

const xsapi_internal_string&
MultiplayerMember::TeamId() const
{
    return m_teamId;
}

const xsapi_internal_string& 
MultiplayerMember::InitialTeam() const
{
    return m_initialTeam;
}

uint64_t
MultiplayerMember::Xuid() const
{
    return m_xuid;
}

const xsapi_internal_string&
MultiplayerMember::DebugGamertag() const
{
    return m_gamertag;
}

const xsapi_internal_string&
MultiplayerMember::DeviceToken() const
{
    return m_deviceToken;
}

bool
MultiplayerMember::IsLocal() const
{
    return m_isLocal;
}

bool
MultiplayerMember::IsInLobby() const
{
    return m_isInLobby;
}

bool
MultiplayerMember::IsInGame() const
{
    return m_isInGame;
}

XblMultiplayerSessionMemberStatus
MultiplayerMember::Status() const
{
    return m_status;
}

const xsapi_internal_string&
MultiplayerMember::ConnectionAddress() const
{
    return m_connectionAddress;
}

const xsapi_internal_string&
MultiplayerMember::CustomPropertiesJson() const
{
    return m_jsonProperties;
}

bool
MultiplayerMember::IsMemberOnSameDevice(
    _In_ std::shared_ptr<MultiplayerMember> member
    ) const
{
    if(member == nullptr)
    {
        return false;
    }

    return utils::str_icmp_internal(m_deviceToken, member->DeviceToken()) == 0;
}

std::shared_ptr<MultiplayerMember> MultiplayerMember::CreateFromSessionMember(
    _In_ const XblMultiplayerSessionMember* member,
    _In_ const std::shared_ptr<XblMultiplayerSession>& lobbySession,
    _In_ const std::shared_ptr<XblMultiplayerSession>& gameSession,
    _In_ const xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>>& xboxLiveContextMap
    )
{
    bool isLocal = false;
    for (const auto& xboxLiveContext : xboxLiveContextMap)
    {
        const auto& localUser = xboxLiveContext.second;
        if (localUser != nullptr && member->Xuid == localUser->Xuid())
        {
            isLocal = true;
            break;
        }
    }
    return CreateFromSessionMember(member, lobbySession, gameSession, isLocal);
}

std::shared_ptr<MultiplayerMember> MultiplayerMember::CreateFromSessionMember(
    _In_ const XblMultiplayerSessionMember* member,
    _In_ const std::shared_ptr<XblMultiplayerSession>& lobbySession,
    _In_ const std::shared_ptr<XblMultiplayerSession>& gameSession,
    _In_ bool isLocal
    )
{
    return MakeShared<MultiplayerMember>(
        member,
        isLocal,                                                           // isLocal
        XblMultiplayerSession::IsHost(member->DeviceToken.Value, gameSession),      // isGameHost
        XblMultiplayerSession::IsHost(member->DeviceToken.Value, lobbySession),     // isLobbyHost
        XblMultiplayerSession::IsPlayerInSession(member->Xuid, lobbySession), // isinLobby
        XblMultiplayerSession::IsPlayerInSession(member->Xuid, gameSession)   // isinGame
        );
}

XblMultiplayerManagerMember MultiplayerMember::GetReference() const
{
    XblMultiplayerManagerMember ref = XblMultiplayerManagerMember{};
    ref.MemberId = m_memberId;
    ref.InitialTeam = m_initialTeam.data();
    ref.Xuid = m_xuid;
    ref.DebugGamertag = m_gamertag.data();
    ref.IsLocal = m_isLocal;
    ref.IsInLobby = m_isInLobby;
    ref.IsInGame = m_isInGame;
    ref.Status = m_status;
    ref.ConnectionAddress = m_connectionAddress.data();
    ref.PropertiesJson = m_jsonProperties.data();
    ref.DeviceToken = m_deviceToken.data();
    return ref;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END

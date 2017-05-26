// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "xsapi/multiplayer_manager.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

multiplayer_member::multiplayer_member() :
    m_isLocal(false),
    m_isGameHost(false),
    m_isLobbyHost(false),
    m_isInLobby(false),
    m_isInGame(false),
    m_status(multiplayer_session_member_status::inactive)
{
}

multiplayer_member::multiplayer_member(
    _In_ std::shared_ptr<multiplayer_session_member> member,
    _In_ bool isLocal,
    _In_ bool isGameHost,
    _In_ bool isLobbyHost,
    _In_ bool isInLobby,
    _In_ bool isInGame
    ):
    m_memberId(member->member_id()),
    m_teamId(member->team_id()),
    m_xboxUserid(std::move(member->xbox_user_id())),
    m_gamertag(std::move(member->gamertag())),
    m_deviceToken(std::move(member->device_token())),
    m_isLocal(isLocal),
    m_isGameHost(isGameHost),
    m_isLobbyHost(isLobbyHost),
    m_isInLobby(isInLobby),
    m_isInGame(isInGame),
    m_status(member->status()),
    m_jsonProperties(std::move(member->member_custom_properties_json()))
{
    std::vector<unsigned char> base64ConnectionAddress(utility::conversions::from_base64(std::move(member->secure_device_base_address64())));
    m_connectionAddress = string_t(base64ConnectionAddress.begin(), base64ConnectionAddress.end());
}

uint32_t
multiplayer_member::member_id() const
{
    return m_memberId;
}

const string_t&
multiplayer_member::team_id() const
{
    return m_teamId;
}

const string_t&
multiplayer_member::xbox_user_id() const
{
    return m_xboxUserid;
}

const string_t&
multiplayer_member::debug_gamertag() const
{
    return m_gamertag;
}

const string_t&
multiplayer_member::_Device_token() const
{
    return m_deviceToken;
}

bool
multiplayer_member::is_local() const
{
    return m_isLocal;
}

bool
multiplayer_member::is_in_lobby() const
{
    return m_isInLobby;
}

bool
multiplayer_member::is_in_game() const
{
    return m_isInGame;
}

multiplayer_session_member_status
multiplayer_member::status() const
{
    return m_status;
}

const string_t&
multiplayer_member::connection_address() const
{
    return m_connectionAddress;
}

const web::json::value&
multiplayer_member::properties() const
{
    return m_jsonProperties;
}

bool
multiplayer_member::is_member_on_same_device(
    _In_ std::shared_ptr<multiplayer_member> member
    ) const
{
    if(member == nullptr)
    {
        return false;
    }

    return utils::str_icmp(m_deviceToken, member->_Device_token()) == 0;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
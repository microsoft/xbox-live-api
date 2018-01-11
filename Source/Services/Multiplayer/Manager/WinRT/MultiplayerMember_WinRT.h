// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/multiplayer_manager.h"
#include "MultiplayerSessionMemberStatus_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Represents a reference to a member in a multiplayer game.
/// </summary>
public ref class MultiplayerMember sealed
{
public:
    /// <summary>
    /// Id for the member.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(MemberId, member_id, uint32);

    /// <summary>
    /// Only applicable if you are using Xbox Live Tournaments.
    /// Id of this members' team in a tournament.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(TeamId, team_id);

    /// <summary>
    /// Xbox User ID of the member.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// The Gamertag of the member. This is only to be used for debugging purposes
    /// as this gamertag may be out of date. It is recommended you use social manager
    /// or the profile service to get this information.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(DebugGamertag, debug_gamertag);

    /// <summary>
    /// Indicates if this member is playing on the local device.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(IsLocal, is_local, bool);

    /// <summary>
    /// Indicates if this member is part of the lobby.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(IsInLobby, is_in_lobby, bool);

    /// <summary>
    /// Indicates if this member is part of the game.
    /// </summary>
    DEFINE_PTR_PROP_GET_OBJ(IsInGame, is_in_game, bool);

    /// <summary>
    /// The status of this member.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(Status, status, Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionMemberStatus);

    /// <summary>
    /// The address used for network connection.
    /// This can be used for secure socket connection.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(ConnectionAddress, connection_address);

    /// <summary>
    /// JSON value that specify the custom properties of the member.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_FROM_JSON_OBJ(Properties, properties);

    /// <summary>
    /// Determines whether the member is on the same device.
    /// </summary>
    bool IsMemberOnSameDevice(
        _In_ MultiplayerMember^ member
        );

    /// <summary>
    /// Internal function
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(_DeviceToken, _Device_token);

internal:
    MultiplayerMember(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_member> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_member> GetCppObj() const;
private:
    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_member> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END

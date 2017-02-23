// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "Macros_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN
/// <summary>
/// Represents the capabilities for a MultiplayerSession Class object. 
/// </summary>
/// <example>
/// The following example shows the format of the JSON object that represents
/// this class:
/// <code>
/// "capabilities": {
///     "clientMatchmaking": true,
///      "connectivity" : true,
///      "suppressPresenceActivityCheck" : true,
///      "gameplay" : true,
///      "large" : false
/// }
/// </code>
/// </example>

public ref class MultiplayerSessionCapabilities sealed
{
public:
    /// <summary>
    /// Creates a MultiplayerSessionCapabilities object.
    /// </summary>
    MultiplayerSessionCapabilities();

    /// <summary>
    /// A value that indicates if the session supports connectivity. 
    /// If false, the session can't enable any metrics and the session members can't set their SecureDeviceAddress.
    /// This value can't be set for large sessions.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(Connectivity, connectivity, bool);

    /// <summary>
    /// A value that indicates if active users of the session are demoted to inactive status. 
    /// By default (false), active users are required to remain online playing the title, otherwise they get demoted to 
    /// inactive status. Setting this flag to true disables this check so that members stay active indefinitely.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(SuppressPresenceActivityCheck, suppress_presence_activity_check, bool);
    
    /// <summary>
    /// A value that indicates whether the session represents actual gameplay, as opposed to setup/menu time
    /// such as a lobby or matchmaking.
    /// If true, then the session is in gameplay mode.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(Gameplay, gameplay, bool);
    
    /// <summary>
    /// A value that indicates if the session can host a large number of users, which has impact on other session properties.
    /// If true, this session can host a large number of users.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(Large, large, bool);

    /// <summary>
    /// Aa Boolean value that determines whether a connection is required for active members in a multiplayer session. 
    /// If true, a connection is required for active members in the multiplayer session; otherwise, false.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(ConnectionRequiredForActiveMembers, connection_required_for_active_members, bool);

    /// <summary>
    /// a Boolean value that determines whether a multiplayer session uses user authorization style, which means that
    /// the session supports calls from platforms without strong title identity.
    /// This calue can't be set on large sessions.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(UserAuthorizationStyle, user_authorization_style, bool);

    /// <summary>
    /// A Boolean value that determines whether a multiplayer session supports crossplay between Xbox One and Windows 10.
    /// True indicates that crossplay is supported.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(Crossplay, crossplay, bool);

    /// <summary>
    /// True if the session supports team capability used for a tournament. 
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(Team, team, bool);

    /// <summary>
    /// True if the session supports arbitration used for a tournament.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(Arbitration, arbitration, bool);

    /// <summary>
    /// True, if the session can be linked to a search handle for searching.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(Searchable, searchable, bool);

    /// <summary>
    /// True, if the session has owners. If you have user_authorization_style set, then in order to be searchable, you must have owners set.
    /// </summary>
    DEFINE_PROP_GETSET_OBJ(HasOwners, has_owners, bool);

internal:
    MultiplayerSessionCapabilities(
        _In_ xbox::services::multiplayer::multiplayer_session_capabilities cppObj
        );

    xbox::services::multiplayer::multiplayer_session_capabilities GetCppObj() const;
private:
    xbox::services::multiplayer::multiplayer_session_capabilities m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
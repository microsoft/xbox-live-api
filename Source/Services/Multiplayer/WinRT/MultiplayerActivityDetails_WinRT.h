// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/multiplayer.h"
#include "MultiplayerSessionVisibility_WinRT.h"
#include "MultiplayerSessionRestriction_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents a user's current multiplayer activity, along with some details about the corresponding session.
/// </summary>
public ref class MultiplayerActivityDetails sealed
{
public:
    /// <summary>
    /// The multiplayer session reference that contains identifying information for the session.
    /// </summary>
    property MultiplayerSessionReference^ SessionReference { MultiplayerSessionReference^ get(); }

    /// <summary>
    /// The identifier of the handles that corresponds to the multiplayer activity.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(HandleId, handle_id);

    /// <summary>
    /// The identifier of the title that must be launched in order to join the multiplayer activity.
    /// </summary>
    DEFINE_PROP_GET_OBJ(TitleId, title_id, uint32);

    /// <summary>
    /// The visibility state of the session, which indicates Whether other users can see, or join, etc.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(Visibility, visibility, MultiplayerSessionVisibility);

    /// <summary>
    /// The join restriction of the session, which applies if visibility is "open".
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(JoinRestriction, join_restriction, MultiplayerSessionRestriction);

    /// <summary>
    /// Indicates whether the session is temporarily closed for joining.
    /// </summary>
    DEFINE_PROP_GET_OBJ(Closed, closed, bool);

    /// <summary>
    /// The Xbox User ID of the owner for the multiplayer activity. 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(OwnerXboxUserId, owner_xbox_user_id);

    /// <summary>
    /// The number of total session member slots supported for the multiplayer activity.
    /// </summary>
    DEFINE_PROP_GET_OBJ(MaxMembersCount, max_members_count, uint32);

    /// <summary>
    /// The number of slots occupied.
    /// </summary>
    DEFINE_PROP_GET_OBJ(MembersCount, members_count, uint32);

    /// <summary>
    /// String containing custom session properties JSON blob.
    /// </summary>
    DEFINE_PROP_GET_STR_FROM_JSON_OBJ(CustomSessionPropertiesJson, custom_session_properties_json);

internal:
    MultiplayerActivityDetails(
        _In_ xbox::services::multiplayer::multiplayer_activity_details cppObj
        );

private:
    xbox::services::multiplayer::multiplayer_activity_details m_cppObj;
    MultiplayerSessionReference^ m_sessionReference;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
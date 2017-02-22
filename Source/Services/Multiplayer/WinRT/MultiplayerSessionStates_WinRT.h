// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "Macros_WinRT.h"
#include "MultiplayerSessionReference_WinRT.h" 
#include "MultiplayerSessionStatus_WinRT.h"
#include "MultiplayerSessionVisibility_WinRT.h"
#include "MultiplayerSessionRestriction_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

//Example:


/// <summary>
/// Represents high-level information about the state of a session.
/// </summary>
/// <example>
/// The following example shows the format of the JSON object that represents
/// this class:
/// <code>
///    "results": [
///    {
///        "xuid": "9876",
///        "startTime": "2013-02-01T00:00:00Z",
///        "sessionRef": {
///            "scid": "foo",
///            "templateName": "bar",
///            "name": "session-seven"
///        },
///        "accepted": 4,
///        "status": "Active",
///        "visibility": "Open",
///        "joinRestriction": "Local",
///        "myTurn": true,
///        "keywords": [
///            "one",
///            "two"
///        ]
///    }
///    ]
/// </code>
/// </example>
public ref class MultiplayerSessionStates sealed
{
public:
    /// <summary>
    /// The data and time that the session began.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(StartTime, start_time);

    /// <summary>
    /// A reference to the object that contains identifying information for the session.
    /// </summary>
    property MultiplayerSessionReference^ SessionReference { MultiplayerSessionReference^ get(); }

    /// <summary>
    /// The current status of the session.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(Status, status, MultiplayerSessionStatus);

    /// <summary>
    /// The visibility state of the session. Whether other users can see, or join, etc.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(Visibility, visibility, MultiplayerSessionVisibility);

    /// <summary>
    /// Indicates if it is the session member's turn.
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsMyTurn, is_my_turn, bool);

    /// <summary>
    /// The Xbox User ID of the member. 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(XboxUserId, xbox_user_id);

    /// <summary>
    /// Approximate number of non-reserved members that have been accepted into the session.
    /// </summary>
    DEFINE_PROP_GET_OBJ(AcceptedMemberCount, accepted_member_count, uint32);

    /// <summary>
    /// The restriction applied to the member when joining the session.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(JoinRestriction, join_restriction, MultiplayerSessionRestriction);

    /// <summary>
    /// (Optional) A collection of case-insensitive keywords associated with the session. This value might be empty.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ Keywords
    {
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get();
    }

internal:
    MultiplayerSessionStates(
        _In_ xbox::services::multiplayer::multiplayer_session_states cppObj
        );

private:
    xbox::services::multiplayer::multiplayer_session_states m_cppObj;
    MultiplayerSessionReference^ m_sessionReference;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_keywords;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
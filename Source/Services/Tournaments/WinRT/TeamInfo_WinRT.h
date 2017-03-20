// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/tournaments.h"
#include "TeamState_WinRT.h"
#include "TeamCompletedReason_WinRT.h"
#include "CurrentMatchMetadata_WinRT.h"
#include "PreviousMatchMetadata_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents details of the team participating in the tournament.
/// </summary>
public ref class TeamInfo sealed
{
public:
    /// <summary>
    /// The ID of the team. It is an opaque string specified by the tournament organizer.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Id, id);

    /// <summary>
    /// The display name of the team
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary>
    /// Member xbox_user_ids for this team.
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ MemberXboxUserIds
    { 
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); 
    }

    /// <summary>
    /// The timestamp at which the team was registered. Used to determine ordering if team is waitlisted. 
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(RegistrationDate, registration_date);

    /// <summary>
    /// A formatted string that describes the team's standing in the tournament. For example '1W - 0L'.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Standing, standing);

    /// <summary>
    /// The state of the team.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(TeamState, state, Microsoft::Xbox::Services::Tournaments::TeamState);

    /// <summary>
    /// The reason the team is in the Completed state.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(CompletedReason, completed_reason, Microsoft::Xbox::Services::Tournaments::TeamCompletedReason);

    /// <summary>
    /// The team's final rank within the tournament, if it is available. If missing, the rank is not available.
    /// </summary>
    DEFINE_PROP_GET_OBJ(Ranking, ranking, uint64);

    /// <summary>
    /// A protocol activation URI the title can follow after the match is done to take the user back 
    /// to the Xbox App's Tournament Details Page.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ContinuationUri, continuation_uri);

    /// <summary>
    /// Information about the team's current or upcoming match. Absent if the team does not have an current or upcoming match.
    /// </summary>
    property Microsoft::Xbox::Services::Tournaments::CurrentMatchMetadata^ CurrentMatchMetadata 
    { 
        Microsoft::Xbox::Services::Tournaments::CurrentMatchMetadata^ get();
    }

    /// <summary>
    /// Information about the previously played match. Absent if the player has not played any matches.
    /// </summary>
    property Microsoft::Xbox::Services::Tournaments::PreviousMatchMetadata^ PreviousMatchMetadata
    {
        Microsoft::Xbox::Services::Tournaments::PreviousMatchMetadata^ get();
    }

internal:
    TeamInfo(
        _In_ xbox::services::tournaments::team_info cppObj
        );

    const xbox::services::tournaments::team_info& GetCppObj() const;

private:
    xbox::services::tournaments::team_info m_cppObj;
    Microsoft::Xbox::Services::Tournaments::CurrentMatchMetadata^ m_currentMatchMetadata;
    Microsoft::Xbox::Services::Tournaments::PreviousMatchMetadata^ m_previousMatchMetadata;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_memberXboxUserIds;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END

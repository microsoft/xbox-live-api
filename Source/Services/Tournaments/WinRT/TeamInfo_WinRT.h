//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/tournaments.h"
#include "TeamState_WinRT.h"
#include "TeamCompletedReason_WinRT.h"
#include "CurrentMatchMetadata_WinRT.h"
#include "PreviousMatchMetadata_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents 
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

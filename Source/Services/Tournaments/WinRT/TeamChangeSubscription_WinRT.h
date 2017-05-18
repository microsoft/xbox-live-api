#pragma once
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/tournaments.h"
#include "Tournament_WinRT.h"
#include "RealTimeActivitySubscriptionState_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Handles notification when the state of a team subscription changes.
/// </summary>
public ref class TeamChangeSubscription sealed
{
public:
    /// <summary>
    /// Indicates the state of the subscription.
    /// </summary>
    property Microsoft::Xbox::Services::RealTimeActivity::RealTimeActivitySubscriptionState State 
    { 
        Microsoft::Xbox::Services::RealTimeActivity::RealTimeActivitySubscriptionState get(); 
    }

    /// <summary>
    /// The organizer ID the subscription is for.
    /// </summary> 
    DEFINE_PTR_PROP_GET_STR_OBJ(OrganizerId, organizer_id);

    /// <summary>
    /// The tournament ID the subscription is for.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(TournamentId, tournament_id);

    /// <summary>
    /// The team ID the subscription is for.
    /// </summary>
    DEFINE_PTR_PROP_GET_STR_OBJ(TeamId, team_id);

internal:
    TeamChangeSubscription(_In_ std::shared_ptr<xbox::services::tournaments::team_change_subscription> cppObj);
    std::shared_ptr<xbox::services::tournaments::team_change_subscription> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::tournaments::team_change_subscription> m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
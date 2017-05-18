// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once 
#include "xsapi/tournaments.h"
#include "Tournament_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Contains information about a change to a subscribed team.
/// </summary>
public ref class TournamentChangeEventArgs sealed
{
public:
    /// <summary>
    /// The organizer ID used to create the subscription.
    /// </summary> 
    DEFINE_PROP_GET_STR_OBJ(OrganizerId, organizer_id);

    /// <summary>
    /// The tournament ID used to create the subscription.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(TournamentId, tournament_id);

internal:
    TournamentChangeEventArgs(_In_ xbox::services::tournaments::tournament_change_event_args cppObj);

private:
    xbox::services::tournaments::tournament_change_event_args m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
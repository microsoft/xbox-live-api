#pragma once
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "MultiplayerEventArgs_WinRT.h"
#include "TournamentRegistrationState_WinRT.h"
#include "TournamentRegistrationReason_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Contains information for an event that indicates when the tournament registration state changes.
/// </summary>
public ref class TournamentRegistrationStateChangedEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// The tournament team's registration state.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(RegistrationState, registration_state, Microsoft::Xbox::Services::Tournaments::TournamentRegistrationState);

    /// <summary>
    /// The tournament team's registration reason for the certain registration states.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(RegistrationReason, registration_reason, Microsoft::Xbox::Services::Tournaments::TournamentRegistrationReason);

internal:
    /// <summary>
    /// Internal function.
    /// </summary>
    TournamentRegistrationStateChangedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::tournament_registration_state_changed_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::tournament_registration_state_changed_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::tournament_registration_state_changed_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END

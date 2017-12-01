// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "MultiplayerEventArgs_WinRT.h"
#include "MatchStatus_WinRT.h"
#include "MultiplayerMeasurementFailure_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

/// <summary>
/// Contains information for an event that indicates when a multiplayer match is found.
/// </summary>
public ref class FindMatchCompletedEventArgs sealed : MultiplayerEventArgs
{
public:

    /// <summary>
    /// Provides the current matchmaking status.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(MatchStatus, match_status, Microsoft::Xbox::Services::Multiplayer::Manager::MatchStatus);

    /// <summary>
    /// The cause of why the initialization failed, or MultiplayerMeasurementFailure::None if there was no failure.
    /// Set when transitioning out of the "joining" or "measuring" stage if this member doesn't pass.
    /// </summary>
    DEFINE_PTR_PROP_GET_ENUM_OBJ(InitializationFailureCause, initialization_failure_cause, Microsoft::Xbox::Services::Multiplayer::MultiplayerMeasurementFailure);

internal:
    /// <summary>
    /// Internal function.
    /// </summary>
    FindMatchCompletedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::find_match_completed_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::find_match_completed_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::find_match_completed_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END

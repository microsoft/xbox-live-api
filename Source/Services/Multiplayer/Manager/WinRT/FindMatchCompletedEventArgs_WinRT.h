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
    FindMatchCompletedEventArgs(
        _In_ std::shared_ptr<xbox::services::multiplayer::manager::find_match_completed_event_args> cppObj
        );

    std::shared_ptr<xbox::services::multiplayer::manager::find_match_completed_event_args> GetCppObj() const;

private:
    std::shared_ptr<xbox::services::multiplayer::manager::find_match_completed_event_args> m_cppObj;

};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
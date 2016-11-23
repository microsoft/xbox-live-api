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
#include "shared_macros.h"
#include "xsapi/multiplayer.h"
#include "MultiplayerSessionReference_WinRT.h"
#include "TournamentTeamResult_WinRT.h"
#include "TournamentArbitrationState_WinRT.h"
#include "TournamentGameResultSource_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

/// <summary>
/// Represents the server that holds info about a tournament games result. 
/// </summary>
public ref class MultiplayerSessionArbitrationServer sealed
{
public:
    /// <summary>
    /// The state of the result
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(ResultState, result_state, Microsoft::Xbox::Services::Tournaments::TournamentArbitrationState);

    /// <summary>
    /// The source of the result
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(ResultSource, result_source, Microsoft::Xbox::Services::Tournaments::TournamentGameResultSource);

    /// <summary>
    /// A value from 0-100 that indicates the confidence level of this result
    /// </summary>
    DEFINE_PROP_GET_OBJ(ResultConfidenceLevel, result_confidence_level, uint32)

    /// <summary>
    /// The results of the game
    /// </summary>
    property Windows::Foundation::Collections::IMapView<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ Results { Windows::Foundation::Collections::IMapView<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ get(); }

internal:
    MultiplayerSessionArbitrationServer(_In_ xbox::services::multiplayer::multiplayer_session_arbitration_server cppObj);

private:
    xbox::services::multiplayer::multiplayer_session_arbitration_server m_cppObj;
    Platform::Collections::Map<Platform::String^, Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^>^ m_results;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
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
#include "MatchMetadata_WinRT.h"
#include "TournamentTeamResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents a reference to a multiplayer session.
/// </summary>
public ref class PreviousMatchMetadata sealed
{
public:

    /// <summary>
    /// The last game's result for the tournament.
    /// </summary>
    property Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ Result
    {
        Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ get();
    }

    /// <summary>
    /// The timestamp when the match ended.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(MatchEndTime, match_end_time);

    /// <summary>
    /// 
    /// </summary>
    property MatchMetadata^ MatchDetails { MatchMetadata^ get(); }

internal:
    PreviousMatchMetadata(
        _In_ xbox::services::tournaments::previous_match_metadata cppObj
        );

    const xbox::services::tournaments::previous_match_metadata& GetCppObj() const;

private:
    xbox::services::tournaments::previous_match_metadata m_cppObj;
    MatchMetadata^ m_matchDetails;
    Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ m_result;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END

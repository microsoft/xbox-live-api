// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
    /// Previous game's result for the tournament.
    /// </summary>
    property Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ Result
    {
        Microsoft::Xbox::Services::Tournaments::TournamentTeamResult^ get();
    }

    /// <summary>
    /// The timestamp when the match ended.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(EndTime, end_time);

    /// <summary>
    /// Metadata associated with the team's previously played match.
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

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/tournaments.h"
#include "MultiplayerSessionReference_WinRT.h"
#include "MatchMetadata_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

/// <summary>
/// Represents a reference to a multiplayer session.
/// </summary>
public ref class CurrentMatchMetadata sealed
{
public:

    /// <summary>
    /// The uniquely identifying information for the game session.
    /// </summary>
    property Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ GameSessionReference 
    { 
        Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ get(); 
    }

    /// <summary>
    /// Metadata associated with the team's current or upcoming match.
    /// </summary>
    property MatchMetadata^ MatchDetails { MatchMetadata^ get(); }

internal:
    CurrentMatchMetadata(
        _In_ xbox::services::tournaments::current_match_metadata cppObj
        );

    const xbox::services::tournaments::current_match_metadata& GetCppObj() const;

private:
    xbox::services::tournaments::current_match_metadata m_cppObj;
    Microsoft::Xbox::Services::Multiplayer::MultiplayerSessionReference^ m_gameSessionReference;
    MatchMetadata^ m_matchDetails;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END

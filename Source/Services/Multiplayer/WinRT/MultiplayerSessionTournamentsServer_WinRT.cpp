//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "MultiplayerSessionTournamentsServer_WinRT.h"
#include "TournamentReference_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionTournamentsServer::MultiplayerSessionTournamentsServer(
    _In_ xbox::services::multiplayer::multiplayer_session_tournaments_server cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    if (!m_cppObj.next_game_session_reference().is_null())
    {
        m_nextGameSessionRef = ref new MultiplayerSessionReference(m_cppObj.next_game_session_reference());
    }

    if (!m_cppObj.tournament_reference().is_null())
    {
        m_tournamentRef = ref new Microsoft::Xbox::Services::Tournaments::TournamentReference(m_cppObj.tournament_reference());
    }

    m_lastTeamResult = ref new Microsoft::Xbox::Services::Tournaments::TournamentTeamResult(m_cppObj.last_team_result());

    m_teams = UtilsWinRT::CreatePlatformMapObjectWithStringKeyFromStdMapObj<MultiplayerSessionReference>(m_cppObj.teams());
}

MultiplayerSessionReference^
MultiplayerSessionTournamentsServer::NextGameSessionRef::get()
{
    return m_nextGameSessionRef;
}

TournamentReference^
MultiplayerSessionTournamentsServer::TournamentReference::get()
{
    return m_tournamentRef;
}

TournamentTeamResult^
MultiplayerSessionTournamentsServer::LastTeamResult::get()
{
    return m_lastTeamResult;
}

Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerSessionReference^>^
MultiplayerSessionTournamentsServer::Teams::get()
{
    return m_teams->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
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
#include "TournamentService_WinRT.h"
#include "Utils_WinRT.h"

using namespace Platform;
using namespace concurrency;
using namespace xbox::services;
using namespace xbox::services::tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TournamentService::TournamentService(
    _In_ xbox::services::tournaments::tournament_service cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

const xbox::services::tournaments::tournament_service&
TournamentService::GetCppObj() const
{
    return m_cppObj;
}

Windows::Foundation::IAsyncOperation<TournamentRequestResult^>^  
TournamentService::GetTournamentsAsync(
    _In_ TournamentRequest^ request
    )
{
    auto task = m_cppObj.get_tournaments(request->GetCppObj())
    .then([](xbox_live_result<tournament_request_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new TournamentRequestResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<Tournament^>^ 
TournamentService::GetTournamentDetailsAsync(
    _In_ Platform::String^ organizerId,
    _In_ Platform::String^ tournamentId
    )
{
    auto task = m_cppObj.get_tournament_details(
        STRING_T_FROM_PLATFORM_STRING(organizerId),
        STRING_T_FROM_PLATFORM_STRING(tournamentId)
        )
    .then([](xbox_live_result<tournament> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new Tournament(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<TeamRequestResult^>^
TournamentService::GetTeamsAsync(
    _In_ TeamRequest^ request
    )
{
    auto task = m_cppObj.get_teams(request->GetCppObj())
    .then([](xbox_live_result<team_request_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new TeamRequestResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<TeamInfo^>^
TournamentService::GetTeamDetailsAsync(
    _In_ Platform::String^ organizerId,
    _In_ Platform::String^ tournamentId,
    _In_ Platform::String^ teamId
    )
{
    auto task = m_cppObj.get_team_details(
        STRING_T_FROM_PLATFORM_STRING(organizerId),
        STRING_T_FROM_PLATFORM_STRING(tournamentId),
        STRING_T_FROM_PLATFORM_STRING(teamId)
    )
    .then([](xbox_live_result<team_info> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new TeamInfo(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
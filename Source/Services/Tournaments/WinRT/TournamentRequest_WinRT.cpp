// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TournamentRequest_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services::tournaments;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TournamentRequest::TournamentRequest(
    _In_ bool filterResultsForUser
    ) :
    m_cppObj(
        tournament_request(filterResultsForUser)
        )
{
}

TournamentRequest::TournamentRequest(
    _In_ xbox::services::tournaments::tournament_request cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

const xbox::services::tournaments::tournament_request&
TournamentRequest::GetCppObj() const
{
    return m_cppObj;
}

Windows::Foundation::Collections::IVectorView<TournamentState>^
TournamentRequest::StateFilter::get()
{
    return m_tournamentStates;
}

void
TournamentRequest::StateFilter::set(
    _In_ Windows::Foundation::Collections::IVectorView<TournamentState>^ states
    )
{
    m_tournamentStates = std::move(states);

    std::vector<tournament_state> tournamentStates;
    if (m_tournamentStates != nullptr)
    {
        for (const auto& state : m_tournamentStates)
        {
            tournamentStates.push_back(static_cast<tournament_state>(state));
        }
    }
    m_cppObj.set_state_filter(tournamentStates);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "TeamRequest_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services::tournaments;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TeamRequest::TeamRequest(
    _In_ Platform::String^ organizerId,
    _In_ Platform::String^ tournamentId,
    _In_ bool filterResultsForUser
    ) :
    m_cppObj(
        team_request(
            STRING_T_FROM_PLATFORM_STRING(organizerId),
            STRING_T_FROM_PLATFORM_STRING(tournamentId),
            filterResultsForUser
            )
        )
{
}

TeamRequest::TeamRequest(
    _In_ xbox::services::tournaments::team_request cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

const xbox::services::tournaments::team_request&
TeamRequest::GetCppObj() const
{
    return m_cppObj;
}

Windows::Foundation::Collections::IVectorView<TeamState>^
TeamRequest::StateFilter::get()
{
    return m_teamStates;
}

void
TeamRequest::StateFilter::set(
    _In_ Windows::Foundation::Collections::IVectorView<TeamState>^ states
    )
{
    m_teamStates = std::move(states);

    std::vector<team_state> teamStates;
    if (m_teamStates != nullptr)
    {
        for (const auto& state : m_teamStates)
        {
            teamStates.push_back(static_cast<team_state>(state));
        }
    }
    m_cppObj.set_state_filter(teamStates);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_END
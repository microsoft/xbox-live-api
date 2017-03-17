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
#include "TeamRequest_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services::tournaments;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_BEGIN

TeamRequest::TeamRequest(
    _In_ Platform::String^ organizerId,
    _In_ Platform::String^ tournamentId
    ) :
    m_cppObj(
        STRING_T_FROM_PLATFORM_STRING(organizerId),
        STRING_T_FROM_PLATFORM_STRING(tournamentId)
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
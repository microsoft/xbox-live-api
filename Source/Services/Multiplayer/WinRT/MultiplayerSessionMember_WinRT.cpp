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
#include "MultiplayerSessionMember_WinRT.h"
#include "Utils_WinRT.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Tournaments;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_BEGIN

MultiplayerSessionMember::MultiplayerSessionMember(
    _In_ std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member> cppObj
    ) :
    m_cppObj(cppObj)
{
    THROW_INVALIDARGUMENT_IF_NULL(cppObj);
    THROW_INVALIDARGUMENT_IF_NULL(m_cppObj->member_measurements());

    m_roles = ref new Platform::Collections::Map<Platform::String^, Platform::String^>();
    for (const auto& role : m_cppObj->roles())
    {
        m_roles->Insert(ref new Platform::String(role.first.c_str()), ref new Platform::String(role.second.c_str()));
    }

    m_memberMeasurements = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerQualityOfServiceMeasurements, xbox::services::multiplayer::multiplayer_quality_of_service_measurements>(
        *m_cppObj->member_measurements()
        );

    if (!m_cppObj->tournament_team_session_reference().is_null())
    {
        m_tournamentTeamSessionRef = ref new MultiplayerSessionReference(m_cppObj->tournament_team_session_reference());
    }

    m_results = UtilsWinRT::CreatePlatformMapObjectWithStringKeyFromStdMapObj<TournamentTeamResult>(m_cppObj->results());
}

IVector<MultiplayerSessionMember^>^
MultiplayerSessionMember::MembersInGroup::get()
{
    return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerSessionMember, std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member>>(
        m_cppObj->members_in_group()
        );
}

IVector<MultiplayerQualityOfServiceMeasurements^>^
MultiplayerSessionMember::MemberMeasurements::get()
{
    return m_memberMeasurements;
}

IVector<Platform::String^>^
MultiplayerSessionMember::Groups::get()
{
    return UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj->groups());
}

void
MultiplayerSessionMember::Groups::set(_In_ IVector<Platform::String^>^ value)
{
    m_cppObj->set_groups(UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(value->GetView()));
}

IVector<Platform::String^>^
MultiplayerSessionMember::Encounters::get()
{
    return UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj->encounters());
}

void
MultiplayerSessionMember::Encounters::set(_In_ IVector<Platform::String^>^ value)
{
    m_cppObj->set_encounters(UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(value->GetView()));
}

Windows::Foundation::Collections::IMap<Platform::String^, TournamentTeamResult^>^ 
MultiplayerSessionMember::Results::get()
{
    return m_results;
}

std::shared_ptr<xbox::services::multiplayer::multiplayer_session_member>
MultiplayerSessionMember::GetCppObj() const
{
    return m_cppObj;
}

MultiplayerSessionReference^
MultiplayerSessionMember::TournamentTeamSessionRef::get()
{
    return m_tournamentTeamSessionRef;
}

IMapView<Platform::String^, Platform::String^>^
MultiplayerSessionMember::Roles::get()
{
    return m_roles->GetView();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_END
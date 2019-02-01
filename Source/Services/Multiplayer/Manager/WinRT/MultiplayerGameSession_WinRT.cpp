// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MultiplayerGameSession_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Multiplayer;
using namespace Microsoft::Xbox::Services::Tournaments;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

MultiplayerGameSession::MultiplayerGameSession(
    _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_game_session> cppObj
    ) :
    m_cppObj(cppObj)
{
    XSAPI_ASSERT(cppObj != nullptr);
    UpdateCppObj(cppObj);
}

MultiplayerSessionReference^
MultiplayerGameSession::SessionReference::get()
{
    return m_sessionReference;
}

MultiplayerMember^
MultiplayerGameSession::Host::get()
{
    if (m_cppObj->host() == nullptr)
    {
        return nullptr;
    }

    return ref new MultiplayerMember(m_cppObj->host());
}

IVectorView<MultiplayerMember^>^
MultiplayerGameSession::Members::get()
{
    return m_members->GetView();

}

MultiplayerSessionConstants^
MultiplayerGameSession::SessionConstants::get()
{
    return m_sessionConstants;
}

Windows::Foundation::Collections::IMapView<Platform::String^, MultiplayerSessionReference^>^
MultiplayerGameSession::TournamentTeams::get()
{
    return m_teams->GetView();
}

Windows::Foundation::Collections::IMapView<Platform::String^, TournamentTeamResult^>^
MultiplayerGameSession::TournamentTeamResults::get()
{
    return m_tournamentTeamResults->GetView();
}

std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_game_session>
MultiplayerGameSession::GetCppObj() const
{
    return m_cppObj;
}

void
MultiplayerGameSession::UpdateCppObj(
    _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_game_session> cppObj
    )
{
    m_cppObj = cppObj;
    m_sessionReference = ref new MultiplayerSessionReference(m_cppObj->session_reference());
    m_sessionConstants = ref new MultiplayerSessionConstants(m_cppObj->session_constants());
    m_teams = UtilsWinRT::CreatePlatformMapObjectWithStringKeyFromStdMapObj<MultiplayerSessionReference>(m_cppObj->tournament_teams());
    m_tournamentTeamResults = UtilsWinRT::CreatePlatformMapObjectWithStringKeyFromStdMapObj<TournamentTeamResult>(m_cppObj->tournament_team_results());
    m_members = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerMember>(m_cppObj->members());
}

bool
MultiplayerGameSession::IsHost( 
    _In_ Platform::String^ xboxUserId
    )
{
    return m_cppObj->is_host(STRING_T_FROM_PLATFORM_STRING(xboxUserId));
}

void
MultiplayerGameSession::SetProperties( 
    _In_ Platform::String^ name,
    _In_ Platform::String^ valueJson,
    _In_opt_ context_t context
    )
{
    auto valueJsonString = UtilsWinRT::JsonValueFromPlatformString(valueJson);
    auto result = m_cppObj->set_properties(
        STRING_T_FROM_PLATFORM_STRING(name),
        valueJsonString,
        context
    );

    THROW_IF_ERR(result);
}

void
MultiplayerGameSession::SetLocalMemberProperties(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ name,
    _In_opt_ Platform::String^ valueJson,
    _In_opt_ context_t context
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);

    auto valueJsonString = UtilsWinRT::JsonValueFromPlatformString(valueJson);
    auto result = m_cppObj->set_local_member_properties(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name),
        valueJsonString,
        context
    );

    THROW_IF_ERR(result);
}

void
MultiplayerGameSession::SetLocalMemberGroups(
    _In_ XboxLiveUser_t user,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String ^>^ Groups,
    _In_opt_ context_t context
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);

    auto groupsVector = UtilsWinRT::CovertVectorViewToStdVectorString(Groups);
    auto result = m_cppObj->set_local_member_groups(
        user_context::user_convert(user),
        groupsVector,
        context
    );

    THROW_IF_ERR(result);
}

void
MultiplayerGameSession::SetLocalMemberServerQoSMeasurements(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ jsonValueString,
    _In_opt_ context_t context
)
{
    THROW_INVALIDARGUMENT_IF_NULL(user);

    auto jsonValue = UtilsWinRT::JsonValueFromPlatformString(jsonValueString);
    auto result = m_cppObj->set_local_member_server_qos_measurements(
        user_context::user_convert(user),
        jsonValue,
        context
	);

    THROW_IF_ERR(result);
}

void
MultiplayerGameSession::SetSynchronizedProperties( 
    _In_ Platform::String^ name,
    _In_opt_ Platform::String^ valueJson,
    _In_opt_ context_t context
    )
{
    auto valueJsonString = UtilsWinRT::JsonValueFromPlatformString(valueJson);
    auto result = m_cppObj->set_synchronized_properties(
        STRING_T_FROM_PLATFORM_STRING(name),
        valueJsonString,
        context
    );

    THROW_IF_ERR(result);
}

void
MultiplayerGameSession::SetSynchronizedHost( 
    _In_ MultiplayerMember^ gameHost,
    _In_opt_ context_t context
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(gameHost);

    auto result = m_cppObj->set_synchronized_host(gameHost->GetCppObj(), context);
    THROW_IF_ERR(result);
}

Windows::Foundation::Collections::IVector<Platform::String^>^
MultiplayerGameSession::ServerConnectionCandidates::get()
{
    return UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj->server_connection_string_candidates());
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
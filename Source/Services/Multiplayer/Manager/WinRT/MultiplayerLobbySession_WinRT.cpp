// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "MultiplayerLobbySession_WinRT.h"
#include "XboxLiveContext_WinRT.h"
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

MultiplayerLobbySession::MultiplayerLobbySession(
    _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_lobby_session> cppObj
    ) :
    m_cppObj(cppObj)
{
    XSAPI_ASSERT(cppObj != nullptr);

    m_localMembers = ref new Platform::Collections::Vector<MultiplayerMember^>();
    m_lobbyMembers = ref new Platform::Collections::Vector<MultiplayerMember^>();
}

IVectorView<MultiplayerMember^>^
MultiplayerLobbySession::LocalMembers::get()
{
    return m_localMembers->GetView();
}

IVectorView<MultiplayerMember^>^
MultiplayerLobbySession::Members::get()
{
    return m_lobbyMembers->GetView();
}

MultiplayerSessionReference^
MultiplayerLobbySession::SessionReference::get()
{
    return m_sessionReference;
}

MultiplayerMember^
MultiplayerLobbySession::Host::get()
{
    if (m_cppObj->host() == nullptr)
    {
        return nullptr;
    }

    return ref new MultiplayerMember(m_cppObj->host());
}

MultiplayerSessionConstants^
MultiplayerLobbySession::SessionConstants::get()
{
    return m_sessionConstants;
}

TournamentTeamResult^
MultiplayerLobbySession::LastTournamentTeamResult::get()
{
    return m_lastTournamentTeamResult;
}

std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_lobby_session>
MultiplayerLobbySession::GetCppObj() const
{
    return m_cppObj;
}

void
MultiplayerLobbySession::UpdateCppObj(
    _In_ std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_lobby_session> cppObj
    )
{
    if (m_cppObj->_Change_number() != cppObj->_Change_number())
    {
        m_cppObj = cppObj;
        if (m_cppObj->_Change_number() != 0)
        {
            m_sessionReference = ref new MultiplayerSessionReference(m_cppObj->session_reference());
            m_sessionConstants = ref new MultiplayerSessionConstants(m_cppObj->session_constants());
            m_lastTournamentTeamResult = ref new TournamentTeamResult(m_cppObj->last_tournament_team_result());
            m_localMembers = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerMember>(m_cppObj->local_members());
            m_lobbyMembers = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerMember>(m_cppObj->members());
        }
        else
        {
            m_sessionReference = nullptr;
            m_sessionConstants = nullptr;
            m_lastTournamentTeamResult = nullptr;
            m_localMembers->Clear();
            m_lobbyMembers->Clear();
        }
    }
}

void
MultiplayerLobbySession::AddLocalUser( 
    _In_ XboxLiveUser_t user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);

    xbox_live_result<void> result;
    result = m_cppObj->add_local_user(user_context::user_convert(user));

    THROW_IF_ERR(result);
}

void
MultiplayerLobbySession::RemoveLocalUser( 
    _In_ XboxLiveUser_t user
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);

    xbox_live_result<void> result;
    result = m_cppObj->remove_local_user(user_context::user_convert(user));
    THROW_IF_ERR(result);
}

void
MultiplayerLobbySession::SetLocalMemberProperties( 
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
MultiplayerLobbySession::SetLocalMemberGroups( 
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
MultiplayerLobbySession::SetLocalMemberServerQoSMeasurements(
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
MultiplayerLobbySession::DeleteLocalMemberProperties( 
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ name,
    _In_opt_ context_t context
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);

    auto result = m_cppObj->delete_local_member_properties(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(name),
        context
    );

    THROW_IF_ERR(result);
}

void 
MultiplayerLobbySession::SetLocalMemberConnectionAddress(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ connectionAddress,
    _In_opt_ context_t context
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);

    auto result = m_cppObj->set_local_member_connection_address(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(connectionAddress),
        context
    );

    THROW_IF_ERR(result);
}

bool
MultiplayerLobbySession::IsHost( 
    _In_ Platform::String^ xboxUserId
    )
{
    return m_cppObj->is_host(STRING_T_FROM_PLATFORM_STRING(xboxUserId));
}

void
MultiplayerLobbySession::SetProperties( 
    _In_ Platform::String^ name,
    _In_opt_ Platform::String^ valueJson,
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
MultiplayerLobbySession::SetSynchronizedProperties( 
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
MultiplayerLobbySession::SetSynchronizedHost( 
    _In_ MultiplayerMember^ gameHost,
    _In_opt_ context_t context
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(gameHost);

    auto result = m_cppObj->set_synchronized_host(
        gameHost->GetCppObj(),
        context
    );
    THROW_IF_ERR(result);
}

void
MultiplayerLobbySession::InviteFriends(
    _In_ XboxLiveUser_t user,
    _In_ Platform::String^ contextStringId,
    _In_ Platform::String^ customActivationContext
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);

    auto result = m_cppObj->invite_friends(
        user_context::user_convert(user),
        STRING_T_FROM_PLATFORM_STRING(contextStringId),
        STRING_T_FROM_PLATFORM_STRING(customActivationContext)
        );
    THROW_IF_ERR(result);
}

void
MultiplayerLobbySession::InviteUsers(
    _In_ XboxLiveUser_t user,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds,
    _In_ Platform::String^ contextStringId,
    _In_ Platform::String^ customActivationContext
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(user);
    THROW_INVALIDARGUMENT_IF_NULL(xboxUserIds);

    auto result = m_cppObj->invite_users(
        user_context::user_convert(user),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(xboxUserIds),
        STRING_T_FROM_PLATFORM_STRING(contextStringId),
        STRING_T_FROM_PLATFORM_STRING(customActivationContext)
        );
    THROW_IF_ERR(result);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
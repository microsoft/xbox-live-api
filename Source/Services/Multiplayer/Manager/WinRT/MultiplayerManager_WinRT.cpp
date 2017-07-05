// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "MultiplayerManager_WinRT.h"
#include "MultiplayerEvent_WinRT.h"
#include "Utils_WinRT.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::multiplayer::manager;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Microsoft::Xbox::Services::Multiplayer;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

MultiplayerManager::MultiplayerManager() :
    m_cppObj(multiplayer_manager::get_singleton_instance())
{
}

MultiplayerManager^
MultiplayerManager::SingletonInstance::get()
{
    auto xsapiSingleton = get_xsapi_singleton();
    if (xsapiSingleton->m_winrt_multiplayerManagerInstance == nullptr)
    {
        MultiplayerManager^ mpManager = ref new MultiplayerManager();

        {
            std::lock_guard<std::mutex> guard(xsapiSingleton->m_singletonLock);
            if (xsapiSingleton->m_winrt_multiplayerManagerInstance == nullptr)
            {
                xsapiSingleton->m_winrt_multiplayerManagerInstance = mpManager;
            }
        }
    }
    
    return xsapiSingleton->m_winrt_multiplayerManagerInstance;
}

std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_manager>
MultiplayerManager::GetCppObj() const
{
    return m_cppObj;
}

void
MultiplayerManager::Initialize(
    _In_ Platform::String^ lobbySessionTemplateName
    )
{
    m_cppObj->initialize(
        STRING_T_FROM_PLATFORM_STRING(lobbySessionTemplateName)
        );

    m_multiplayerLobbySession = ref new MultiplayerLobbySession(m_cppObj->lobby_session());
}

void
MultiplayerManager::FindMatch(
    _In_ Platform::String^ hopperName,
    _In_opt_ Platform::String^ attributes,
    _In_opt_ Windows::Foundation::TimeSpan timeout
    )
{
    auto valueJsonString = UtilsWinRT::JsonValueFromPlatformString(attributes);
    auto result = m_cppObj->find_match(
        STRING_T_FROM_PLATFORM_STRING(hopperName),
        valueJsonString,
        UtilsWinRT::ConvertTimeSpanToSeconds<std::chrono::seconds>(timeout)
        );

    THROW_IF_ERR(result);
}

void
MultiplayerManager::CancelMatch()
{
    m_cppObj->cancel_match();
}

void
MultiplayerManager::JoinLobby(
    _In_ Platform::String^ handleId,
    _In_ XboxLiveUser_t user
    )
{
    auto result = m_cppObj->join_lobby(
        STRING_T_FROM_PLATFORM_STRING(handleId),
        user_context::user_convert(user)
        );

    THROW_IF_ERR(result);
}

#if TV_API
void
MultiplayerManager::JoinLobby(
    _In_ Platform::String^ handleId,
    _In_ Windows::Foundation::Collections::IVectorView<Windows::Xbox::System::User^>^ users
    )
{
    auto vector = std::vector<Windows::Xbox::System::User^>();
    for (const auto& user : users)
    {
        vector.push_back(user);
    }

    auto result = m_cppObj->join_lobby(
        STRING_T_FROM_PLATFORM_STRING(handleId),
        vector
        );

    THROW_IF_ERR(result);
}

void
MultiplayerManager::JoinLobby(
    _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
    _In_ Windows::Foundation::Collections::IVectorView<Windows::Xbox::System::User^>^ users
    )
{
    auto vector = std::vector<Windows::Xbox::System::User^>();
    for (const auto& user : users)
    {
        vector.push_back(user);
    }

    auto result = m_cppObj->join_lobby(
        eventArgs,
        vector
        );

    THROW_IF_ERR(result);
}

void
MultiplayerManager::InvitePartyToGame()
{
}
#endif

void
MultiplayerManager::JoinLobby(
    _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
    _In_ XboxLiveUser_t user
    )
{
    auto result = m_cppObj->join_lobby(
        eventArgs,
        user_context::user_convert(user)
        );

    THROW_IF_ERR(result);
}

void
MultiplayerManager::JoinGameFromLobby(
    _In_ Platform::String^ sessionTemplateName
    )
{
    auto result = m_cppObj->join_game_from_lobby(STRING_T_FROM_PLATFORM_STRING(sessionTemplateName));
    THROW_IF_ERR(result);
}

void
MultiplayerManager::JoinGame(
    _In_ Platform::String^ sessionName,
    _In_ Platform::String^ sessionTemplateName
)
{
    auto result = m_cppObj->join_game(
        STRING_T_FROM_PLATFORM_STRING(sessionName),
        STRING_T_FROM_PLATFORM_STRING(sessionTemplateName)
    );
    THROW_IF_ERR(result);
}

void
MultiplayerManager::JoinGame(
    _In_ Platform::String^ sessionName,
    _In_ Platform::String^ sessionTemplateName,
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ xboxUserIds
    )
{
    auto result = m_cppObj->join_game(
        STRING_T_FROM_PLATFORM_STRING(sessionName),
        STRING_T_FROM_PLATFORM_STRING(sessionTemplateName),
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(xboxUserIds)
        );
    THROW_IF_ERR(result);
}

void
MultiplayerManager::LeaveGame()
{
    auto result = m_cppObj->leave_game();
    THROW_IF_ERR(result);
}


MultiplayerLobbySession^
MultiplayerManager::LobbySession::get()
{
    if (m_multiplayerLobbySession == nullptr)
    {
        throw Microsoft::Xbox::Services::System::UtilsWinRT::ConvertHRESULTToException(E_FAIL, ref new Platform::String(L"Call MultiplayerManager::Initialize() first."));
    }
    return m_multiplayerLobbySession;
}

MultiplayerGameSession^
MultiplayerManager::GameSession::get()
{
    return m_multiplayerGameSession;
}

IVectorView<MultiplayerEvent^>^
MultiplayerManager:: DoWork()
{
    auto multiplayerEvents = m_cppObj->do_work();
    UpdateSessions();

    return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<MultiplayerEvent, multiplayer_event>(multiplayerEvents)->GetView();
}

void
MultiplayerManager::UpdateSessions()
{
    if (m_multiplayerLobbySession != nullptr)
    {
        m_multiplayerLobbySession->UpdateCppObj(m_cppObj->lobby_session());
    }

    if (m_cppObj->game_session() != nullptr)
    {
        if (m_multiplayerGameSession == nullptr)
        {
            m_multiplayerGameSession = ref new MultiplayerGameSession(m_cppObj->game_session());
        }
        else
        {
            m_multiplayerGameSession->UpdateCppObj(m_cppObj->game_session());
        }
    }
    else
    {
        m_multiplayerGameSession = nullptr;
    }
}

Microsoft::Xbox::Services::Multiplayer::Manager::MatchStatus
MultiplayerManager::MatchStatus::get()
{
    return static_cast<Microsoft::Xbox::Services::Multiplayer::Manager::MatchStatus>(m_cppObj->match_status());
}

Microsoft::Xbox::Services::Multiplayer::Manager::Joinability
MultiplayerManager::Joinability::get()
{
    return static_cast<Microsoft::Xbox::Services::Multiplayer::Manager::Joinability>(m_cppObj->joinability());
}

TimeSpan
MultiplayerManager::EstimatedMatchWaitTime::get()
{
    return UtilsWinRT::ConvertSecondsToTimeSpan(m_cppObj->estimated_match_wait_time());
}

bool
MultiplayerManager::AutoFillMembersDuringMatchmaking::get()
{
    return m_cppObj->auto_fill_members_during_matchmaking();
}

void
MultiplayerManager::AutoFillMembersDuringMatchmaking::set(_In_ bool value)
{
    m_cppObj->set_auto_fill_members_during_matchmaking(value);
}

void
MultiplayerManager::SetJoinInProgress(
    _In_ Microsoft::Xbox::Services::Multiplayer::Manager::Joinability value
    )
{
    auto result = m_cppObj->set_joinability(
        static_cast<xbox::services::multiplayer::manager::joinability>(value)
    );

    THROW_IF_ERR(result);
}

void
MultiplayerManager::SetQualityOfServiceMeasurements( 
    _In_ Windows::Foundation::Collections::IVectorView<Xbox::Services::Multiplayer::MultiplayerQualityOfServiceMeasurements^>^ measurements
    )
{
    m_cppObj->set_quality_of_service_measurements(
        std::make_shared<std::vector<xbox::services::multiplayer::multiplayer_quality_of_service_measurements>>(
            UtilsWinRT::CreateStdVectorObjectFromPlatformVectorObj<xbox::services::multiplayer::multiplayer_quality_of_service_measurements>(
                measurements
                )
            )
        );
}

#if UNIT_TEST_SERVICES
void
MultiplayerManager::_Shutdown()
{
    m_cppObj->_Shutdown();
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
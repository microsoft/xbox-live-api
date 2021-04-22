// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::multiplayer;
using namespace xbox::services::multiplayer::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerManager::~MultiplayerManager()
{
    if (m_queue)
    {
        XTaskQueueCloseHandle(m_queue);
    }
}

bool MultiplayerManager::IsInitialized()
{
    return m_multiplayerClientManager != nullptr;
}

void MultiplayerManager::Initialize(
    _In_ const xsapi_internal_string& lobbySessionTemplateName,
    _In_opt_ XTaskQueueHandle asyncQueue
)
{
    XSAPI_ASSERT(!lobbySessionTemplateName.empty());

    if (asyncQueue)
    {
        XTaskQueueDuplicateHandle(asyncQueue, &m_queue);
    }

    std::lock_guard<std::mutex> guard(m_lock);
    if(m_multiplayerClientManager != nullptr)
    {
        m_multiplayerClientManager->Shutdown();

        m_multiplayerLobbySession = nullptr;
        m_multiplayerGameSession = nullptr;
        m_multiplayerClientManager = nullptr;
    }

    m_multiplayerClientManager = MakeShared<MultiplayerClientManager>(lobbySessionTemplateName, m_queue);

    m_multiplayerClientManager->RegisterLocalUserManagerEvents();
    m_multiplayerLobbySession = MakeShared<MultiplayerLobbySession>(m_multiplayerClientManager);
}

bool MultiplayerManager::IsDirty()
{
    return m_isDirty;
}

const MultiplayerEventQueue& MultiplayerManager::DoWork()
{
    std::lock_guard<std::mutex> guard(m_lock);
    m_eventQueue.Clear();

    if (!IsInitialized())
    {
        // The title hasn't called initialize() on the manager yet.
        m_isDirty = false;
        return m_eventQueue;
    }
    else if (!m_multiplayerClientManager->IsUpdateAvailable())
    {
        m_isDirty = false;

        // To handle the scenario of returning InvitedXuid info in the join_lobby_completed event
        if (m_multiplayerClientManager->EventQueue().Size() > 0)
        {
            m_eventQueue = m_multiplayerClientManager->EventQueue();
            m_multiplayerClientManager->ClearEventQueue();
        }

        return m_eventQueue;
    }

    try
    {
        m_isDirty = true;
        m_eventQueue = m_multiplayerClientManager->DoWork();

        std::shared_ptr<MultiplayerClientPendingReader> clientRequest = m_multiplayerClientManager->LastPendingRead();
        if (clientRequest != nullptr)
        {
            m_joinability = clientRequest->LobbyClient()->Joinability();
            SetMultiplayerGameSession(clientRequest->GameClient()->Game());
            SetMultiplayerLobbySession(clientRequest->LobbyClient()->Lobby());
        }
        else
        {
            m_joinability = XblMultiplayerJoinability::None;
            SetMultiplayerGameSession(nullptr);
            SetMultiplayerLobbySession(nullptr);
        }
    }
    catch(...){}

    return m_eventQueue;
}

std::shared_ptr<MultiplayerGameSession>
MultiplayerManager::GameSession() const
{
    std::lock_guard<std::mutex> guard(m_lock);
    return m_multiplayerGameSession;
}

std::shared_ptr<MultiplayerLobbySession>
MultiplayerManager::LobbySession() const
{
    std::lock_guard<std::mutex> guard(m_lock);
    return m_multiplayerLobbySession;
}

void
MultiplayerManager::SetMultiplayerGameSession(
    _In_ std::shared_ptr<MultiplayerGameSession> gameSession
    ) 
{
    // Note: This function does not require a lock as the functions that call this already has a m_lock.

    if (gameSession == nullptr)
    {
        m_multiplayerGameSession = nullptr;
    }
    else
    {
        m_multiplayerGameSession = gameSession;
        m_multiplayerGameSession->SetMultiplayerClientManager(m_multiplayerClientManager);
    }
}

void
MultiplayerManager::SetMultiplayerLobbySession(
    _In_ std::shared_ptr<MultiplayerLobbySession> multiplayerLobby
    ) 
{
    // Note: This function does not require a lock as the functions that call this already has a m_lock.

    if (multiplayerLobby == nullptr)
    {
        if (m_multiplayerLobbySession != nullptr)
        {
            m_multiplayerLobbySession.reset();
        }

        // Don't set the lobby to null so that the title can add local users whenever it chooses.
        m_multiplayerLobbySession = MakeShared<MultiplayerLobbySession>(m_multiplayerClientManager);
    }
    else
    {
        m_multiplayerLobbySession = multiplayerLobby;
        m_multiplayerLobbySession->SetMultiplayerClientManager(m_multiplayerClientManager);
    }
}

HRESULT
MultiplayerManager::JoinLobby(
    _In_ const xsapi_internal_string& handleId,
    _In_ xbox_live_user_t user
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager == nullptr, E_UNEXPECTED, "Call multiplayer_manager::initialize() first.");

    xsapi_internal_vector<xbox_live_user_t> users;
    users.push_back(user);
    return m_multiplayerClientManager->JoinLobbyByHandle(handleId, users);
}

#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK 
HRESULT
MultiplayerManager::JoinLobby(
    _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
    _In_ xbox_live_user_t user
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager == nullptr, E_UNEXPECTED, "Call multiplayer_manager::initialize() first.");

    xsapi_internal_vector<xbox_live_user_t> users{ user };
    return m_multiplayerClientManager->JoinLobby(eventArgs, users);
}
#endif

HRESULT
MultiplayerManager::JoinGameFromLobby(
    _In_ const xsapi_internal_string& sessionTemplateName
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager == nullptr, E_UNEXPECTED, "Call multiplayer_manager::initialize() first.");
    return m_multiplayerClientManager->JoinGameFromLobby(sessionTemplateName);
}

HRESULT
MultiplayerManager::JoinGame(
    _In_ const xsapi_internal_string& sessionName,
    _In_ const xsapi_internal_string& sessionTemplateName,
    _In_ const xsapi_internal_vector<uint64_t>& xuids
    )
{
    RETURN_HR_IF(m_multiplayerClientManager == nullptr, E_UNEXPECTED);
    RETURN_EXCEPTION_FREE_HRESULT(m_multiplayerClientManager->JoinGame(sessionName, sessionTemplateName, xuids));
}

HRESULT
MultiplayerManager::LeaveGame()
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager == nullptr, E_UNEXPECTED, "Call multiplayer_manager::initialize() first.");
    return m_multiplayerClientManager->LeaveGame();
}

HRESULT
MultiplayerManager::FindMatch(
    _In_ const xsapi_internal_string& hopperName,
    _In_ JsonValue& attributes,
    _In_ const std::chrono::seconds& timeout
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager == nullptr, E_UNEXPECTED, "Call multiplayer_manager::initialize() first.");
    return m_multiplayerClientManager->FindMatch(hopperName, attributes, timeout);
}

void
MultiplayerManager::CancelMatch()
{
    if (m_multiplayerClientManager != nullptr && m_multiplayerClientManager->MatchClient() != nullptr)
    {
        m_multiplayerClientManager->MatchClient()->CancelMatch();
    }
}

XblMultiplayerMatchStatus
MultiplayerManager::MatchStatus() const
{
    if (m_multiplayerClientManager != nullptr && m_multiplayerClientManager->MatchClient() != nullptr)
    {
        return m_multiplayerClientManager->MatchClient()->MatchStatus();
    }

    return XblMultiplayerMatchStatus::None;
}

std::chrono::seconds
MultiplayerManager::EstimatedMatchWaitTime() const
{
    if (m_multiplayerClientManager != nullptr && m_multiplayerClientManager->MatchClient() != nullptr)
    {
        return m_multiplayerClientManager->MatchClient()->EstimatedMatchWaitTime();
    }

    return std::chrono::seconds(0);
}

void
MultiplayerManager::SetQosMeasurements(
    _In_ const JsonValue& measurements
    )
{
    if (m_multiplayerClientManager != nullptr && m_multiplayerClientManager->MatchClient() != nullptr)
    {
        m_multiplayerClientManager->MatchClient()->SetQosMeasurements(measurements);
    }
}

bool
MultiplayerManager::AutoFillMembersDuringMatchmaking() const
{
    return false;
}

void
MultiplayerManager::SetAutoFillMembersDuringMatchmaking(
    _In_ bool autoFillMembers
    )
{
    if (m_multiplayerClientManager != nullptr)
    {
        m_multiplayerClientManager->SetAutoFillMembersDuringMatchmaking(autoFillMembers);
    }
}

XblMultiplayerJoinability
MultiplayerManager::Joinability() const
{
    return m_joinability;
}

HRESULT
MultiplayerManager::SetJoinability(
    _In_ XblMultiplayerJoinability value,
    _In_opt_ context_t context
    )
{
    RETURN_HR_IF_LOG_DEBUG(m_multiplayerClientManager == nullptr, E_UNEXPECTED, "Call multiplayer_manager::initialize() first.");
    return m_multiplayerClientManager->SetJoinability(value, context);
}

std::shared_ptr<MultiplayerGameClient>
MultiplayerManager::GameClient()
{
    auto clientManger = m_multiplayerClientManager;
    if (clientManger == nullptr || clientManger->LatestPendingRead() == nullptr)
    {
        return nullptr;
    }
    return clientManger->LatestPendingRead()->GameClient();
}

std::shared_ptr<MultiplayerLobbyClient>
MultiplayerManager::LobbyClient()
{
    auto clientManger = m_multiplayerClientManager;
    if (clientManger == nullptr || clientManger->LatestPendingRead() == nullptr)
    {
        return nullptr;
    }
    return clientManger->LatestPendingRead()->LobbyClient();
}

#if XSAPI_UNIT_TESTS
void
MultiplayerManager::Shutdown()
{
    auto clientManger = m_multiplayerClientManager;
    if (clientManger == nullptr || clientManger->LatestPendingRead() == nullptr)
    {
        return;
    }
    return clientManger->Shutdown();
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END

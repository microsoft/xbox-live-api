// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

void MultiplayerClientPendingReader::deep_copy_if_updated(
    _In_ const MultiplayerClientPendingReader& other
    )
{
    std::lock_guard<std::mutex> lock(other.m_clientRequestLock);

    m_multiplayerEventQueue = other.m_multiplayerEventQueue;
    if (other.m_lobbyClient == nullptr)
    {
        m_lobbyClient = nullptr;
    }
    else
    {
        m_lobbyClient->deep_copy_if_updated(*other.m_lobbyClient);
    }

    if (other.m_gameClient == nullptr)
    {
        m_gameClient = nullptr;
    }
    else
    {
        m_gameClient->deep_copy_if_updated(*other.m_gameClient);
    }

    if (other.m_matchClient == nullptr)
    {
        m_matchClient = nullptr;
    }
    else
    {
        m_matchClient->deep_copy_if_updated(*other.m_matchClient);
    }
}

MultiplayerClientPendingReader::MultiplayerClientPendingReader(const TaskQueue& queue) :
    m_queue{ queue.DeriveWorkerQueue() },
    m_autoFillMembers(false)
{
    m_multiplayerLocalUserManager = MakeShared<MultiplayerLocalUserManager>();
    m_lobbyClient = MakeShared<MultiplayerLobbyClient>(m_queue);
    m_gameClient = MakeShared<MultiplayerGameClient>(m_queue);
    m_matchClient = MakeShared<manager::MultiplayerMatchClient>(m_queue, m_multiplayerLocalUserManager);
}

MultiplayerClientPendingReader::~MultiplayerClientPendingReader()
{
    m_lobbyClient.reset();
    m_gameClient.reset();
    m_matchClient.reset();
    m_multiplayerLocalUserManager.reset();
}

MultiplayerClientPendingReader::MultiplayerClientPendingReader(
    _In_ const TaskQueue& queue,
    _In_ const xsapi_internal_string& lobbySessionTemplateName,
    _In_ std::shared_ptr<MultiplayerLocalUserManager> localUserManager
    ) :
    m_queue{ queue.DeriveWorkerQueue() },
    m_autoFillMembers(false),
    m_multiplayerLocalUserManager(localUserManager)
{
    m_lobbyClient = MakeShared<MultiplayerLobbyClient>(m_queue, lobbySessionTemplateName, m_multiplayerLocalUserManager);
    m_gameClient = MakeShared<MultiplayerGameClient>(m_queue, m_multiplayerLocalUserManager);
    m_matchClient = MakeShared<manager::MultiplayerMatchClient>(m_queue, m_multiplayerLocalUserManager);
    m_lobbyClient->Initialize();
    m_gameClient->Initialize();
}

bool
MultiplayerClientPendingReader::IsUpdateAvailable(
    _In_ const MultiplayerClientPendingReader& other
    )
{
    std::lock_guard<std::mutex> lock(other.m_clientRequestLock);

    if (other.m_lobbyClient->IsPendingLobbyChanges() ||
        other.m_gameClient->IsPendingGameChanges())
    {
        return true;
    }

    auto lobbySession = m_lobbyClient->Session();
    auto otherLobbySession = other.m_lobbyClient->Session();
    auto gameSession = m_gameClient->Session();
    auto otherGameSession = other.m_gameClient->Session();
    if ( !MultiplayerManagerUtils::CompareSessions(lobbySession, otherLobbySession) ||
         !MultiplayerManagerUtils::CompareSessions(gameSession, otherGameSession) ||
         !MultiplayerManagerUtils::CompareSessions(m_matchClient->Session(), other.m_matchClient->Session()) ||
         m_lobbyClient->EventQueue().Size() != other.m_lobbyClient->EventQueue().Size() ||
         m_gameClient->EventQueue().Size() != other.m_gameClient->EventQueue().Size() ||
         m_matchClient->EventQueue().Size() != other.m_matchClient->EventQueue().Size() ||
         m_multiplayerEventQueue.Size() != other.m_multiplayerEventQueue.Size())
    {
        return true;
    }

    return false;
}

std::shared_ptr<MultiplayerLobbyClient>
MultiplayerClientPendingReader::LobbyClient()
{
    return m_lobbyClient;
}

std::shared_ptr<MultiplayerGameClient>
MultiplayerClientPendingReader::GameClient()
{
    return m_gameClient;
}

std::shared_ptr<MultiplayerMatchClient>
MultiplayerClientPendingReader::MatchClient()
{
    return m_matchClient;
}

void 
MultiplayerClientPendingReader::UpdateSession(
    _In_ XblMultiplayerSessionReference sessionRef,
    _In_ std::shared_ptr<XblMultiplayerSession> session
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    if (IsLobby(sessionRef))
    {
        m_lobbyClient->UpdateSession(session);
    }
    else if(IsGame(sessionRef))
    {
        m_gameClient->UpdateSession(session);
    }
}

std::shared_ptr<XblMultiplayerSession>
MultiplayerClientPendingReader::GetSession(
    _In_ XblMultiplayerSessionReference sessionRef
    )
{
    if (IsLobby(sessionRef))
    {
        return m_lobbyClient->Session();
    }
    else if (IsGame(sessionRef))
    {
        return m_gameClient->Session();
    }

    return nullptr;
}

bool
MultiplayerClientPendingReader::IsLobby(
    _In_ XblMultiplayerSessionReference sessionRef
    )
{
    if (!XblMultiplayerSessionReferenceIsValid(&sessionRef) || m_lobbyClient == nullptr || m_lobbyClient->Session() == nullptr)
    {
        return false;
    }

    return sessionRef == m_lobbyClient->Session()->SessionReference();
}

bool
MultiplayerClientPendingReader::IsGame(
    _In_ XblMultiplayerSessionReference sessionRef
    )
{
    if (!XblMultiplayerSessionReferenceIsValid(&sessionRef) || m_gameClient == nullptr || m_gameClient->Session() == nullptr)
    {
        return false;
    }

    return sessionRef == m_gameClient->Session()->SessionReference();
}

bool
MultiplayerClientPendingReader::IsMatch(
    _In_ XblMultiplayerSessionReference sessionRef
)
{
    if (!XblMultiplayerSessionReferenceIsValid(&sessionRef) || m_matchClient == nullptr || m_matchClient->Session() == nullptr)
    {
        return false;
    }

    return sessionRef == m_matchClient->Session()->SessionReference();
}

const MultiplayerEventQueue&
MultiplayerClientPendingReader::EventQueue() const
{
    return m_multiplayerEventQueue;
}

void
MultiplayerClientPendingReader::ClearEventQueue()
{
    m_multiplayerEventQueue.Clear();
}

void MultiplayerClientPendingReader::AddEvent(
    _In_ XblMultiplayerEventType eventType,
    _In_ std::shared_ptr<XblMultiplayerEventArgs> eventArgs,
    _In_ XblMultiplayerSessionType sessionType,
    _In_ Result<void> error,
    _In_opt_ context_t context
)
{
    // TODO: add logging for error message
    m_multiplayerEventQueue.AddEvent(eventType, sessionType, eventArgs, error, context);
}

void
MultiplayerClientPendingReader::AddEvent(
    _In_ const XblMultiplayerEvent& multiplayerEvent
    )
{
    m_multiplayerEventQueue.AddEvent(multiplayerEvent);
}

void
MultiplayerClientPendingReader::AddEvents(
    _In_ const MultiplayerEventQueue& multiplayerEventQueue
    )
{
    if (multiplayerEventQueue.Size() > 0)
    {
        for (const auto& multiplayerEvent : multiplayerEventQueue)
        {
            m_multiplayerEventQueue.AddEvent(multiplayerEvent);
        }
    }
}

void
MultiplayerClientPendingReader::DoWork()
{
    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();

    if (primaryContext == nullptr && 
        m_lobbyClient->EventQueue().Size() == 0 && 
        m_gameClient->EventQueue().Size() == 0 &&
        m_matchClient->EventQueue().Size() == 0 &&
        m_multiplayerEventQueue.Size() == 0)
    {
        // After the primaryContext has been deleted, the lobbyClient could have userRemoved event in the queue.
        // The pending reader will also fire a client_disconnected_from_multiplayer_service event.
        return;
    }

    auto lobbySession = m_lobbyClient->Session();
    auto gameSession = m_gameClient->Session();
    m_lobbyClient->UpdateObjects(lobbySession, gameSession);
    m_gameClient->UpdateObjects(gameSession, lobbySession);

    auto lobbyClientEventQueue = m_lobbyClient->DoWork();
    AddEvents(lobbyClientEventQueue);

    auto gameClientEventQueue = m_gameClient->DoWork();
    AddEvents(gameClientEventQueue);

    ProcessMatchEvents();
}

HRESULT
MultiplayerClientPendingReader::SetJoinability(
    _In_ XblMultiplayerJoinability value,
    _In_opt_ context_t context
    )
{
    return m_lobbyClient->SetJoinability(value, context);
}

HRESULT
MultiplayerClientPendingReader::SetProperties(
    _In_ const XblMultiplayerSessionReference& sessionRef,
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
    pendingRequest->SetSessionProperties(name, valueJson, context);
    AddToPendingQueue(sessionRef, pendingRequest);
    return S_OK;
}

HRESULT
MultiplayerClientPendingReader::SetSynchronizedHost(
    _In_ const XblMultiplayerSessionReference& sessionRef,
    _In_ const xsapi_internal_string& hostDeviceToken,
    _In_opt_ context_t context
    )
{
    auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
    pendingRequest->SetSynchronizedHostDeviceToken(hostDeviceToken, context);
    AddToPendingQueue(sessionRef, pendingRequest);
    return S_OK;
}

HRESULT
MultiplayerClientPendingReader::SetSynchronizedProperties(
    _In_ const XblMultiplayerSessionReference& sessionRef,
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
    pendingRequest->SetSynchronizedSessionProperties(name, valueJson, context);
    AddToPendingQueue(sessionRef, pendingRequest);
    return S_OK;
}

void
MultiplayerClientPendingReader::AddToPendingQueue(
    _In_ const XblMultiplayerSessionReference& sessionRef,
    _In_ std::shared_ptr<MultiplayerClientPendingRequest> pendingRequest
    )
{
    if (sessionRef.SessionName[0] == 0 || IsLobby(sessionRef))
    {
        m_lobbyClient->AddToPendingQueue(pendingRequest);
    }
    else if (IsGame(sessionRef))
    {
        m_gameClient->AddToPendingQueue(pendingRequest);
    }
}

bool
MultiplayerClientPendingReader::IsLocal(
    _In_ uint64_t xuid,
    _In_ const xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>>& xboxLiveContextMap
    )
{
    for(auto& xboxLiveContext : xboxLiveContextMap)
    {
        std::shared_ptr<MultiplayerLocalUser> localUser =  xboxLiveContext.second;
        if (localUser != nullptr && xuid == localUser->Xuid())
        {
            return true;
        }
    }

    return false;
}

std::shared_ptr<MultiplayerMember>
MultiplayerClientPendingReader::ConvertToGameMember(
    _In_ const XblMultiplayerSessionMember* member
    )
{
    return MultiplayerMember::CreateFromSessionMember(
        member,
        m_lobbyClient->Session(),
        m_gameClient->Session(),
        m_multiplayerLocalUserManager->GetLocalUserMap()
        );
}

void
MultiplayerClientPendingReader::ProcessMatchEvents()
{
    if (m_matchClient == nullptr || m_matchClient->MatchStatus() == XblMultiplayerMatchStatus::None)
    {
        return;
    }

    auto matchEventQueue = m_matchClient->DoWork();
    auto matchSession = m_matchClient->Session();

    if (matchEventQueue.Size() > 0)
    {
        for (const auto& multiplayerEvent : matchEventQueue)
        {
            AddEvent(multiplayerEvent);

            if (multiplayerEvent.EventType == XblMultiplayerEventType::FindMatchCompleted)
            {
                auto matchStatus = m_matchClient->MatchStatus();
                if (FAILED(multiplayerEvent.Result))
                {
                    if (!XblMultiplayerSession::DoSessionsMatch(m_gameClient->Session(), matchSession) &&
                        !XblMultiplayerSession::DoSessionsMatch(m_lobbyClient->Session(), matchSession))
                    {
                        // TODO should be able to control the async queue here
                        m_lobbyClient->SessionWriter()->LeaveRemoteSession(matchSession, nullptr);
                    }
                }

                if (matchStatus == XblMultiplayerMatchStatus::Resubmitting)
                {
                    auto lobbySessionCopy = MakeShared<XblMultiplayerSession>(*m_lobbyClient->Session());
                    m_matchClient->ResubmitMatchmaking(lobbySessionCopy);
                    break;
                }

                if (matchStatus == XblMultiplayerMatchStatus::Completed)
                {
                    m_gameClient->UpdateGameSession(matchSession);
                    m_gameClient->UpdateObjects(matchSession, m_lobbyClient->Session());
                    m_lobbyClient->AdvertiseGameSession();
                }

                XblMultiplayerSessionReadLockGuard matchSessionSafe(matchSession);
                if (m_autoFillMembers && matchSession != nullptr &&
                    matchSessionSafe.Members().size() < matchSessionSafe.SessionConstants().MaxMembersInSession &&
                    (matchStatus == XblMultiplayerMatchStatus::Completed || matchStatus == XblMultiplayerMatchStatus::Expired))
                {
                    // Continue looking for more players
                    m_matchClient->SetMatchStatus(XblMultiplayerMatchStatus::None);
                    m_matchClient->FindMatch(matchSession, true);
                    break;
                }

                m_matchClient->SetMatchStatus(XblMultiplayerMatchStatus::None);
                m_matchClient->UpdateSession(nullptr);
                break;
            }
        }
    }
}

HRESULT
MultiplayerClientPendingReader::FindMatch(
    _In_ const xsapi_internal_string& hopperName,
    _In_ JsonValue& attributes,
    _In_ const std::chrono::seconds& timeout
    )
{
    RETURN_HR_IF_LOG_DEBUG(!m_autoFillMembers && m_gameClient->Session() != nullptr, E_UNEXPECTED, "A game already exists. Call leave_game() before you can start matchmaking.");
    RETURN_HR_IF_LOG_DEBUG(!m_autoFillMembers && !m_lobbyClient->GetTransferHandle().empty(), E_UNEXPECTED, "A game already exists for your Lobby. Call leave_game() for all Lobby members before you can start matchmaking.");

    if (m_autoFillMembers && m_gameClient->Session() != nullptr)
    {
        return m_matchClient->FindMatch(hopperName, attributes, timeout, m_gameClient->Session(), true);
    }

    return m_matchClient->FindMatch(hopperName, attributes, timeout, m_lobbyClient->Session(), false);
}

void
MultiplayerClientPendingReader::SetAutoFillMembersDuringMatchmaking(
    _In_ bool autoFillMembers
    )
{
    m_autoFillMembers = autoFillMembers;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
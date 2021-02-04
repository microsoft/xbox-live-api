// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"
#include "xbox_live_app_config_internal.h"

using namespace xbox::services::multiplayer;
using namespace xbox::services::system;
using namespace xbox::services::legacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerGameClient::MultiplayerGameClient(
    const TaskQueue& queue
) noexcept :
    m_queue{ queue.DeriveWorkerQueue() },
    m_sessionWriter{ MakeShared<MultiplayerSessionWriter>(queue) }
{
}

MultiplayerGameClient::MultiplayerGameClient(
    const TaskQueue& queue,
    _In_ std::shared_ptr<MultiplayerLocalUserManager> localUserManager
) noexcept :
    m_queue{ queue.DeriveWorkerQueue() },
    m_sessionWriter{ MakeShared<MultiplayerSessionWriter>(queue, localUserManager) },
    m_multiplayerLocalUserManager{ std::move(localUserManager) }
{
}

MultiplayerGameClient::~MultiplayerGameClient()
{
    m_sessionWriter.reset();
}

void
MultiplayerGameClient::Initialize()
{
    std::weak_ptr<MultiplayerGameClient> weakSessionWriter = shared_from_this();
    m_sessionWriter->AddMultiplayerSessionUpdatedHandler([weakSessionWriter](_In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession)
    {
        std::shared_ptr<MultiplayerGameClient> pThis(weakSessionWriter.lock());
        if (pThis != nullptr)
        {
            pThis->UpdateSession(updatedSession);
        }
    });
}

std::shared_ptr<MultiplayerLobbyClient>
MultiplayerGameClient::LobbyClient() const
{
    // TODO this should be changed such that nothing here needs to depend on the GlobalState.
    // For now just try to get the global state and return null (which is handled by callers) if
    // it that fails.
    auto state{ GlobalState::Get() };
    return state ? state->MultiplayerManager()->LobbyClient() : nullptr;
}

std::shared_ptr<XblMultiplayerSession>
MultiplayerGameClient::LobbySession() const
{
    auto lobbyClient{ LobbyClient() };
    return lobbyClient ? lobbyClient->Session() : nullptr;
}

void
MultiplayerGameClient::SetGameSessionTemplate(
    _In_ const xsapi_internal_string& sessionTemplateName
    )
{
    m_gameSessionTemplateName = sessionTemplateName;
}

void
MultiplayerGameClient::deep_copy_if_updated(
    _In_ const MultiplayerGameClient& other
    )
{
    std::lock_guard<std::mutex> lock(other.m_clientRequestLock);

    if (other.m_sessionWriter->Session() == nullptr)
    {
        m_sessionWriter->UpdateSession(nullptr);
        m_multiplayerGame = nullptr;
    }
    else if (other.m_multiplayerGame == nullptr)
    {
        m_multiplayerGame = nullptr;
    }
    else if (m_sessionWriter->Session() == nullptr ||
            other.m_sessionWriter->Session()->SessionInfo().ChangeNumber > m_sessionWriter->Session()->SessionInfo().ChangeNumber ||
            other.m_sessionWriter->Session()->ETag() > m_sessionWriter->Session()->ETag())
    {
        m_sessionWriter->UpdateSession(MakeShared<XblMultiplayerSession>(*other.m_sessionWriter->Session()));
        m_multiplayerGame = MakeShared<MultiplayerGameSession>(*other.m_multiplayerGame);
    }
    else if (m_updateNumber != other.m_updateNumber)
    {
        m_multiplayerGame = MakeShared<MultiplayerGameSession>(*other.m_multiplayerGame);
    }
}

std::shared_ptr<MultiplayerSessionWriter>
MultiplayerGameClient::SessionWriter() const
{
    return m_sessionWriter;
}

std::shared_ptr<MultiplayerGameSession>
MultiplayerGameClient::Game() const
{
    return m_multiplayerGame;
}

void
MultiplayerGameClient::UpdateGame(
    _In_ const std::shared_ptr<MultiplayerGameSession>& multiplayerGame
    )
{
    ++m_updateNumber;
    m_multiplayerGame = multiplayerGame;
}

std::shared_ptr<XblMultiplayerSession>
MultiplayerGameClient::Session() const
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    return m_sessionWriter->Session();
}

void 
MultiplayerGameClient::UpdateSession(
    _In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession
    )
{
    auto cachedSession = Session();
    if (updatedSession == nullptr || cachedSession == nullptr ||
        updatedSession->SessionInfo().ChangeNumber > cachedSession->SessionInfo().ChangeNumber ||
        updatedSession->ETag() != cachedSession->ETag())
    {
        UpdateGameSession(updatedSession);
    }
}

void 
MultiplayerGameClient::UpdateGameSession(
    _In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_sessionWriter->UpdateSession(updatedSession);
}

void
MultiplayerGameClient::UpdateObjects(
    const std::shared_ptr<XblMultiplayerSession>& updatedSession,
    const std::shared_ptr<XblMultiplayerSession>& lobbySession
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    if (updatedSession == nullptr)
    {
        UpdateGame(nullptr);
    }
    else
    {
        auto multiplayerGame = ConvertToMultiplayerGame(updatedSession, lobbySession);
        UpdateGame(multiplayerGame);
    }
}

std::shared_ptr<MultiplayerGameSession>
MultiplayerGameClient::ConvertToMultiplayerGame(
    _In_ const std::shared_ptr<XblMultiplayerSession>& sessionToConvert,
    _In_ const std::shared_ptr<XblMultiplayerSession>& lobbySession
    )
{
    if (sessionToConvert == nullptr)
    {
        return nullptr;
    }

    std::shared_ptr<MultiplayerMember> hostMember = nullptr;
    xsapi_internal_vector<std::shared_ptr<MultiplayerMember>> gameMembers;
    XblMultiplayerSessionReadLockGuard sessionToConvertSafe(sessionToConvert);
    for (const auto& member : sessionToConvertSafe.Members())
    {
        auto gameMember = MultiplayerMember::CreateFromSessionMember(
            &member,
            lobbySession,
            sessionToConvert,
            m_multiplayerLocalUserManager->GetLocalUserMap()
            );
        if (member.DeviceToken.Value[0] != 0 && utils::str_icmp(member.DeviceToken.Value, sessionToConvertSafe.SessionProperties().HostDeviceToken.Value) == 0)
        {
            hostMember = gameMember;
        }

        gameMembers.push_back(gameMember);
    }

    return MakeShared<MultiplayerGameSession>(
        sessionToConvert,
        hostMember,
        gameMembers
        );
}

const MultiplayerEventQueue&
MultiplayerGameClient::EventQueue()
{
    // Don't require a lock as STL is multithread read safe
    return m_multiplayerEventQueue;
}

void
MultiplayerGameClient::ClearPendingQueue()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    while (!m_pendingRequestQueue.empty())
    {
        m_pendingRequestQueue.pop();
    }
}

void
MultiplayerGameClient::AddToPendingQueue(
    _In_ std::shared_ptr<MultiplayerClientPendingRequest> pendingRequest
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_pendingRequestQueue.push(pendingRequest);
}

void
MultiplayerGameClient::AddToProcessingQueue(
    _In_ xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue
    )
{
    for (const auto& request : processingQueue)
    {
        m_processingQueue.push_back(request);
    }
}

void
MultiplayerGameClient::RemoveFromProcessingQueue(
    _In_ uint32_t identifier
)
{
    for (auto request = m_processingQueue.begin(); request != m_processingQueue.end(); ++request)
    {
        if ((*request)->Identifier() == identifier)
        {
            m_processingQueue.erase(request);
            break;
        }
    }
}

xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>>
MultiplayerGameClient::GetProcessingQueue()
{
    return m_processingQueue;
}

MultiplayerEventQueue
MultiplayerGameClient::DoWork()
{
    bool expected = false;
    if (m_pendingCommitInProgress.compare_exchange_strong(expected, true))
    {
        if (m_pendingRequestQueue.size() > 0)
        {
            xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue;
            bool applySynchronizedChanges = false;
            bool doneProcessing = false;
            do
            {
                std::lock_guard<std::mutex> lock(m_clientRequestLock);
                {
                    auto pendingRequest = m_pendingRequestQueue.front();
                    processingQueue.push_back(pendingRequest);
                    m_pendingRequestQueue.pop();

                    if (m_pendingRequestQueue.size() > 0)
                    {
                        if (pendingRequest->RequestType() != m_pendingRequestQueue.front()->RequestType())
                        {
                            doneProcessing = true;
                        }
                    }

                    if (!applySynchronizedChanges && pendingRequest->RequestType() == PendingRequestType::SynchronizedChanges)
                    {
                        applySynchronizedChanges = true;
                    }
                }

            } while (!doneProcessing && m_pendingRequestQueue.size() > 0);

            if (processingQueue.size() > 0)
            {
                AddToProcessingQueue(processingQueue);
                std::weak_ptr<MultiplayerGameClient> weakSessionWriter = shared_from_this();

                Callback<Result<MultiplayerEventQueue>> callback = 
                    [weakSessionWriter, processingQueue](Result<MultiplayerEventQueue> result)
                {
                    std::shared_ptr<MultiplayerGameClient> pThis(weakSessionWriter.lock());
                    if (pThis != nullptr)
                    {
                        std::lock_guard<std::mutex> lock(pThis->m_clientRequestLock);
                        {
                            auto eventQueue = result.Payload();
                            for (const auto& ev : eventQueue)
                            {
                                pThis->m_multiplayerEventQueue.AddEvent(ev);
                            }
                        }

                        for (const auto& processingRequest : processingQueue)
                        {
                            pThis->RemoveFromProcessingQueue(processingRequest->Identifier());
                        }

                        pThis->m_pendingCommitInProgress.store(false);
                    }
                };

                // TODO we should have a way to configure the queue here
                if (applySynchronizedChanges)
                {
                    m_sessionWriter->CommitPendingSynchronizedChanges(processingQueue, XblMultiplayerSessionType::GameSession, callback);
                }
                else
                {
                    m_sessionWriter->CommitPendingChanges(processingQueue, XblMultiplayerSessionType::GameSession, false, callback);
                }
            }
            else
            {
                m_pendingCommitInProgress.store(false);
            }
        }
        else
        {
            m_pendingCommitInProgress.store(false);
        }
    }

    MultiplayerEventQueue eventQueue;
    {
        std::lock_guard<std::mutex> lock(m_clientRequestLock);
        eventQueue = m_multiplayerEventQueue;
        m_multiplayerEventQueue.Clear();
    }

    return eventQueue;
}

bool
MultiplayerGameClient::IsPendingGameChanges()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    if (m_pendingRequestQueue.size() > 0)
    {
        return true;
    }

    if (m_sessionWriter != nullptr)
    {
        auto latestSession = m_sessionWriter->Session();
        if (latestSession != nullptr &&
            m_multiplayerGame != nullptr &&
            latestSession->SessionInfo().ChangeNumber != m_multiplayerGame->ChangeNumber())
        {
            return true;
        }
    }

    return false;
}

bool
MultiplayerGameClient::IsRequestInProgress()
{
    return m_pendingRequestQueue.size() > 0 || m_processingQueue.size() > 0 || m_multiplayerEventQueue.Size() > 0;
}

void MultiplayerGameClient::SetLocalMemberPropertiesToRemoteSession(
    _In_ const std::shared_ptr<xbox::services::multiplayer::manager::MultiplayerLocalUser>& localUser,
    _In_ const Map<String, JsonDocument>& propertiesToWrite,
    _In_ const String& localUserSecureDeviceAddress
) noexcept
{
    if (Session() == nullptr || localUser == nullptr)
    {
        return;
    }

    // This operation will wait until any pending commits complete, set the provided custom properties for the local user,
    // and then write the game session.
    struct SetPropertiesOperation : public std::enable_shared_from_this<SetPropertiesOperation>
    {
        SetPropertiesOperation(
            std::shared_ptr<MultiplayerGameClient> gameClient,
            std::shared_ptr<MultiplayerLocalUser> localUser,
            const Map<String, JsonDocument>& propertiesToWrite,
            String localUserSecureDeviceAddressToWrite
        ) noexcept
            : m_gameClient{ std::move(gameClient) },
            m_localUser{ std::move(localUser) },
            m_secureDeviceAddressToWrite{std::move(localUserSecureDeviceAddressToWrite)}
        {
            for (const auto& prop : propertiesToWrite)
            {
                m_propertiesToWrite[prop.first] = JsonDocument{};
                JsonUtils::CopyFrom(m_propertiesToWrite[prop.first], prop.second);
            }
        }

        void Run() noexcept
        {
            bool commitInProgress = false;
            if (!m_gameClient->m_pendingCommitInProgress.compare_exchange_weak(commitInProgress, true))
            {
                // Already a commit in progress. Reschedule operation.
                // Seems like we could add a little sleep here, but keeping behavior consistent since 1806 implementation
                m_gameClient->m_queue.RunWork([op{ shared_from_this() }]
                    {
                        op->Run();
                    });
            }
            else if (auto gameSession{ m_gameClient->Session() })
            {
                assert(m_gameClient->m_pendingCommitInProgress);

                // Set the properties and write the session if it still exists
                auto sessionToCommit = MakeShared<XblMultiplayerSession>(m_localUser->Xuid(), &gameSession->SessionReference(), nullptr);
                sessionToCommit->Join(nullptr, false, true);

                // Ok to use "unsafe" method here because we just created and have the only instance of this session
                auto sessionUser{ sessionToCommit->CurrentUserInternalUnsafe() };
                assert(sessionUser);

                for (const auto& prop : m_propertiesToWrite)
                {
                    sessionUser->SetCustomPropertyJson(prop.first, prop.second);
                }

                if (!m_secureDeviceAddressToWrite.empty())
                {
                    sessionUser->SetSecureDeviceBaseAddress64(m_secureDeviceAddressToWrite);
                }

                m_gameClient->m_sessionWriter->WriteSession(
                    m_localUser->Context(),
                    sessionToCommit,
                    XblMultiplayerSessionWriteMode::UpdateExisting,
                    true,
                    [gameClient{ m_gameClient }](Result<std::shared_ptr<XblMultiplayerSession>>)
                {
                    gameClient->m_pendingCommitInProgress = false;
                });
            }
        }

    private:
        std::shared_ptr<MultiplayerGameClient> m_gameClient;
        std::shared_ptr<MultiplayerLocalUser> m_localUser;
        Map<String, JsonDocument> m_propertiesToWrite;
        String m_secureDeviceAddressToWrite;
    };

    auto operation = MakeShared<SetPropertiesOperation>(shared_from_this(), localUser, propertiesToWrite, localUserSecureDeviceAddress);
    operation->Run();
}

void
MultiplayerGameClient::RemoveStaleUsersFromRemoteSession()
{
    auto gameSession = Session();
    if (gameSession == nullptr)
    {
        return;
    }

    auto xboxLiveContextMap = m_multiplayerLocalUserManager->GetLocalUserMap();
    for (auto xboxLiveContext : xboxLiveContextMap)
    {
        auto localUser = xboxLiveContext.second;
        if (localUser != nullptr && localUser->LobbyState() == MultiplayerLocalUserLobbyState::Remove)
        {
            // Leave the game session if it exists.
            if (gameSession != nullptr)
            {
                std::weak_ptr<MultiplayerGameClient> weakSessionWriter = shared_from_this();

                auto sessionToCommit = MakeShared<XblMultiplayerSession>(localUser->Xuid(), &gameSession->SessionReference(), nullptr);
                sessionToCommit->Leave();
                m_sessionWriter->WriteSession(localUser->Context(), sessionToCommit, XblMultiplayerSessionWriteMode::UpdateExisting, true,
                [weakSessionWriter](Result<std::shared_ptr<XblMultiplayerSession>> result)
                {
                    std::shared_ptr<MultiplayerGameClient> pThis(weakSessionWriter.lock());
                    if (pThis)
                    {
                        auto lobbyClient{ pThis->LobbyClient() };
                        if (lobbyClient)
                        {
                            lobbyClient->StopAdvertisingGameSession(result);
                        }
                    }
                });
            }
        }
    }
}

HRESULT MultiplayerGameClient::JoinHelper(
    _In_ std::shared_ptr<MultiplayerLocalUser> localUser,
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ bool writeMemberPropertiesFromLobby,
    _In_ const String& handleId,
    _In_ MultiplayerSessionCallback callback
) const noexcept
{
    RETURN_HR_IF_LOG_DEBUG(localUser == nullptr || localUser->Context() == nullptr, E_UNEXPECTED, "Call add_local_user() first.");

    session->Join(nullptr, false, true);
    XblMultiplayerSessionReadLockGuard sessionSafe(session);
    if (sessionSafe.CurrentUser() != nullptr)
    {
        sessionSafe.CurrentUserInternal()->SetSecureDeviceBaseAddress64(localUser->ConnectionAddress());
    }
    session->SetSessionChangeSubscription(XblMultiplayerSessionChangeTypes::Everything);

    if (writeMemberPropertiesFromLobby)
    {
        auto localMember = XblMultiplayerSession::GetPlayerInSession(localUser->Xuid(), LobbySession());
        if (localMember != nullptr && strlen(localMember->CustomPropertiesJson) > 0)
        {
            JsonDocument jsonPropertyObj;
            jsonPropertyObj.Parse(localMember->CustomPropertiesJson);
            for (const auto& prop : jsonPropertyObj.GetObject())
            {
                session->SetCurrentUserMemberCustomPropertyJson(prop.name.GetString(), prop.value);
            }
        }
    }
    if (handleId.empty())
    {
        m_sessionWriter->WriteSession(
            localUser->Context(),
            session,
            XblMultiplayerSessionWriteMode::UpdateOrCreateNew,
            true,
            std::move(callback)
        );
    }
    else
    {
        m_sessionWriter->WriteSessionByHandle(
            localUser->Context(),
            session,
            XblMultiplayerSessionWriteMode::UpdateOrCreateNew,
            handleId,
            true,
            std::move(callback)
        );
    }
    return S_OK;
}

HRESULT MultiplayerGameClient::JoinGameHelper(
    _In_ const String& sessionName,
    _In_ Callback<Result<void>> callback
) noexcept
{
    XblMultiplayerSessionReference gameSessionRef = XblMultiplayerSessionReferenceCreate(
        AppConfig::Instance()->OverrideScid().data(),
        m_gameSessionTemplateName.data(),
        sessionName.data()
    );

    return JoinGameBySessionReference(gameSessionRef,
        [
            sharedThis{ shared_from_this() },
            callback
        ]
    (Result<std::shared_ptr<XblMultiplayerSession>> joinResult)
    {
        if (SUCCEEDED(joinResult.Hresult()) && joinResult.Payload() != nullptr && sharedThis->LobbyClient() != nullptr)
        {
            // If join_game succeeds, advertise game sessions via the lobby. If the advertising fails, 
            // we simply eat the error as it isn't actionable for the title.
            sharedThis->LobbyClient()->AdvertiseGameSession();
        }
        callback(joinResult.Hresult());
    });
}

HRESULT MultiplayerGameClient::JoinGameBySessionReference(
    _In_ const XblMultiplayerSessionReference& gameSessionRef,
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    RETURN_HR_IF_LOG_DEBUG(primaryContext == nullptr, E_UNEXPECTED, "Call add_local_user() before joining.");

    return JoinGameForAllLocalMembers(gameSessionRef, String{}, false, std::move(callback));
}

HRESULT MultiplayerGameClient::JoinGameByHandle(
    _In_ const String& handleId,
    _In_ bool createGameIfFailedToJoin,
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    RETURN_HR_IF_LOG_DEBUG(primaryContext == nullptr, E_UNEXPECTED, "Call add_local_user() before joining.");

    return JoinGameForAllLocalMembers(XblMultiplayerSessionReference{}, handleId, createGameIfFailedToJoin, std::move(callback));
}

HRESULT MultiplayerGameClient::JoinGameForAllLocalMembers(
    _In_ const XblMultiplayerSessionReference& sessionRefToJoin,
    _In_ const String& handleId,
    _In_ bool createGameIfFailedToJoin,
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    RETURN_HR_INVALIDARGUMENT_IF(!XblMultiplayerSessionReferenceIsValid(&sessionRefToJoin) && handleId.empty());

    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    RETURN_HR_IF_LOG_DEBUG(primaryContext == nullptr, E_UNEXPECTED, "Call add_local_user() before joining.");

    // Leave existing game without updating the latest as the leave may comeback after the actual join and overwrite it.
    auto cachedSession = Session();
    if (cachedSession != nullptr)
    {
        LeaveRemoteSession(cachedSession, false, false);
    }
    UpdateSession(nullptr);

    // Join either an existing or new game session. If attempting to join an existing session fails,
    // optionally creates a new game session from the lobby. When all users have joined the game, a JoinGameCompleted
    // event will be raised.
    struct JoinGameOperation : public std::enable_shared_from_this<JoinGameOperation>
    {
        JoinGameOperation(
            std::shared_ptr<MultiplayerGameClient> gameClient,
            const XblMultiplayerSessionReference& sessionRefToJoin,
            String handleIdToJoin,
            bool createGameIfFailedToJoin,
            MultiplayerSessionCallback&& callback
        ) noexcept :
            m_gameClient{ std::move(gameClient) },
            m_sessionRefToJoin{ sessionRefToJoin },
            m_handleIdToJoin{ std::move(handleIdToJoin) },
            m_createGameIfFailedToJoin{ createGameIfFailedToJoin },
            m_localUsers{ m_gameClient->m_multiplayerLocalUserManager->GetLocalUserMap() },
            m_callback{ std::move(callback) }
        {
        }

        void Run() noexcept
        {
            JoinGameWithNextUser();
        }

    private:
        void JoinGameWithNextUser() noexcept
        {
            assert(!m_localUsers.empty());
            auto localUser{ m_localUsers.begin()->second };
            m_localUsers.erase(m_localUsers.begin());

            auto sessionToCommit = MakeShared<XblMultiplayerSession>(localUser->Xuid(), &m_sessionRefToJoin, nullptr);
            HRESULT hr = m_gameClient->JoinHelper(localUser, sessionToCommit, true, m_handleIdToJoin,
                [
                    this,
                    sharedThis{ shared_from_this() },
                    localUser
                ]
            (Result<std::shared_ptr<XblMultiplayerSession>> joinSessionResult)
            {
                if (Succeeded(joinSessionResult))
                {
                    localUser->SetGameState(MultiplayerLocalUserGameState::InSession);
                }

                if (Failed(joinSessionResult) || m_localUsers.empty())
                {
                    OnJoinCompleted(std::move(joinSessionResult));
                }
                else
                {
                    JoinGameWithNextUser();
                }
            });

            if (FAILED(hr))
            {
                OnJoinCompleted(hr);
            }
        }

        void OnJoinCompleted(Result<std::shared_ptr<XblMultiplayerSession>>&& joinResult) noexcept
        {
            if (auto lobbyClient{ m_gameClient->LobbyClient() })
            {
                if (joinResult.Hresult() == HTTP_E_STATUS_NOT_FOUND && !m_handleIdToJoin.empty())
                {
                    // We tried to join an existing session but it didn't exist. Create a new game session
                    // from the lobby and clear the existing handleId since it must be invalid.
                    if (m_createGameIfFailedToJoin)
                    {
                        m_callback(lobbyClient->CreateGameFromLobby());
                        return;
                    }
                    lobbyClient->ClearGameSessionFromLobby();
                }
                else if (Failed(joinResult) && m_handleIdToJoin.empty())
                {
                    // Tried to create a new game and we failed. Clear the transfer handle pending state (GameSessionTransferHandle=pending~xuid).
                    lobbyClient->ClearGameSessionFromLobby();
                }
            }

            if (Succeeded(joinResult))
            {
                m_gameClient->UpdateSession(joinResult.Payload());
                m_gameClient->m_multiplayerLocalUserManager->ChangeAllLocalUserGameState(MultiplayerLocalUserGameState::InSession);
            }

            {
                std::lock_guard<std::mutex> lock(m_gameClient->m_clientRequestLock);
                m_gameClient->m_multiplayerEventQueue.AddEvent(
                    XblMultiplayerEventType::JoinGameCompleted,
                    XblMultiplayerSessionType::GameSession,
                    MakeShared<XblMultiplayerEventArgs>(),
                    joinResult
                );
            }
            m_callback(joinResult);
        }

        std::shared_ptr<MultiplayerGameClient> m_gameClient;
        XblMultiplayerSessionReference m_sessionRefToJoin;
        String m_handleIdToJoin;
        bool m_createGameIfFailedToJoin;
        Map<uint64_t, std::shared_ptr<MultiplayerLocalUser>> m_localUsers;
        MultiplayerSessionCallback m_callback;
    };

    auto operation = MakeShared<JoinGameOperation>(
        shared_from_this(),
        sessionRefToJoin,
        handleId,
        createGameIfFailedToJoin,
        std::move(callback)
    );

    operation->Run();
    return S_OK;
}

HRESULT MultiplayerGameClient::JoinGameFromLobbyHelper(
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    auto lobbySession = LobbySession();
    auto lobbyClient = LobbyClient();
    RETURN_HR_IF_LOG_DEBUG(primaryContext == nullptr || lobbyClient == nullptr || lobbySession == nullptr, E_UNEXPECTED, "No lobby session exists. Call add_local_user() to create a lobby first.");

    // Check if the lobby has a game session associated with it to join.
    XblMultiplayerSessionReadLockGuard lobbySessionSafe(lobbySession);

    JsonDocument jsonDoc;
    jsonDoc.Parse(lobbySessionSafe.SessionProperties().SessionCustomPropertiesJson);

    xsapi_internal_string transferHandle;
    if (!jsonDoc.HasParseError())
    {
        JsonUtils::ExtractJsonString(jsonDoc, MultiplayerLobbyClient_TransferHandlePropertyName, transferHandle, false);
    }
    if (transferHandle.empty()) // aka the field isn't there
    {
        return lobbyClient->CreateGameFromLobby();
    }

    String handleId;
    if (lobbyClient->IsTransferHandleState("pending"))
    {
        // Wait for the property changed event to join.
        m_multiplayerLocalUserManager->ChangeAllLocalUserGameState(MultiplayerLocalUserGameState::PendingJoin);
        return S_OK;
    }
    else if (lobbyClient->IsTransferHandleState("completed"))
    {
        handleId = lobbyClient->GetTransferHandle();
    }
    return JoinGameByHandle(handleId, true, std::move(callback));
}

void MultiplayerGameClient::LeaveRemoteSession(
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ bool stopAdvertisingGameSession,
    _In_ bool triggerCompletionEvent
) noexcept
{
    auto processingRequest = MakeShared<MultiplayerClientPendingRequest>();
    m_processingQueue.push_back(processingRequest);

    m_sessionWriter->LeaveRemoteSession(session,
        [
            weakThis = std::weak_ptr<MultiplayerGameClient>{ shared_from_this() },
            this,
            stopAdvertisingGameSession,
            triggerCompletionEvent,
            processingRequest
        ]
    (Result<std::shared_ptr<XblMultiplayerSession>> result)
    {
        if (auto sharedThis{ weakThis.lock() })
        {
            auto lobbyClient{ LobbyClient() };
            if (stopAdvertisingGameSession && lobbyClient)
            {
                lobbyClient->StopAdvertisingGameSession(result);
            }

            // Always set your local game session to null upon leaving.
            UpdateSession(nullptr);

            if (triggerCompletionEvent)
            {
                {
                    std::lock_guard<std::mutex> lock(m_clientRequestLock);
                    m_multiplayerEventQueue.AddEvent(
                        XblMultiplayerEventType::LeaveGameCompleted,
                        XblMultiplayerSessionType::GameSession
                    );
                }

                RemoveFromProcessingQueue(processingRequest->Identifier());
            }
        }
    });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
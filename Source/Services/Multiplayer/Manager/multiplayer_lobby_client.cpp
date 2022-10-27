// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"
#include "xbox_live_app_config_internal.h"

using namespace xbox::services::multiplayer;
using namespace xbox::services::system;
using namespace xbox::services::legacy;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

#if XSAPI_UNIT_TESTS
#define RETRY_DELAY_MS 0
#else
#define RETRY_DELAY_MS 1000
#endif
#define MAX_CONNECTION_ATTEMPTS 3

MultiplayerLobbyClient::MultiplayerLobbyClient(
    _In_ const TaskQueue& queue
) noexcept :
    m_queue{ queue.DeriveWorkerQueue() },
    m_sessionWriter{ MakeShared<MultiplayerSessionWriter>(queue) }
{
}

MultiplayerLobbyClient::MultiplayerLobbyClient(
    _In_ const TaskQueue& queue,
    _In_ String lobbySessionTemplateName,
    _In_ std::shared_ptr<MultiplayerLocalUserManager> localUserManager
) noexcept :
    m_queue{ queue.DeriveWorkerQueue() },
    m_lobbySessionTemplateName{ std::move(lobbySessionTemplateName) },
    m_sessionWriter{ MakeShared<MultiplayerSessionWriter>(queue, localUserManager) },
    m_multiplayerLocalUserManager{ std::move(localUserManager) }
{
}

MultiplayerLobbyClient::~MultiplayerLobbyClient() noexcept
{
    m_sessionWriter.reset();
}

void
MultiplayerLobbyClient::Initialize()
{
    std::weak_ptr<MultiplayerLobbyClient> weakThis = shared_from_this();
    m_sessionWriter->AddMultiplayerSessionUpdatedHandler([weakThis](_In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession)
    {
        std::shared_ptr<MultiplayerLobbyClient> pThis(weakThis.lock());
        if (pThis != nullptr)
        {
            pThis->UpdateSession(updatedSession);
        }
    });
}

void MultiplayerLobbyClient::deep_copy_if_updated(
    _In_ const MultiplayerLobbyClient& other
    )
{
    std::lock_guard<std::mutex> lock(other.m_clientRequestLock);

    m_joinability = other.m_joinability;
    if (other.m_sessionWriter->Session() == nullptr)
    {
        m_sessionWriter->UpdateSession(nullptr);
        m_multiplayerLobby = nullptr;
    }
    else if (other.m_multiplayerLobby == nullptr)
    {
        m_multiplayerLobby = nullptr;
    }
    else if (m_sessionWriter->Session() == nullptr ||
            other.m_sessionWriter->Session()->SessionInfo().ChangeNumber > m_sessionWriter->Session()->SessionInfo().ChangeNumber ||
            other.m_sessionWriter->Session()->ETag() > m_sessionWriter->Session()->ETag())
    {
        m_sessionWriter->UpdateSession(MakeShared<XblMultiplayerSession>(*other.m_sessionWriter->Session()));
        m_multiplayerLobby = MakeShared<MultiplayerLobbySession>(*other.m_multiplayerLobby);
    }
    else if (m_updateNumber != other.m_updateNumber)
    {
        m_multiplayerLobby = MakeShared<MultiplayerLobbySession>(*other.m_multiplayerLobby);
    }
}

std::shared_ptr<MultiplayerGameClient>
MultiplayerLobbyClient::GameClient()
{
    // TODO this should be changed such that nothing here needs to depend on the GlobalState.
    // For now just try to get the global state and return null (which is handled by callers) if
    // it that fails.
    auto state{ GlobalState::Get() };
    return state ? state->MultiplayerManager()->GameClient() : nullptr;
}

std::shared_ptr<XblMultiplayerSession>
MultiplayerLobbyClient::GameSession()
{
    auto gameClient{ GameClient() };
    return gameClient ? gameClient->Session() : nullptr;
}

const std::shared_ptr<MultiplayerSessionWriter>&
MultiplayerLobbyClient::SessionWriter() const
{
    return m_sessionWriter;
}

const std::shared_ptr<MultiplayerLobbySession>&
MultiplayerLobbyClient::Lobby() const
{
    return m_multiplayerLobby;
}

void
MultiplayerLobbyClient::UpdateLobby(
    _In_ std::shared_ptr<MultiplayerLobbySession> multiplayerLobby
    )
{
    ++m_updateNumber;
    m_multiplayerLobby = multiplayerLobby;
}

const std::shared_ptr<XblMultiplayerSession>&
MultiplayerLobbyClient::Session() const
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    return m_sessionWriter->Session();
}

void
MultiplayerLobbyClient::UpdateSession(
    _In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession
    )
{
    auto cachedSession = Session();
    if (updatedSession == nullptr || cachedSession == nullptr ||
        updatedSession->SessionInfo().ChangeNumber > cachedSession->SessionInfo().ChangeNumber ||
        updatedSession->ETag() != cachedSession->ETag())
    {
        UpdateLobbySession(updatedSession);
    }
}

void 
MultiplayerLobbyClient::UpdateLobbySession(
    _In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_sessionWriter->UpdateSession(updatedSession);
}

void
MultiplayerLobbyClient::UpdateObjects(
    const std::shared_ptr<XblMultiplayerSession>& updatedSession, 
    const std::shared_ptr<XblMultiplayerSession>& gameSession
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    if (updatedSession == nullptr)
    {
        UpdateLobby(nullptr);
        m_localLobbyMembers.clear();
        m_joinability = XblMultiplayerJoinability::None;
    }
    else
    {
        UpdateLocalLobbyMembers(updatedSession, gameSession);
        auto multiplayerLobby = ConvertToMultiplayerLobby(updatedSession, gameSession);
        UpdateLobby(multiplayerLobby);
        XblMultiplayerSessionReadLockGuard updatedSessionSafe(updatedSession);
        m_joinability = MultiplayerManagerUtils::GetJoinability(updatedSessionSafe.SessionProperties());
    }
}

void
MultiplayerLobbyClient::UpdateLocalLobbyMembers(
    _In_ const std::shared_ptr<XblMultiplayerSession>& updatedLobbySession,
    _In_ const std::shared_ptr<XblMultiplayerSession>& gameSession
    )
{
    m_localLobbyMembers.clear();
    if (updatedLobbySession == nullptr)
    {
        return;
    }

    auto localLobbyGameMembers = xsapi_internal_vector<std::shared_ptr<MultiplayerMember>>();

    auto xboxLiveContextMap = m_multiplayerLocalUserManager->GetLocalUserMap();
    for(auto xboxLiveContext : xboxLiveContextMap)
    {
        auto localUser =  xboxLiveContext.second;
        if (localUser != nullptr)
        {
            auto member = XblMultiplayerSession::GetPlayerInSession(localUser->Xuid(), updatedLobbySession);
            if (member != nullptr)
            {
                auto localMember = MultiplayerMember::CreateFromSessionMember(member, updatedLobbySession, gameSession, true);
                localLobbyGameMembers.push_back(localMember);
            }
        }
    }

    m_localLobbyMembers = localLobbyGameMembers;
}

std::shared_ptr<MultiplayerLobbySession>
MultiplayerLobbyClient::ConvertToMultiplayerLobby(
    _In_ const  std::shared_ptr<XblMultiplayerSession>& sessionToConvert,
    _In_ const  std::shared_ptr<XblMultiplayerSession>& gameSession
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
            sessionToConvert,
            gameSession,
            m_multiplayerLocalUserManager->GetLocalUserMap()
            );
        if (member.DeviceToken.Value[0] != 0 && utils::str_icmp(member.DeviceToken.Value, sessionToConvertSafe.SessionProperties().HostDeviceToken.Value) == 0)
        {
            hostMember = gameMember;
        }

        gameMembers.push_back(gameMember);
    }

    return MakeShared<MultiplayerLobbySession>(
        sessionToConvert,
        hostMember,
        gameMembers,
        m_localLobbyMembers
        );
}

XblMultiplayerJoinability
MultiplayerLobbyClient::Joinability()
{
    return m_joinability;
}

const MultiplayerEventQueue&
MultiplayerLobbyClient::EventQueue()
{
    // Don't require a lock as STL is multithread read safe
    return m_multiplayerEventQueue;
}

void
MultiplayerLobbyClient::ClearPendingQueue()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    while (!m_pendingRequestQueue.empty())
    {
        m_pendingRequestQueue.pop();
    }
}

void
MultiplayerLobbyClient::AddToPendingQueue(
    _In_ std::shared_ptr<MultiplayerClientPendingRequest> pendingRequest
    )
{
    // No lock required. Methods that call this already has a lock
    m_pendingRequestQueue.push(pendingRequest);
}

void
MultiplayerLobbyClient::AddToProcessingQueue(
    _In_ xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue
    )
{
    for (const auto& request : processingQueue)
    {
        m_processingQueue.push_back(request);
    }
}

void
MultiplayerLobbyClient::RemoveFromProcessingQueue(
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
MultiplayerLobbyClient::GetProcessingQueue()
{
    return m_processingQueue;
}

HRESULT
MultiplayerLobbyClient::AddLocalUser(
    _In_ xbox_live_user_t user,
    _In_ MultiplayerLocalUserLobbyState userState,
    _In_ const xsapi_internal_string& handleId
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr);
    auto localUser = m_multiplayerLocalUserManager->GetLocalUserHelper(user);
    RETURN_HR_IF_LOG_DEBUG(localUser != nullptr && userState == MultiplayerLocalUserLobbyState::Add, E_UNEXPECTED, "User already added.");

    if (localUser == nullptr)
    {
        auto localUserResult = m_multiplayerLocalUserManager->AddUserToXboxLiveContextToMap(user);
        RETURN_HR_IF_FAILED(localUserResult.Hresult());

        localUser = localUserResult.ExtractPayload();
    }

    auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
    pendingRequest->SetLocalUser(localUser);
    pendingRequest->SetLobbyState(userState);
    if (userState == MultiplayerLocalUserLobbyState::Join)
    {
        if (!handleId.empty())
        {
            pendingRequest->SetLobbyHandleId(handleId);
        }
    }
    AddToPendingQueue(pendingRequest);

    return S_OK;
}

void
MultiplayerLobbyClient::AddLocalUsers(
    _In_ xsapi_internal_vector<xbox_live_user_t> users,
    _In_ const xsapi_internal_string& handleId
    )
{
    for (const auto& user : users)
    {
        AddLocalUser(user, MultiplayerLocalUserLobbyState::Join, handleId);
    }
}


void
MultiplayerLobbyClient::AddLocalUsers(
    _In_ xsapi_internal_vector<xbox_live_user_t> users
    )
{
    for (const auto& user : users)
    {
        AddLocalUser(user, MultiplayerLocalUserLobbyState::Join, xsapi_internal_string());
    }
}

HRESULT
MultiplayerLobbyClient::RemoveLocalUser(
    _In_ xbox_live_user_t user
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr);

    auto localUser = m_multiplayerLocalUserManager->GetLocalUserHelper(user);
    RETURN_HR_IF_LOG_DEBUG(localUser == nullptr || localUser->Context() == nullptr, E_UNEXPECTED, "Call add_local_user() first.");

    auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
    pendingRequest->SetLocalUser(localUser);
    pendingRequest->SetLobbyState(MultiplayerLocalUserLobbyState::Leave);
    AddToPendingQueue(pendingRequest);

    return S_OK;
}

void
MultiplayerLobbyClient::RemoveAllLocalUsers()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    if (m_multiplayerLocalUserManager != nullptr)
    {
        const auto& xboxLiveContextMap = m_multiplayerLocalUserManager->GetLocalUserMap();
        for (auto xboxLiveContext : xboxLiveContextMap)
        {
            const auto& localUser = xboxLiveContext.second;
            if (localUser != nullptr)
            {
                auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
                pendingRequest->SetLocalUser(localUser);
                pendingRequest->SetLobbyState(MultiplayerLocalUserLobbyState::Leave);
                AddToPendingQueue(pendingRequest);
            }
        }
    }
}

HRESULT
MultiplayerLobbyClient::SetLocalMemberProperties(
    _In_ xbox_live_user_t user,
    _In_ const xsapi_internal_string& name,
    _In_ const JsonValue& valueJson,
    _In_opt_ context_t context
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr);

    auto localUser = m_multiplayerLocalUserManager->GetLocalUserHelper(user);
    RETURN_HR_IF_LOG_DEBUG(localUser == nullptr || localUser->Context() == nullptr, E_UNEXPECTED, "Call add_local_user() before setting local member properties.");

    auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
    pendingRequest->SetLocalUserProperties(localUser, name, valueJson, context);
    AddToPendingQueue(pendingRequest);

    return S_OK;
}

HRESULT
MultiplayerLobbyClient::DeleteLocalMemberProperties(
    _In_ xbox_live_user_t user,
    _In_ const xsapi_internal_string& name,
    _In_opt_ context_t context
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr);

    auto localUser = m_multiplayerLocalUserManager->GetLocalUserHelper(user);
    RETURN_HR_IF_LOG_DEBUG(localUser == nullptr || localUser->Context() == nullptr, E_UNEXPECTED, "Call add_local_user() before deleting local member properties.");

    auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
    pendingRequest->SetLocalUserProperties(localUser, name, JsonValue(), context);
    AddToPendingQueue(pendingRequest);

    return S_OK;
}

HRESULT
MultiplayerLobbyClient::SetLocalMemberConnectionAddress(
    _In_ xbox_live_user_t user,
    _In_ const xsapi_internal_string& address,
    _In_opt_ context_t context
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    RETURN_HR_INVALIDARGUMENT_IF(user == nullptr);

    auto localUser = m_multiplayerLocalUserManager->GetLocalUserHelper(user);
    RETURN_HR_IF_LOG_DEBUG(localUser == nullptr || localUser->Context() == nullptr, E_UNEXPECTED, "Call add_local_user() before setting local member connection address.");

    auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
    pendingRequest->SetLocalUserConnectionAddress(localUser, address, context);
    AddToPendingQueue(pendingRequest);

    return S_OK;
}

HRESULT MultiplayerLobbyClient::SetJoinability(
    _In_ XblMultiplayerJoinability value,
    _In_opt_ context_t context
    )
{
    RETURN_HR_INVALIDARGUMENT_IF(value < XblMultiplayerJoinability::JoinableByFriends || value > XblMultiplayerJoinability::Closed);

    auto pendingRequest = MakeShared<MultiplayerClientPendingRequest>();
    pendingRequest->SetJoinability(value, context);
    AddToPendingQueue(pendingRequest);

    return S_OK;
}

MultiplayerEventQueue
MultiplayerLobbyClient::DoWork()
{
    bool expected = false;
    if (m_pendingCommitInProgress.compare_exchange_strong(expected, true))
    {
        if (m_pendingRequestQueue.size() > 0)
        {
            xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue;
            XblMultiplayerSessionReference teamSessionRef{};
            bool applySynchronizedChanges = false;
            bool lobbyStateIsJoin = false;
            bool joinByHandleId = false;
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

                    if (pendingRequest->LocalUser() != nullptr)
                    {
                        auto lobbyState = pendingRequest->LobbyState();
                        if (lobbyState == MultiplayerLocalUserLobbyState::Join)
                        {
                            lobbyStateIsJoin = true;

                            // Leave existing lobby without updating the latest as the leave may comeback after the actual join and overwrite it.
                            auto latestSession = m_sessionWriter->Session();
                            if (latestSession != nullptr)
                            {
                                LeaveRemoteSession(latestSession);
                            }

                            m_sessionWriter->UpdateSession(nullptr);
                            UpdateLobby(nullptr);
                            m_localLobbyMembers.clear();
                            m_joinability = XblMultiplayerJoinability::None;

                            if (!pendingRequest->LobbyHandleId().empty())
                            {
                                pendingRequest->LocalUser()->SetLobbyHandleId(pendingRequest->LobbyHandleId());
                                joinByHandleId = true;
                            }
                        }

                        if (lobbyState != MultiplayerLocalUserLobbyState::Unknown)
                        {
                            pendingRequest->LocalUser()->SetLobbyState(lobbyState);
                        }
                        
                        pendingRequest->LocalUser()->SetWriteChangesToService(true);
                    }
                }

            } while (!doneProcessing && m_pendingRequestQueue.size() > 0);

            if (processingQueue.size() > 0)
            {
                AddToProcessingQueue(processingQueue);

                std::weak_ptr<MultiplayerLobbyClient> weakThis = shared_from_this();

                MultiplayerEventQueueCallback callback = [weakThis, processingQueue](Result<MultiplayerEventQueue> result)
                {
                    std::shared_ptr<MultiplayerLobbyClient> pThis(weakThis.lock());
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

                if (applySynchronizedChanges)
                {
                    m_sessionWriter->CommitPendingSynchronizedChanges(processingQueue, XblMultiplayerSessionType::LobbySession, callback);
                }
                else
                {
                    Vector<uint64_t> xuidsInOrder;

                    if (lobbyStateIsJoin)
                    {
                        // If users are joining from an invite than the first multiplayer_client_pending_request in processingQueue references was the invited user.
                        // We want the invited user to join first since it is possible that users in the local graph might not meet the criteria
                        // to join the invited session until the invited user joins. Imagine that the inviting session has the session privacy set to joinable_by_friends
                        // and only the invited user is a friend of the inviting user. If any additional users attempt to join the inviting session before the invited user
                        // than the join fails for the whole list of users.
                        for (auto pendingRequest : processingQueue)
                        {
                            if (pendingRequest->LocalUser())
                            {
                                xuidsInOrder.push_back(pendingRequest->LocalUser()->Xuid());
                            }
                        }
                    }

                    CommitPendingLobbyChanges(xuidsInOrder, joinByHandleId, teamSessionRef, callback);
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
MultiplayerLobbyClient::IsPendingLobbyChanges()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    if (m_pendingRequestQueue.size() > 0 || IsPendingLobbyLocalUserChanges())
    {
        return true;
    }

    if (m_sessionWriter != nullptr)
    {
        auto latestSession = m_sessionWriter->Session();
        if (latestSession != nullptr &&
            m_multiplayerLobby != nullptr &&
            latestSession->SessionInfo().ChangeNumber != m_multiplayerLobby->ChangeNumber())
        {
            return true;
        }
    }

    return false;
}

bool
MultiplayerLobbyClient::IsRequestInProgress()
{
    return m_pendingRequestQueue.size() > 0 || m_processingQueue.size() > 0 || m_multiplayerEventQueue.Size() > 0;
}

bool
MultiplayerLobbyClient::IsPendingLobbyLocalUserChanges()
{
    const auto& xboxLiveContextMap = m_multiplayerLocalUserManager->GetLocalUserMap();
    for (auto xboxLiveContext : xboxLiveContextMap)
    {
        const auto& localUser = xboxLiveContext.second;
        if (localUser != nullptr && localUser->WriteChangesToService())
        {
            return true;
        }
    }

    return false;
}

HRESULT MultiplayerLobbyClient::CommitPendingLobbyChanges(
    _In_ const Vector<uint64_t>& xuidsInOrder,
    _In_ bool joinByHandleId,
    _In_ XblMultiplayerSessionReference sessionRef,
    _In_ MultiplayerEventQueueCallback callback
) noexcept
{
    if (IsPendingLobbyLocalUserChanges())
    {
        // All local member changes always happen on the lobby session.
        std::shared_ptr<XblMultiplayerSession> latestLobbySession;
        auto lobbySession = Session();
        if (lobbySession == nullptr)
        {
            auto primaryContext  = m_multiplayerLocalUserManager->GetPrimaryContext();
            if (joinByHandleId)
            {
                latestLobbySession = MakeShared<XblMultiplayerSession>(primaryContext->Xuid(), nullptr, nullptr);
            }
            else
            {
                if (sessionRef.Scid[0] == 0)
                {
                    String sessionName = utils::create_guid(true);
                    sessionRef = XblMultiplayerSessionReferenceCreate(
                        AppConfig::Instance()->OverrideScid().data(),
                        m_lobbySessionTemplateName.data(),
                        sessionName.data()
                    );
                }

                latestLobbySession = MakeShared<XblMultiplayerSession>(primaryContext->Xuid(), &sessionRef, nullptr);
            }
        }
        else
        {
            latestLobbySession = MakeShared<XblMultiplayerSession>(*lobbySession);
        }

        // Committing  local user changes will also update any pending lobby properties.
        return CommitLobbyChanges(xuidsInOrder, latestLobbySession, [callback](Result<void> result)
        {
            callback(Result<MultiplayerEventQueue>{ result.Hresult(), result.ErrorMessage() });
        });
    }
    else
    {
        bool isGameInProgress = GameSession() != nullptr;
        return m_sessionWriter->CommitPendingChanges(GetProcessingQueue(), XblMultiplayerSessionType::LobbySession, isGameInProgress, callback);
    }
}

HRESULT MultiplayerLobbyClient::CommitLobbyChanges(
    _In_ const Vector<uint64_t>& xuidsInOrder,
    _In_ std::shared_ptr<XblMultiplayerSession> lobbySessionToCommit,
    _In_ Callback<Result<void>> callback
) noexcept
{
    // For each User in xuidsInOrder, prepare an XblMultiplayerSession with their pending lobby changes
    // and write it to MPSD. Will also update the user's multiplayer activity & set the session host token
    // as neeeded. Consistent with 1806 XDK behavior, if any individual MPSD call fails unexpectedly,
    // we abort the entire operation and return that result.
    struct CommitLobbyChangesOperation : public std::enable_shared_from_this<CommitLobbyChangesOperation>
    {
        CommitLobbyChangesOperation(
            std::shared_ptr<MultiplayerLobbyClient> lobbyClient,
            const Vector<uint64_t>& xuidsInOrder,
            std::shared_ptr<XblMultiplayerSession> lobbySessionToCommit,
            Callback<Result<void>>&& callback
        ) noexcept :
            m_lobbyClient{ std::move(lobbyClient) },
            m_lobbySessionToCommit{ std::move(lobbySessionToCommit) },
            m_callback{ std::move(callback) }
        {
            auto& localUsers{ m_lobbyClient->GetLocalUserMap() };
            if (xuidsInOrder.empty())
            {
                for (auto pair : localUsers)
                {
                    m_users.push_back(pair.second);
                }
            }
            else
            {
                for (auto xuid : xuidsInOrder)
                {
                    auto iter = localUsers.find(xuid);
                    if (iter != localUsers.end())
                    {
                        m_users.push_back(iter->second);
                    }
                }
            }
        }

        void Run() noexcept
        {
            CommitChangesForNextUser();
        }

    private:
        void CommitChangesForNextUser() noexcept
        {
            if (m_users.empty())
            {
                Complete(S_OK);
            }
            else
            {
                auto user{ m_users.front() };
                m_users.pop_front();
                CommitChangesForUser(user);
            }
        }

        void CommitChangesForUser(
            std::shared_ptr<MultiplayerLocalUser> user
        ) noexcept
        {
            // Prepare a session with changes specific to this user.
            // Ok to use MPSD 'unsafe' methods here since we created the session locally, thus it is the only reference
            auto session = MakeShared<XblMultiplayerSession>(user->Xuid(), &m_lobbySessionToCommit->SessionReference(), nullptr);

            switch (user->LobbyState())
            {
            case MultiplayerLocalUserLobbyState::Add:
            {
                session->Join();
                if (!user->ConnectionAddress().empty())
                {
                    session->CurrentUserInternalUnsafe()->SetSecureDeviceBaseAddress64(user->ConnectionAddress());
                }
                session->SetSessionChangeSubscription(XblMultiplayerSessionChangeTypes::Everything);

                // Only set this for the first user. Could cause a race condition if XblMultiplayerJoinability was changed during the second write.
                if (m_setJoinability)
                {
                    m_setJoinability = false;
                    session->SetJoinRestriction(XblMultiplayerSessionRestriction::Followed);
                    session->SetReadRestriction(XblMultiplayerSessionRestriction::Followed);

                    String jsonValueStr = MultiplayerManagerUtils::ConvertJoinabilityToString(XblMultiplayerJoinability::JoinableByFriends);
                    JsonDocument jsonValue;
                    jsonValue.SetString(jsonValueStr.c_str(), jsonValue.GetAllocator());
                    session->SetSessionCustomPropertyJson(MultiplayerLobbyClient_JoinabilityPropertyName, jsonValue);
                }
                break;
            }
            case MultiplayerLocalUserLobbyState::Join:
            {
                session->Join();

                if (!user->ConnectionAddress().empty())
                {
                    session->CurrentUserInternalUnsafe()->SetSecureDeviceBaseAddress64(user->ConnectionAddress());
                }
                session->SetSessionChangeSubscription(XblMultiplayerSessionChangeTypes::Everything);
                break;
            }
            case MultiplayerLocalUserLobbyState::InSession:
            {
                // Forces to set the current user to yourself.
                session->Join();
                break;
            }
            case MultiplayerLocalUserLobbyState::Leave:
            {
                if (XblMultiplayerSession::IsPlayerInSession(user->Xuid(), m_lobbySessionToCommit))
                {
                    session->Leave();
                }
                else
                {
                    // In a scenario where the user was removed before he could have been added.
                    m_lobbyClient->UserStateChanged({ xbl_error_code::logic_error, "The user was removed before they could be added" }, MultiplayerLocalUserLobbyState::Add, user->Xuid());
                    m_lobbyClient->UserStateChanged({ xbl_error_code::no_error }, MultiplayerLocalUserLobbyState::Leave, user->Xuid());

                    m_removeStaleUsers = true;
                    user->SetLobbyState(MultiplayerLocalUserLobbyState::Remove);

                    // Since the user has been removed, no work to be done for this user
                    this->CommitChangesForNextUser();
                    return;
                }
                break;
            }
            default:
            {
                break;
            }
            }

            bool isGameInProgress = m_lobbyClient->GameSession() != nullptr;

            // Update any pending local user or lobby session properties.
            for (auto& request : m_lobbyClient->m_processingQueue)
            {
                request->AppendPendingChanges(session, user, isGameInProgress);
            }

            // Now write the session to MPSD
            HRESULT hr{ S_OK };
            if (user->LobbyHandleId().empty())
            {
                hr = m_lobbyClient->m_sessionWriter->WriteSession(
                    user->Context(),
                    session,
                    XblMultiplayerSessionWriteMode::UpdateOrCreateNew,
                    true,
                    [
                        op{ shared_from_this() },
                        user
                    ]
                (Result<std::shared_ptr<XblMultiplayerSession>> result)
                {
                    op->HandleWriteSessionResult(user, std::move(result));
                });
            }
            else
            {
                hr = m_lobbyClient->m_sessionWriter->WriteSessionByHandle(
                    user->Context(),
                    session,
                    XblMultiplayerSessionWriteMode::UpdateOrCreateNew,
                    user->LobbyHandleId(),
                    true,
                    [
                        op{ shared_from_this() },
                        user
                    ]
                (Result<std::shared_ptr<XblMultiplayerSession>> result)
                {
                    op->HandleWriteSessionResult(user, std::move(result));
                });

                user->SetLobbyHandleId(String{});
            }
            if (FAILED(hr))
            {
                Complete(hr);
            }
        }

        void HandleWriteSessionResult(
            std::shared_ptr<MultiplayerLocalUser> user,
            Result<std::shared_ptr<XblMultiplayerSession>>&& sessionResult
        ) noexcept
        {
            m_lobbyClient->UserStateChanged(sessionResult, user->LobbyState(), user->Xuid());
            m_lobbyClient->HandleLobbyChangeEvents(sessionResult, user, m_lobbyClient->m_processingQueue);

            if (Failed(sessionResult))
            {
                m_lobbyClient->JoinLobbyCompleted(sessionResult, user->Xuid());

                // If we failed to join the lobby, make sure the local user context is cleaned up
                switch (user->LobbyState())
                {
                case MultiplayerLocalUserLobbyState::Add:
                case MultiplayerLocalUserLobbyState::Join:
                {
                    m_removeStaleUsers = true;
                    user->SetLobbyState(MultiplayerLocalUserLobbyState::Remove);
                }
                default:
                {
                    break;
                }
                }

                Complete(sessionResult);
                return;
            }

            switch (user->LobbyState())
            {
            case MultiplayerLocalUserLobbyState::Add:
            case MultiplayerLocalUserLobbyState::Join:
            {
                auto updatedSession = sessionResult.ExtractPayload();
                m_lobbyClient->UpdateSession(updatedSession);
                auto oldLobbyState = user->LobbyState();
                user->SetLobbyState(MultiplayerLocalUserLobbyState::InSession);

                if (oldLobbyState == MultiplayerLocalUserLobbyState::Join)
                {
                    m_lobbyClient->HandleJoinLobbyCompleted(sessionResult, user->Xuid());
                }

                if (oldLobbyState == MultiplayerLocalUserLobbyState::Add && m_lobbyClient->ShouldUpdateHostToken(user, updatedSession))
                {
                    UpdateHostDeviceToken(user, updatedSession);
                }
                else
                {
                    SetActivityForUser(user, updatedSession);
                }
                return;
            }
            case MultiplayerLocalUserLobbyState::Leave:
            {
                m_removeStaleUsers = true;

                // If you leave the session you were advertising, you don't need to clear the activity.
                user->SetLobbyState(MultiplayerLocalUserLobbyState::Remove);

                // Intentional fallthrough
            }
            default:
            {
                // Work done for this user, continue operation
                CommitChangesForNextUser();
                return;
            }
            }
        }

        void UpdateHostDeviceToken(
            std::shared_ptr<MultiplayerLocalUser> user,
            std::shared_ptr<XblMultiplayerSession> session
        ) noexcept
        {
            XblMultiplayerSessionReadLockGuard sessionSafe{ session };
            session->SetHostDeviceToken(sessionSafe.CurrentUser()->DeviceToken);

            auto hr = m_lobbyClient->m_sessionWriter->WriteSession(
                user->Context(),
                session,
                XblMultiplayerSessionWriteMode::UpdateExisting,
                true,
                [
                    op{ shared_from_this() },
                    user
                ]
            (Result<std::shared_ptr<XblMultiplayerSession>> writeHostTokenResult)
            {
                if (Failed(writeHostTokenResult))
                {
                    op->Complete(writeHostTokenResult);
                }
                else
                {
                    op->SetActivityForUser(user, writeHostTokenResult.ExtractPayload());
                }
            });

            if (FAILED(hr))
            {
                Complete(hr);
            }
        }

        void SetActivityForUser(
            std::shared_ptr<MultiplayerLocalUser> user,
            std::shared_ptr<XblMultiplayerSession> session
        ) noexcept
        {
            auto hr = user->Context()->MultiplayerService()->SetActivity(
                session->SessionReference(),
                AsyncContext<Result<void>>{ m_lobbyClient->m_queue,
                [
                    op{ shared_from_this() }
                ]
            (Result<void> setActivityResult)
            {
                if (Failed(setActivityResult))
                {
                    op->Complete(std::move(setActivityResult));
                }
                else
                {
                    op->CommitChangesForNextUser();
                }
            }
            });

            if (FAILED(hr))
            {
                Complete(hr);
            }
        }

        void Complete(Result<void>&& result)
        {
            LOGS_DEBUG << __FUNCTION__ << ": HRESULT=" << result.Hresult() << ", ErrorMessage=" << result.ErrorMessage();

            if (m_removeStaleUsers)
            {
                m_lobbyClient->RemoveStaleXboxLiveContextFromMap();
            }
            m_callback(result);
        }

        std::shared_ptr<MultiplayerLobbyClient> m_lobbyClient;
        List<std::shared_ptr<MultiplayerLocalUser>> m_users;
        std::shared_ptr<XblMultiplayerSession> m_lobbySessionToCommit;
        bool m_setJoinability{ true };
        bool m_removeStaleUsers{ false };
        Callback<Result<void>> m_callback;
    };

    auto operation = MakeShared<CommitLobbyChangesOperation>(
        shared_from_this(),
        xuidsInOrder,
        lobbySessionToCommit,
        std::move(callback)
    );

    operation->Run();
    return S_OK;
}

void
MultiplayerLobbyClient::RemoveStaleXboxLiveContextFromMap()
{
    auto gameClient = GameClient();
    if (gameClient != nullptr)
    {
        gameClient->RemoveStaleUsersFromRemoteSession();
    }

    // Remove stale context, switch primary context and re-activate multiplayer events.
    m_multiplayerLocalUserManager->RemoveStaleLocalUsersFromMap();
}

const xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>>&
MultiplayerLobbyClient::GetLocalUserMap()
{
    return m_multiplayerLocalUserManager->GetLocalUserMap();
}

std::shared_ptr<XblContext>
MultiplayerLobbyClient::GetPrimaryContext()
{
    return m_multiplayerLocalUserManager->GetPrimaryContext();
}

HRESULT MultiplayerLobbyClient::CreateGameFromLobby() noexcept
{
    auto lobbySession = Session();
    RETURN_HR_IF_LOG_DEBUG(lobbySession == nullptr, E_UNEXPECTED, "No lobby session exists. Call add_local_user() to create a lobby first");

    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    RETURN_HR_IF_LOG_DEBUG(primaryContext == nullptr, E_UNEXPECTED, "Call add_local_user() before joining.");

    // Try to write transfer handle to the lobby as pending state
    // If succeeded, then create the game session, and join it. The game session will later be advertised.
    // If failed, complete the operation. The game session will later be joined when we get a SessionPropertyChanged event
    struct CreateGameOperation : public std::enable_shared_from_this<CreateGameOperation>
    {
        CreateGameOperation(
            std::shared_ptr<MultiplayerLobbyClient> lobbyClient,
            std::shared_ptr<XblMultiplayerSession> lobbySession,
            uint64_t primaryXuid
        ) noexcept :
            m_lobbyClient{ std::move(lobbyClient) },
            m_sessionToCommit{ MakeShared<XblMultiplayerSession>(*lobbySession) },
            m_primaryXuid{ primaryXuid }
        {
        }

        void Run() noexcept
        {
            SetTransferHandleToPending();
        }

    private:
        void SetTransferHandleToPending()
        {
            // Add transfer handle property to lobby session
            Stringstream jsonValue;
            jsonValue << "pending~" << m_primaryXuid;
            JsonDocument value;
            value.SetString(jsonValue.str().data(), value.GetAllocator());
            m_sessionToCommit->SetSessionCustomPropertyJson(MultiplayerLobbyClient_TransferHandlePropertyName, value);

            auto hr = m_lobbyClient->m_sessionWriter->CommitSynchronizedChanges(m_sessionToCommit,
                [
                    sharedThis{ shared_from_this() }
                ]
            (Result<std::shared_ptr<XblMultiplayerSession>> result)
            {
                sharedThis->HandleWriteSessionResult(std::move(result));
            });

            if (FAILED(hr))
            {
                Complete(hr);
            }
        }

        void HandleWriteSessionResult(Result<std::shared_ptr<XblMultiplayerSession>>&& writeSessionResult)
        {
            auto op{ shared_from_this() };
            auto gameClient = m_lobbyClient->GameClient();

            if (gameClient == nullptr)
            {
                op->Complete(Result<void>{ E_FAIL, "MultiplayerGameClient destroyed" });
            }
            else if (writeSessionResult.Hresult() == HTTP_E_STATUS_PRECOND_FAILED)
            {
                if (m_lobbyClient->IsTransferHandleState("completed") || m_lobbyClient->IsTransferHandleState("pending"))
                {
                    // We couldn't set transfer handle, but a game session already exists. Join that instead
                    HRESULT hr = gameClient->JoinGameFromLobbyHelper([op](Result<std::shared_ptr<XblMultiplayerSession>> result)
                    {
                        op->Complete(result);
                    });

                    if (FAILED(hr))
                    {
                        Complete(hr);
                    }
                }
                else if(m_setTransferHandleAttempt++ < MAX_CONNECTION_ATTEMPTS)
                {

                    std::shared_ptr<XblMultiplayerSession> sessionToCommitTemp = writeSessionResult.ExtractPayload();

                    HRESULT hr = S_OK;
                    if (sessionToCommitTemp != nullptr) // handle rare case where an empty body is returned
                    {
                        m_sessionToCommit = sessionToCommitTemp;
                        // Retry setting transfer handle after a small delay
                        hr = m_lobbyClient->m_queue.RunWork([op] {
                            op->SetTransferHandleToPending();
                        }, RETRY_DELAY_MS);
                    }
                    else
                    {
                        hr = writeSessionResult.Hresult();
                    }

                    if (FAILED(hr))
                    {
                        op->Complete(hr);
                    }
                }
                else
                {
                    // We were unable to set the transfer handle after MAX_CONNECTION_ATTEMPTS
                    m_lobbyClient->UpdateSession(m_sessionToCommit);

                    Result<void> opResult{ writeSessionResult.Hresult(), "Max connection attempts exceeded" };
                    m_lobbyClient->JoinLobbyCompleted(
                        opResult,
                        m_primaryXuid
                    );

                    op->Complete(std::move(opResult));
                }
            }
            else 
            {
                // Should we not check for WriteSession failures other than 412?
                // 1806 XDK assumes the transfer handle was successfully set, creates a game session, and joins it

                auto sessionName = utils::create_guid(true);
                HRESULT hr = gameClient->JoinGameHelper(sessionName, [op](Result<void> result)
                {
                    op->Complete(std::move(result));
                });

                if (FAILED(hr))
                {
                    op->Complete(hr);
                }
            }
        }

        void Complete(Result<void>&& result) noexcept
        {
            // This operation is strange in that nothing directly looks at the result.
            // The result is exposed to title via events, but MPM doesn't seem to handle failures.
            // Trying to keep the purpose of each operation as consistent with 1806 XDK as possible, so leaving as is for now.

            LOGS_DEBUG << __FUNCTION__ << ": HRESULT=" << result.Hresult() << ", ErrorMessage=" << result.ErrorMessage();
        }

        std::shared_ptr<MultiplayerLobbyClient> m_lobbyClient;
        std::shared_ptr<XblMultiplayerSession> m_sessionToCommit;
        uint64_t const m_primaryXuid;
        uint8_t m_setTransferHandleAttempt{ 0 };
    };

    auto operation = MakeShared<CreateGameOperation>(
        shared_from_this(),
        lobbySession,
        primaryContext->Xuid()
    );

    operation->Run();
    return S_OK;
}

void MultiplayerLobbyClient::AdvertiseGameSession() noexcept
{
    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    if (primaryContext == nullptr || GameSession() == nullptr)
    {
        return;
    }

    // This operation performs several sub-steps as follows:
    // 1. If a pending commit is currently in progress, wait until it is completed
    // 2. Establish a lobby session & commit any pending lobby changes
    // 3. Create an MPSD transfer handle from the lobby session to the game session
    // 4. Update the lobby session properties (custom transfer handle & joinability)
    struct AdvertiseGameSessionOperation : public std::enable_shared_from_this<AdvertiseGameSessionOperation>
    {
        AdvertiseGameSessionOperation(
            std::shared_ptr<MultiplayerLobbyClient> lobbyClient,
            std::shared_ptr<XblContext> primaryContext
        ) noexcept
            : m_lobbyClient{ std::move(lobbyClient) },
            m_primaryContext{ std::move(primaryContext) }
        {
        }

        void Run() noexcept
        {
            bool expected{ false };
            if (!m_lobbyClient->m_pendingCommitInProgress.compare_exchange_strong(expected, true))
            {
                // Wait until there isn't commit in progress before continuing.
                // Reschedule op with no delay (1806 XDK behavior)
                HRESULT hr = m_lobbyClient->m_queue.RunWork([op{ shared_from_this() }]
                    {
                        op->Run();
                    });

                if (FAILED(hr))
                {
                    Complete(hr);
                }
            }
            else
            {
                EstablishLobbySession();
            }
        }

    private:
        void EstablishLobbySession() noexcept
        {
            auto lobbySession{ m_lobbyClient->Session() };
            if (!lobbySession)
            {
                if (m_lobbyClient->m_multiplayerLocalUserManager->GetLocalUserMap().empty())
                {
                    // There are no remaining local users. Complete the operation
                    return Complete(S_OK);
                }

                m_lobbyClient->m_multiplayerLocalUserManager->ChangeAllLocalUserLobbyState(MultiplayerLocalUserLobbyState::Add);
                auto hr = m_lobbyClient->CommitPendingLobbyChanges(Vector<uint64_t>{}, false, XblMultiplayerSessionReference{},
                    [
                        op{ shared_from_this() }
                    ]
                (Result<MultiplayerEventQueue> joinLobbyResult)
                {
                    op->m_lobbyClient->m_pendingCommitInProgress = false;
                    op->m_lobbyClient->JoinLobbyCompleted(joinLobbyResult, op->m_primaryContext->Xuid());
                    if (Failed(joinLobbyResult))
                    {
                        op->Complete(joinLobbyResult);
                    }
                    else
                    {
                        op->CreateTransferHandle(op->m_lobbyClient->Session());
                    }
                });

                if (FAILED(hr))
                {
                    Complete(hr);
                }
            }
            else
            {
                m_lobbyClient->m_pendingCommitInProgress = false;
                CreateTransferHandle(lobbySession);
            }
        }

        void CreateTransferHandle(std::shared_ptr<XblMultiplayerSession> lobbySession) noexcept
        {
            JsonDocument lobbyProperties;
            {
                XblMultiplayerSessionReadLockGuard lobbySessionSafe(lobbySession);
                lobbyProperties.Parse(lobbySessionSafe.SessionProperties().SessionCustomPropertiesJson);
            }

            if (!lobbyProperties.HasMember(MultiplayerLobbyClient_TransferHandlePropertyName) ||
                (m_lobbyClient->IsTransferHandleState("pending") && m_lobbyClient->GetTransferHandle() == utils::uint64_to_internal_string(m_primaryContext->Xuid())))
            {
                auto gameSession{ m_lobbyClient->GameSession() };
                if (!gameSession)
                {
                    return Complete(Result<void>{E_ABORT, "GameSession null"});
                }

                auto hr = m_primaryContext->MultiplayerService()->SetTransferHandle(
                    gameSession->SessionReference(),
                    lobbySession->SessionReference(),
                    AsyncContext<Result<String>>{ m_lobbyClient->m_queue,
                    [
                        op{ shared_from_this() },
                        lobbySession
                    ]
                (Result<String> result)
                {
                    if (Succeeded(result))
                    {
                        op->UpdateLobbySession(MakeShared<XblMultiplayerSession>(*lobbySession), result.ExtractPayload());
                    }
                    else
                    {
                        if (result.Hresult() == HTTP_E_STATUS_FORBIDDEN)
                        {
                            // By MPSD design, if the game session doesn't exist on transfer handle creation, it throws a 403.
                            op->m_lobbyClient->ClearGameSessionFromLobby();
                        }
                        op->Complete(result);
                    }
                }
                });

                if (FAILED(hr))
                {
                    return Complete(hr);
                }
            }
        }

        void UpdateLobbySession(
            std::shared_ptr<XblMultiplayerSession> lobbySession,
            String&& transferHandle
        ) noexcept
        {
            if (m_lobbyClient->m_joinability == XblMultiplayerJoinability::DisableWhileGameInProgress)
            {
                lobbySession->SetClosed(true);
            }

            Stringstream jsonHandleValue;
            jsonHandleValue << "completed~" << transferHandle;

            JsonDocument jsonValue;
            jsonValue.SetString(jsonHandleValue.str().data(), jsonValue.GetAllocator());
            lobbySession->SetSessionCustomPropertyJson(
                MultiplayerLobbyClient_TransferHandlePropertyName,
                jsonValue
            );

            HRESULT hr = m_lobbyClient->m_sessionWriter->WriteSession(
                m_primaryContext,
                lobbySession,
                XblMultiplayerSessionWriteMode::UpdateExisting,
                true,
                [
                    op{ shared_from_this() }
                ]
            (Result<std::shared_ptr<XblMultiplayerSession>> result)
            {
                op->Complete(result);
            });

            if (FAILED(hr))
            {
                Complete(hr);
            }
        }

        void Complete(Result<void>&& result)
        {
            // This operation is strange in that nothing directly looks at the result.
            // The result is exposed to title via events, but MPM doesn't seem to handle failures.
            // Trying to keep the purpose of each operation as consistent with 1806 XDK as possible, so leaving as is for now.

            LOGS_DEBUG << __FUNCTION__ << ": HRESULT=" << result.Hresult() << ", ErrorMessage=" << result.ErrorMessage();
        }

        std::shared_ptr<MultiplayerLobbyClient> m_lobbyClient;
        std::shared_ptr<XblContext> m_primaryContext;
    };

    auto operation = MakeShared<AdvertiseGameSessionOperation>(shared_from_this(), primaryContext);
    operation->Run();
}

void
MultiplayerLobbyClient::StopAdvertisingGameSession(
    _In_ Result<std::shared_ptr<XblMultiplayerSession>> result
    )
{
    bool bClearGameSession = false;
    auto lobbySession = Session();
    XblMultiplayerSessionReadLockGuard lobbySessionSafe(lobbySession);
    if (SUCCEEDED(result.Hresult()) && result.Payload() == nullptr)
    {
        // When you are the last person to leave the session (handling err 204).
        bClearGameSession = true;
    }
    else if (result.Payload() != nullptr && lobbySession != nullptr)
    {
        auto latestGameSession = result.Payload();

        bool found = false;
        auto members = lobbySessionSafe.Members();
        for (const auto& member : members)
        {
            if (XblMultiplayerSession::IsPlayerInSession(member.Xuid, latestGameSession))
            {
                found = true;
                break;
            }
        }

        if (!found && members.size() > 0)
        {
            bClearGameSession = true;
        }
    }

    if (bClearGameSession)
    {
        ClearGameSessionFromLobby();
    }
}

void
MultiplayerLobbyClient::ClearGameSessionFromLobby()
{
    auto lobbySession = Session();
    if (lobbySession != nullptr)
    {
        auto lobbySessionCopy = MakeShared<XblMultiplayerSession>(*lobbySession);

        if (m_joinability == XblMultiplayerJoinability::DisableWhileGameInProgress)
        {
            // Re-open the lobby to be joinable when leaving the game session.
            lobbySessionCopy->SetClosed(false);
        }

        lobbySessionCopy->DeleteSessionCustomPropertyJson(MultiplayerLobbyClient_TransferHandlePropertyName);
        m_sessionWriter->WriteSession(m_multiplayerLocalUserManager->GetPrimaryContext(), lobbySessionCopy, XblMultiplayerSessionWriteMode::UpdateExisting, true, nullptr);
    }
}

bool
MultiplayerLobbyClient::IsTransferHandleState(
    _In_ const xsapi_internal_string& state
    )
{
    auto lobbySession = Session();
    if (lobbySession == nullptr)
    {
        return false;
    }

    XblMultiplayerSessionReadLockGuard lobbySessionSafe(lobbySession);

    JsonDocument jsonDoc;
    jsonDoc.Parse(lobbySessionSafe.SessionProperties().SessionCustomPropertiesJson);

    if (!jsonDoc.HasParseError())
    {
        xsapi_internal_string transferHandleProp;
        if (SUCCEEDED(JsonUtils::ExtractJsonString(jsonDoc, MultiplayerLobbyClient_TransferHandlePropertyName, transferHandleProp, true)))
        {
            xsapi_internal_vector<xsapi_internal_string> transferHandleSplit = utils::string_split_internal(transferHandleProp, '~');

            if (transferHandleSplit.size() > 0 &&
                utils::str_icmp_internal(transferHandleSplit[0], state) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

xsapi_internal_string
MultiplayerLobbyClient::GetTransferHandle()
{
    auto lobbySession = Session();
    if (lobbySession == nullptr)
    {
        return xsapi_internal_string();
    }

    XblMultiplayerSessionReadLockGuard lobbySessionSafe(lobbySession);
    JsonDocument jsonDoc;
    jsonDoc.Parse(lobbySessionSafe.SessionProperties().SessionCustomPropertiesJson);

    if (!jsonDoc.HasParseError())
    {
        xsapi_internal_string transferHandleProp;
        if (SUCCEEDED(JsonUtils::ExtractJsonString(jsonDoc, MultiplayerLobbyClient_TransferHandlePropertyName, transferHandleProp, true)))
        {
            xsapi_internal_vector<xsapi_internal_string> transferHandleSplit = utils::string_split_internal(transferHandleProp, '~');

            if (transferHandleSplit.size() == 2)
            {
                return transferHandleSplit[1];
            }
        }
    }

    return xsapi_internal_string();
}

void
MultiplayerLobbyClient::LeaveRemoteSession(
    _In_ std::shared_ptr<XblMultiplayerSession> session
    )
{
    m_sessionWriter->LeaveRemoteSession(session, nullptr);
}

bool
MultiplayerLobbyClient::ShouldUpdateHostToken(
    _In_ std::shared_ptr<MultiplayerLocalUser> localUser,
    _In_ std::shared_ptr<XblMultiplayerSession> session
    )
{
    if(XblMultiplayerSession::HostMember(session) != nullptr)
    {
        return false;
    }

    if(!localUser->IsPrimaryXboxLiveContext())
    {
        return false;
    }

    // If the local user is already the host, skip it.
    if (XblMultiplayerSession::IsHost(utils::uint64_to_internal_string(localUser->Xuid()), session))
    {
        return false;
    }

    return true;
}

void
MultiplayerLobbyClient::UserStateChanged(
    _In_ Result<void> error,
    _In_ MultiplayerLocalUserLobbyState localUserLobbyState,
    _In_ uint64_t xboxUserId
    )
{
    if (localUserLobbyState != MultiplayerLocalUserLobbyState::Add &&
        localUserLobbyState != MultiplayerLocalUserLobbyState::Leave)
    {
        return;
    }

    std::shared_ptr<XblMultiplayerEventArgs> eventArgs;
    XblMultiplayerEventType eventType = XblMultiplayerEventType::UserAdded;
    if (localUserLobbyState == MultiplayerLocalUserLobbyState::Add)
    {
        eventType = XblMultiplayerEventType::UserAdded;
        std::shared_ptr<UserAddedEventArgs> userAddedEventArgs = MakeShared<UserAddedEventArgs>(xboxUserId);
        eventArgs = std::dynamic_pointer_cast<UserAddedEventArgs>(userAddedEventArgs);
    }
    else if (localUserLobbyState == MultiplayerLocalUserLobbyState::Leave)
    {
        eventType = XblMultiplayerEventType::UserRemoved;
        std::shared_ptr<UserRemovedEventArgs> userRemovedEventArgs = MakeShared<UserRemovedEventArgs>(xboxUserId);
        eventArgs = std::dynamic_pointer_cast<UserRemovedEventArgs>(userRemovedEventArgs);
    }
    else
    {
        // We only care about user_added & user_removed.
        return;
    }

    AddEvent(eventType, eventArgs, error);
}

void
MultiplayerLobbyClient::HandleLobbyChangeEvents(
    _In_ Result<void> error,
    _In_ std::shared_ptr<MultiplayerLocalUser> localUser,
    _In_ const xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>>& processingQueue
    )
{
    xsapi_internal_map<xsapi_internal_string, JsonDocument> localUsersMap;
    xsapi_internal_string localUserSecureDeviceAddress;
    for (auto& request : processingQueue)
    {
        // Handle local user change events
        if (localUser != nullptr && request->LocalUser() != nullptr && localUser->Xuid() == request->LocalUser()->Xuid())
        {
            if (!request->LocalUserSecureDeivceAddress().empty())
            {
                AddEvent(
                    XblMultiplayerEventType::LocalMemberConnectionAddressWriteCompleted,
                    nullptr,
                    error,
                    request->Context()
                );

                localUserSecureDeviceAddress = request->LocalUserSecureDeivceAddress();
            }

            // Fire events for each of the properties
            for (const auto& prop : request->LocalUserProperties())
            {
                AddEvent(
                    XblMultiplayerEventType::LocalMemberPropertyWriteCompleted,
                    nullptr,
                    error,
                    request->Context()
                );

                localUsersMap[prop.first] = JsonDocument();
                JsonUtils::CopyFrom(localUsersMap[prop.first], prop.second);
            }
        }
    }

    // Handle lobby change events (session properties, etc)
    auto eventQueue = m_sessionWriter->HandleEvents(processingQueue, error, XblMultiplayerSessionType::LobbySession);
    if (eventQueue.Size() > 0)
    {
        std::lock_guard<std::mutex> lock(m_clientRequestLock);
        for (auto& ev : eventQueue)
        {
            m_multiplayerEventQueue.AddEvent(ev);
        }
    }

    if (localUsersMap.size() != 0 || !localUserSecureDeviceAddress.empty())
    {
        // write member properties to the game session.
        auto gameClient = GameClient();
        if (gameClient != nullptr)
        {
            gameClient->SetLocalMemberPropertiesToRemoteSession(localUser, localUsersMap, localUserSecureDeviceAddress);
        }
    }
}

void
MultiplayerLobbyClient::HandleJoinLobbyCompleted(
    _In_ Result<void> error,
    _In_ uint64_t joinedXuid
    )
{
    JoinLobbyCompleted(error, joinedXuid);
    if (FAILED(error.Hresult()))
    {
        // If joining the lobby succeeded, check if it has a game session associated with it to join.

        auto gameClient = GameClient();
        if (gameClient == nullptr)
        {
            return;
        }

        auto lobbySession = Session();

        // Join game via the transfer handle.
        XblMultiplayerSessionReadLockGuard lobbySessionSafe(lobbySession);

        JsonDocument lobbyProperties;
        lobbyProperties.Parse(lobbySessionSafe.SessionProperties().SessionCustomPropertiesJson);

        if (!lobbyProperties.HasParseError())
        {
            if (lobbyProperties.IsObject() && lobbyProperties.MemberCount() > 0)
            {
                xsapi_internal_string transferHandle;

                if (IsTransferHandleState("completed"))
                {
                    transferHandle = GetTransferHandle();
                }
                else
                {
                    // No existing game session
                    return;
                }

                gameClient->JoinGameByHandle(transferHandle, false, nullptr);
            }
        }
    }
}

void
MultiplayerLobbyClient::JoinLobbyCompleted(
    _In_ Result<void> error,
    _In_ uint64_t invitedXboxUserId
    )
{
    std::shared_ptr<JoinLobbyCompletedEventArgs> joinLobbyEventArgs = MakeShared<JoinLobbyCompletedEventArgs>(
        invitedXboxUserId
        );

    AddEvent(
        XblMultiplayerEventType::JoinLobbyCompleted,
        std::dynamic_pointer_cast<JoinLobbyCompletedEventArgs>(joinLobbyEventArgs),
        error
    );
}

// TODO remove this and add locking to event queue class
void 
MultiplayerLobbyClient::AddEvent(
    _In_ XblMultiplayerEventType eventType,
    _In_opt_ std::shared_ptr<XblMultiplayerEventArgs> eventArgs,
    _In_opt_ Result<void> error,
    _In_opt_ context_t context
)
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_multiplayerEventQueue.AddEvent(eventType,
        XblMultiplayerSessionType::LobbySession,
        eventArgs,
        error,
        context
    );
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"
#include "multiplayer_internal.h"

using namespace xbox::services;
using namespace xbox::services::legacy;
using namespace xbox::services::system;
using namespace xbox::services::multiplayer;
using namespace xbox::services::matchmaking;
#if HC_PLATFORM == HC_PLATFORM_XDK
using namespace Windows::Xbox::Networking;
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerMatchClient::MultiplayerMatchClient(
    _In_ const TaskQueue& queue,
    _In_ std::shared_ptr<MultiplayerLocalUserManager> localUserManager
) noexcept :
    m_queue{ queue.DeriveWorkerQueue() },
    m_multiplayerLocalUserManager{ localUserManager }
{
}

void MultiplayerMatchClient::deep_copy_if_updated(
    _In_ const MultiplayerMatchClient& other
)
{
    std::lock_guard<std::mutex> lock(m_lock);
    if (other.m_matchSession == nullptr)
    {
        m_matchSession = nullptr;
    }
    else if (m_matchSession == nullptr || other.m_matchSession->SessionInfo().ChangeNumber > m_matchSession->SessionInfo().ChangeNumber)
    {
        m_matchSession = MakeShared<XblMultiplayerSession>(*other.m_matchSession);
    }
}

const MultiplayerEventQueue&
MultiplayerMatchClient::EventQueue()
{
    return m_multiplayerEventQueue;
}

MultiplayerEventQueue
MultiplayerMatchClient::DoWork()
{
    switch (static_cast<XblMultiplayerMatchStatus>(m_matchStatus))
    {
        case XblMultiplayerMatchStatus::None:
        {
            return MultiplayerEventQueue();
        }

        case XblMultiplayerMatchStatus::Searching:
        {
            CheckNextTimer();
            break;
        }

        case XblMultiplayerMatchStatus::Found:
        case XblMultiplayerMatchStatus::Canceled:
        {
            // Nothing to do here. Wait for match_status_changed event.
            break;
        }

        case XblMultiplayerMatchStatus::Joining:
        {
            if(m_joinTargetSessionComplete)
            {
                HandleSessionJoined();
            }
            break;
        }

        case XblMultiplayerMatchStatus::WaitingForRemoteClientsToJoin:
        case XblMultiplayerMatchStatus::WaitingForRemoteClientsToUploadQos:
        {
            HandleInitializationStateChanged(Session());
            break;
        }

        case XblMultiplayerMatchStatus::Measuring:
        {
            // Waiting for title to perform qos and provide qos measurements (via set_quality_of_service_measurements)
            HandleInitializationStateChanged(Session());
            break;
        }

        case XblMultiplayerMatchStatus::Evaluating:
        {
            // Nothing to do here. Wait for initialization stage changed event.
            break;
        }
            
        default:
            break;
    }

    MultiplayerEventQueue eventQueue;
    {
        std::lock_guard<std::mutex> lock(m_multiplayerEventQueueLock);
        eventQueue = m_multiplayerEventQueue;
        m_multiplayerEventQueue.Clear();
    }

    return eventQueue;
}

void
MultiplayerMatchClient::DisableNextTimer(bool value)
{
    m_disableNextTimer = value;
}

void
MultiplayerMatchClient::CheckNextTimer()
{
    if (m_disableNextTimer) return;     // Only used for Unit tests

    int64_t delta = m_nextTimerToFetchSession.to_interval() - xbox::services::datetime::utc_now().to_interval();
    if ( delta < 0)
    {
        if (m_matchStatus == XblMultiplayerMatchStatus::Searching)
        {
            std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
            if (primaryContext == nullptr || m_getSessionInProgress)
            {
                return;
            }

            // Fetch ticket session
            std::weak_ptr<MultiplayerMatchClient> weakSessionWriter = shared_from_this();
            m_getSessionInProgress = true;

            primaryContext->MultiplayerService()->GetCurrentSession(m_matchTicketSessionRef, { m_queue,
            [weakSessionWriter](Result<std::shared_ptr<XblMultiplayerSession>> sessionResult)
            {
                std::shared_ptr<MultiplayerMatchClient> pThis(weakSessionWriter.lock());
                if (pThis != nullptr)
                {
                    if (SUCCEEDED(sessionResult.Hresult()))
                    {
                        pThis->HandleMatchStatusChanged(sessionResult.Payload());
                    }
                    pThis->m_getSessionInProgress = false;
                }
            }
            });
        }
        else
        {
            GetLatestSession();
        }
    }
}

void
MultiplayerMatchClient::HandleQosMeasurements()
{
    std::shared_ptr<PerformQosMeasurementsEventArgs> performQosEventArgs = MakeShared<PerformQosMeasurementsEventArgs>();

    XblMultiplayerSessionReadLockGuard sessionSafe(Session());
    for (const auto& member : sessionSafe.Members())
    {
        if (!member.IsCurrentUser)
        {
            std::vector<unsigned char> base64ConnectionAddress(xbox::services::convert::from_base64(member.SecureDeviceBaseAddress64));
            const xsapi_internal_string& secureDeviceAddress = xsapi_internal_string(base64ConnectionAddress.begin(), base64ConnectionAddress.end());
            if (!secureDeviceAddress.empty())
            {
                performQosEventArgs->AddRemoteClient(secureDeviceAddress, member.DeviceToken.Value);
            }
        }
    }

    if (performQosEventArgs->remoteClients.size() > 0)
    {
        m_matchStatus = XblMultiplayerMatchStatus::Measuring;

        {
            std::lock_guard<std::mutex> lock(m_multiplayerEventQueueLock);
            m_multiplayerEventQueue.AddEvent(
                XblMultiplayerEventType::PerformQosMeasurements,
                XblMultiplayerSessionType::GameSession,
                std::dynamic_pointer_cast<PerformQosMeasurementsEventArgs>(performQosEventArgs)
            );
        }
    }
    else
    {
        // If clients fail to join, the stage advances to "measuring".
        // Wait until memberInitialization either succeeds or fails.
        CheckNextTimer();
    }
}

void
MultiplayerMatchClient::HandleFindMatchCompleted(
    _In_ Result<void> error
    )
{
    XblMultiplayerMeasurementFailure failure = XblMultiplayerMeasurementFailure::Unknown;

    auto matchSession = Session();
    XblMultiplayerSessionReadLockGuard matchSessionSafe(matchSession);
    if (matchSession != nullptr && matchSessionSafe.CurrentUser() != nullptr)
    {
        failure = matchSessionSafe.CurrentUser()->InitializationFailureCause;
    }

    std::shared_ptr<FindMatchCompletedEventArgs> findMatchEventArgs = MakeShared<FindMatchCompletedEventArgs>(
        m_matchStatus,
        failure
        );

    {
        std::lock_guard<std::mutex> lock(m_multiplayerEventQueueLock);
        m_multiplayerEventQueue.AddEvent(
            XblMultiplayerEventType::FindMatchCompleted,
            XblMultiplayerSessionType::GameSession,
            std::dynamic_pointer_cast<FindMatchCompletedEventArgs>(findMatchEventArgs),
            error
        );
    }
}

void
MultiplayerMatchClient::HandleMatchStatusChanged(
    _In_ std::shared_ptr<XblMultiplayerSession> matchSession
    )
{
    switch (matchSession->MatchmakingServer()->Status)
    {
        case XblMatchmakingStatus::Searching:
        {
            XblMultiplayerMatchStatus expected = XblMultiplayerMatchStatus::None;
            if (m_matchStatus.compare_exchange_strong(expected, XblMultiplayerMatchStatus::Searching, std::memory_order_release))
            {
                // Wait for status to change on ticket session or fetch after 2 mins.
                m_nextTimerToFetchSession = xbox::services::datetime::utc_now() + xbox::services::datetime::from_seconds(120);
            }
            else 
            {
                if (m_disableNextTimer) return;     // Only used for Unit tests

                int64_t delta = m_nextTimerToFetchSession.to_interval() - xbox::services::datetime::utc_now().to_interval();
                if ( delta < 0)
                {
                    // Delete the match ticket and let the title know that it failed.
                    if (!m_hopperName.empty() && m_matchTicketResponse.matchTicketId[0] != '\0')
                    {
                        // Only the host has the ticketId info. Since we aren't the host who started the match, we cannot cancel it either.
                        std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
                        if (primaryContext == nullptr)
                        {
                            return;
                        }

                        auto asyncBlock = utils::MakeDefaultAsyncBlock(m_queue.GetHandle());
                        primaryContext->MatchmakingService()->DeleteMatchTicketAsync(
                            AppConfig::Instance()->Scid(),
                            m_hopperName,
                            m_matchTicketResponse.matchTicketId,
                            asyncBlock
                        );
                    }

                    m_matchStatus = XblMultiplayerMatchStatus::Failed;
                    HandleFindMatchCompleted({ xbl_error_code::generic_error, "Timer exceeded" });
                }
            }
            break;
        }
        case XblMatchmakingStatus::Expired:
        {
            m_matchStatus = XblMultiplayerMatchStatus::Expired;
            HandleFindMatchCompleted({ xbl_error_code::generic_error, "Match status: Expired" });
            break;
        }
        case XblMatchmakingStatus::Canceled:
        {
            m_matchStatus = XblMultiplayerMatchStatus::Canceled;
            HandleFindMatchCompleted({ xbl_error_code::generic_error, "Match status: Canceled" });
            break;
        }
        case XblMatchmakingStatus::Found:
        {
            HandleMatchFound(matchSession);
            break;
        }
        default:
            break;
    }
}

void
MultiplayerMatchClient::HandleInitializationStateChanged(
    _In_ std::shared_ptr<XblMultiplayerSession> matchSession
    )
{
    UpdateSession(matchSession);
    if (matchSession->InitializationInfo().Episode > 0)
    {
        switch (matchSession->InitializationInfo().Stage)
        {
            case XblMultiplayerInitializationStage::Joining:
            {
                CheckNextTimer();
                break;
            }
            case XblMultiplayerInitializationStage::Measuring:
            {
                if (m_matchStatus == XblMultiplayerMatchStatus::WaitingForRemoteClientsToUploadQos ||
                    m_matchStatus == XblMultiplayerMatchStatus::Measuring)
                {
                    CheckNextTimer();
                }
                else if (m_matchStatus == XblMultiplayerMatchStatus::WaitingForRemoteClientsToJoin)
                {
                    HandleQosMeasurements();
                }
                break;
            }
            case XblMultiplayerInitializationStage::Failed:
            {
                m_matchStatus = XblMultiplayerMatchStatus::Failed;
                HandleFindMatchCompleted({ xbl_error_code::generic_error, "Initialization failed" });
                return;
            }
                
            default:
                break;
        }
    }
    else
    {
        XblMultiplayerSessionReadLockGuard matchSessionSafe(matchSession);
        if (matchSessionSafe.CurrentUser()->InitializationFailureCause == XblMultiplayerMeasurementFailure::None)
        {
            // QoS succeeded.
            m_matchStatus = XblMultiplayerMatchStatus::Completed;
            HandleFindMatchCompleted({ xbl_error_code::no_error });
        }
        else
        {
            // Resubmit
            m_matchStatus = XblMultiplayerMatchStatus::Resubmitting;
            HandleFindMatchCompleted({ xbl_error_code::generic_error, "Measurement failure" });
        }
    }
}

HRESULT
MultiplayerMatchClient::FindMatch(
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ bool preserveSession
    )
{
    return FindMatch(m_hopperName, m_attributes, m_timeout, session, preserveSession);
}

HRESULT
MultiplayerMatchClient::FindMatch(
    _In_ const xsapi_internal_string& hopperName,
    _In_ JsonValue& attributes,
    _In_ const std::chrono::seconds& timeout,
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ bool preserveSession
    )
{
    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    RETURN_HR_IF_LOG_DEBUG(primaryContext == nullptr || session == nullptr, E_UNEXPECTED, "No local user added. Call add_local_user() first.");

    XblMultiplayerMatchStatus expected = XblMultiplayerMatchStatus::None;
    RETURN_HR_IF_LOG_DEBUG(!m_matchStatus.compare_exchange_strong(expected, XblMultiplayerMatchStatus::SubmittingMatchTicket, std::memory_order_release), E_UNEXPECTED, "Match search already in progress.");

    if (preserveSession)
    {
        UpdateSession(session);
    }
    else
    {
        UpdateSession(nullptr);
    }

    m_hopperName = hopperName;
    JsonUtils::CopyFrom(m_attributes, attributes);
    m_timeout = timeout;
    m_preservingMatchmakingSession = preserveSession;
    m_matchTicketSessionRef = session->SessionReference();

    std::weak_ptr<MultiplayerMatchClient> thisWeakPtr = shared_from_this();

    auto asyncBlock = utils::MakeAsyncBlock(
        m_queue.GetHandle(),
        utils::store_weak_ptr<MultiplayerMatchClient>(shared_from_this()),
        [](XAsyncBlock* asyncBlock)
    {
        auto pThis = utils::get_shared_ptr<MultiplayerMatchClient>(asyncBlock->context);
        if (pThis != nullptr)
        {
            XblCreateMatchTicketResponse result;
            auto hr = XblMatchmakingCreateMatchTicketResult(asyncBlock, &result);              
            if (SUCCEEDED(hr))
            {
                pThis->m_matchTicketResponse = result;
                pThis->m_matchStatus = XblMultiplayerMatchStatus::Searching;
                pThis->m_nextTimerToFetchSession = xbox::services::datetime::utc_now()
                   + xbox::services::datetime::from_seconds(static_cast<int32_t>(pThis ->m_timeout.count()))
                   + xbox::services::datetime::from_seconds(5);   // some extra delay to be safe
            }

            if (FAILED(hr))
            {
                pThis->m_matchStatus = XblMultiplayerMatchStatus::Failed;
                pThis->HandleFindMatchCompleted({ hr, "MatchTicketResult failed" });
            }
        }
        Delete(asyncBlock);
    });

    return primaryContext->MatchmakingService()->CreateMatchTicket(
        session->SessionReference(),
        session->SessionReference().Scid,
        hopperName,
        timeout,
        preserveSession ? XblPreserveSessionMode::Always : XblPreserveSessionMode::Never,
        attributes,
        asyncBlock);
}

void
MultiplayerMatchClient::CancelMatch()
{
    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    if (primaryContext == nullptr)
    {
         return;
    }

    if (m_matchStatus == XblMultiplayerMatchStatus::None || m_matchStatus == XblMultiplayerMatchStatus::Expired ||
        m_matchStatus == XblMultiplayerMatchStatus::Canceled || m_matchStatus == XblMultiplayerMatchStatus::Failed)
    {
        return;
    }

    if (m_hopperName.empty() && m_matchTicketResponse.matchTicketId[0] != '\0')
    {
        // Since we aren't the host who started the match, we cannot cancel it either.
        return;
    }

    m_matchStatus = XblMultiplayerMatchStatus::Canceling;
    auto asyncBlock = utils::MakeDefaultAsyncBlock(m_queue.GetHandle());
    primaryContext->MatchmakingService()->DeleteMatchTicketAsync(
        AppConfig::Instance()->Scid(),
        m_hopperName,
        m_matchTicketResponse.matchTicketId,
        asyncBlock
    );
}

XblMultiplayerMatchStatus
MultiplayerMatchClient::MatchStatus() const
{
    return m_matchStatus;
}

void
MultiplayerMatchClient::SetMatchStatus(
    _In_ XblMultiplayerMatchStatus status
    )
{
    m_matchStatus = status;
}

const std::chrono::seconds
MultiplayerMatchClient::EstimatedMatchWaitTime() const
{
    return std::chrono::seconds(m_matchTicketResponse.estimatedWaitTime);
}

void
MultiplayerMatchClient::OnSessionChanged(
    _In_ const XblMultiplayerSessionChangeEventArgs& args
    )
{
    auto matchSession = Session();
    if (matchSession != nullptr &&
        matchSession->SessionReference() == args.SessionReference &&
        args.ChangeNumber > matchSession->SessionInfo().ChangeNumber)
    {
        GetLatestSession();
    }
}

void 
MultiplayerMatchClient::UpdateSession(
    _In_ std::shared_ptr<XblMultiplayerSession> session
    )
{
    std::lock_guard<std::mutex> lock(m_lock);

    if (m_matchSession == nullptr || session == nullptr)
    {
        m_matchSession = session;
        m_matchTicketSessionRef = {};
    }
    else if(XblMultiplayerSession::DoSessionsMatch(m_matchSession, session) &&
        session->SessionInfo().ChangeNumber > m_matchSession->SessionInfo().ChangeNumber)
    {
        m_matchSession = session;
        m_nextTimerToFetchSession = xbox::services::datetime::utc_now() + xbox::services::datetime::from_seconds(static_cast<uint32_t>(session->SessionInfo().NextTimer - session->TimeOfSession()));
    }
}

std::shared_ptr<XblMultiplayerSession> 
MultiplayerMatchClient::Session()
{
    std::lock_guard<std::mutex> lock(m_lock);
    return m_matchSession;
}

void
MultiplayerMatchClient::HandleSessionJoined()
{
    if (FAILED(m_joinTargetSessionResult.Hresult()))
    {
        m_matchStatus = XblMultiplayerMatchStatus::Failed;
        HandleFindMatchCompleted({ m_joinTargetSessionResult.Hresult(), "JoinSession failed" });
        return;
    }

    if (Session()->InitializationInfo().Episode == 0)
    {
        m_matchStatus = XblMultiplayerMatchStatus::Completed;
        HandleFindMatchCompleted({ xbl_error_code::no_error });
    }
    else
    {
        m_matchStatus = XblMultiplayerMatchStatus::WaitingForRemoteClientsToJoin;
    }
}

void MultiplayerMatchClient::HandleMatchFound(
    _In_ std::shared_ptr<XblMultiplayerSession> currentSession
) noexcept
{
    std::shared_ptr<XblContext> primaryXboxLiveContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    if(primaryXboxLiveContext == nullptr)
    {
        m_matchStatus = XblMultiplayerMatchStatus::Failed;
        // No primary xbox live context
        HandleFindMatchCompleted({ xbl_error_code::runtime_error, "No primary xbox live context" });
        return;
    }

    m_matchStatus = XblMultiplayerMatchStatus::Found;
    auto& targetSessionRef = currentSession->MatchmakingServer()->TargetSessionRef;
    auto targetGameSession = MakeShared<XblMultiplayerSession>(
        primaryXboxLiveContext->Xuid(),
        &targetSessionRef,
        nullptr
        );

    auto state{ GlobalState::Get() };
    auto gameClient = state ? state->MultiplayerManager()->GameClient() : nullptr;
    if (m_preservingMatchmakingSession && gameClient != nullptr)
    {
        auto gameSession = gameClient->Session();
        if (gameSession != nullptr && gameSession->SessionReference() == targetSessionRef)
        {
            targetGameSession = gameSession;
        }
    }

    UpdateSession(targetGameSession);

    JoinSession(targetGameSession,
        [
            weakThis = std::weak_ptr<MultiplayerMatchClient>{ shared_from_this() }
        ]
    (Result<std::shared_ptr<XblMultiplayerSession>> result)
    {
        auto pThis = weakThis.lock();
        if (pThis)
        {
            // Perhaps its OK to call handle_session_joined() immediately here, but to maintain behavioral parody with
            // pplx code, differ that until the next do_work call
            pThis->m_joinTargetSessionResult = std::move(result);
            pThis->m_joinTargetSessionComplete = true;
        }
    });
}

HRESULT MultiplayerMatchClient::JoinSession(
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    // Join Match session for each local user.
    struct JoinSessionOperation : public std::enable_shared_from_this<JoinSessionOperation>
    {
        JoinSessionOperation(
            std::shared_ptr<MultiplayerMatchClient> matchClient,
            std::shared_ptr<XblMultiplayerSession> session,
            MultiplayerSessionCallback&& callback
        ) noexcept :
            m_matchClient{ std::move(matchClient) },
            m_latestSession{ std::move(session) },
            m_callback{ std::move(callback) }
        {
            for (auto& pair : m_matchClient->m_multiplayerLocalUserManager->GetLocalUserMap())
            {
                m_users.push_back(pair.second);
            }
        }

        void Run() noexcept
        {
            m_matchClient->m_matchStatus = XblMultiplayerMatchStatus::Joining;

            if (m_matchClient->m_preservingMatchmakingSession)
            {
                // Matchmaking session was preserved so we're already part of it
                Complete(m_latestSession);
            }
            else
            {
                JoinNextUser();
            }
        }

    private:
        void JoinNextUser() noexcept
        {
            if (m_users.empty())
            {
                Complete(m_latestSession);
            }
            else
            {
                auto user{ m_users.front() };
                m_users.pop_front();
                JoinSession(user);
            }
        }

        void JoinSession(std::shared_ptr<MultiplayerLocalUser> user)
        {
            auto userSession = MakeShared<XblMultiplayerSession>(user->Xuid(), &m_latestSession->SessionReference(), nullptr);
            userSession->Join();
            // Ok to use 'unsafe' method here since we have the only instance
            userSession->CurrentUserInternalUnsafe()->SetSecureDeviceBaseAddress64(user->ConnectionAddress());
            userSession->SetSessionChangeSubscription(XblMultiplayerSessionChangeTypes::Everything);

            HRESULT hr = user->Context()->MultiplayerService()->WriteSession(
                userSession,
                XblMultiplayerSessionWriteMode::UpdateOrCreateNew,
                AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>>{ m_matchClient->m_queue,
                [
                    op{ shared_from_this() }
                ]
            (Result<std::shared_ptr<XblMultiplayerSession>> writeSessionResult)
            {
                if (Failed(writeSessionResult))
                {
                    op->Complete(std::move(writeSessionResult));
                }
                else
                {
                    op->m_latestSession = writeSessionResult.ExtractPayload();
                    op->JoinNextUser();
                }
            }
            });

            if (FAILED(hr))
            {
                Complete(hr);
            }
        }

        void Complete(Result<std::shared_ptr<XblMultiplayerSession>>&& result) noexcept
        {
            m_matchClient->m_queue.RunCompletion([op{shared_from_this()}, result]
                {
                    op->m_matchClient->UpdateSession(result.Payload());
                    op->m_callback(std::move(result));
                });
        }

        std::shared_ptr<MultiplayerMatchClient> m_matchClient;
        std::shared_ptr<XblMultiplayerSession> m_latestSession;
        List<std::shared_ptr<MultiplayerLocalUser>> m_users;
        MultiplayerSessionCallback m_callback;
    };

    auto operation = MakeShared<JoinSessionOperation>(shared_from_this(), session, std::move(callback));
    operation->Run();
    return S_OK;
}

void
MultiplayerMatchClient::GetLatestSession()
{
    std::lock_guard<std::mutex> lock(m_getSessionLock);
    if (Session() == nullptr) return;

    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    if (primaryContext == nullptr || m_getSessionInProgress)
    {
        return;
    }

    std::weak_ptr<MultiplayerMatchClient> weakSessionWriter = shared_from_this();
    m_getSessionInProgress = true;
    primaryContext->MultiplayerService()->GetCurrentSession(Session()->SessionReference(), { m_queue,
    [weakSessionWriter](Result<std::shared_ptr<XblMultiplayerSession>> sessionResult)
    {
        if (SUCCEEDED(sessionResult.Hresult()))
        {
            std::shared_ptr<MultiplayerMatchClient> pThis(weakSessionWriter.lock());
            if (pThis != nullptr)
            {
                pThis->UpdateSession(sessionResult.Payload());
                pThis->m_getSessionInProgress = false;
            }
        }
    }
    });
}

void MultiplayerMatchClient::SetQosMeasurements(
    _In_ const JsonValue& measurements
) noexcept
{
    // Set QoS measurements for each local user.
    // Result delivered by updating the latest session and updating the match status.
    struct SetQosMeasurementsOperation : public std::enable_shared_from_this<SetQosMeasurementsOperation>
    {
        SetQosMeasurementsOperation(
            std::shared_ptr<MultiplayerMatchClient> matchClient,
            const JsonValue& measurements,
            const XblMultiplayerSessionReference& matchSessionRef
        ) noexcept :
            m_matchClient{ std::move(matchClient) },
            m_matchSessionRef{ matchSessionRef }
        {
            JsonUtils::CopyFrom(m_measurements, measurements);
            for (auto& pair : m_matchClient->m_multiplayerLocalUserManager->GetLocalUserMap())
            {
                m_users.push_back(pair.second);
            }
        }

        void Run() noexcept
        {
            m_matchClient->m_matchStatus = XblMultiplayerMatchStatus::UploadingQosMeasurements;
            SetMeasurementsForNextUser();
        }

    private:
        void SetMeasurementsForNextUser() noexcept
        {
            if (m_users.empty())
            {
                Complete(S_OK);
            }
            else
            {
                auto user{ m_users.front() };
                m_users.pop_front();

                auto session = MakeShared<XblMultiplayerSession>(user->Xuid(), &m_matchSessionRef, nullptr);
                session->Join();
                // Fine to use 'unsafe' method here since this is the only instance of the session
                session->CurrentUserInternalUnsafe()->SetQosMeasurementsJson(JsonUtils::SerializeJson(m_measurements));

                HRESULT hr = user->Context()->MultiplayerService()->WriteSession(
                    session,
                    XblMultiplayerSessionWriteMode::UpdateExisting,
                    AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>>{ m_matchClient->m_queue,
                    [
                        op{ shared_from_this() }
                    ]
                (Result<std::shared_ptr<XblMultiplayerSession>> result)
                {
                    if (Failed(result))
                    {
                        op->Complete(result);
                    }
                    else
                    {
                        op->m_latestSession = result.ExtractPayload();
                        op->SetMeasurementsForNextUser();
                    }
                }
                });

                if (FAILED(hr))
                {
                    Complete(hr);
                }
            }
        }

        void Complete(Result<void>&& result)
        {
            LOGS_DEBUG << __FUNCTION__ << ": HRESULT=" << result.Hresult() << ", ErrorMessage=" << result.ErrorMessage();

            m_matchClient->UpdateSession(m_latestSession);
            m_matchClient->m_matchStatus = XblMultiplayerMatchStatus::WaitingForRemoteClientsToUploadQos;
        }

        std::shared_ptr<MultiplayerMatchClient> m_matchClient;
        JsonDocument m_measurements;
        std::shared_ptr<XblMultiplayerSession> m_latestSession;
        List<std::shared_ptr<MultiplayerLocalUser>> m_users;
        XblMultiplayerSessionReference m_matchSessionRef;
    };

    if (auto matchSession{ Session() })
    {
        auto operation = MakeShared<SetQosMeasurementsOperation>(shared_from_this(), measurements, matchSession->SessionReference());
        operation->Run();
    }
}

void
MultiplayerMatchClient::ResubmitMatchmaking(
    _In_ std::shared_ptr<XblMultiplayerSession> session
    )
{
    if (session == nullptr)
    {
        return;
    }

    std::shared_ptr<XblContext> primaryContext = m_multiplayerLocalUserManager->GetPrimaryContext();
    if (primaryContext == nullptr)
    {
        return;
    }

    m_matchStatus = XblMultiplayerMatchStatus::Resubmitting;
    session->SetMatchmakingResubmit(true);
    std::weak_ptr<MultiplayerMatchClient> weakSessionWriter = shared_from_this();
    primaryContext->MultiplayerService()->WriteSession(session, XblMultiplayerSessionWriteMode::UpdateExisting, { m_queue,
    [weakSessionWriter](Result<std::shared_ptr<XblMultiplayerSession>> sessionResult)
    {
        if (FAILED(sessionResult.Hresult()))
        {
            std::shared_ptr<MultiplayerMatchClient> pThis(weakSessionWriter.lock());
            if (pThis != nullptr)
            {
                pThis->m_matchStatus = XblMultiplayerMatchStatus::Failed;
                pThis->HandleFindMatchCompleted({ sessionResult.Hresult(), "Resubmit failed" });
            }
        }
    }
    });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END

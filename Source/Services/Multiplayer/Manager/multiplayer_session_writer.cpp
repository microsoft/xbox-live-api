// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::system;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK
#define TIME_PER_CALL_MS (1 * 1000)
#elif XSAPI_UNIT_TESTS
#define TIME_PER_CALL_MS (30 * 1000)
#endif

MultiplayerSessionWriter::MultiplayerSessionWriter(
    const TaskQueue& queue
) noexcept :
    m_queue{ queue.DeriveWorkerQueue() }
{
}

MultiplayerSessionWriter::MultiplayerSessionWriter(
    const TaskQueue& queue,
    _In_ std::shared_ptr<MultiplayerLocalUserManager> localUserManager
) noexcept :
    m_queue{ queue.DeriveWorkerQueue() },
    m_multiplayerLocalUserManager{ std::move(localUserManager) }
{
}

void
MultiplayerSessionWriter::Destroy()
{
    m_id++;
    m_session = nullptr;
    m_isTapReceived = false;
    m_numOfWritesInProgress = 0;
    m_tapChangeNumber = 0;
}

std::shared_ptr<XblContext>
MultiplayerSessionWriter::GetPrimaryContext()
{
    return m_multiplayerLocalUserManager->GetPrimaryContext();
}

uint64_t
MultiplayerSessionWriter::Id() const
{
    return m_id;
}

const std::shared_ptr<XblMultiplayerSession>&
MultiplayerSessionWriter::Session() const
{
    return m_session;
}

void
MultiplayerSessionWriter::UpdateSession(
    _In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession
    )
{
    if (updatedSession == nullptr)
    {
        Destroy();
    }
    else
    {
        m_session = updatedSession;
    }
}

XblFunctionContext
MultiplayerSessionWriter::AddMultiplayerSessionUpdatedHandler(
    _In_ Callback<const std::shared_ptr<XblMultiplayerSession>&> handler
    )
{
    std::lock_guard<std::mutex> lock(m_stateLock);

    XblFunctionContext context = 0;
    if (handler != nullptr)
    {
		context = m_sessionUpdateEventHandlerCounter++;
        m_sessionUpdateEventHandler[m_sessionUpdateEventHandlerCounter] = std::move(handler);
    }

    return context;
}

void
MultiplayerSessionWriter::OnResyncMessageReceived()
{
    m_handleResyncEventCounter++;
    Resync();
}

void
MultiplayerSessionWriter::OnSessionUpdated(
    _In_ const std::shared_ptr<XblMultiplayerSession>& updatedSession
    )
{
    std::lock_guard<std::mutex> lock(m_stateLock);

    for (const auto& handler : m_sessionUpdateEventHandler)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(updatedSession);
            }
            catch (...)
            {
                LOG_ERROR("MultiplayerSessionWriter::on_session_updated call threw an exception");
            }
        }
    }
}

bool
MultiplayerSessionWriter::IsTapReceived() const
{
    return m_isTapReceived;
}

void
MultiplayerSessionWriter::SetTapReceived(
    _In_ bool bReceived
    )
{
    m_isTapReceived = bReceived;
}

bool
MultiplayerSessionWriter::IsWriteInProgress() const
{
    return m_numOfWritesInProgress > 0;
}

void
MultiplayerSessionWriter::SetWriteInProgress(
    _In_ bool writeInProgress
    )
{
    if (writeInProgress)
    {
        m_numOfWritesInProgress++;
    }
    else
    {
        m_numOfWritesInProgress--;
    }
}

uint64_t
MultiplayerSessionWriter::TapChangeNumber() const
{
    return m_tapChangeNumber;
}

void
MultiplayerSessionWriter::SetTapChangeNumber(
    _In_ uint64_t changeNumber
    )
{
    m_tapChangeNumber = changeNumber;
}

HRESULT MultiplayerSessionWriter::CommitSynchronizedChanges(
    _In_ std::shared_ptr<XblMultiplayerSession> sessionToCommit,
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    // Retrieve the latest session and write the pending commit changes to it.
    return WriteSession(
        m_multiplayerLocalUserManager->GetPrimaryContext(),
        sessionToCommit,
        XblMultiplayerSessionWriteMode::SynchronizedUpdate,
        true,
        std::move(callback)
    );
}

HRESULT MultiplayerSessionWriter::CommitPendingSynchronizedChanges(
    _In_ Vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue,
    _In_ XblMultiplayerSessionType sessionType,
    _In_ MultiplayerEventQueueCallback callback
) noexcept
{
    if (m_session == nullptr)
    {
        auto eventQueue = HandleEvents(processingQueue, { E_FAIL, "Session null" }, sessionType);
        // TODO this may need to be invoked on another thread
        callback(Result<MultiplayerEventQueue>(eventQueue, xbl_error_code::generic_error));
        return S_OK;
    }

    auto sessionToCommitCopy = MakeShared<XblMultiplayerSession>(*m_session);

    // Update any pending local user or lobby session properties.
    for (auto& request : processingQueue)
    {
        request->AppendPendingChanges(sessionToCommitCopy, nullptr);
    }

    return WriteSession(GetPrimaryContext(), sessionToCommitCopy, XblMultiplayerSessionWriteMode::SynchronizedUpdate, true,
        [
            weakThis = std::weak_ptr<MultiplayerSessionWriter>{ shared_from_this() },
            processingQueue,
            sessionType,
            callback
        ]
    (Result<std::shared_ptr<XblMultiplayerSession>> sessionResult)
    {
        MultiplayerEventQueue eventQueue;
        auto sharedThis{ weakThis.lock() };
        if (sharedThis)
        {
            std::lock_guard<std::mutex> lock(sharedThis->m_stateLock);
            eventQueue = sharedThis->HandleEvents(processingQueue, sessionResult, sessionType);
        }
        callback(Result<MultiplayerEventQueue>(eventQueue, sessionResult.Hresult()));
    });
}

HRESULT MultiplayerSessionWriter::CommitPendingChanges(
    _In_ Vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue,
    _In_ XblMultiplayerSessionType sessionType,
    _In_ bool isGameInProgress,
    _In_ MultiplayerEventQueueCallback callback
) noexcept
{
    if (m_session == nullptr)
    {
        auto eventQueue = HandleEvents(processingQueue, { E_FAIL, "Session is null" }, sessionType);
        // TODO this may need to be invoked on another thread
        callback(Result<MultiplayerEventQueue>(eventQueue, xbl_error_code::generic_error, "Session is null"));
        return S_OK;
    }

    std::shared_ptr<XblMultiplayerSession> sessionToCommitCopy = MakeShared<XblMultiplayerSession>(*m_session);

    // Update any pending local user or lobby session properties.
    for (auto& request : processingQueue)
    {
        request->AppendPendingChanges(sessionToCommitCopy, nullptr, isGameInProgress);
    }

    return WriteSession(m_multiplayerLocalUserManager->GetPrimaryContext(), sessionToCommitCopy, XblMultiplayerSessionWriteMode::UpdateExisting, true,
        [
            weakThis = std::weak_ptr<MultiplayerSessionWriter>{ shared_from_this() },
            processingQueue,
            sessionType,
            callback
        ]
    (Result<std::shared_ptr<XblMultiplayerSession>> sessionResult)
    {
        MultiplayerEventQueue eventQueue;
        auto sharedThis{ weakThis.lock() };
        if (sharedThis)
        {
            std::lock_guard<std::mutex> lock(sharedThis->m_stateLock);
            eventQueue = sharedThis->HandleEvents(processingQueue, sessionResult, sessionType);
        }
        callback(Result<MultiplayerEventQueue>(eventQueue, sessionResult.Hresult(), sessionResult.ErrorMessage()));
    });
}

HRESULT MultiplayerSessionWriter::WriteSession(
    _In_ std::shared_ptr<XblContext> xboxLiveContext,
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ XblMultiplayerSessionWriteMode mode,
    _In_ bool updateLatest,
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    RETURN_HR_IF_LOG_DEBUG(xboxLiveContext == nullptr, E_UNEXPECTED, "Call add_local_user() first.");

    auto hr = xboxLiveContext->MultiplayerService()->WriteSession(
        session,
        mode,
        AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>>{ m_queue,
        [
            weakThis = std::weak_ptr<MultiplayerSessionWriter>{ shared_from_this() },
            sessionWriterId = m_id,
            updateLatest,
            callback
        ]
    (Result<std::shared_ptr<XblMultiplayerSession>> sessionResult)
    {
        if (auto sharedThis{ weakThis.lock() })
        {
            if (sharedThis->Id() != sessionWriterId)
            {
                callback({ E_FAIL, "Session writer has been reset" });
            }
            else
            {
                sharedThis->HandleWriteSessionResult(sessionResult, updateLatest, callback);
            }
        }
        else
        {
            callback({ E_FAIL, "Session writer is null" });
        }
    }
    });

    if (SUCCEEDED(hr))
    {
        SetWriteInProgress(true);
    }
    return hr;
}

HRESULT MultiplayerSessionWriter::WriteSessionByHandle(
    _In_ std::shared_ptr<XblContext> xboxLiveContext,
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ XblMultiplayerSessionWriteMode mode,
    _In_ const String& handleId,
    _In_ bool updateLatest,
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    RETURN_HR_IF_LOG_DEBUG(xboxLiveContext == nullptr, E_UNEXPECTED, "Call add_local_user() first.");

    auto hr = xboxLiveContext->MultiplayerService()->WriteSessionByHandle(
        session,
        mode,
        handleId,
        AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>>{ m_queue,
        [
            weakThis = std::weak_ptr<MultiplayerSessionWriter>{ shared_from_this() },
            sessionWriterId = m_id,
            updateLatest,
            callback
        ]
    (Result<std::shared_ptr<XblMultiplayerSession>> sessionResult)
    {
        if (auto sharedThis{ weakThis.lock() })
        {
            if (sharedThis->Id() != sessionWriterId)
            {
                callback({ E_FAIL, "Session writer has been reset" });
            }
            else
            {
                sharedThis->HandleWriteSessionResult(sessionResult, updateLatest, callback);
            }
        }
        else
        {
            callback({ E_FAIL, "Session writer is null" });
        }
    }
    });

    if (SUCCEEDED(hr))
    {
        SetWriteInProgress(true);
    }
    return hr;
}

void MultiplayerSessionWriter::HandleWriteSessionResult(
    _In_ Result<std::shared_ptr<XblMultiplayerSession>> writeSessionResult,
    _In_ bool updateLatest,
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    std::lock_guard<std::mutex> guard{ m_synchronizeWriteWithTapLock };

    if (Succeeded(writeSessionResult) || writeSessionResult.Hresult() == HTTP_E_STATUS_PRECOND_FAILED)
    {
        if (updateLatest)
        {
            OnSessionUpdated(writeSessionResult.Payload());
        }
    }

    SetWriteInProgress(false);
    if (IsTapReceived())
    {
        SetTapReceived(false);
        auto latestSession = m_session;  // always check against the latest session().
        if(updateLatest && latestSession != nullptr && latestSession->SessionInfo().ChangeNumber < TapChangeNumber())
        {
            GetCurrentSessionHelper(m_multiplayerLocalUserManager->GetPrimaryContext(), latestSession->SessionReference(), callback);
        }
        else
        {
            callback(writeSessionResult);
        }
    }
    else
    {
        callback(writeSessionResult);
    }
}

void MultiplayerSessionWriter::OnSessionChanged(
    _In_ XblMultiplayerSessionChangeEventArgs args
) noexcept
{
    std::lock_guard<std::mutex> guard(m_synchronizeWriteWithTapLock);

    if (IsWriteInProgress())
    {
        if (args.ChangeNumber > TapChangeNumber())
        {
            SetTapReceived(true);
            SetTapChangeNumber(args.ChangeNumber);
        }
    }
    else
    {
        auto latestSession = Session();
        if(latestSession != nullptr && args.ChangeNumber > latestSession->SessionInfo().ChangeNumber)
        {
            GetCurrentSessionHelper(m_multiplayerLocalUserManager->GetPrimaryContext(), latestSession->SessionReference(), nullptr);
        }
    }
}

HRESULT MultiplayerSessionWriter::GetCurrentSessionHelper(
    _In_ std::shared_ptr<XblContext> xboxLiveContext,
    _In_ const XblMultiplayerSessionReference& sessionReference,
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    RETURN_HR_IF_LOG_DEBUG(xboxLiveContext == nullptr, E_UNEXPECTED, "Call add_local_user() first.");

    return xboxLiveContext->MultiplayerService()->GetCurrentSession(
        sessionReference,
        AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>>{ m_queue,
        [
            weakThis = std::weak_ptr<MultiplayerSessionWriter>{ shared_from_this() },
            callback
        ]
    (Result<std::shared_ptr<XblMultiplayerSession>> sessionResult)
    {
        if (auto sharedThis{ weakThis.lock() })
        {
            if (Succeeded(sessionResult) || sessionResult.Hresult() == HTTP_E_STATUS_PRECOND_FAILED)
            {
                sharedThis->OnSessionUpdated(sessionResult.Payload());
            }
            callback(sessionResult);
        }
        else
        {
            callback({E_FAIL, "Session writer is null"});
        }
    }
    });
}

void MultiplayerSessionWriter::Resync()
{
#if HC_PLATFORM == HC_PLATFORM_UWP || HC_PLATFORM == HC_PLATFORM_XDK || XSAPI_UNIT_TESTS
    std::lock_guard<std::mutex> lock(m_resyncLock);

    auto cachedSession = Session();
    if (cachedSession != nullptr && !m_isResyncTaskInProgress && m_handleResyncEventCounter > 0)
    {
        m_isResyncTaskInProgress = true;
        m_handleResyncEventCounter = 0;
        XblMultiplayerSessionChangeEventArgs changeEventArgs{ cachedSession->SessionReference(), "", cachedSession->SessionInfo().ChangeNumber + 1 };
        OnSessionChanged(changeEventArgs);

        // You could get multiple resync events. To avoid fetching the session too often, only check back after TIME_PER_CALL_MS secs
        m_queue.RunWork([ weakThis = std::weak_ptr<MultiplayerSessionWriter>{ shared_from_this() }, this ]
            {
                if (auto sharedThis{ weakThis.lock() })
                {
                    std::unique_lock<std::mutex> lock{ m_resyncLock };
                    m_isResyncTaskInProgress = false;
                    lock.unlock();

                    Resync();
                }
            }, TIME_PER_CALL_MS);
    }
#else
    UNREFERENCED_PARAMETER(m_isResyncTaskInProgress);
#endif
}

HRESULT MultiplayerSessionWriter::LeaveRemoteSession(
    _In_ std::shared_ptr<XblMultiplayerSession> session,
    _In_ MultiplayerSessionCallback callback
) noexcept
{
    RETURN_HR_IF_LOG_DEBUG(session == nullptr, E_UNEXPECTED, "Session is null");

    // LeaveSessionOperation will leave the provided session for each local user.
    // Because a User must leave the session on their own, this requires an MPSD call for each local user.
    // To be consistent with 1806 XDK behavior, if any of these MPSD calls fail, the rest of the operation
    // will be aborted and that failure will be returned.
    struct LeaveSessionOperation : public std::enable_shared_from_this<LeaveSessionOperation>
    {
        LeaveSessionOperation(
            std::shared_ptr<MultiplayerSessionWriter> sessionWriter,
            const XblMultiplayerSessionReference& sessionRefToLeave,
            TaskQueue& queue,
            MultiplayerSessionCallback&& callback
        ) noexcept :
            m_sessionWriter{ std::move(sessionWriter) },
            m_sessionToLeaveRef{ sessionRefToLeave },
            m_queue{ queue },
            m_localUsers{ m_sessionWriter->m_multiplayerLocalUserManager->GetLocalUserMap() },
            m_callback{ std::move(callback) }
        {
        }

        void Run() noexcept
        {
            if (m_localUsers.empty())
            {
                // Nothing left to do, return latest session
                m_callback(m_latestSession);
            }
            else
            {
                auto userContext{ m_localUsers.begin()->second->Context() };
                m_localUsers.erase(m_localUsers.begin());
                HRESULT hr = LeaveSession(userContext);
                if (FAILED(hr))
                {
                    m_callback(hr);
                }
            }
        }

    private:
        HRESULT LeaveSession(std::shared_ptr<XblContext> userContext) noexcept
        {
            auto sessionToCommit = MakeShared<XblMultiplayerSession>(userContext->Xuid(), &m_sessionToLeaveRef, nullptr);
            RETURN_HR_IF_FAILED(sessionToCommit->Leave());

            // Never update latest copy upon leaving.

            return userContext->MultiplayerService()->WriteSession(
                sessionToCommit,
                XblMultiplayerSessionWriteMode::UpdateExisting,
                AsyncContext<Result<std::shared_ptr<XblMultiplayerSession>>>{ m_queue,
                [
                    sharedThis{ shared_from_this() }, this
                ]
            (Result<std::shared_ptr<XblMultiplayerSession>> writeSessionResult)
            {
                if (Failed(writeSessionResult))
                {
                    m_callback(writeSessionResult);
                }
                else
                {
                    m_latestSession = writeSessionResult.ExtractPayload();
                    sharedThis->Run();
                }
            }});
        }

        std::shared_ptr<MultiplayerSessionWriter> m_sessionWriter;
        XblMultiplayerSessionReference m_sessionToLeaveRef;
        TaskQueue m_queue;
        Map<uint64_t, std::shared_ptr<MultiplayerLocalUser>> m_localUsers;
        std::shared_ptr<XblMultiplayerSession> m_latestSession{ nullptr };
        MultiplayerSessionCallback m_callback;
    };

    auto operation = MakeShared<LeaveSessionOperation>(
        shared_from_this(),
        session->SessionReference(),
        m_queue,
        std::move(callback)
    );

    operation->Run();
    return S_OK;
}

MultiplayerEventQueue
MultiplayerSessionWriter::HandleEvents(
    _In_ xsapi_internal_vector<std::shared_ptr<MultiplayerClientPendingRequest>> processingQueue,
    _In_ Result<void> error,
    _In_ XblMultiplayerSessionType sessionType
    )
{
    MultiplayerEventQueue eventQueue;
    for (auto& request : processingQueue)
    {
        if (request->Joinability() != XblMultiplayerJoinability::None)
        {
            eventQueue.AddEvent(
                XblMultiplayerEventType::JoinabilityStateChanged,
                sessionType,
                nullptr,
                error,
                request->Context()
            );
        }

        if (request->SessionProperties().size() > 0)
        {
            // Fire events for each of the properties
            for (auto iter = request->SessionProperties().begin(); iter != request->SessionProperties().end(); ++iter)
            {
                eventQueue.AddEvent(
                    XblMultiplayerEventType::SessionPropertyWriteCompleted,
                    sessionType,
                    nullptr,
                    error,
                    request->Context()
                );
            }
        }

        if (!request->SynchronizedHostDeviceToken().empty())
        {
            eventQueue.AddEvent(
                XblMultiplayerEventType::SynchronizedHostWriteCompleted,
                sessionType,
                MakeShared<XblMultiplayerEventArgs>(),
                error,
                request->Context()
            );
        }

        if (request->SynchronizedSessionProperties().size() > 0)
        {
            // Fire events for each of the properties
            for (auto iter = request->SynchronizedSessionProperties().begin(); iter != request->SynchronizedSessionProperties().end(); ++iter)
            {
                eventQueue.AddEvent(
                    XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted,
                    sessionType,
                    nullptr,
                    error,
                    request->Context()
                );
            }
        }
    }
    return eventQueue;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END

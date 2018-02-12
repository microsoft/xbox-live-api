// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"
#if !XSAPI_U
#include "ppltasks_extra.h"
using namespace Concurrency::extras;
#endif

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

const std::chrono::milliseconds TIME_PER_CALL_MS =
#if UNIT_TEST_SERVICES
std::chrono::milliseconds(1 * 1000);
#else
std::chrono::milliseconds(30 * 1000);
#endif

multiplayer_session_writer::multiplayer_session_writer() :
    m_isTapReceived(false),
    m_numOfWritesInProgress(0),
    m_tapChangeNumber(0),
    m_sessionUpdateEventHandlerCounter(0),
    m_handleResyncEventCounter(0),
    m_isTaskInProgress(false)
{
}

multiplayer_session_writer::multiplayer_session_writer(
    _In_ std::shared_ptr<multiplayer_local_user_manager> localUserManager
    ) :
    m_multiplayerLocalUserManager(localUserManager),
    m_isTapReceived(false),
    m_numOfWritesInProgress(0),
    m_tapChangeNumber(0),
    m_sessionUpdateEventHandlerCounter(0),
    m_handleResyncEventCounter(0),
    m_isTaskInProgress(false)
{
}

void
multiplayer_session_writer::destroy()
{
    m_session = nullptr;
    m_isTapReceived = false;
    m_numOfWritesInProgress = 0;
    m_tapChangeNumber = 0;
}

std::shared_ptr<xbox_live_context_impl>
multiplayer_session_writer::get_primary_context()
{
    return m_multiplayerLocalUserManager->get_primary_context();
}

const std::shared_ptr<multiplayer_session>&
multiplayer_session_writer::session() const
{
    return m_session;
}

void
multiplayer_session_writer::update_session(
    _In_ const std::shared_ptr<multiplayer_session>& updatedSession
    )
{
    if (updatedSession == nullptr)
    {
        destroy();
    }
    else
    {
        m_session = updatedSession;
    }
}

function_context
multiplayer_session_writer::add_multiplayer_session_updated_handler(
    _In_ std::function<void(const std::shared_ptr<multiplayer_session>& )> handler
    )
{
    std::lock_guard<std::mutex> lock(m_stateLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_sessionUpdateEventHandlerCounter;
        m_sessionUpdateEventHandler[m_sessionUpdateEventHandlerCounter] = std::move(handler);
    }

    return context;
}

void
multiplayer_session_writer::on_resync_message_received()
{
    m_handleResyncEventCounter++;
    resync();
}

void
multiplayer_session_writer::on_session_updated(
    _In_ const std::shared_ptr<multiplayer_session>& updatedSession
    )
{
    std::lock_guard<std::mutex> lock(m_stateLock.get());

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
                LOG_ERROR("multiplayer_session_writer::on_session_updated call threw an exception");
            }
        }
    }
}

bool
multiplayer_session_writer::is_tap_received() const
{
    return m_isTapReceived;
}

void
multiplayer_session_writer::set_tap_received(
    _In_ bool bReceived
    )
{
    m_isTapReceived = bReceived;
}

bool
multiplayer_session_writer::is_write_in_progress() const
{
    return m_numOfWritesInProgress > 0;
}

void
multiplayer_session_writer::set_write_in_progress(
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
multiplayer_session_writer::tap_change_number() const
{
    return m_tapChangeNumber;
}

void
multiplayer_session_writer::set_tap_change_number(
    _In_ uint64_t changeNumber
    )
{
    m_tapChangeNumber = changeNumber;
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_session_writer::commit_synchronized_changes(
    _In_ std::shared_ptr<multiplayer_session> sessionToCommit
    )
{
    // Retrieve the latest session and write the pending commit changes to it.
    auto task = write_session(m_multiplayerLocalUserManager->get_primary_context(), sessionToCommit, multiplayer_session_write_mode::synchronized_update)
    .then([](xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult)
    {
        return sessionResult;
    });

    return utils::create_exception_free_task<std::shared_ptr<multiplayer_session>>(task);
}

pplx::task<xbox_live_result<std::vector<multiplayer_event>>>
multiplayer_session_writer::commit_pending_synchronized_changes(
    _In_ std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue,
    _In_ multiplayer_session_type sessionType
    )
{
    if (m_session == nullptr)
    {
        auto eventQueue = handle_events(processingQueue, xbox_live_error_code::generic_error, "Session no longer exists.", sessionType);
        return pplx::task_from_result(xbox_live_result<std::vector<multiplayer_event>>(eventQueue, xbox_live_error_code::generic_error, "Session no longer exists."));
    }
    
    const auto& sessionToCommitCopy = m_session->_Create_deep_copy();

    // Update any pending local user or lobby session properties.
    for (auto& request : processingQueue)
    {
        request->append_pending_changes(sessionToCommitCopy, nullptr);
    }

    std::weak_ptr<multiplayer_session_writer> thisWeakPtr = shared_from_this();
    auto task = write_session(get_primary_context(), sessionToCommitCopy, multiplayer_session_write_mode::synchronized_update)
    .then([thisWeakPtr, processingQueue, sessionType](xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult)
    {
        std::shared_ptr<multiplayer_session_writer> pThis(thisWeakPtr.lock());
        std::vector<multiplayer_event> eventQueue;
        if (pThis != nullptr)
        {
            std::lock_guard<std::mutex> lock(pThis->m_stateLock.get());
            eventQueue = pThis->handle_events(processingQueue, sessionResult.err(), sessionResult.err_message(), sessionType);
        }
        return xbox_live_result<std::vector<multiplayer_event>>(eventQueue, sessionResult.err(), sessionResult.err_message());
    });

    return utils::create_exception_free_task<std::vector<multiplayer_event>>(task);
}

pplx::task<xbox_live_result<std::vector<multiplayer_event>>>
multiplayer_session_writer::commit_pending_changes(
    _In_ std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue,
    _In_ multiplayer_session_type sessionType,
    _In_ bool isGameInProgress /*= false */
    )
{
    if (m_session == nullptr)
    {
        auto eventQueue = handle_events(processingQueue, xbox_live_error_code::generic_error, "Session no longer exists.", sessionType);
        return pplx::task_from_result(xbox_live_result<std::vector<multiplayer_event>>(eventQueue, xbox_live_error_code::generic_error, "Session no longer exists."));
    }

    std::shared_ptr<multiplayer_session> sessionToCommit = m_session->_Create_deep_copy();

    // Update any pending local user or lobby session properties.
    for (auto& request : processingQueue)
    {
        request->append_pending_changes(sessionToCommit, nullptr, isGameInProgress);
    }

    std::weak_ptr<multiplayer_session_writer> thisWeakPtr = shared_from_this();
    auto task = write_session(m_multiplayerLocalUserManager->get_primary_context(), sessionToCommit, multiplayer_session_write_mode::update_existing)
    .then([thisWeakPtr, processingQueue, sessionType](xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult)
    {
        std::shared_ptr<multiplayer_session_writer> pThis(thisWeakPtr.lock());
        std::vector<multiplayer_event> eventQueue;
        if (pThis != nullptr)
        {
            std::lock_guard<std::mutex> lock(pThis->m_stateLock.get());
            eventQueue = pThis->handle_events(processingQueue, sessionResult.err(), sessionResult.err_message(), sessionType);
        }
        return xbox_live_result<std::vector<multiplayer_event>>(eventQueue, sessionResult.err(), sessionResult.err_message());
    });

    return utils::create_exception_free_task<std::vector<multiplayer_event>>(task);
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_session_writer::write_session(
    _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContext,
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ multiplayer_session_write_mode mode,
    _In_ bool updateLatest
    )
{
    RETURN_TASK_CPP_IF(xboxLiveContext == nullptr, std::shared_ptr<multiplayer_session>, "Call add_local_user() first.");

    set_write_in_progress(true);
    std::weak_ptr<multiplayer_session_writer> thisWeakPtr = shared_from_this();
    return xboxLiveContext->multiplayer_service().write_session(session, mode)
    .then([thisWeakPtr, updateLatest](xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult)
    {
        std::shared_ptr<multiplayer_session_writer> pThis(thisWeakPtr.lock());
        RETURN_CPP_IF(pThis == nullptr, std::shared_ptr<multiplayer_session>, xbox_live_error_code::generic_error, "multiplayer_session_writer class was destroyed.");

        return pThis->write_session_helper(sessionResult, updateLatest);
    });
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_session_writer::write_session_by_handle(
    _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContext,
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ multiplayer_session_write_mode mode,
    _In_ const string_t& handleId,
    _In_ bool updateLatest
    )
{
    RETURN_TASK_CPP_IF(xboxLiveContext == nullptr, std::shared_ptr<multiplayer_session>, "Call add_local_user() first.");

    set_write_in_progress(true);
    std::weak_ptr<multiplayer_session_writer> thisWeakPtr = shared_from_this();
    return xboxLiveContext->multiplayer_service().write_session_by_handle(session, mode, handleId)
    .then([thisWeakPtr, updateLatest](xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult)
    {
        std::shared_ptr<multiplayer_session_writer> pThis(thisWeakPtr.lock());
        RETURN_CPP_IF(pThis == nullptr, std::shared_ptr<multiplayer_session>, xbox_live_error_code::generic_error, "multiplayer_session_writer class was destroyed.");

        return pThis->write_session_helper(sessionResult, updateLatest);
    });
}

xbox_live_result<std::shared_ptr<multiplayer_session>>
multiplayer_session_writer::write_session_helper(
    _In_ xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult,
    _In_ bool updateLatest
    )
{
    std::lock_guard<std::mutex> guard(m_synchronizeWriteWithTapLock);

    xbox_live_result<std::shared_ptr<multiplayer_session>> xboxLiveResult = sessionResult;
    if (!xboxLiveResult.err() || xboxLiveResult.err() == xbox_live_error_condition::http_412_precondition_failed)
    {
        auto latestSession = xboxLiveResult.payload();
        if (updateLatest)
        {
            on_session_updated(latestSession);
        }
    }

    set_write_in_progress(false);
    if (is_tap_received())
    {
        set_tap_received(false);
        auto latestSession = session();  // always check against the latest session().
        if(updateLatest && latestSession != nullptr && latestSession->change_number() < tap_change_number())
        {
            return get_current_session_helper(m_multiplayerLocalUserManager->get_primary_context(), latestSession->session_reference()).get();
        }
    }

    return xboxLiveResult;
}

void
multiplayer_session_writer::on_session_changed(
    _In_ const multiplayer_session_change_event_args& args
    )
{
    std::lock_guard<std::mutex> guard(m_synchronizeWriteWithTapLock);

    multiplayer_session_reference sessionRef = args.session_reference();
    uint64_t argsChangeNumber = args.change_number();
    if (is_write_in_progress())
    {
        if (argsChangeNumber > tap_change_number())
        {
            set_tap_received(true);
            set_tap_change_number(argsChangeNumber);
        }
    }
    else
    {
        auto latestSession = session();
        if(latestSession != nullptr && argsChangeNumber > latestSession->change_number())
        {
            get_current_session_helper(m_multiplayerLocalUserManager->get_primary_context(), latestSession->session_reference());
        }
    }
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_session_writer::get_current_session_helper(
    _In_ std::shared_ptr<xbox::services::xbox_live_context_impl> xboxLiveContext,
    _In_ const multiplayer_session_reference& sessionReference
    )
{
    RETURN_TASK_CPP_IF(xboxLiveContext == nullptr, std::shared_ptr<multiplayer_session>, "Call add_local_user() first.");

    std::weak_ptr<multiplayer_session_writer> thisWeakPtr = shared_from_this();
    return xboxLiveContext->multiplayer_service().get_current_session(sessionReference)
    .then([thisWeakPtr, sessionReference](xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult)
    {
        if (!sessionResult.err() || sessionResult.err() == xbox_live_error_code::http_status_412_precondition_failed)
        {
            std::shared_ptr<multiplayer_session_writer> pThis(thisWeakPtr.lock());
            RETURN_CPP_IF(pThis == nullptr, std::shared_ptr<multiplayer_session>, xbox_live_error_code::generic_error, "multiplayer_session_writer class was destroyed.");

            pThis->on_session_updated(sessionResult.payload());
        }

        return sessionResult;
    });
}

void
multiplayer_session_writer::resync()
{
#if UWP_API || TV_API || UNIT_TEST_SERVICES
    std::lock_guard<std::mutex> lock(m_resyncLock.get());

    auto cachedSession = session();
    if (cachedSession != nullptr && !m_isTaskInProgress && m_handleResyncEventCounter > 0)
    {
        m_isTaskInProgress = true;
        m_handleResyncEventCounter = 0;
        multiplayer_session_change_event_args changeEventArgs(cachedSession->session_reference(), string_t(), cachedSession->change_number() + 1);
        on_session_changed(changeEventArgs);

        // You could get multiple resync events. To avoid fetching the session too often, check back after TIME_PER_CALL_MS secs
        std::weak_ptr<multiplayer_session_writer> thisWeakPtr = shared_from_this();
        create_delayed_task(
            TIME_PER_CALL_MS,
            [thisWeakPtr]()
        {
            std::shared_ptr<multiplayer_session_writer> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                std::lock_guard<std::mutex> lock(pThis->m_resyncLock.get());
                pThis->m_isTaskInProgress = false;

                // Call resync from another task to avoid m_resyncLock from deadlocking.
                pplx::create_task([thisWeakPtr]()
                {
                    std::shared_ptr<multiplayer_session_writer> pThis2(thisWeakPtr.lock());
                    if (pThis2 != nullptr)
                    {
                        pThis2->resync();
                    }
                });
            }
        });
    }
#endif
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_session_writer::leave_remote_session(
    _In_ std::shared_ptr<multiplayer_session> session
    )
{
    RETURN_TASK_CPP_IF(session == nullptr, std::shared_ptr<multiplayer_session>, "Session is null");

    auto sessionRefToCommit = session->session_reference();
    std::weak_ptr<multiplayer_session_writer> thisWeakPtr = shared_from_this();
    auto task = pplx::create_task([thisWeakPtr, sessionRefToCommit]()
    {
        std::shared_ptr<multiplayer_session_writer> pThis(thisWeakPtr.lock());
        RETURN_CPP_IF(pThis == nullptr, std::shared_ptr<multiplayer_session>, xbox_live_error_code::generic_error, "multiplayer_session_writer class was destroyed.");

        xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSessionResult;
        auto xboxLiveContextMap = pThis->m_multiplayerLocalUserManager->get_local_user_map();
        for (auto xboxLiveContext : xboxLiveContextMap)
        {
            auto localUser = xboxLiveContext.second;
            if (localUser != nullptr)
            {
                auto sessionToCommit = std::make_shared<multiplayer_session>(localUser->xbox_user_id(), sessionRefToCommit);
                sessionToCommit->leave();

                // Never update latest copy upon leaving.
                multiplayerSessionResult = pThis->write_session(localUser->context(), sessionToCommit, multiplayer_session_write_mode::update_existing, false).get();
                if (multiplayerSessionResult.err())
                {
                    break;
                }
            }
        }

        return multiplayerSessionResult;
    });

    return utils::create_exception_free_task<std::shared_ptr<multiplayer_session>>(task);
}

std::vector<multiplayer_event>
multiplayer_session_writer::handle_events(
    _In_ std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue,
    _In_ const std::error_code& errorCode,
    _In_ const std::string& errorMessage,
    _In_ multiplayer_session_type sessionType
    )
{
    std::vector<multiplayer_event> eventQueue;
    for (auto& request : processingQueue)
    {
        if (request->joinability() != joinability::none)
        {
            multiplayer_event multiplayerEvent(
                errorCode,
                errorMessage,
                multiplayer_event_type::joinability_state_changed,
                std::make_shared<multiplayer_event_args>(),
                sessionType,
                request->context()
                );

            eventQueue.push_back(multiplayerEvent);
        }

        if (request->session_properties().size() > 0)
        {
            // Fire events for each of the properties
            auto iter = request->session_properties().begin();
            for (iter; iter != request->session_properties().end(); ++iter)
            {
                multiplayer_event multiplayerEvent(
                    errorCode,
                    errorMessage,
                    multiplayer_event_type::session_property_write_completed,
                    std::make_shared<multiplayer_event_args>(),
                    sessionType,
                    request->context()
                    );

                eventQueue.push_back(multiplayerEvent);
            }
        }

        if (!request->synchronized_host_device_token().empty())
        {
            multiplayer_event multiplayerEvent(
                errorCode,
                errorMessage,
                multiplayer_event_type::synchronized_host_write_completed,
                std::make_shared<multiplayer_event_args>(),
                sessionType,
                request->context()
                );

            eventQueue.push_back(multiplayerEvent);
        }

        if (request->synchronized_session_properties().size() > 0)
        {
            // Fire events for each of the properties
            auto iter = request->synchronized_session_properties().begin();
            for (iter; iter != request->synchronized_session_properties().end(); ++iter)
            {
                multiplayer_event multiplayerEvent(
                    errorCode,
                    errorMessage,
                    multiplayer_event_type::session_synchronized_property_write_completed,
                    std::make_shared<multiplayer_event_args>(),
                    sessionType,
                    request->context()
                    );

                eventQueue.push_back(multiplayerEvent);
            }
        }
    }

    return eventQueue;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
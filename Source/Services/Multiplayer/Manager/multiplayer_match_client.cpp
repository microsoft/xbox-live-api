// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include <atomic>
#include "pplx/pplxtasks.h"
#include "multiplayer_manager_internal.h"
#include "xsapi/services.h"
#include "user_context.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::matchmaking;
using namespace xbox::services::real_time_activity;
using namespace pplx;
#if TV_API
using namespace Windows::Xbox::Networking;
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

multiplayer_match_client::multiplayer_match_client(
    _In_ std::shared_ptr<multiplayer_local_user_manager> localUserManager
    ) :
    m_multiplayerLocalUserManager(localUserManager),
    m_matchStatus(match_status::none),
    m_disableNextTimer(false),
    m_preservingMatchmakingSession(false)
{
    m_getSessionTask = pplx::create_task([]{});
}

void
multiplayer_match_client::deep_copy_if_updated(
    _In_ const multiplayer_match_client& other
    )
{
    std::lock_guard<std::mutex> lock(m_lock.get());
    if (other.m_matchSession == nullptr)
    {
        m_matchSession = nullptr;
    }
    else if (m_matchSession == nullptr || other.m_matchSession->change_number() > m_matchSession->change_number())
    {
        m_matchSession = other.m_matchSession->_Create_deep_copy();
    }
}

const std::vector<multiplayer_event>&
multiplayer_match_client::multiplayer_event_queue()
{
    return m_multiplayerEventQueue;
}

std::vector<multiplayer_event>
multiplayer_match_client::do_work()
{
    switch (static_cast<enum match_status>(m_matchStatus))
    {
        case match_status::none:
        {
            return std::vector<multiplayer_event>();
        }

        case match_status::searching:
        {
            check_next_timer();
            break;
        }

        case match_status::found:
        case match_status::canceled:
        {
            // Nothing to do here. Wait for match_status_changed event.
            break;
        }

        case match_status::joining:
        {
            if(m_joinTargetSessionTask.is_done())
            {
                handle_session_joined();
            }
            break;
        }

        case match_status::waiting_for_remote_clients_to_join:
        case match_status::waiting_for_remote_clients_to_upload_qos:
        {
            handle_initialization_state_changed(session());
            break;
        }

        case match_status::measuring:
        {
            // Waiting for title to perform qos and provide qos measurements (via set_quality_of_service_measurements)
            handle_initialization_state_changed(session());
            break;
        }

        case match_status::evaluating:
        {
            // Nothing to do here. Wait for initialization stage changed event.
            break;
        }
    }

    std::vector<multiplayer_event> eventQueue;
    {
        std::lock_guard<std::mutex> lock(m_multiplayerEventQueueLock);
        eventQueue = m_multiplayerEventQueue;
        m_multiplayerEventQueue.clear();
    }

    return eventQueue;
}

void
multiplayer_match_client::disable_next_timer(bool value)
{
    m_disableNextTimer = value;
}

void
multiplayer_match_client::check_next_timer()
{
    if (m_disableNextTimer) return;     // Only used for Unit tests

    int64_t delta = m_nextTimerToFetchSession.to_interval() - utility::datetime::utc_now().to_interval();
    if ( delta < 0)
    {
        if (m_matchStatus == match_status::searching)
        {
            std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
            if (primaryContext == nullptr) return;
            if (!m_getSessionTask.is_done()) return;

            // Fetch ticket session
            std::weak_ptr<multiplayer_match_client> thisWeakPtr = shared_from_this();
            m_getSessionTask = primaryContext->multiplayer_service().get_current_session(m_matchTicketSessionRef)
            .then([thisWeakPtr](xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult)
            {
                if (!sessionResult.err())
                {
                    std::shared_ptr<multiplayer_match_client> pThis(thisWeakPtr.lock());
                    if (pThis != nullptr)
                    {
                        pThis->handle_match_status_changed(sessionResult.payload());
                    }
                }
            });
        }
        else
        {
            get_latest_session();
        }
    }
}

void
multiplayer_match_client::handle_qos_measurements()
{
    std::map<string_t, string_t> addressDeviceTokenMap;
    for (const auto& member : session()->members())
    {
        if (!member->is_current_user())
        {
            std::vector<unsigned char> base64ConnectionAddress(utility::conversions::from_base64(std::move(member->secure_device_base_address64())));
            const string_t& secureDeviceAddress = string_t(base64ConnectionAddress.begin(), base64ConnectionAddress.end());
            if (!secureDeviceAddress.empty())
            {
                addressDeviceTokenMap[secureDeviceAddress] = member->device_token();
            }
        }
    }

    if (addressDeviceTokenMap.size() > 0)
    {
        m_matchStatus = match_status::measuring;

        std::shared_ptr<perform_qos_measurements_event_args> performQosEventArgs = std::make_shared<perform_qos_measurements_event_args>(addressDeviceTokenMap);
        multiplayer_event multiplayerEvent(
            xbox_live_error_code::no_error,
            std::string(),
            multiplayer_event_type::perform_qos_measurements,
            std::dynamic_pointer_cast<perform_qos_measurements_event_args>(performQosEventArgs),
            multiplayer_session_type::game_session
            );

        std::lock_guard<std::mutex> lock(m_multiplayerEventQueueLock);
        m_multiplayerEventQueue.push_back(multiplayerEvent);
    }
    else
    {
        // If clients fail to join, the stage advances to "measuring".
        // Wait until memberInitialization either succeeds or fails.
        check_next_timer();
    }
}

void
multiplayer_match_client::handle_find_match_completed(
    _In_ std::error_code errorCode,
    _In_ std::string errorMessage
    )
{
    multiplayer_measurement_failure failure = multiplayer_measurement_failure::unknown;

    auto matchSession = session();
    if (matchSession != nullptr && matchSession->current_user() != nullptr)
    {
        failure = matchSession->current_user()->initialization_failure_cause();
    }

    std::shared_ptr<find_match_completed_event_args> findMatchEventArgs = std::make_shared<find_match_completed_event_args>(
        m_matchStatus,
        failure
        );

    multiplayer_event multiplayerEvent(
        errorCode,
        errorMessage,
        multiplayer_event_type::find_match_completed,
        std::dynamic_pointer_cast<find_match_completed_event_args>(findMatchEventArgs),
        multiplayer_session_type::game_session
        );

    std::lock_guard<std::mutex> lock(m_multiplayerEventQueueLock);
    m_multiplayerEventQueue.push_back(multiplayerEvent);
}

void
multiplayer_match_client::handle_match_status_changed(
    _In_ std::shared_ptr<multiplayer_session> matchSession
    )
{
    matchmaking_status status = matchSession->matchmaking_server().status();
    switch (status)
    {
        case matchmaking_status::searching:
        {
            xbox::services::multiplayer::manager::match_status expected = match_status::none;
            if (m_matchStatus.compare_exchange_strong(expected, match_status::searching, std::memory_order_release))
            {
                // Wait for status to change on ticket session or fetch after 2 mins.
                m_nextTimerToFetchSession = utility::datetime::utc_now() + utility::datetime::from_seconds(120);
            }
            else 
            {
                if (m_disableNextTimer) return;     // Only used for Unit tests

                int64_t delta = m_nextTimerToFetchSession.to_interval() - utility::datetime::utc_now().to_interval();
                if ( delta < 0)
                {
                    // Delete the match ticket and let the title know that it failed.
                    if (!m_hopperName.empty() && !m_matchTicketResponse.match_ticket_id().empty())
                    {
                        // Only the host has the ticketId info. Since we aren't the host who started the match, we cannot cancel it either.
                        std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
                        if (primaryContext == nullptr) return;

                        primaryContext->matchmaking_service().delete_match_ticket(
                            xbox::services::xbox_live_app_config::get_app_config_singleton()->scid(),
                            m_hopperName,
                            m_matchTicketResponse.match_ticket_id()
                        );
                    }

                    m_matchStatus = match_status::failed;
                    handle_find_match_completed(xbox_live_error_code::generic_error, "Matchmaking request failed.");
                }
            }
            break;
        }
        case matchmaking_status::expired:
        {
            m_matchStatus = match_status::expired;
            handle_find_match_completed(xbox_live_error_code::generic_error, "Matchmaking request expired.");
            break;
        }
        case matchmaking_status::canceled:
        {
            m_matchStatus = match_status::canceled;
            handle_find_match_completed(xbox_live_error_code::generic_error, "Matchmaking request was canceled.");
            break;
        }
        case matchmaking_status::found:
        {
            handle_match_found(matchSession);
            break;
        }
        default:
            break;
    }
}

void
multiplayer_match_client::handle_initialization_state_changed(
    _In_ std::shared_ptr<multiplayer_session> matchSession
    )
{
    update_session(matchSession);
    if (matchSession->intializing_episode() > 0)
    {
        switch (matchSession->initialization_stage())
        {
            case multiplayer_initialization_stage::joining:
            {
                check_next_timer();
                break;
            }

            case multiplayer_initialization_stage::measuring:
            {
                if (m_matchStatus == match_status::waiting_for_remote_clients_to_upload_qos ||
                    m_matchStatus == match_status::measuring)
                {
                    check_next_timer();
                }
                else if (m_matchStatus == match_status::waiting_for_remote_clients_to_join)
                {
                    handle_qos_measurements();
                }
                break;
            }

            case multiplayer_initialization_stage::failed:
            {
                m_matchStatus = match_status::failed;
                handle_find_match_completed(xbox_live_error_code::generic_error, "multiplayer_initialization_stage failed");
                return;
            }
        }
    }
    else
    {
        if (matchSession->current_user()->initialization_failure_cause() == multiplayer_measurement_failure::none)
        {
            // QoS succeeded.
            m_matchStatus = match_status::completed;
            handle_find_match_completed(xbox_live_error_code::no_error, std::string());
        }
        else
        {
            // Resubmit
            m_matchStatus = match_status::resubmitting;
            handle_find_match_completed(xbox_live_error_code::generic_error, "multiplayer_initialization_stage failed");
        }
    }
}

xbox_live_result<void>
multiplayer_match_client::find_match(
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ bool preserveSession
    )
{
    return find_match(m_hopperName, m_attributes, m_timeout, session, preserveSession);
}

xbox_live_result<void>
multiplayer_match_client::find_match(
    _In_ const string_t& hopperName,
    _In_ const web::json::value& attributes,
    _In_ const std::chrono::seconds& timeout,
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ bool preserveSession
    )
{
    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    if( primaryContext == nullptr || session == nullptr)
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "No local user added. Call add_local_user() first.");
    }

    xbox::services::multiplayer::manager::match_status expected = match_status::none;
    if (!m_matchStatus.compare_exchange_strong(expected, match_status::submitting_match_ticket, std::memory_order_release))
    {
         return xbox_live_result<void>(xbox_live_error_code::logic_error, "Match search already in progress.");
    }

    if (preserveSession)
    {
        update_session(session);
    }
    else
    {
        update_session(nullptr);
    }

    m_hopperName = hopperName;
    m_attributes = attributes;
    m_timeout = timeout;
    m_preservingMatchmakingSession = preserveSession;
    m_matchTicketSessionRef = session->session_reference();

    std::weak_ptr<multiplayer_match_client> thisWeakPtr = shared_from_this();
    primaryContext->matchmaking_service().create_match_ticket(
        session->session_reference(),
        session->session_reference().service_configuration_id(),
        hopperName,
        timeout,
        preserveSession ? preserve_session_mode::always : preserve_session_mode::never,
        attributes
        )
    .then([thisWeakPtr, timeout](xbox_live_result<create_match_ticket_response> result)
    {
        std::shared_ptr<multiplayer_match_client> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            if (!result.err())
            {
                pThis->m_matchTicketResponse = result.payload();
                pThis->m_matchStatus = match_status::searching;
                pThis->m_nextTimerToFetchSession = utility::datetime::utc_now() 
                    + utility::datetime::from_seconds(static_cast<int32_t>(timeout.count()))
                    + utility::datetime::from_seconds(5);   // some extra delay to be safe
            }
            else
            {
                pThis->m_matchStatus = match_status::failed;
                pThis->handle_find_match_completed(result.err(), result.err_message());
            }
        }
    });

    return xbox_live_result<void>();
}

void
multiplayer_match_client::cancel_match()
{
    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    if (primaryContext == nullptr)
    {
         return;
    }

    if (m_matchStatus == match_status::none || m_matchStatus == match_status::expired ||
        m_matchStatus == match_status::canceled || m_matchStatus == match_status::failed)
    {
        return;
    }

    if (m_hopperName.empty() || m_matchTicketResponse.match_ticket_id().empty())
    {
        // Since we aren't the host who started the match, we cannot cancel it either.
        return;
    }
    
    m_matchStatus = match_status::canceling;
    primaryContext->matchmaking_service().delete_match_ticket(
        xbox::services::xbox_live_app_config::get_app_config_singleton()->scid(),
        m_hopperName,
        m_matchTicketResponse.match_ticket_id()
        );
}

xbox::services::multiplayer::manager::match_status
multiplayer_match_client::match_status() const
{
    return m_matchStatus;
}

void
multiplayer_match_client::set_match_status(
    _In_ xbox::services::multiplayer::manager::match_status status
    )
{
    m_matchStatus = status;
}

std::chrono::seconds
multiplayer_match_client::estimated_match_wait_time() const
{
    return m_matchTicketResponse.estimated_wait_time(); 
}

void
multiplayer_match_client::on_session_changed(
    _In_ const multiplayer_session_change_event_args& args
    )
{
    auto matchSession = session();
    if (matchSession != nullptr &&
        multiplayer_manager_utils::do_session_references_match(matchSession->session_reference(), args.session_reference()) &&
        args.change_number() > matchSession->change_number())
    {
        get_latest_session();
    }
}

void 
multiplayer_match_client::update_session(
    _In_ std::shared_ptr<multiplayer_session> session
    )
{
    std::lock_guard<std::mutex> lock(m_lock.get());

    if (m_matchSession == nullptr || session == nullptr)
    {
        m_matchSession = session;
    }
    else if(multiplayer_manager_utils::do_sessions_match(m_matchSession, session) &&
        session->change_number() > m_matchSession->change_number())
    {
        m_matchSession = session;
        m_nextTimerToFetchSession = utility::datetime::utc_now() + utility::datetime::from_seconds(session->date_of_next_timer() - session->date_of_session());
    }
}

std::shared_ptr<multiplayer_session> 
multiplayer_match_client::session()
{
    std::lock_guard<std::mutex> lock(m_lock.get());
    return m_matchSession;
}

void
multiplayer_match_client::handle_session_joined()
{
    auto result = m_joinTargetSessionTask.get();
    if (result.err())
    {
        m_matchStatus = match_status::failed;
        handle_find_match_completed(result.err(), result.err_message());
        return;
    }

    if (session()->intializing_episode() == 0)
    {
        m_matchStatus = match_status::completed;
        handle_find_match_completed(xbox_live_error_code::no_error, std::string());
    }
    else
    {
        m_matchStatus = match_status::waiting_for_remote_clients_to_join;
    }
}

void
multiplayer_match_client::handle_match_found(
    _In_ std::shared_ptr<multiplayer_session> currentSession
    )
{
    std::shared_ptr<xbox_live_context_impl> primaryXboxLiveContext = m_multiplayerLocalUserManager->get_primary_context();
    if(primaryXboxLiveContext == nullptr)
    {
        m_matchStatus = match_status::failed;
        handle_find_match_completed(xbox_live_error_code::runtime_error, "No primary xbox live context.");
        return;
    }

    m_matchStatus = match_status::found;
    auto targetSessionRef = currentSession->matchmaking_server().target_session_ref();
    auto targetGameSession = std::make_shared<multiplayer_session>(
        utils::string_t_from_internal_string(primaryXboxLiveContext->xbox_live_user_id()),
        targetSessionRef
        );

    auto gameClient = multiplayer_manager::get_singleton_instance()->_Game_client();
    if (m_preservingMatchmakingSession && gameClient != nullptr)
    {
        auto gameSession = gameClient->session();
        if (gameSession != nullptr && 
            multiplayer_manager_utils::do_session_references_match(gameSession->session_reference(), targetSessionRef))
        {
            targetGameSession = gameSession;
        }
    }

    update_session(targetGameSession);
    m_joinTargetSessionTask = join_session_helper(targetGameSession);
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_match_client::join_session_helper(
    _In_ std::shared_ptr<multiplayer_session> session
    )
{
    std::weak_ptr<multiplayer_match_client> thisWeakPtr = shared_from_this();
    auto task = pplx::create_task([thisWeakPtr, session]()
    {
        std::shared_ptr<multiplayer_match_client> pThis(thisWeakPtr.lock());
        RETURN_CPP_IF(pThis == nullptr, std::shared_ptr<multiplayer_session>, xbox_live_error_code::generic_error, "matchmaking_client_manager class was destroyed.");
        
        xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSessionResult;
        pThis->m_matchStatus = match_status::joining;
        if (pThis->m_preservingMatchmakingSession)
        {
            // Matchmaking session was preserved so we're already part of it
            return xbox_live_result<std::shared_ptr<multiplayer_session>>(session);
        }

        auto xboxLiveContextMap = pThis->m_multiplayerLocalUserManager->get_local_user_map();
        for(auto xboxLiveContext : xboxLiveContextMap)
        {
            auto localUser =  xboxLiveContext.second;
            if (localUser != nullptr)
            {
                auto updatedSession = std::make_shared<multiplayer_session>(localUser->xbox_user_id(), session->session_reference());
                updatedSession->join();
                updatedSession->set_current_user_secure_device_address_base64(localUser->connection_address());
                updatedSession->set_session_change_subscription(multiplayer_session_change_types::everything);
                auto writeSessionResult = localUser->context()->multiplayer_service().write_session(updatedSession, multiplayer_session_write_mode::update_or_create_new).get();
                if (writeSessionResult.err())
                {
                    return writeSessionResult;
                }

                multiplayerSessionResult = writeSessionResult;
            }
        }

        auto matchSession = multiplayerSessionResult.payload();
        pThis->update_session(matchSession);
        return multiplayerSessionResult;
    });

    return utils::create_exception_free_task<std::shared_ptr<multiplayer_session>>(task);
}

void
multiplayer_match_client::get_latest_session()
{
    std::lock_guard<std::mutex> lock(m_getSessionLock.get());
    if (session() == nullptr) return;

    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    if (primaryContext == nullptr) return;
    if (!m_getSessionTask.is_done()) return;

    std::weak_ptr<multiplayer_match_client> thisWeakPtr = shared_from_this();
    m_getSessionTask = primaryContext->multiplayer_service().get_current_session(session()->session_reference())
    .then([thisWeakPtr](xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult)
    {
        if (!sessionResult.err())
        {
            std::shared_ptr<multiplayer_match_client> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->update_session(sessionResult.payload());
            }
        }
    });
}

void 
multiplayer_match_client::set_quality_of_service_measurements(
    _In_ std::shared_ptr<std::vector<multiplayer_quality_of_service_measurements>> measurements
    )
{
    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    auto matchSession = session();
    if (primaryContext == nullptr || matchSession == nullptr) return;

    auto matchSessionRef = matchSession->session_reference();
    std::weak_ptr<multiplayer_match_client> thisWeakPtr = shared_from_this();
    auto task = pplx::create_task([thisWeakPtr, matchSessionRef, measurements]()
    {
        std::shared_ptr<multiplayer_match_client> pThis(thisWeakPtr.lock());
        if (pThis == nullptr) return;

        pThis->m_matchStatus = match_status::uploading_qos_measurements;

        xbox_live_result<std::shared_ptr<multiplayer_session>> matchSessionResult;
        auto xboxLiveContextMap = pThis->m_multiplayerLocalUserManager->get_local_user_map();
        for (auto xboxLiveContext : xboxLiveContextMap)
        {
            auto localUser = xboxLiveContext.second;
            if (localUser != nullptr)
            {
                auto matchSession = std::make_shared<multiplayer_session>(localUser->xbox_user_id(), matchSessionRef);
                matchSession->join();
                matchSession->set_current_user_quality_of_service_measurements(measurements);
                matchSessionResult = localUser->context()->multiplayer_service().write_session(matchSession, multiplayer_session_write_mode::update_existing).get();
                if (matchSessionResult.err())
                {
                    break;
                }
            }
        }

        pThis->update_session(matchSessionResult.payload());
        pThis->m_matchStatus = match_status::waiting_for_remote_clients_to_upload_qos;
    });
}

void
multiplayer_match_client::resubmit_matchmaking(
    _In_ std::shared_ptr<multiplayer_session> session
    )
{
    if (session == nullptr) return;

    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    if (primaryContext == nullptr) return;

    m_matchStatus = match_status::resubmitting;
    session->set_matchmaking_resubmit(true);
    std::weak_ptr<multiplayer_match_client> thisWeakPtr = shared_from_this();
    primaryContext->multiplayer_service().write_session(session, multiplayer_session_write_mode::update_existing)
    .then([thisWeakPtr](xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult)
    {
        if (sessionResult.err())
        {
            std::shared_ptr<multiplayer_match_client> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->m_matchStatus = match_status::failed;
                pThis->handle_find_match_completed(sessionResult.err(), sessionResult.err_message());
            }
        }
    });
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
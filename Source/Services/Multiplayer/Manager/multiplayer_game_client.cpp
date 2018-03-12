// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"
#include <thread>

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

multiplayer_game_client::multiplayer_game_client() :
    m_updateNumber(0),
    m_pendingCommitInProgress(false)
{
    m_sessionWriter = std::make_shared<multiplayer_session_writer>();
}

multiplayer_game_client::multiplayer_game_client(
    _In_ std::shared_ptr<multiplayer_local_user_manager> localUserManager
    ) :
    m_multiplayerLocalUserManager(localUserManager),
    m_updateNumber(0),
    m_pendingCommitInProgress(false)
{
    m_sessionWriter = std::make_shared<multiplayer_session_writer>(m_multiplayerLocalUserManager);
}

multiplayer_game_client::~multiplayer_game_client()
{
    m_sessionWriter.reset();
}

void
multiplayer_game_client::initialize()
{
    std::weak_ptr<multiplayer_game_client> thisWeakPtr = shared_from_this();
    m_sessionWriter->add_multiplayer_session_updated_handler([thisWeakPtr](_In_ const std::shared_ptr<multiplayer_session>& updatedSession)
    {
        std::shared_ptr<multiplayer_game_client> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->update_session(updatedSession);
        }
    });
}

std::shared_ptr<multiplayer_lobby_client>
multiplayer_game_client::lobby_client()
{
    return multiplayer_manager::get_singleton_instance()->_Lobby_client();
}

std::shared_ptr<multiplayer_session>
multiplayer_game_client::lobby_session()
{
    auto mpInstance = multiplayer_manager::get_singleton_instance();
    if (mpInstance->_Lobby_client() == nullptr)
    {
        return nullptr;
    }

    return mpInstance->_Lobby_client()->session();
}

void
multiplayer_game_client::set_game_session_template(
    _In_ const string_t& sessionTemplateName
    )
{
    m_gameSessionTemplateName = sessionTemplateName;
}

void
multiplayer_game_client::deep_copy_if_updated(
    _In_ const multiplayer_game_client& other
    )
{
    std::lock_guard<std::mutex> lock(other.m_clientRequestLock);

    if (other.m_sessionWriter->session() == nullptr)
    {
        m_sessionWriter->update_session(nullptr);
        m_multiplayerGame = nullptr;
    }
    else if (other.m_multiplayerGame == nullptr)
    {
        m_multiplayerGame = nullptr;
    }
    else if (m_sessionWriter->session() == nullptr ||
            other.m_sessionWriter->session()->change_number() > m_sessionWriter->session()->change_number() ||
            other.m_sessionWriter->session()->e_tag() > m_sessionWriter->session()->e_tag())
    {
        m_sessionWriter->update_session(other.m_sessionWriter->session()->_Create_deep_copy());
        m_multiplayerGame = other.m_multiplayerGame->_Create_deep_copy();
    }
    else if (m_updateNumber != other.m_updateNumber)
    {
        m_multiplayerGame = other.m_multiplayerGame->_Create_deep_copy();
    }
}

const std::shared_ptr<multiplayer_session_writer>&
multiplayer_game_client::session_writer() const
{
    return m_sessionWriter;
}

const std::shared_ptr<multiplayer_game_session>&
multiplayer_game_client::game() const
{
    return m_multiplayerGame;
}

void
multiplayer_game_client::update_game(
    _In_ const std::shared_ptr<multiplayer_game_session>& multiplayerGame
    )
{
    ++m_updateNumber;
    m_multiplayerGame = multiplayerGame;
}

const std::shared_ptr<multiplayer_session>&
multiplayer_game_client::session() const
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    return m_sessionWriter->session();
}

void 
multiplayer_game_client::update_session(
    _In_ const std::shared_ptr<multiplayer_session>& updatedSession
    )
{
    auto cachedSession = session();
    if (updatedSession == nullptr || cachedSession == nullptr ||
        updatedSession->change_number() > cachedSession->change_number() ||
        updatedSession->e_tag() != cachedSession->e_tag())
    {
        update_game_session(updatedSession);
    }
}

void 
multiplayer_game_client::update_game_session(
    _In_ const std::shared_ptr<multiplayer_session>& updatedSession
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_sessionWriter->update_session(updatedSession);
}

void
multiplayer_game_client::update_objects(
    const std::shared_ptr<multiplayer_session>& updatedSession,
    const std::shared_ptr<multiplayer_session>& lobbySession
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    if (updatedSession == nullptr)
    {
        update_game(nullptr);
    }
    else
    {
        auto multiplayerGame = convert_to_multiplayer_game(updatedSession, lobbySession);
        update_game(multiplayerGame);
    }
}

std::shared_ptr<multiplayer_game_session>
multiplayer_game_client::convert_to_multiplayer_game(
    _In_ const std::shared_ptr<multiplayer_session>& sessionToConvert,
    _In_ const std::shared_ptr<multiplayer_session>& lobbySession
    )
{
    if (sessionToConvert == nullptr)
    {
        return nullptr;
    }

    std::shared_ptr<multiplayer_member> hostMember = nullptr;
    std::vector<std::shared_ptr<multiplayer_member>> gameMembers;
    for (const auto& member : sessionToConvert->members())
    {
        auto gameMember = multiplayer_manager_utils::convert_to_game_member(
            member,
            lobbySession,
            sessionToConvert,
            m_multiplayerLocalUserManager->get_local_user_map()
            );
        if (!member->device_token().empty() &&
            utils::str_icmp(member->device_token(), sessionToConvert->session_properties()->host_device_token()) == 0)
        {
            hostMember = gameMember;
        }

        gameMembers.push_back(gameMember);
    }

    return std::make_shared<multiplayer_game_session>(
        sessionToConvert,
        hostMember,
        gameMembers
        );
}

const std::vector<multiplayer_event>&
multiplayer_game_client::multiplayer_event_queue()
{
    // Don't require a lock as STL is multithread read safe
    return m_multiplayerEventQueue;
}

void
multiplayer_game_client::clear_pending_queue()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    while (!m_pendingRequestQueue.empty())
    {
        m_pendingRequestQueue.pop();
    }
}

void
multiplayer_game_client::add_to_pending_queue(
    _In_ std::shared_ptr<multiplayer_client_pending_request> pendingRequest
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_pendingRequestQueue.push(pendingRequest);
}

void
multiplayer_game_client::add_to_processing_queue(
    _In_ std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue
    )
{
    for (const auto& request : processingQueue)
    {
        m_processingQueue.push_back(request);
    }
}

void
multiplayer_game_client::remove_from_processing_queue(
    _In_ uint32_t identifier
)
{
    for (auto request = m_processingQueue.begin(); request != m_processingQueue.end(); ++request)
    {
        if ((*request)->identifier() == identifier)
        {
            m_processingQueue.erase(request);
            break;
        }
    }
}

std::vector<std::shared_ptr<multiplayer_client_pending_request>>
multiplayer_game_client::get_processing_queue()
{
    return m_processingQueue;
}

std::vector<multiplayer_event>
multiplayer_game_client::do_work()
{
    bool expected = false;
    if (m_pendingCommitInProgress.compare_exchange_strong(expected, true))
    {
        if (m_pendingRequestQueue.size() > 0)
        {
            std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue;
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
                        if (pendingRequest->request_type() != m_pendingRequestQueue.front()->request_type())
                        {
                            doneProcessing = true;
                        }
                    }

                    if (!applySynchronizedChanges && pendingRequest->request_type() == pending_request_type::synchronized_changes)
                    {
                        applySynchronizedChanges = true;
                    }
                }

            } while (!doneProcessing && m_pendingRequestQueue.size() > 0);

            if (processingQueue.size() > 0)
            {
                add_to_processing_queue(processingQueue);
                pplx::task<xbox_live_result<std::vector<multiplayer_event>>> asyncOp;
                if (applySynchronizedChanges)
                {
                    asyncOp = m_sessionWriter->commit_pending_synchronized_changes(processingQueue, multiplayer_session_type::game_session);
                }
                else
                {
                    asyncOp = m_sessionWriter->commit_pending_changes(processingQueue, multiplayer_session_type::game_session);
                }

                std::weak_ptr<multiplayer_game_client> thisWeakPtr = shared_from_this();

                create_task(asyncOp)
                .then([thisWeakPtr, processingQueue](xbox_live_result<std::vector<multiplayer_event>> result)
                {
                    std::shared_ptr<multiplayer_game_client> pThis(thisWeakPtr.lock());
                    if (pThis != nullptr)
                    {
                        std::lock_guard<std::mutex> lock(pThis->m_clientRequestLock);
                        {
                            auto eventQueue = result.payload();
                            for (const auto& ev : eventQueue)
                            {
                                pThis->m_multiplayerEventQueue.push_back(ev);
                            }
                        }

                        for (const auto& processingRequest : processingQueue)
                        {
                            pThis->remove_from_processing_queue(processingRequest->identifier());
                        }

                        pThis->m_pendingCommitInProgress.store(false);
                    }
                });
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

    std::vector<multiplayer_event> eventQueue;
    {
        std::lock_guard<std::mutex> lock(m_clientRequestLock);
        eventQueue = m_multiplayerEventQueue;
        m_multiplayerEventQueue.clear();
    }

    return eventQueue;
}

bool
multiplayer_game_client::is_pending_game_changes()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    if (m_pendingRequestQueue.size() > 0)
    {
        return true;
    }

    if (m_sessionWriter != nullptr)
    {
        auto latestSession = m_sessionWriter->session();
        if (latestSession != nullptr &&
            m_multiplayerGame != nullptr &&
            latestSession->change_number() != m_multiplayerGame->_Change_number())
        {
            return true;
        }
    }

    return false;
}

bool
multiplayer_game_client::is_request_in_progress()
{
    return m_pendingRequestQueue.size() > 0 || m_processingQueue.size() > 0 || m_multiplayerEventQueue.size() > 0;
}

void
multiplayer_game_client::set_local_member_properties_to_remote_session(
    _In_ const std::shared_ptr<multiplayer_local_user>& localUser,
    _In_ const std::map<string_t, web::json::value>& localUsersMap,
    _In_ const string_t& localUserConnectionAddress
    )
{
    if (session() == nullptr || localUser == nullptr)
    {
        return;
    }

    std::weak_ptr<multiplayer_game_client> thisWeakPtr = shared_from_this();
    pplx::create_task([thisWeakPtr, localUser, localUsersMap, localUserConnectionAddress]()
    {
        std::shared_ptr<multiplayer_game_client> pThis(thisWeakPtr.lock());
        if (pThis == nullptr) return;

        bool expected = false;
        while (!pThis->m_pendingCommitInProgress.compare_exchange_weak(expected, true))
        {
            expected = false;
            utils::sleep(0);
        }

        auto gameSession = pThis->session();
        if (gameSession != nullptr)
        {
            auto sessionToCommit = std::make_shared<multiplayer_session>(localUser->xbox_user_id(), gameSession->session_reference());
            sessionToCommit->join(web::json::value::null(), false, true, false);
            for (const auto& prop : localUsersMap)
            {
                sessionToCommit->set_current_user_member_custom_property_json(prop.first, prop.second);
            }

            if (!localUserConnectionAddress.empty())
            {
                sessionToCommit->set_current_user_secure_device_address_base64(localUserConnectionAddress);
            }
            
            pThis->m_sessionWriter->write_session(localUser->context(), sessionToCommit, multiplayer_session_write_mode::update_existing).get();
        }

        pThis->m_pendingCommitInProgress = false;
    });
}

void
multiplayer_game_client::remove_stale_users_from_remote_session()
{
    auto gameSession = session();
    if (gameSession == nullptr) return;

    auto xboxLiveContextMap = m_multiplayerLocalUserManager->get_local_user_map();
    for (auto xboxLiveContext : xboxLiveContextMap)
    {
        auto localUser = xboxLiveContext.second;
        if (localUser != nullptr && localUser->lobby_state() == multiplayer_local_user_lobby_state::remove)
        {
            // Leave the game session if it exists.
            if (gameSession != nullptr)
            {
                std::weak_ptr<multiplayer_game_client> thisWeakPtr = shared_from_this();

                auto sessionToCommit = std::make_shared<multiplayer_session>(localUser->xbox_user_id(), gameSession->session_reference());
                sessionToCommit->leave();
                m_sessionWriter->write_session(localUser->context(), sessionToCommit, multiplayer_session_write_mode::update_existing)
                .then([thisWeakPtr](xbox_live_result<std::shared_ptr<multiplayer_session>> result)
                {
                    std::shared_ptr<multiplayer_game_client> pThis(thisWeakPtr.lock());
                    if (pThis != nullptr && pThis->lobby_client() != nullptr)
                    {
                        pThis->lobby_client()->stop_advertising_game_session(result);
                    }
                });
            }
        }
    }
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_game_client::join_game_for_all_local_members_helper(
    _In_ const std::shared_ptr<multiplayer_session>& session,
    _In_ bool writeMemberPropertiesFromLobby,
    _In_ const string_t& handleId
    )
{
    std::weak_ptr<multiplayer_game_client> thisWeakPtr = shared_from_this();
    auto task = pplx::create_task([thisWeakPtr, session, writeMemberPropertiesFromLobby, handleId]()
    {
        std::shared_ptr<multiplayer_game_client> pThis(thisWeakPtr.lock());
        RETURN_CPP_IF(pThis == nullptr, std::shared_ptr<multiplayer_session>, xbox_live_error_code::generic_error, "multiplayer_game_client class was destroyed.");

        xbox_live_result<std::shared_ptr<multiplayer_session>> multiplayerSessionResult;

        auto xboxLiveContextMap = pThis->m_multiplayerLocalUserManager->get_local_user_map();
        for (auto xboxLiveContext : xboxLiveContextMap)
        {
            auto localUser = xboxLiveContext.second;
            if (localUser != nullptr)
            {
                auto updatedSession = std::make_shared<multiplayer_session>(localUser->xbox_user_id(), session->session_reference());
                auto joinSessionResult = pThis->join_helper(localUser, updatedSession, writeMemberPropertiesFromLobby, handleId).get();
                if (joinSessionResult.err())
                {
                    return joinSessionResult;
                }

                localUser->set_game_state(multiplayer_local_user_game_state::in_session);
                multiplayerSessionResult = joinSessionResult;
            }
        }

        return multiplayerSessionResult;
    });

    return utils::create_exception_free_task<std::shared_ptr<multiplayer_session>>(
        task
        );
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_game_client::join_helper(
    _In_ std::shared_ptr<multiplayer_local_user> localUser,
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ bool writeMemberPropertiesFromLobby,
    _In_ const string_t& handleId
    )
{
    RETURN_TASK_CPP_IF(localUser == nullptr || localUser->context() == nullptr, std::shared_ptr<multiplayer_session>, "Call add_local_user() first.");

    session->join(web::json::value::null(), false, true, false);
    session->set_current_user_secure_device_address_base64(localUser->connection_address());
    session->set_session_change_subscription(multiplayer_session_change_types::everything);

    if (writeMemberPropertiesFromLobby)
    {
        auto localMember = multiplayer_manager_utils::get_player_in_session(localUser->xbox_user_id(), lobby_session());
        if (localMember != nullptr && localMember->member_custom_properties_json().size() > 0)
        {
            auto jsonPropertyObj = localMember->member_custom_properties_json();
            for (const auto& prop : jsonPropertyObj.as_object())
            {
                session->set_current_user_member_custom_property_json(prop.first, prop.second);
            }
        }
    }
    if (handleId.empty())
    {
        return m_sessionWriter->write_session(localUser->context(), session, multiplayer_session_write_mode::update_or_create_new);
    }
    else
    {
        return m_sessionWriter->write_session_by_handle(localUser->context(), session, multiplayer_session_write_mode::update_or_create_new, handleId);
    }
}

xbox_live_result<void>
multiplayer_game_client::join_game_helper(
    _In_ const string_t& sessionName
    )
{
    multiplayer_session_reference gameSessionRef(
        utils::string_t_from_internal_string(utils::try_get_override_scid()),
        m_gameSessionTemplateName,
        sessionName
        );

    auto joinResult = join_game_by_session_reference(gameSessionRef).get();
    if (!joinResult.err() && joinResult.payload() != nullptr && lobby_client() != nullptr)
    {
        // If jon_game succeeds, advertise game sessions via the lobby. If the advertising fails, 
        // we simply eat the error as it isn't actionable for the title.

        lobby_client()->advertise_game_session();
    }

    return xbox_live_result<void>(joinResult.err(), joinResult.err_message());
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_game_client::join_game_by_session_reference(
    _In_ const multiplayer_session_reference& gameSessionRef
    )
{
    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    RETURN_TASK_CPP_IF(primaryContext == nullptr, std::shared_ptr<multiplayer_session>, "Call add_local_user() before joining.");

    auto gameSession = std::make_shared<multiplayer_session>(
        utils::string_t_from_internal_string(primaryContext->xbox_live_user_id()),
        gameSessionRef
        );

    return join_game_for_all_local_members(gameSession, string_t(), false);
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_game_client::join_game_by_handle(
    _In_ const string_t& handleId,
    _In_ bool createGameIfFailedToJoin
    )
{
    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    RETURN_TASK_CPP_IF(primaryContext == nullptr, std::shared_ptr<multiplayer_session>, "Call add_local_user() before joining.");

    auto gameSession = std::make_shared<multiplayer_session>(utils::string_t_from_internal_string(primaryContext->xbox_live_user_id()));

    return join_game_for_all_local_members(gameSession, handleId, createGameIfFailedToJoin);
}

pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>>
multiplayer_game_client::join_game_for_all_local_members( 
    _In_ const std::shared_ptr<multiplayer_session>& gameSession,
    _In_ const string_t& handleId,
    _In_ bool createGameIfFailedToJoin
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(gameSession == nullptr || (gameSession->session_reference().session_name().empty() && handleId.empty()), std::shared_ptr<multiplayer_session>, "Multiplayer session reference and handleId is empty.");

    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    RETURN_TASK_CPP_IF(primaryContext == nullptr, std::shared_ptr<multiplayer_session>, "Call add_local_user() before joining.");

    // Leave existing game without updating the latest as the leave may comeback after the actual join and overwrite it.
    auto cachedSession = session();
    if (cachedSession != nullptr)
    {
        leave_remote_session(cachedSession, false, false);
    }
    
    update_session(nullptr);
    std::weak_ptr<multiplayer_game_client> thisWeakPtr = shared_from_this();

    auto task = join_game_for_all_local_members_helper(gameSession, true, handleId)
    .then([thisWeakPtr, handleId, createGameIfFailedToJoin](xbox_live_result<std::shared_ptr<multiplayer_session>> joinResult)
    {
        std::shared_ptr<multiplayer_game_client> pThis(thisWeakPtr.lock());
        RETURN_CPP_IF(pThis == nullptr, std::shared_ptr<multiplayer_session>, xbox_live_error_code::generic_error, "multiplayer_game_client class was destroyed.");

        auto lobbyClient = pThis->lobby_client();
        if (lobbyClient != nullptr)
        {
            if (joinResult.err() == xbox_live_error_condition::http_404_not_found && !handleId.empty())
            {
                if (createGameIfFailedToJoin)
                {
                    lobbyClient->create_game_from_lobby();
                    return xbox_live_result<std::shared_ptr<multiplayer_session>>();
                }

                // Tried to join by handleId and we failed. The handleId must be invalid.
                lobbyClient->clear_game_session_from_lobby();
            }
            else if (joinResult.err() && handleId.empty())
            {
                // Tried to create a new game and we failed. Clear the transfer handle pending state (GameSessionTransferHandle=pending~xuid).
                lobbyClient->clear_game_session_from_lobby();
            }
        }

        if (!joinResult.err())
        {
            pThis->update_session(joinResult.payload());
            pThis->m_multiplayerLocalUserManager->change_all_local_user_game_state(multiplayer_local_user_game_state::in_session);
        }

        pThis->add_multiplayer_event_helper(
            joinResult.err(), 
            joinResult.err_message(),
            multiplayer_event_type::join_game_completed,
            std::make_shared<multiplayer_event_args>()
            );
        return joinResult;
    });

    return utils::create_exception_free_task<std::shared_ptr<multiplayer_session>>(
        task
        );
}

xbox_live_result<void>
multiplayer_game_client::join_game_from_lobby_helper()
{
    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    auto lobbySession = lobby_session();
    auto lobbyClient = lobby_client();
    if( primaryContext == nullptr || lobbyClient == nullptr || lobbySession == nullptr)
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "No lobby session exists. Call add_local_user() to create a lobby first.");
    }

    // Check if the lobby has a game session associated with it to join.
    auto lobbyProperties = lobbySession->session_properties()->session_custom_properties_json();
    if (!lobbyProperties.has_field(multiplayer_lobby_client::c_transferHandlePropertyName))
    {
        return lobbyClient->create_game_from_lobby();
    }

    string_t handleId;
    if (lobbyClient->is_transfer_handle_state(_T("pending")))
    {
        // Wait for the property changed event to join.
        m_multiplayerLocalUserManager->change_all_local_user_game_state(multiplayer_local_user_game_state::pending_join);
        return xbox_live_result<void>();
    }
    else if (lobbyClient->is_transfer_handle_state(_T("completed")))
    {
        handleId = lobbyClient->get_transfer_handle();
    }

    std::weak_ptr<multiplayer_game_client> thisWeakPtr = shared_from_this();
    pplx::create_task([thisWeakPtr, handleId, primaryContext]()
    {
        std::shared_ptr<multiplayer_game_client> pThis(thisWeakPtr.lock());
        RETURN_CPP_IF(pThis == nullptr, void, xbox_live_error_code::generic_error, "multiplayer_game_client class was destroyed.");

        auto joinResult = pThis->join_game_by_handle(handleId, true).get();
        return xbox_live_result<void>(joinResult.err(), joinResult.err_message());
    });

    return xbox_live_result<void>();
}

void
multiplayer_game_client::leave_remote_session(
    _In_ std::shared_ptr<multiplayer_session> session,
    _In_ bool stopAdvertisingGameSession,
    _In_ bool triggerCompletionEvent
    )
{
    auto processingRequest = std::make_shared<multiplayer_client_pending_request>();
    m_processingQueue.push_back(processingRequest);

    std::weak_ptr<multiplayer_game_client> thisWeakPtr = shared_from_this();
    m_sessionWriter->leave_remote_session(session)
    .then([thisWeakPtr, stopAdvertisingGameSession, triggerCompletionEvent, processingRequest](xbox_live_result<std::shared_ptr<multiplayer_session>> result)
    {
        std::shared_ptr<multiplayer_game_client> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            if (stopAdvertisingGameSession && pThis->lobby_client() != nullptr)
            {
                pThis->lobby_client()->stop_advertising_game_session(result);
            }

            // Always set your local game session to null upon leaving.
            pThis->update_session(nullptr);
            
            if (triggerCompletionEvent)
            {
                pThis->add_multiplayer_event_helper(
                    xbox_live_error_code::no_error,
                    std::string(),
                    multiplayer_event_type::leave_game_completed,
                    std::make_shared<multiplayer_event_args>()
                );

                pThis->remove_from_processing_queue(processingRequest->identifier());
            }
        }
    });
}

void
multiplayer_game_client::add_multiplayer_event_helper(
    _In_ const std::error_code& errorCode,
    _In_ const std::string& errorMessage,
    _In_ multiplayer_event_type eventType,
    _In_ std::shared_ptr<multiplayer_event_args> eventArgs,
    _In_opt_ context_t context
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    multiplayer_event multiplayerEvent(
        errorCode,
        errorMessage,
        eventType,
        eventArgs,
        multiplayer_session_type::game_session,
        context
        );

    m_multiplayerEventQueue.push_back(multiplayerEvent);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
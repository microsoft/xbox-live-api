// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN
const string_t multiplayer_lobby_client::c_transferHandlePropertyName = _T("GameSessionTransferHandle");
const string_t multiplayer_lobby_client::c_joinabilityPropertyName = _T("Joinability");
#if UNIT_TEST_SERVICES
const std::chrono::seconds RETRY_LENGTH(0);
#else
const std::chrono::seconds RETRY_LENGTH(1);
#endif
const int MAX_CONNECTION_ATTEMPTS = 3;

multiplayer_lobby_client::multiplayer_lobby_client() :
    m_pendingCommitInProgress(false),
    m_updateNumber(0),
    m_joinability(joinability::none)
{
    m_sessionWriter = std::make_shared<multiplayer_session_writer>();
}

multiplayer_lobby_client::multiplayer_lobby_client(
    _In_ string_t lobbySessionTemplateName,
    _In_ std::shared_ptr<multiplayer_local_user_manager> localUserManager
    ) :
    m_lobbySessionTemplateName(std::move(lobbySessionTemplateName)),
    m_multiplayerLocalUserManager(localUserManager),
    m_pendingCommitInProgress(false),
    m_updateNumber(0),
    m_joinability(joinability::none)
{
    m_sessionWriter = std::make_shared<multiplayer_session_writer>(localUserManager);
}

multiplayer_lobby_client::~multiplayer_lobby_client()
{
    m_sessionWriter.reset();
}

void
multiplayer_lobby_client::initialize()
{
    std::weak_ptr<multiplayer_lobby_client> thisWeakPtr = shared_from_this();
    m_sessionWriter->add_multiplayer_session_updated_handler([thisWeakPtr](_In_ const std::shared_ptr<multiplayer_session>& updatedSession)
    {
        std::shared_ptr<multiplayer_lobby_client> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->update_session(updatedSession);
        }
    });
}

void multiplayer_lobby_client::deep_copy_if_updated(
    _In_ const multiplayer_lobby_client& other
    )
{
    std::lock_guard<std::mutex> lock(other.m_clientRequestLock);

    m_joinability = other.m_joinability;
    if (other.m_sessionWriter->session() == nullptr)
    {
        m_sessionWriter->update_session(nullptr);
        m_multiplayerLobby = nullptr;
    }
    else if (other.m_multiplayerLobby == nullptr)
    {
        m_multiplayerLobby = nullptr;
    }
    else if (m_sessionWriter->session() == nullptr ||
            other.m_sessionWriter->session()->change_number() > m_sessionWriter->session()->change_number() ||
            other.m_sessionWriter->session()->e_tag() > m_sessionWriter->session()->e_tag())
    {
        m_sessionWriter->update_session(other.m_sessionWriter->session()->_Create_deep_copy());
        m_multiplayerLobby = other.m_multiplayerLobby->_Create_deep_copy();
    }
    else if (m_updateNumber != other.m_updateNumber)
    {
        m_multiplayerLobby = other.m_multiplayerLobby->_Create_deep_copy();
    }
}

std::shared_ptr<multiplayer_game_client>
multiplayer_lobby_client::game_client()
{
    return multiplayer_manager::get_singleton_instance()->_Game_client();
}

std::shared_ptr<multiplayer_session>
multiplayer_lobby_client::game_session()
{
    auto mpInstance = multiplayer_manager::get_singleton_instance();
    if (mpInstance->_Game_client() == nullptr)
    {
        return nullptr;
    }

    return mpInstance->_Game_client()->session();
}

const std::shared_ptr<multiplayer_session_writer>&
multiplayer_lobby_client::session_writer() const
{
    return m_sessionWriter;
}

const std::shared_ptr<multiplayer_lobby_session>&
multiplayer_lobby_client::lobby() const
{
    return m_multiplayerLobby;
}

void
multiplayer_lobby_client::update_lobby(
    _In_ std::shared_ptr<multiplayer_lobby_session> multiplayerLobby
    )
{
    ++m_updateNumber;
    m_multiplayerLobby = multiplayerLobby;
}

const std::shared_ptr<multiplayer_session>&
multiplayer_lobby_client::session() const
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    return m_sessionWriter->session();
}

void
multiplayer_lobby_client::update_session(
    _In_ const std::shared_ptr<multiplayer_session>& updatedSession
    )
{
    auto cachedSession = session();
    if (updatedSession == nullptr || cachedSession == nullptr ||
        updatedSession->change_number() > cachedSession->change_number() ||
        updatedSession->e_tag() != cachedSession->e_tag())
    {
        update_lobby_session(updatedSession);
    }
}

void 
multiplayer_lobby_client::update_lobby_session(
    _In_ const std::shared_ptr<multiplayer_session>& updatedSession
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_sessionWriter->update_session(updatedSession);
}

void
multiplayer_lobby_client::update_objects(
    const std::shared_ptr<multiplayer_session>& updatedSession, 
    const std::shared_ptr<multiplayer_session>& gameSession
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    if (updatedSession == nullptr)
    {
        update_lobby(nullptr);
        m_localLobbyMembers.clear();
        m_joinability = joinability::none;
    }
    else
    {
        update_local_lobby_members(updatedSession, gameSession);
        auto multiplayerLobby = convert_to_multiplayer_lobby(updatedSession, gameSession);
        update_lobby(multiplayerLobby);
        m_joinability = multiplayer_manager_utils::get_joinability(updatedSession->session_properties());
    }
}

void
multiplayer_lobby_client::update_local_lobby_members(
    _In_ const std::shared_ptr<multiplayer_session>& updatedLobbySession,
    _In_ const std::shared_ptr<multiplayer_session>& gameSession
    )
{
    m_localLobbyMembers.clear();
    if (updatedLobbySession == nullptr)
    {
        return;
    }

    std::vector<std::shared_ptr<multiplayer_member>> localLobbyGameMembers = std::vector<std::shared_ptr<multiplayer_member>>();
    
    auto xboxLiveContextMap = m_multiplayerLocalUserManager->get_local_user_map();
    for(auto xboxLiveContext : xboxLiveContextMap)
    {
        auto localUser =  xboxLiveContext.second;
        if (localUser != nullptr)
        {
            std::shared_ptr<multiplayer_session_member> member = multiplayer_manager_utils::get_player_in_session(localUser->xbox_user_id(), updatedLobbySession);
            if (member != nullptr)
            {
                auto localMember = multiplayer_manager_utils::convert_to_game_member(member, updatedLobbySession, gameSession, true);
                localLobbyGameMembers.push_back(localMember);
            }
        }
    }

    m_localLobbyMembers = localLobbyGameMembers;
}

std::shared_ptr<multiplayer_lobby_session>
multiplayer_lobby_client::convert_to_multiplayer_lobby(
    _In_ const  std::shared_ptr<multiplayer_session>& sessionToConvert,
    _In_ const  std::shared_ptr<multiplayer_session>& gameSession
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
            sessionToConvert,
            gameSession,
            m_multiplayerLocalUserManager->get_local_user_map()
            );
        if (!member->device_token().empty() && 
            utils::str_icmp(member->device_token(), sessionToConvert->session_properties()->host_device_token()) == 0)
        {
            hostMember = gameMember;
        }

        gameMembers.push_back(gameMember);
    }

    return std::make_shared<multiplayer_lobby_session>(
        sessionToConvert,
        hostMember,
        gameMembers,
        m_localLobbyMembers
        );
}

multiplayer::manager::joinability
multiplayer_lobby_client::joinability()
{
    return m_joinability;
}

const std::vector<multiplayer_event>&
multiplayer_lobby_client::multiplayer_event_queue()
{
    // Don't require a lock as STL is multithread read safe
    return m_multiplayerEventQueue;
}

void
multiplayer_lobby_client::clear_pending_queue()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    while (!m_pendingRequestQueue.empty())
    {
        m_pendingRequestQueue.pop();
    }
}

void
multiplayer_lobby_client::add_to_pending_queue(
    _In_ std::shared_ptr<multiplayer_client_pending_request> pendingRequest
    )
{
    // No lock required. Methods that call this already has a lock
    m_pendingRequestQueue.push(pendingRequest);
}

void
multiplayer_lobby_client::add_to_processing_queue(
    _In_ std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue
    )
{
    for (const auto& request : processingQueue)
    {
        m_processingQueue.push_back(request);
    }
}

void
multiplayer_lobby_client::remove_from_processing_queue(
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
multiplayer_lobby_client::get_processing_queue()
{
    return m_processingQueue;
}

xbox_live_result<void>
multiplayer_lobby_client::add_local_user(
    _In_ xbox_live_user_t user,
    _In_ multiplayer_local_user_lobby_state userState,
    _In_ const string_t& handleId,
    _In_ const xbox::services::multiplayer::multiplayer_session_reference& sessionRef
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    if (user == nullptr) return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Invalid user argument passed");

    auto localUser = m_multiplayerLocalUserManager->get_local_user_helper(user);
    RETURN_CPP_IF(localUser != nullptr && userState == multiplayer_local_user_lobby_state::add, void, xbox_live_error_code::logic_error, "User already added.");
    
    if (localUser == nullptr)
    {
        localUser = m_multiplayerLocalUserManager->add_user_to_xbox_live_context_to_map(user);
    }

    auto pendingRequest = std::make_shared<multiplayer_client_pending_request>();
    pendingRequest->set_local_user(localUser);
    pendingRequest->set_lobby_state(userState);
    if (userState == multiplayer_local_user_lobby_state::join)
    {
        if (!handleId.empty())
        {
            pendingRequest->set_lobby_handle_id(handleId);
        }
        else if(!sessionRef.is_null())
        {
            pendingRequest->set_team_session_reference(sessionRef);
        }
    }
    add_to_pending_queue(pendingRequest);

    return xbox_live_result<void>();
}

void
multiplayer_lobby_client::add_local_users(
    _In_ std::vector<xbox_live_user_t> users,
    _In_ const string_t& handleId
    )
{
    for (const auto& user : users)
    {
        add_local_user(user, multiplayer_local_user_lobby_state::join, handleId);
    }
}


void
multiplayer_lobby_client::add_local_users(
    _In_ std::vector<xbox_live_user_t> users,
    _In_ const multiplayer_session_reference& sessionRef
    )
{
    for (const auto& user : users)
    {
        add_local_user(user, multiplayer_local_user_lobby_state::join, string_t(), sessionRef);
    }
}

xbox_live_result<void>
multiplayer_lobby_client::remove_local_user(
    _In_ xbox_live_user_t user
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    if (user == nullptr) return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Invalid user argument passed");

    auto localUser = m_multiplayerLocalUserManager->get_local_user_helper(user);
    RETURN_CPP_IF(localUser == nullptr || localUser->context() == nullptr, void, xbox_live_error_code::logic_error, "Call add_local_user() first.");

    auto pendingRequest = std::make_shared<multiplayer_client_pending_request>();
    pendingRequest->set_local_user(localUser);
    pendingRequest->set_lobby_state(multiplayer_local_user_lobby_state::leave);
    add_to_pending_queue(pendingRequest);

    return xbox_live_result<void>();
}

void
multiplayer_lobby_client::remove_all_local_users()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    if (m_multiplayerLocalUserManager == nullptr) return;

    const auto& xboxLiveContextMap = m_multiplayerLocalUserManager->get_local_user_map();
    for (auto xboxLiveContext : xboxLiveContextMap)
    {
        const auto& localUser = xboxLiveContext.second;
        if (localUser != nullptr)
        {
            auto pendingRequest = std::make_shared<multiplayer_client_pending_request>();
            pendingRequest->set_local_user(localUser);
            pendingRequest->set_lobby_state(multiplayer_local_user_lobby_state::leave);
            add_to_pending_queue(pendingRequest);
        }
    }
}

xbox_live_result<void>
multiplayer_lobby_client::set_local_member_properties(
    _In_ xbox_live_user_t user,
    _In_ string_t name,
    _In_ web::json::value valueJson,
    _In_opt_ context_t context
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    if (user == nullptr) return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Invalid user argument passed");

    auto localUser = m_multiplayerLocalUserManager->get_local_user_helper(user);
    RETURN_CPP_IF(localUser == nullptr || localUser->context() == nullptr, void, xbox_live_error_code::logic_error, "Call add_local_user() before setting local member properties.");

    auto pendingRequest = std::make_shared<multiplayer_client_pending_request>();
    pendingRequest->set_local_user_properties(localUser, name, valueJson, context);
    add_to_pending_queue(pendingRequest);

    return xbox_live_result<void>();
}

xbox_live_result<void>
multiplayer_lobby_client::delete_local_member_properties(
    _In_ xbox_live_user_t user,
    _In_ const string_t& name,
    _In_opt_ context_t context
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    if (user == nullptr) return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Invalid user argument passed");

    auto localUser = m_multiplayerLocalUserManager->get_local_user_helper(user);
    RETURN_CPP_IF(localUser == nullptr || localUser->context() == nullptr, void, xbox_live_error_code::logic_error, "Call add_local_user() before deleting local member properties.");

    auto pendingRequest = std::make_shared<multiplayer_client_pending_request>();
    pendingRequest->set_local_user_properties(localUser, name, web::json::value::null(), context);
    add_to_pending_queue(pendingRequest);

    return xbox_live_result<void>();
}

xbox_live_result<void>
multiplayer_lobby_client::set_local_member_connection_address(
    _In_ xbox_live_user_t user,
    _In_ string_t address,
    _In_opt_ context_t context
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    if (user == nullptr) return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Invalid user argument passed");

    auto localUser = m_multiplayerLocalUserManager->get_local_user_helper(user);
    RETURN_CPP_IF(localUser == nullptr || localUser->context() == nullptr, void, xbox_live_error_code::logic_error, "Call add_local_user() before setting local member connection address.");

    auto pendingRequest = std::make_shared<multiplayer_client_pending_request>();
    pendingRequest->set_local_user_connection_address(localUser, address, context);
    add_to_pending_queue(pendingRequest);

    return xbox_live_result<void>();
}

xbox_live_result<void> multiplayer_lobby_client::set_joinability(
    _In_ xbox::services::multiplayer::manager::joinability value,
    _In_opt_ context_t context
    )
{
    if (value < joinability::joinable_by_friends || value > joinability::closed)
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Join restriction enum out of range");
    }

    auto pendingRequest = std::make_shared<multiplayer_client_pending_request>();
    pendingRequest->set_joinability(value, context);
    add_to_pending_queue(pendingRequest);

    return xbox_live_result<void>();
}

std::vector<multiplayer_event>
multiplayer_lobby_client::do_work()
{
    bool expected = false;
    if (m_pendingCommitInProgress.compare_exchange_strong(expected, true))
    {
        if (m_pendingRequestQueue.size() > 0)
        {
            std::vector<std::shared_ptr<multiplayer_client_pending_request>> processingQueue;
            multiplayer_session_reference teamSessionRef;
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
                        if (pendingRequest->request_type() != m_pendingRequestQueue.front()->request_type())
                        {
                            doneProcessing = true;
                        }
                    }

                    if (!applySynchronizedChanges && pendingRequest->request_type() == pending_request_type::synchronized_changes)
                    {
                        applySynchronizedChanges = true;
                    }

                    if (pendingRequest->local_user() != nullptr)
                    {
                        auto lobbyState = pendingRequest->lobby_state();
                        if (lobbyState == multiplayer_local_user_lobby_state::join)
                        {
                            lobbyStateIsJoin = true;

                            // Leave existing lobby without updating the latest as the leave may comeback after the actual join and overwrite it.
                            auto latestSession = m_sessionWriter->session();
                            if (latestSession != nullptr)
                            {
                                leave_remote_session(latestSession);
                            }

                            m_sessionWriter->update_session(nullptr);
                            update_lobby(nullptr);
                            m_localLobbyMembers.clear();
                            m_joinability = joinability::none;

                            if (!pendingRequest->lobby_handle_id().empty())
                            {
                                pendingRequest->local_user()->set_lobby_handle_id(pendingRequest->lobby_handle_id());
                                joinByHandleId = true;
                            }
                            else if (!pendingRequest->team_session_reference().is_null())
                            {
                                teamSessionRef = pendingRequest->team_session_reference();
                            }
                        }

                        if (lobbyState != multiplayer_local_user_lobby_state::unknown)
                        {
                            pendingRequest->local_user()->set_lobby_state(lobbyState);
                        }
                        
                        pendingRequest->local_user()->set_write_changes_to_service(true);
                    }
                }

            } while (!doneProcessing && m_pendingRequestQueue.size() > 0);

            if (processingQueue.size() > 0)
            {
                add_to_processing_queue(processingQueue);
                pplx::task<xbox_live_result<std::vector<multiplayer_event>>> asyncOp;
                if (applySynchronizedChanges)
                {
                    asyncOp = m_sessionWriter->commit_pending_synchronized_changes(processingQueue, multiplayer_session_type::lobby_session);
                }
                else
                {
                    std::vector<string_t> xuidsInOrder;

                    if (lobbyStateIsJoin)
                    {
                        // If users are joining from an invite than the first multiplayer_client_pending_request in processingQueue references was the invited user.
                        // We want the invited user to join first since it is possible that users in the local graph might not meet the criteria
                        // to join the invited session until the invited user joins. Imagine that the inviting session has the session privacy set to joinable_by_friends
                        // and only the invited user is a friend of the inviting user. If any additional users attempt to join the inviting session before the invited user
                        // than the join fails for the whole list of users.
                        for (auto pendingRequest : processingQueue)
                        {
                            xuidsInOrder.push_back(pendingRequest->local_user()->xbox_user_id());
                        }
                    }

                    asyncOp = commit_pending_lobby_changes(xuidsInOrder, joinByHandleId, teamSessionRef);
                }

                std::weak_ptr<multiplayer_lobby_client> thisWeakPtr = shared_from_this();

                create_task(asyncOp)
                .then([thisWeakPtr, processingQueue](xbox_live_result<std::vector<multiplayer_event>> result)
                {
                    std::shared_ptr<multiplayer_lobby_client> pThis(thisWeakPtr.lock());
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
multiplayer_lobby_client::is_pending_lobby_changes()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    if (m_pendingRequestQueue.size() > 0 || is_pending_lobby_local_user_changes())
    {
        return true;
    }

    if (m_sessionWriter != nullptr)
    {
        auto latestSession = m_sessionWriter->session();
        if (latestSession != nullptr &&
            m_multiplayerLobby != nullptr &&
            latestSession->change_number() != m_multiplayerLobby->_Change_number())
        {
            return true;
        }
    }

    return false;
}

bool
multiplayer_lobby_client::is_request_in_progress()
{
    return m_pendingRequestQueue.size() > 0 || m_processingQueue.size() > 0 || m_multiplayerEventQueue.size() > 0;
}

bool
multiplayer_lobby_client::is_pending_lobby_local_user_changes()
{
    const auto& xboxLiveContextMap = m_multiplayerLocalUserManager->get_local_user_map();
    for (auto xboxLiveContext : xboxLiveContextMap)
    {
        const auto& localUser = xboxLiveContext.second;
        if (localUser != nullptr && localUser->write_changes_to_service())
        {
            return true;
        }
    }

    return false;
}

pplx::task<xbox_live_result<std::vector<multiplayer_event>>>
multiplayer_lobby_client::commit_pending_lobby_changes(
    _In_ std::vector<string_t> xuidsInOrder,
    _In_ bool joinByHandleId,
    _In_ xbox::services::multiplayer::multiplayer_session_reference sessionRef
    )
{
    if (is_pending_lobby_local_user_changes())
    {
        // All local member changes always happen on the lobby session.
        std::shared_ptr<multiplayer_session> latestLobbySession;
        auto lobbySession = session();
        if (lobbySession == nullptr)
        {
            auto primaryContext  = m_multiplayerLocalUserManager->get_primary_context();
            if (joinByHandleId)
            {
                latestLobbySession = std::make_shared<multiplayer_session>(utils::string_t_from_internal_string(primaryContext->xbox_live_user_id()));
            }
            else
            {
                if (sessionRef.is_null())
                {
                    string_t sessionName = utils::string_t_from_internal_string(utils::create_guid(true));
                    sessionRef = multiplayer_session_reference(
                        utils::string_t_from_internal_string(utils::try_get_override_scid()),
                        m_lobbySessionTemplateName,
                        sessionName
                        );
                }

                latestLobbySession = std::make_shared<multiplayer_session>(utils::string_t_from_internal_string(primaryContext->xbox_live_user_id()), sessionRef);
            }
        }
        else
        {
            latestLobbySession = lobbySession->_Create_deep_copy();
        }

        // Committing  local user changes will also update any pending lobby properties.
        return commit_lobby_changes(xuidsInOrder, latestLobbySession);
    }

    bool isGameInProgress = game_session() != nullptr;
    return m_sessionWriter->commit_pending_changes(get_processing_queue(), multiplayer_session_type::lobby_session, isGameInProgress);
}

pplx::task<xbox_live_result<std::vector<multiplayer_event>>>
multiplayer_lobby_client::commit_lobby_changes(
    _In_ std::vector<string_t> xuidsInOrder,
    _In_ std::shared_ptr<multiplayer_session> lobbySessionToCommit
    )
{
    std::weak_ptr<multiplayer_lobby_client> thisWeakPtr = shared_from_this();
    auto task = pplx::create_task([thisWeakPtr, xuidsInOrder, lobbySessionToCommit]()
    {
        std::shared_ptr<multiplayer_lobby_client> pThis(thisWeakPtr.lock());
        RETURN_CPP_IF(pThis == nullptr, std::vector<multiplayer_event>, xbox_live_error_code::generic_error, "multiplayer_lobby_client class was destroyed.");

        return pThis->commit_lobby_changes_helper(xuidsInOrder, lobbySessionToCommit);
    });

    return utils::create_exception_free_task<std::vector<multiplayer_event>>(task);
}

xbox_live_result<std::vector<multiplayer_event>>
multiplayer_lobby_client::commit_lobby_changes_helper(
    _In_ std::vector<string_t> xuids,
    _In_ std::shared_ptr<multiplayer_session> lobbySession
    )
{
    auto sessionRefToCommit = lobbySession->session_reference();

    uint32_t count = 0;
    bool removeStaleUsers = false;
    auto xboxLiveContextMap = m_multiplayerLocalUserManager->get_local_user_map();

    if (xuids.empty())
    {
        for (auto xboxLiveContext : xboxLiveContextMap)
        {
            xuids.push_back(xboxLiveContext.first);
        }
    }

    for(auto xuid : xuids)
    {
        auto it = xboxLiveContextMap.find(xuid);
        if (it == xboxLiveContextMap.end()) 
        {
            continue;
        }

        auto localUser =  it->second;
        if (localUser != nullptr && localUser->write_changes_to_service())
        {
            auto lobbySessionToCommit = std::make_shared<multiplayer_session>(localUser->xbox_user_id(), sessionRefToCommit);
            switch (localUser->lobby_state())
            {
            case multiplayer_local_user_lobby_state::add:
                {
                    lobbySessionToCommit->join();
                    lobbySessionToCommit->set_current_user_secure_device_address_base64(localUser->connection_address());
                    lobbySessionToCommit->set_session_change_subscription(multiplayer_session_change_types::everything);

                    // Only set this for the first user. Could cause a race condition if joinability was changed during the second write.
                    if (count == 0)
                    {
                        count++;
                        lobbySessionToCommit->session_properties()->set_join_restriction(multiplayer_session_restriction::followed);
                        lobbySessionToCommit->session_properties()->set_read_restriction(multiplayer_session_restriction::followed);

                        string_t jsonValueStr = multiplayer_manager_utils::convert_joinability_to_string(joinability::joinable_by_friends);
                        lobbySessionToCommit->set_session_custom_property_json(c_joinabilityPropertyName, web::json::value::string(jsonValueStr));
                        break;
                    }
                }

            case multiplayer_local_user_lobby_state::join:
                {
                    lobbySessionToCommit->join();

                    lobbySessionToCommit->set_current_user_secure_device_address_base64(localUser->connection_address());
                    lobbySessionToCommit->set_session_change_subscription(multiplayer_session_change_types::everything);
                    break;
                }

            case multiplayer_local_user_lobby_state::in_session:
                {
                    // Forces to set the current user to yourself.
                    lobbySessionToCommit->join();
                    break;
                }
            case multiplayer_local_user_lobby_state::leave:
                {
                    if(multiplayer_manager_utils::is_player_in_session(localUser->xbox_user_id(), lobbySession))
                    {
                        lobbySessionToCommit->leave();
                    }
                    else
                    {
                        // In a scenario where the user was removed before he could have been added.
                        user_state_changed(xbox_live_error_code::logic_error, "User was removed before he could be added", multiplayer_local_user_lobby_state::add, localUser->xbox_user_id());
                        user_state_changed(xbox_live_error_code::no_error, std::string(), multiplayer_local_user_lobby_state::leave, localUser->xbox_user_id());

                        removeStaleUsers = true;
                        localUser->set_lobby_state(multiplayer_local_user_lobby_state::remove);
                        continue;
                    }
                    break;
                }

            default:
                break;
            }

            bool isGameInProgress = game_session() != nullptr;

            // Update any pending local user or lobby session properties.
            auto processingQueue = get_processing_queue();
            for(auto& request : processingQueue)
            {
                request->append_pending_changes(lobbySessionToCommit, localUser, isGameInProgress);
            }

            std::weak_ptr<multiplayer_lobby_client> thisWeakPtr = shared_from_this();
            pplx::task<xbox_live_result<std::shared_ptr<multiplayer_session>>> writeSessionOp;

            if (localUser->lobby_handle_id().empty())
            {
                writeSessionOp = m_sessionWriter->write_session(localUser->context(), lobbySessionToCommit, multiplayer_session_write_mode::update_or_create_new);
            }
            else
            {
                writeSessionOp = m_sessionWriter->write_session_by_handle(localUser->context(), lobbySessionToCommit, multiplayer_session_write_mode::update_or_create_new, localUser->lobby_handle_id());
                localUser->set_lobby_handle_id(string_t());
            }

            auto writeSessionTask = pplx::create_task(writeSessionOp)
            .then([thisWeakPtr, localUser, sessionRefToCommit, processingQueue, &removeStaleUsers](xbox_live_result<std::shared_ptr<multiplayer_session>> sessionResult)
            {
                std::shared_ptr<multiplayer_lobby_client> pThis(thisWeakPtr.lock());
                RETURN_CPP_IF(pThis == nullptr, std::vector<multiplayer_event>, xbox_live_error_code::generic_error, "multiplayer_lobby_client class was destroyed.");

                pThis->user_state_changed(sessionResult.err(), sessionResult.err_message(), localUser->lobby_state(), localUser->xbox_user_id());
                pThis->handle_lobby_change_events(sessionResult.err(), sessionResult.err_message(), localUser, processingQueue);
                
                if (sessionResult.err())
                {
                    pThis->join_lobby_completed(sessionResult.err(), sessionResult.err_message(), string_t());
                    return xbox_live_result<std::vector<multiplayer_event>>(sessionResult.err(), sessionResult.err_message());
                }

                if (localUser->lobby_state() == multiplayer_local_user_lobby_state::add ||
                    localUser->lobby_state() == multiplayer_local_user_lobby_state::join)
                {
                    auto updatedSession = sessionResult.payload();
                    pThis->update_session(updatedSession);
                    auto lobbyState = localUser->lobby_state();
                    localUser->set_lobby_state(multiplayer_local_user_lobby_state::in_session);

                    if (lobbyState == multiplayer_local_user_lobby_state::join)
                    {
                        pThis->handle_join_lobby_completed(sessionResult.err(), sessionResult.err_message());
                    }

                    auto setActivityResult = localUser->context()->multiplayer_service().set_activity(updatedSession->session_reference()).get();
                    if (setActivityResult.err())
                    {
                        return xbox_live_result<std::vector<multiplayer_event>>(setActivityResult.err(), setActivityResult.err_message());
                    }

                    if (lobbyState == multiplayer_local_user_lobby_state::add &&
                        pThis->should_update_host_token(localUser, updatedSession))
                    {
                        updatedSession->set_host_device_token(updatedSession->current_user()->device_token());
                        auto writeHostTokenResult = pThis->m_sessionWriter->write_session(localUser->context(), updatedSession, multiplayer_session_write_mode::update_existing).get();
                        if (writeHostTokenResult.err())
                        {
                            return xbox_live_result<std::vector<multiplayer_event>>(writeHostTokenResult.err(), writeHostTokenResult.err_message());
                        }
                    }
                }
                else if (localUser->lobby_state() == multiplayer_local_user_lobby_state::leave)
                {
                    removeStaleUsers = true;

                    // If you leave the session you were advertising, you don't need to clear the activity.
                    localUser->set_lobby_state(multiplayer_local_user_lobby_state::remove);
                }

                return xbox_live_result<std::vector<multiplayer_event>>();

            }).get();  // write_session

            if (writeSessionTask.err())
            {
                return xbox_live_result<std::vector<multiplayer_event>>(writeSessionTask.err(), writeSessionTask.err_message());
            }
        }   // end if
    }   // end for

    if (removeStaleUsers)
    {
        remove_stale_xbox_live_context_from_map();
    }

    return xbox_live_result<std::vector<multiplayer_event>>();
}

void
multiplayer_lobby_client::remove_stale_xbox_live_context_from_map()
{
    auto gameClient = game_client();
    if (gameClient != nullptr)
    {
        gameClient->remove_stale_users_from_remote_session();
    }

    // Remove stale context, switch primary context and re-activate multiplayer events.
    m_multiplayerLocalUserManager->remove_stale_local_users_from_map();
}

std::map<string_t, std::shared_ptr<multiplayer_local_user>>
multiplayer_lobby_client::get_local_user_map()
{
    return m_multiplayerLocalUserManager->get_local_user_map();
}

std::shared_ptr<xbox_live_context_impl>
multiplayer_lobby_client::get_primary_context()
{
    return m_multiplayerLocalUserManager->get_primary_context();
}

xbox_live_result<void>
multiplayer_lobby_client::create_game_from_lobby()
{
    auto lobbySession = session();
    RETURN_CPP_IF(lobbySession == nullptr, void, xbox_live_error_code::logic_error, "No lobby session exists. Call add_local_user() to create a lobby first");

    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    RETURN_CPP_IF(primaryContext == nullptr, void, xbox_live_error_code::logic_error, "Call add_local_user() before joining.");

    std::weak_ptr<multiplayer_lobby_client> thisWeakPtr = shared_from_this();
    pplx::create_task([thisWeakPtr, lobbySession, primaryContext]()
    {
        std::shared_ptr<multiplayer_lobby_client> pThis(thisWeakPtr.lock());
        RETURN_CPP_IF(pThis == nullptr, void, xbox_live_error_code::generic_error, "multiplayer_lobby_client class was destroyed.");

        // Try to write transfer handle to the lobby as pending state
        // If succeeded, then create the game session, set the transfer handle, and advertise the game session.
        // If failed, wait for the property changed event.

        int attempts = 0;
        xbox_live_result<std::shared_ptr<multiplayer_session>> commitResult;
        auto sessionToCommitCopy  = lobbySession->_Create_deep_copy();
        while (attempts < MAX_CONNECTION_ATTEMPTS)
        {
            string_t jsonValue;
            jsonValue = _T("pending~") + utils::string_t_from_internal_string(primaryContext->xbox_live_user_id());
            sessionToCommitCopy->set_session_custom_property_json(multiplayer_lobby_client::c_transferHandlePropertyName, web::json::value::string(jsonValue));

            commitResult = pThis->m_sessionWriter->commit_synchronized_changes(sessionToCommitCopy).get();
            auto gameClient = pThis->game_client();
            RETURN_CPP_IF(gameClient == nullptr, void, xbox_live_error_code::generic_error, "multiplayer_game_client class was destroyed.");

            if (commitResult.err() == xbox_live_error_condition::http_412_precondition_failed)
            {
                if (pThis->is_transfer_handle_state(_T("completed")) || pThis->is_transfer_handle_state(_T("pending")))
                {
                    return gameClient->join_game_from_lobby_helper();
                }
                else
                {
                    attempts++;
                    sessionToCommitCopy = commitResult.payload();
                    std::this_thread::sleep_for(RETRY_LENGTH);
                    continue;
                }
            }

            auto sessionName = utils::string_t_from_internal_string(utils::create_guid(true));
            RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(gameClient->join_game_helper(sessionName), void);
        }

        pThis->update_session(sessionToCommitCopy);
        pThis->join_lobby_completed(commitResult.err(), commitResult.err_message(), string_t());
        return xbox_live_result<void>(commitResult.err(), commitResult.err_message());
    });

    return xbox_live_result<void>();
}

void
multiplayer_lobby_client::advertise_game_session()
{
    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    if (primaryContext == nullptr || game_session() == nullptr)
    {
        return;
    }

    std::weak_ptr<multiplayer_lobby_client> thisWeakPtr = shared_from_this();
    pplx::create_task([thisWeakPtr, primaryContext]()
    {
        std::shared_ptr<multiplayer_lobby_client> pThis(thisWeakPtr.lock());
        if (pThis == nullptr) return;

        bool expected = false;
        while (!pThis->m_pendingCommitInProgress.compare_exchange_strong(expected, true))
        {
            expected = false;
            utils::sleep(0);
        }

        auto lobbySession = pThis->session();
        if (pThis->game_session() == nullptr) return;

        // If the advertising fails, we simply eat the error as it isn't actionable for the title.
        if (lobbySession == nullptr)
        {
            if (pThis->m_multiplayerLocalUserManager->get_local_user_map().size() == 0)
            {
                return;
            }

            pThis->m_multiplayerLocalUserManager->change_all_local_user_lobby_state(multiplayer_local_user_lobby_state::add);
            auto joinLobbyResult = pThis->commit_pending_lobby_changes(std::vector<string_t>(), false).get();

            pThis->m_pendingCommitInProgress = false;
            pThis->join_lobby_completed(joinLobbyResult.err(), joinLobbyResult.err_message(), string_t());
            if (joinLobbyResult.err())
            {
                return;
            }

            lobbySession = pThis->session();
        }
        else
        {
            pThis->m_pendingCommitInProgress = false;
        }

        auto lobbyProperties = lobbySession->session_properties()->session_custom_properties_json();
        if (!lobbyProperties.has_field(c_transferHandlePropertyName) ||
            (pThis->is_transfer_handle_state(_T("pending")) && pThis->get_transfer_handle() == utils::string_t_from_internal_string(primaryContext->xbox_live_user_id())))
        {
            auto gameSession = pThis->game_session();
            if (gameSession == nullptr) return;

            auto result = primaryContext->multiplayer_service().set_transfer_handle(gameSession->session_reference(), lobbySession->session_reference()).get();
            if (!result.err())
            {
                auto handleId = result.payload();
                auto lobbySessionCopy = lobbySession->_Create_deep_copy();

                if (pThis->m_joinability == joinability::disable_while_game_in_progress)
                {
                    lobbySessionCopy->set_closed(true);
                }

                stringstream_t jsonHandleValue;
                jsonHandleValue << "completed~";
                jsonHandleValue << handleId;
                lobbySessionCopy->set_session_custom_property_json(c_transferHandlePropertyName, web::json::value::string(jsonHandleValue.str()));
                pThis->m_sessionWriter->write_session(primaryContext, lobbySessionCopy, multiplayer_session_write_mode::update_existing);
            }
            else if (result.err() == xbox_live_error_code::http_status_403_forbidden)
            {
                // By MPSD design, if the game session doesn't exist on transfer handle creation, it throws a 403.
                pThis->clear_game_session_from_lobby();
            }
        }
    });
}

void
multiplayer_lobby_client::stop_advertising_game_session(
    _In_ xbox_live_result<std::shared_ptr<multiplayer_session>> result
    )
{
    bool bClearGameSession = false;
    auto lobbySession = session();
    if (!result.err() && result.payload() == nullptr)
    {
        // When you are the last person to leave the session (handling err 204).
        bClearGameSession = true;
    }
    else if (result.payload() != nullptr && lobbySession != nullptr)
    {
        auto latestGameSession = result.payload();

        bool found = false;
        auto members = lobbySession->members();
        for (const auto& member : members)
        {
            if (multiplayer_manager_utils::is_player_in_session(member->xbox_user_id(), latestGameSession))
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
        clear_game_session_from_lobby();
    }
}

void
multiplayer_lobby_client::clear_game_session_from_lobby()
{
    auto lobbySession = session();
    if (lobbySession != nullptr)
    {
        auto lobbySessionCopy = lobbySession->_Create_deep_copy();

        if (m_joinability == joinability::disable_while_game_in_progress)
        {
            // Re-open the lobby to be joinable when leaving the game session.
            lobbySessionCopy->set_closed(false);
        }

        lobbySessionCopy->delete_session_custom_property_json(c_transferHandlePropertyName);
        m_sessionWriter->write_session(m_multiplayerLocalUserManager->get_primary_context(), lobbySessionCopy, multiplayer_session_write_mode::update_existing);
    }
}

bool
multiplayer_lobby_client::is_transfer_handle_state(
    _In_ const string_t& state
    )
{
    auto lobbySession = session();
    if (lobbySession == nullptr)
    {
        return false;
    }

    auto lobbyProperties = lobbySession->session_properties()->session_custom_properties_json();
    if (lobbyProperties.has_field(c_transferHandlePropertyName))
    {
        auto transferHandleProp = lobbyProperties.at(c_transferHandlePropertyName).as_string();
        std::vector<string_t> transferHandleSplit = utils::string_split(transferHandleProp, '~');

        if (transferHandleSplit.size() > 0 &&
            utils::str_icmp(transferHandleSplit[0], state) == 0)
        {
            return true;
        }
    }

    return false;
}

string_t
multiplayer_lobby_client::get_transfer_handle()
{
    auto lobbySession = session();
    if (lobbySession == nullptr)
    {
        return string_t();
    }

    auto lobbyProperties = lobbySession->session_properties()->session_custom_properties_json();
    if (lobbyProperties.has_field(c_transferHandlePropertyName))
    {
        auto transferHandleProp = lobbyProperties.at(c_transferHandlePropertyName).as_string();
        std::vector<string_t> transferHandleSplit = utils::string_split(transferHandleProp, '~');

        if (transferHandleSplit.size() == 2)
        {
            return transferHandleSplit[1];
        }
    }

    return string_t();
}
void
multiplayer_lobby_client::leave_remote_session(
    _In_ std::shared_ptr<multiplayer_session> session
    )
{
    m_sessionWriter->leave_remote_session(session);
}

bool
multiplayer_lobby_client::should_update_host_token(
    _In_ std::shared_ptr<multiplayer_local_user> localUser,
    _In_ std::shared_ptr<multiplayer_session> session
    )
{
    if(multiplayer_manager_utils::host_member(session) != nullptr)
    {
        return false;
    }

    if(!localUser->is_primary_xbox_live_context())
    {
        return false;
    }

    // If the local user is already the host, skip it.
    if (multiplayer_manager_utils::is_host(localUser->xbox_user_id(), session))
    {
        return false;
    }

    return true;
}

void
multiplayer_lobby_client::user_state_changed(
    _In_ const std::error_code& errorCode,
    _In_ const std::string& errorMessage,
    _In_ multiplayer_local_user_lobby_state localUserLobbyState,
    _In_ string_t xboxUserId
    )
{
    if (localUserLobbyState != multiplayer_local_user_lobby_state::add &&
        localUserLobbyState != multiplayer_local_user_lobby_state::leave)
    {
        return;
    }

    std::shared_ptr<multiplayer_event_args> eventArgs;
    multiplayer_event_type eventType = multiplayer_event_type::user_added;
    if (localUserLobbyState == multiplayer_local_user_lobby_state::add)
    {
        eventType = multiplayer_event_type::user_added;
        std::shared_ptr<user_added_event_args> userAddedEventArgs = std::make_shared<user_added_event_args>(xboxUserId);
        eventArgs = std::dynamic_pointer_cast<user_added_event_args>(userAddedEventArgs);
    }
    else if (localUserLobbyState == multiplayer_local_user_lobby_state::leave)
    {
        eventType = multiplayer_event_type::user_removed;
        std::shared_ptr<user_removed_event_args> userRemovedEventArgs = std::make_shared<user_removed_event_args>(xboxUserId);
        eventArgs = std::dynamic_pointer_cast<user_removed_event_args>(userRemovedEventArgs);
    }
    else
    {
        // We only care about user_added & user_removed.
        return;
    }

    add_multiplayer_event_helper(errorCode, errorMessage, eventType, eventArgs);
}

void
multiplayer_lobby_client::handle_lobby_change_events(
    _In_ const std::error_code& errorCode,
    _In_ const std::string& errorMessage,
    _In_ std::shared_ptr<multiplayer_local_user> localUser,
    _In_ const std::vector<std::shared_ptr<multiplayer_client_pending_request>>& processingQueue
    )
{
    std::map<string_t, web::json::value> localUsersMap;
    string_t localUserConnectionAddress;
    for (auto& request : processingQueue)
    {
        // Handle local user change events
        if (localUser != nullptr && request->local_user() != nullptr &&
            utils::str_icmp(localUser->xbox_user_id(), request->local_user()->xbox_user_id()) == 0)
        {
            if (!request->local_user_connection_address().empty())
            {
                add_multiplayer_event_helper(
                    errorCode,
                    errorMessage,
                    multiplayer_event_type::local_member_connection_address_write_completed,
                    std::make_shared<multiplayer_event_args>(),
                    request->context()
                    );

                localUserConnectionAddress = request->local_user_connection_address();
            }

            // Fire events for each of the properties
            for (const auto& prop : request->local_user_properties())
            {
                add_multiplayer_event_helper(
                    errorCode,
                    errorMessage,
                    multiplayer_event_type::local_member_property_write_completed,
                    std::make_shared<multiplayer_event_args>(),
                    request->context()
                    );

                localUsersMap[prop.first] = prop.second;
            }
        }
    }

    // Handle lobby change events (session properties, etc)
    auto eventQueue = m_sessionWriter->handle_events(processingQueue, errorCode, errorMessage, multiplayer_session_type::lobby_session);
    if (eventQueue.size() > 0)
    {
        std::lock_guard<std::mutex> lock(m_clientRequestLock);
        for (auto& ev : eventQueue)
        {
            m_multiplayerEventQueue.push_back(ev);
        }
    }

    if (localUsersMap.size() != 0 || !localUserConnectionAddress.empty())
    {
        // write member properties to the game session.
        auto gameClient = game_client();
        if (gameClient != nullptr)
        {
            gameClient->set_local_member_properties_to_remote_session(localUser, localUsersMap, localUserConnectionAddress);
        }
    }
}

void
multiplayer_lobby_client::handle_join_lobby_completed(
    _In_ const std::error_code& errorCode,
    _In_ const std::string& errorMessage
    )
{
    join_lobby_completed(errorCode, errorMessage, string_t());
    if (!errorCode)
    {
        // If joining the lobby succeeded, check if it has a game session associated with it to join.

        auto gameClient = game_client();
        if (gameClient == nullptr) return;

        auto lobbySession = session();

        // If its a team session and it already has a GameSession ready to join
        if (lobbySession->session_constants()->capabilities_team() &&
            !lobbySession->tournaments_server().next_game_session_reference().is_null())
        {
            handle_game_session_ready_event(lobbySession);
            return;
        }

        // Join game via the transfer handle.
        auto lobbyProperties = lobbySession->session_properties()->session_custom_properties_json();
        if (lobbyProperties.size() > 0)
        {
            string_t transferHandle;

            if (is_transfer_handle_state(_T("completed")))
            {
                transferHandle = get_transfer_handle();
            }
            else
            {
                // No existing game session
                return;
            }

            gameClient->join_game_by_handle(transferHandle, false);
        }
    }
}

void
multiplayer_lobby_client::join_lobby_completed(
    _In_ const std::error_code& errorCode,
    _In_ const std::string& errorMessage,
    _In_ const string_t& invitedXboxUserId
    )
{
    std::shared_ptr<join_lobby_completed_event_args> joinLobbyEventArgs = std::make_shared<join_lobby_completed_event_args>(
        invitedXboxUserId
        );

    add_multiplayer_event_helper(
        errorCode,
        errorMessage,
        multiplayer_event_type::join_lobby_completed,
        std::dynamic_pointer_cast<join_lobby_completed_event_args>(joinLobbyEventArgs)
        );
}

void
multiplayer_lobby_client::handle_game_session_ready_event(
    _In_ const  std::shared_ptr<xbox::services::multiplayer::multiplayer_session>& lobbySession
    )
{
    auto lobbyTournamentsServer = lobbySession->tournaments_server();
    auto localStartTime = utility::datetime::utc_now() + utility::datetime::from_seconds(lobbyTournamentsServer.next_game_start_time() - lobbySession->date_of_session());
    auto gameSessionReadyEventArgs = std::make_shared<tournament_game_session_ready_event_args>(
        localStartTime
        );

    add_multiplayer_event_helper(
        xbox_live_error_code::no_error,
        std::string(),
        multiplayer_event_type::tournament_game_session_ready,
        gameSessionReadyEventArgs
    );

    auto gameClient = game_client();
    if (gameClient != nullptr)
    {
        gameClient->join_game_by_session_reference(lobbyTournamentsServer.next_game_session_reference());
    }
}

void
multiplayer_lobby_client::add_multiplayer_event_helper(
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
        multiplayer_session_type::lobby_session,
        context
        );

    m_multiplayerEventQueue.push_back(multiplayerEvent);
}
NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
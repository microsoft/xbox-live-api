// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

void multiplayer_client_pending_reader::deep_copy_if_updated(
    _In_ const multiplayer_client_pending_reader& other
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

multiplayer_client_pending_reader::multiplayer_client_pending_reader() :
    m_autoFillMembers(false)
{
    m_multiplayerLocalUserManager = std::make_shared<multiplayer_local_user_manager>();
    m_lobbyClient = std::make_shared<multiplayer_lobby_client>();
    m_gameClient = std::make_shared<multiplayer_game_client>();
    m_matchClient = std::make_shared<manager::multiplayer_match_client>(m_multiplayerLocalUserManager);
}

multiplayer_client_pending_reader::~multiplayer_client_pending_reader()
{
    m_lobbyClient.reset();
    m_gameClient.reset();
    m_matchClient.reset();
    m_multiplayerLocalUserManager.reset();
}

multiplayer_client_pending_reader::multiplayer_client_pending_reader(
    _In_ string_t lobbySessionTemplateName,
    _In_ std::shared_ptr<multiplayer_local_user_manager> localUserManager
    ) :
    m_multiplayerLocalUserManager(localUserManager),
    m_autoFillMembers(false)
{
    m_lobbyClient = std::make_shared<multiplayer_lobby_client>(lobbySessionTemplateName, m_multiplayerLocalUserManager);
    m_gameClient = std::make_shared<multiplayer_game_client>(m_multiplayerLocalUserManager);
    m_matchClient = std::make_shared<manager::multiplayer_match_client>(m_multiplayerLocalUserManager);
    m_lobbyClient->initialize();
    m_gameClient->initialize();
}

bool
multiplayer_client_pending_reader::is_update_avaialable(
    _In_ const multiplayer_client_pending_reader& other
    )
{
    std::lock_guard<std::mutex> lock(other.m_clientRequestLock);

    if (other.m_lobbyClient->is_pending_lobby_changes() ||
        other.m_gameClient->is_pending_game_changes())
    {
        return true;
    }

    auto lobbySession = m_lobbyClient->session();
    auto otherLobbySession = other.m_lobbyClient->session();
    auto gameSession = m_gameClient->session();
    auto otherGameSession = other.m_gameClient->session();
    if ( !multiplayer_manager_utils::compare_sessions(lobbySession, otherLobbySession) ||
         !multiplayer_manager_utils::compare_sessions(gameSession, otherGameSession) ||
         !multiplayer_manager_utils::compare_sessions(m_matchClient->session(), other.m_matchClient->session()) ||
         m_lobbyClient->multiplayer_event_queue().size() != other.m_lobbyClient->multiplayer_event_queue().size() ||
         m_gameClient->multiplayer_event_queue().size() != other.m_gameClient->multiplayer_event_queue().size() ||
         m_matchClient->multiplayer_event_queue().size() != other.m_matchClient->multiplayer_event_queue().size() ||
         m_multiplayerEventQueue.size() != other.m_multiplayerEventQueue.size())
    {
        return true;
    }

    return false;
}

std::shared_ptr<multiplayer_lobby_client>
multiplayer_client_pending_reader::lobby_client()
{
    return m_lobbyClient;
}

std::shared_ptr<multiplayer_game_client>
multiplayer_client_pending_reader::game_client()
{
    return m_gameClient;
}

std::shared_ptr<multiplayer_match_client>
multiplayer_client_pending_reader::match_client()
{
    return m_matchClient;
}

void 
multiplayer_client_pending_reader::update_session(
    _In_ multiplayer_session_reference sessionRef,
    _In_ std::shared_ptr<multiplayer_session> session
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);

    if (is_lobby(sessionRef))
    {
        m_lobbyClient->update_session(session);
    }
    else if(is_game(sessionRef))
    {
        m_gameClient->update_session(session);
    }
}

std::shared_ptr<multiplayer_session>
multiplayer_client_pending_reader::get_session(
    _In_ multiplayer_session_reference sessionRef
    )
{
    if (is_lobby(sessionRef))
    {
        return m_lobbyClient->session();
    }
    else if (is_game(sessionRef))
    {
        return m_gameClient->session();
    }

    return nullptr;
}

bool
multiplayer_client_pending_reader::is_lobby(
    _In_ multiplayer_session_reference sessionRef
    )
{
    if (sessionRef.is_null() || m_lobbyClient == nullptr || m_lobbyClient->session() == nullptr)
    {
        return false;
    }

    return multiplayer_manager_utils::do_session_references_match(sessionRef, m_lobbyClient->session()->session_reference());
}

bool
multiplayer_client_pending_reader::is_game(
    _In_ multiplayer_session_reference sessionRef
    )
{
    if (sessionRef.is_null() || m_gameClient == nullptr || m_gameClient->session() == nullptr)
    {
        return false;
    }

    return multiplayer_manager_utils::do_session_references_match(sessionRef, m_gameClient->session()->session_reference());
}

bool
multiplayer_client_pending_reader::is_match(
    _In_ multiplayer_session_reference sessionRef
)
{
    if (sessionRef.is_null() || m_matchClient == nullptr || m_matchClient->session() == nullptr)
    {
        return false;
    }

    return multiplayer_manager_utils::do_session_references_match(sessionRef, m_matchClient->session()->session_reference());
}

std::vector<multiplayer_event>
multiplayer_client_pending_reader::multiplayer_event_queue() const
{
    return m_multiplayerEventQueue;
}

void
multiplayer_client_pending_reader::add_to_multiplayer_event_queue(
    _In_ multiplayer_event multiplayerEvent
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_multiplayerEventQueue.push_back(multiplayerEvent);
}

void
multiplayer_client_pending_reader::add_to_multiplayer_event_queue(
    _In_ std::vector<multiplayer_event> multiplayerEventQueue
    )
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    if (multiplayerEventQueue.size() > 0)
    {
        for (const auto& multiplayerEvent : multiplayerEventQueue)
        {
            m_multiplayerEventQueue.push_back(multiplayerEvent);
        }
    }
}

void
multiplayer_client_pending_reader::clear_multiplayer_event_queue()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_multiplayerEventQueue.clear();
}

void
multiplayer_client_pending_reader::do_work()
{
    std::shared_ptr<xbox_live_context_impl> primaryContext = m_multiplayerLocalUserManager->get_primary_context();
    if (primaryContext == nullptr && 
        m_lobbyClient->multiplayer_event_queue().size() == 0 && 
        m_gameClient->multiplayer_event_queue().size() == 0 &&
        m_matchClient->multiplayer_event_queue().size() == 0 &&
        m_multiplayerEventQueue.size() == 0)
    {
        // After the primaryContext has been deleted, the lobbyClient could have userRemoved event in the queue.
        // The pending reader will also fire a client_disconnected_from_multiplayer_service event.
        return;
    }

    auto lobbySession = m_lobbyClient->session();
    auto gameSession = m_gameClient->session();
    m_lobbyClient->update_objects(lobbySession, gameSession);
    m_gameClient->update_objects(gameSession, lobbySession);

    auto lobbyClientEventQueue = m_lobbyClient->do_work();
    add_to_multiplayer_event_queue(lobbyClientEventQueue);

    auto gameClientEventQueue = m_gameClient->do_work();
    add_to_multiplayer_event_queue(gameClientEventQueue);

    process_match_events();
}

xbox_live_result<void>
multiplayer_client_pending_reader::set_joinability(
    _In_ xbox::services::multiplayer::manager::joinability value,
    _In_opt_ context_t context
    )
{
    return m_lobbyClient->set_joinability(value, context);
}

xbox_live_result<void>
multiplayer_client_pending_reader::set_properties(
    _In_ multiplayer_session_reference sessionRef,
    _In_ string_t name,
    _In_ web::json::value valueJson,
    _In_opt_ context_t context
    )
{
    auto pendingRequest = std::make_shared<multiplayer_client_pending_request>();
    pendingRequest->set_session_properties(name, valueJson, context);
    add_to_pending_queue(sessionRef, pendingRequest);
    
    return xbox_live_result<void>();
}

xbox_live_result<void>
multiplayer_client_pending_reader::set_synchronized_host(
    _In_ multiplayer_session_reference sessionRef,
    _In_ const string_t& hostDeviceToken,
    _In_opt_ context_t context
    )
{
    auto pendingRequest = std::make_shared<multiplayer_client_pending_request>();
    pendingRequest->set_synchronized_host_device_token(hostDeviceToken, context);
    add_to_pending_queue(sessionRef, pendingRequest);

    return xbox_live_result<void>();
}

xbox_live_result<void>
multiplayer_client_pending_reader::set_synchronized_properties(
    _In_ multiplayer_session_reference sessionRef,
    _In_ string_t name,
    _In_ web::json::value valueJson,
    _In_opt_ context_t context
    )
{
    auto pendingRequest = std::make_shared<multiplayer_client_pending_request>();
    pendingRequest->set_synchronized_session_properties(name, valueJson, context);
    add_to_pending_queue(sessionRef, pendingRequest);

    return xbox_live_result<void>();
}

void
multiplayer_client_pending_reader::add_to_pending_queue(
    _In_ multiplayer_session_reference sessionRef,
    _In_ std::shared_ptr<multiplayer_client_pending_request> pendingRequest
    )
{
    if (sessionRef.session_name().empty() || is_lobby(sessionRef))
    {
        m_lobbyClient->add_to_pending_queue(pendingRequest);
    }
    else if (is_game(sessionRef))
    {
        m_gameClient->add_to_pending_queue(pendingRequest);
    }
}

bool
multiplayer_client_pending_reader::is_local(
    _In_ const string_t& xboxUserId,
    _In_ const std::map<string_t, std::shared_ptr<multiplayer_local_user>>& xboxLiveContextMap
    )
{
    if(xboxUserId.empty()) return false;

    for(auto& xboxLiveContext : xboxLiveContextMap)
    {
        std::shared_ptr<multiplayer_local_user> localUser =  xboxLiveContext.second;
        if (localUser != nullptr && utils::str_icmp(xboxUserId, localUser->xbox_user_id()) == 0)
        {
            return true;
        }
    }

    return false;
}

std::shared_ptr<multiplayer_member>
multiplayer_client_pending_reader::convert_to_game_member(
    _In_ std::shared_ptr<multiplayer_session_member> member
    )
{
    return multiplayer_manager_utils::convert_to_game_member(
        member,
        m_lobbyClient->session(),
        m_gameClient->session(),
        m_multiplayerLocalUserManager->get_local_user_map()
        );
}

void
multiplayer_client_pending_reader::process_match_events()
{
    if (m_matchClient == nullptr || m_matchClient->match_status() == match_status::none) return;

    auto matchEventQueue = m_matchClient->do_work();
    auto matchSession = m_matchClient->session();

    if (matchEventQueue.size() > 0)
    {
        for (uint32_t i = 0; i < matchEventQueue.size(); i++)
        {
            multiplayer_event multiplayerEvent = matchEventQueue.at(i);
            add_to_multiplayer_event_queue(multiplayerEvent);

            if (multiplayerEvent.event_type() == multiplayer_event_type::find_match_completed)
            {
                auto matchStatus = m_matchClient->match_status();
                if (multiplayerEvent.err())
                {
                    if (!multiplayer_manager_utils::do_sessions_match(m_gameClient->session(), matchSession) &&
                        !multiplayer_manager_utils::do_sessions_match(m_lobbyClient->session(), matchSession))
                    {
                        m_lobbyClient->session_writer()->leave_remote_session(matchSession);
                    }
                }

                if (matchStatus == match_status::resubmitting)
                {
                    auto lobbySessionCopy = m_lobbyClient->session()->_Create_deep_copy();
                    m_matchClient->resubmit_matchmaking(lobbySessionCopy);
                    break;
                }

                if (matchStatus == match_status::completed)
                {
                    m_gameClient->update_game_session(matchSession);
                    m_gameClient->update_objects(matchSession, m_lobbyClient->session());
                    m_lobbyClient->advertise_game_session();
                }

                if (m_autoFillMembers && matchSession != nullptr &&
                    matchSession->members().size() < matchSession->session_constants()->max_members_in_session() &&
                    (matchStatus == match_status::completed || matchStatus == match_status::expired))
                {
                    // Continue looking for more players
                    m_matchClient->set_match_status(match_status::none);
                    m_matchClient->find_match(matchSession, true);
                    break;
                }

                m_matchClient->set_match_status(match_status::none);
                m_matchClient->update_session(nullptr);
                break;
            }
        }
    }
}

xbox_live_result<void>
multiplayer_client_pending_reader::find_match(
    _In_ const string_t& hopperName,
    _In_ const web::json::value& attributes,
    _In_ const std::chrono::seconds& timeout
    )
{
    RETURN_CPP_IF(!m_autoFillMembers && m_gameClient->session() != nullptr, void, xbox_live_error_code::logic_error, "A game already exists. Call leave_game() before you can start matchmaking.");
    RETURN_CPP_IF(!m_autoFillMembers && !m_lobbyClient->get_transfer_handle().empty(), void, xbox_live_error_code::logic_error, "A game already exists for your Lobby. Call leave_game() for all Lobby members before you can start matchmaking.");

    if (m_autoFillMembers && m_gameClient->session() != nullptr)
    {
        return m_matchClient->find_match(hopperName, attributes, timeout, m_gameClient->session(), true);
    }

    return m_matchClient->find_match(hopperName, attributes, timeout, m_lobbyClient->session(), false);
}

void
multiplayer_client_pending_reader::set_auto_fill_members_during_matchmaking(
    _In_ bool autoFillMembers
    )
{
    m_autoFillMembers = autoFillMembers;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
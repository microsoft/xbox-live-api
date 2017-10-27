// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/multiplayer.h"
#include "xsapi/multiplayer_manager.h"
#include "multiplayer_manager_internal.h"

#if defined __cplusplus_winrt
using namespace Platform;
using namespace Windows::Foundation::Collections;
#endif
using namespace xbox::services;
using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

multiplayer_manager::multiplayer_manager() :
    m_joinability(joinability::none),
    m_isDirty(false)
{
}

std::shared_ptr<multiplayer_manager>
multiplayer_manager::get_singleton_instance()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->m_singletonLock);
    if (xsapiSingleton->m_multiplayerManagerInstance == nullptr)
    {
        xsapiSingleton->m_multiplayerManagerInstance = std::shared_ptr<multiplayer_manager>(new multiplayer_manager());
    }

    return xsapiSingleton->m_multiplayerManagerInstance;
}

void
multiplayer_manager::initialize(
    _In_ string_t lobbySessionTemplateName
    )
{
    XSAPI_ASSERT(!lobbySessionTemplateName.empty());

    std::lock_guard<std::mutex> guard(m_lock);
    if(m_multiplayerClientManager != nullptr)
    {
        m_multiplayerClientManager->shutdown();

        m_multiplayerLobbySession = nullptr;
        m_multiplayerClientManager = nullptr;
    }

    m_multiplayerClientManager = std::make_shared<multiplayer_client_manager>(
        lobbySessionTemplateName
        );

    m_multiplayerClientManager->register_local_user_manager_events();
    m_multiplayerLobbySession = std::make_shared<multiplayer_lobby_session>(m_multiplayerClientManager);
}

bool
multiplayer_manager::_Is_dirty()
{
    return m_isDirty;
}

std::vector<multiplayer_event>
multiplayer_manager::do_work()
{
    std::lock_guard<std::mutex> guard(m_lock);

    if (m_multiplayerClientManager == nullptr)
    {
        m_isDirty = false;

        // The title hasn't called initialize() on the manager yet.
        return std::vector<multiplayer_event>();
    }
    else if (!m_multiplayerClientManager->is_update_avaialable())
    {
        m_isDirty = false;

        // To handle the scenario of returning InvitedXuid info in the join_lobby_completed event
        std::vector<multiplayer_event> eventQueue; 
        if (m_multiplayerClientManager->event_queue().size() > 0)
        {
            eventQueue = m_multiplayerClientManager->event_queue();
            m_multiplayerClientManager->clear_event_queue();
        }

        return eventQueue;
    }

    std::vector<multiplayer_event> eventQueue;
    
    try
    {
        m_isDirty = true;
        eventQueue = m_multiplayerClientManager->do_work();

        std::shared_ptr<multiplayer_client_pending_reader> clientRequest = m_multiplayerClientManager->last_pending_read();
        if (clientRequest != nullptr)
        {
            m_joinability = clientRequest->lobby_client()->joinability();
            set_multiplayer_game_session(clientRequest->game_client()->game());
            set_multiplayer_lobby_session(clientRequest->lobby_client()->lobby());
        }
        else
        {
            m_joinability = joinability::none;
            set_multiplayer_game_session(nullptr);
            set_multiplayer_lobby_session(nullptr);
        }
    }
    catch(...){}

    return eventQueue;
}

std::shared_ptr<multiplayer_game_session>
multiplayer_manager::game_session() const
{
    std::lock_guard<std::mutex> guard(m_lock);
    return m_multiplayerGameSession;
}

std::shared_ptr<multiplayer_lobby_session>
multiplayer_manager::lobby_session() const
{
    std::lock_guard<std::mutex> guard(m_lock);
    return m_multiplayerLobbySession;
}

void
multiplayer_manager::set_multiplayer_game_session(
    _In_ std::shared_ptr<multiplayer_game_session> gameSession
    ) 
{
    // Note: This function does not require a lock as the functions that call this already has a m_lock.

    if (gameSession == nullptr)
    {
        m_multiplayerGameSession = nullptr;
    }
    else
    {
        m_multiplayerGameSession = gameSession;
        m_multiplayerGameSession->_Set_multiplayer_client_manager(m_multiplayerClientManager);
    }
}

void
multiplayer_manager::set_multiplayer_lobby_session(
    _In_ std::shared_ptr<multiplayer_lobby_session> multiplayerLobby
    ) 
{
    // Note: This function does not require a lock as the functions that call this already has a m_lock.

    if (multiplayerLobby == nullptr)
    {
        if (m_multiplayerLobbySession != nullptr)
        {
            m_multiplayerLobbySession.reset();
        }

        // Don't set the lobby to null so that the title can add local users whenever it chooses.
        m_multiplayerLobbySession = std::make_shared<multiplayer_lobby_session>(m_multiplayerClientManager);
    }
    else
    {
        m_multiplayerLobbySession = multiplayerLobby;
        m_multiplayerLobbySession->_Set_multiplayer_client_manager(m_multiplayerClientManager);
    }
}

xbox_live_result<void>
multiplayer_manager::join_lobby(
    _In_ const string_t& handleId,
    _In_ xbox_live_user_t user
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager == nullptr, void, xbox_live_error_code::logic_error, "Call multiplayer_manager::initialize() first.");

    std::vector<xbox_live_user_t> users;
    users.push_back(user);
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->join_lobby_by_handle(handleId, users), void);
}

#if TV_API
xbox_live_result<void>
multiplayer_manager::join_lobby(
    _In_ const string_t& handleId,
    _In_ std::vector<Windows::Xbox::System::User^> users
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager == nullptr, void, xbox_live_error_code::logic_error, "Call multiplayer_manager::initialize() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->join_lobby_by_handle(handleId, users), void);
}

xbox_live_result<void>
multiplayer_manager::join_lobby(
    _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
    _In_ std::vector<Windows::Xbox::System::User^> users
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager == nullptr, void, xbox_live_error_code::logic_error, "Call multiplayer_manager::initialize() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->join_lobby(eventArgs, users), void);
}

void
multiplayer_manager::invite_party_to_game()
{
}
#endif

#if (TV_API || UWP_API || UNIT_TEST_SERVICES)
xbox_live_result<void>
multiplayer_manager::join_lobby(
    _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
    _In_ xbox_live_user_t user
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager == nullptr, void, xbox_live_error_code::logic_error, "Call multiplayer_manager::initialize() first.");

    std::vector<xbox_live_user_t> users;
    users.push_back(user);
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->join_lobby(eventArgs, users), void);
}
#endif

xbox_live_result<void>
multiplayer_manager::join_game_from_lobby(
    _In_ const string_t& sessionTemplateName
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager == nullptr, void, xbox_live_error_code::logic_error, "Call multiplayer_manager::initialize() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->join_game_from_lobby(sessionTemplateName), void);
}

xbox_live_result<void>
multiplayer_manager::join_game(
    _In_ const string_t& sessionName,
    _In_ const string_t& sessionTemplateName,
    _In_ const std::vector<string_t>& xboxUserIds
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager == nullptr, void, xbox_live_error_code::logic_error, "Call multiplayer_manager::initialize() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->join_game(sessionName, sessionTemplateName, xboxUserIds), void);
}

xbox_live_result<void>
multiplayer_manager::leave_game()
{
    RETURN_CPP_IF(m_multiplayerClientManager == nullptr, void, xbox_live_error_code::logic_error, "Call multiplayer_manager::initialize() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->leave_game(), void);
}

xbox_live_result<void>
multiplayer_manager::find_match(
    _In_ const string_t& hopperName,
    _In_ const web::json::value& attributes,
    _In_ const std::chrono::seconds& timeout
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager == nullptr, void, xbox_live_error_code::logic_error, "Call multiplayer_manager::initialize() first.");
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->find_match(hopperName, attributes, timeout), void);
}

void
multiplayer_manager::cancel_match()
{
    if (m_multiplayerClientManager != nullptr && m_multiplayerClientManager->match_client() != nullptr)
    {
        m_multiplayerClientManager->match_client()->cancel_match();
    }
}

xbox::services::multiplayer::manager::match_status
multiplayer_manager::match_status() const
{
    if (m_multiplayerClientManager != nullptr && m_multiplayerClientManager->match_client() != nullptr)
    {
        return m_multiplayerClientManager->match_client()->match_status();
    }

    return match_status::none;
}

std::chrono::seconds
multiplayer_manager::estimated_match_wait_time() const
{
    if (m_multiplayerClientManager != nullptr && m_multiplayerClientManager->match_client() != nullptr)
    {
        return m_multiplayerClientManager->match_client()->estimated_match_wait_time();
    }

    return std::chrono::seconds(0);
}

void
multiplayer_manager::set_quality_of_service_measurements(
    _In_ std::shared_ptr<std::vector<multiplayer_quality_of_service_measurements>> measurements
    )
{
    if (m_multiplayerClientManager != nullptr && m_multiplayerClientManager->match_client() != nullptr)
    {
        m_multiplayerClientManager->match_client()->set_quality_of_service_measurements(measurements);
    }
}

bool
multiplayer_manager::auto_fill_members_during_matchmaking() const
{
    return false;
}

void
multiplayer_manager::set_auto_fill_members_during_matchmaking(
    _In_ bool autoFillMembers
    )
{
    if (m_multiplayerClientManager != nullptr)
    {
        m_multiplayerClientManager->set_auto_fill_members_during_matchmaking(autoFillMembers);
    }
}

xbox::services::multiplayer::manager::joinability
multiplayer_manager::joinability() const
{
    return m_joinability;
}

xbox_live_result<void>
multiplayer_manager::set_joinability(
    _In_ xbox::services::multiplayer::manager::joinability value,
    _In_opt_ context_t context
    )
{
    RETURN_CPP_IF(m_multiplayerClientManager == nullptr, void, xbox_live_error_code::logic_error, "Call multiplayer_manager::initialize() first.");
    
    RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(m_multiplayerClientManager->set_joinability(value, context), void);
}

std::shared_ptr<multiplayer_game_client>
multiplayer_manager::_Game_client()
{
    auto clientManger = m_multiplayerClientManager;
    if (clientManger == nullptr || clientManger->latest_pending_read() == nullptr)
    {
        return nullptr;
    }
    return clientManger->latest_pending_read()->game_client();
}

std::shared_ptr<multiplayer_lobby_client>
multiplayer_manager::_Lobby_client()
{
    auto clientManger = m_multiplayerClientManager;
    if (clientManger == nullptr || clientManger->latest_pending_read() == nullptr)
    {
        return nullptr;
    }
    return clientManger->latest_pending_read()->lobby_client();
}

#if UNIT_TEST_SERVICES
void
multiplayer_manager::_Shutdown()
{
    auto clientManger = m_multiplayerClientManager;
    if (clientManger == nullptr || clientManger->latest_pending_read() == nullptr)
    {
        return;
    }
    return clientManger->shutdown();
}
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
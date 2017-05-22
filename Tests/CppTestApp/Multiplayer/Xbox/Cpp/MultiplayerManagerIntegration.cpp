// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "GameLogic\Multiplayer.h"
#include "Utils\PerformanceCounters.h"
#include <time.h>

using namespace Concurrency;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::multiplayer::manager;

void
Sample::InitializeMultiplayerManager(_In_ const string_t& templateName)
{
    m_multiplayerManager = multiplayer_manager::get_singleton_instance();
    m_multiplayerManager->initialize(templateName);
}

void Sample::DoWork()
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);
    if (m_multiplayerManager != nullptr)
    {
#if PERF_COUNTERS
        auto perfInstance = performance_counters::get_singleton_instance();
        perfInstance->begin_capture(L"no_updates");
        perfInstance->begin_capture(L"updates");
        m_multiplayerEventQueue = m_multiplayerManager->do_work();
        if (!m_multiplayerManager->_Is_dirty())
        {
            perfInstance->end_capture(L"no_updates");
        }
        else
        {
            perfInstance->end_capture(L"updates");
        }
#else
        m_multiplayerEventQueue = m_multiplayerManager->do_work();
#endif
        for (auto& multiplayerEvent : m_multiplayerEventQueue)
        {
            switch (multiplayerEvent.event_type())
            {
                case multiplayer_event_type::user_added:
                {
                    auto userAddedArgs = std::dynamic_pointer_cast<user_added_event_args>(multiplayerEvent.event_args());
                    if (!multiplayerEvent.err())
                    {
                        m_appState = APP_IN_GAME;
                        ChangeAppStates();
                    }
                    break;
                }

                case multiplayer_event_type::leave_game_completed:
                {
                    m_isLeavingGame = false;
                    break;
                }

                case multiplayer_event_type::join_game_completed:
                {
                    if (multiplayerEvent.err() && m_multiplayerManager->lobby_session()->local_members().empty())
                    {
                        LogErrorFormat(L"JoinGame failed: %S\n", multiplayerEvent.err_message().c_str());
                        m_appState = APP_MAIN_MENU;
                    }
                    break;
                }

                case multiplayer_event_type::join_lobby_completed:
                {
                    if (multiplayerEvent.err())
                    {
                        LogErrorFormat(L"JoinLobby failed: %S\n", multiplayerEvent.err_message().c_str());
                        m_appState = APP_MAIN_MENU;
                    }
                    else
                    {
                        m_appState = APP_IN_GAME;
                        ChangeAppStates();
                    }
                    break;
                }
            }
        }
    }
}

void Sample::AddLocalUser()
{
    assert(m_multiplayerManager != nullptr);
    if (m_liveResources->GetLiveContext() != nullptr)
    {
        Concurrency::critical_section::scoped_lock lock(m_stateLock);

        auto result = m_multiplayerManager->lobby_session()->add_local_user(m_liveResources->GetLiveContext()->user());
        if (result.err())
        {
            LogErrorFormat(L"AddLocalUser failed: %S\n", result.err_message().c_str());
            m_appState = APP_MAIN_MENU;
            return;
        }

        m_multiplayerManager->lobby_session()->set_local_member_properties(
            m_liveResources->GetLiveContext()->user(),
            L"Health", web::json::value::number(rand() % 100), 
            (void*) InterlockedIncrement(&m_multiplayerContext)
        );

        string_t connectionAddress = Windows::Networking::XboxLive::XboxLiveDeviceAddress::GetLocal()->GetSnapshotAsBase64()->Data();
        m_multiplayerManager->lobby_session()->set_local_member_connection_address(m_liveResources->GetLiveContext()->user(), connectionAddress, (void*) InterlockedIncrement(&m_multiplayerContext));
    }
}

void Sample::RemoveLocalUser()
{
    assert(m_multiplayerManager != nullptr);
    Concurrency::critical_section::scoped_lock lock(m_stateLock);

    m_multiplayerManager->lobby_session()->remove_local_user(m_liveResources->GetLiveContext()->user());
}

void Sample::UpdateJoinability()
{
    assert(m_multiplayerManager != nullptr);

    auto joinabilityVal = GetRandJoinabilityValue();
    auto commitResult = m_multiplayerManager->set_joinability(joinabilityVal);
    if (commitResult.err())
    {
        LogErrorFormat(L"Updating joinability failed: %S\n", commitResult.err_message().c_str());
    }
}

void Sample::InviteFriends()
{
    assert(m_multiplayerManager != nullptr);

    // Pass empty string if you don’t want a custom string added to the invite.
    auto result = m_multiplayerManager->lobby_session()->invite_friends(m_liveResources->GetLiveContext()->user(), L"", L"Join my game!!");
    if (result.err())
    {
        LogErrorFormat(L"InviteFriends failed: %S\n", result.err_message().c_str());
    }
}

void Sample::HandleProtocolActivation()
{
    assert(m_multiplayerManager != nullptr);

    m_isProtocolActivated = false;
    auto result = m_multiplayerManager->join_lobby(m_protocolActivatedEventArgs, m_liveResources->GetLiveContext()->user());
    if (result.err())
    {
        LogErrorFormat(L"InviteFriends failed: %S\n", result.err_message().c_str());
        return;
    }

    string_t connectionAddress = Windows::Networking::XboxLive::XboxLiveDeviceAddress::GetLocal()->GetSnapshotAsBase64()->Data();
    m_multiplayerManager->lobby_session()->set_local_member_connection_address(
        m_liveResources->GetLiveContext()->user(), 
        connectionAddress, 
        (void*)InterlockedIncrement(&m_multiplayerContext));

    m_appState = APPSTATE::APP_JOIN_LOBBY;
    ChangeAppStates();
}

void Sample::UpdateLocalMemberProperties()
{
    assert(m_multiplayerManager != nullptr);
    if (m_liveResources->GetLiveContext() != nullptr)
    {
        Concurrency::critical_section::scoped_lock lock(m_stateLock);

        m_multiplayerManager->lobby_session()->set_local_member_properties(
            m_liveResources->GetLiveContext()->user(),
            L"Health",
            web::json::value::number(rand() % 100),
            (void*) InterlockedIncrement(&m_multiplayerContext)
            );

        m_multiplayerManager->lobby_session()->set_local_member_properties(
            m_liveResources->GetLiveContext()->user(),
            L"Skill",
            web::json::value::number(rand() % 100),
            (void*) InterlockedIncrement(&m_multiplayerContext)
            );
    }
}

void Sample::UpdateLobbyProperties()
{
    assert(m_multiplayerManager != nullptr);
    if (m_multiplayerManager->lobby_session() != nullptr)
    {
        Concurrency::critical_section::scoped_lock lock(m_stateLock);

        xbox_live_result<void> commitResult;
        commitResult = m_multiplayerManager->lobby_session()->set_synchronized_properties(
            L"GameMode", web::json::value::string(g_gameModeStrings[GetRandomizedGameModeIndex()]), (void*)InterlockedIncrement(&m_multiplayerContext)
            );

        commitResult = m_multiplayerManager->lobby_session()->set_synchronized_properties(
            L"Map", web::json::value::string(g_mapStrings[GetRandomizedMapIndex()]), (void*)InterlockedIncrement(&m_multiplayerContext)
            );

        if (commitResult.err())
        {
            LogErrorFormat(L"UpdateLobbyProperties failed: %S\n", commitResult.err_message().c_str());
        }
    }
}

void Sample::SetLobbyHost()
{
    assert(m_multiplayerManager != nullptr);
    auto lobby = m_multiplayerManager->lobby_session();
    if (lobby != nullptr)
    {
        Concurrency::critical_section::scoped_lock lock(m_stateLock);

        // Set the first local member to be the host.
        xbox_live_result<void> commitResult = lobby->set_synchronized_host(lobby->local_members().at(0), (void*)InterlockedIncrement(&m_multiplayerContext));
        if (commitResult.err())
        {
            LogErrorFormat(L"SetLobbyHost failed: %S\n", commitResult.err_message().c_str());
        }
    }
}

void Sample::JoinGameFromLobby()
{
    assert(m_multiplayerManager != nullptr);
    xbox_live_result<void> result = m_multiplayerManager->join_game_from_lobby(GAME_SESSION_TEMPLATE_NAME);
    if (result.err())
    {
        LogErrorFormat(L"JoinGameFromLobby failed: %S\n", result.err_message().c_str());
    }
}

void Sample::JoinGame()
{
    assert(m_multiplayerManager != nullptr);
    auto result = m_multiplayerManager->lobby_session()->add_local_user(m_liveResources->GetLiveContext()->user());
    if (result.err())
    {
        LogErrorFormat(L"Failed adding the local user: %S\n", result.err_message().c_str());
        m_appState = APP_MAIN_MENU;
        return;
    }

    string_t connectionAddress = Windows::Networking::XboxLive::XboxLiveDeviceAddress::GetLocal()->GetSnapshotAsBase64()->Data();
    m_multiplayerManager->lobby_session()->set_local_member_connection_address(m_liveResources->GetLiveContext()->user(), connectionAddress, (void*)InterlockedIncrement(&m_multiplayerContext));
    string_t guid = CreateGuid();
    result = m_multiplayerManager->join_game(guid.c_str(), GAME_SESSION_TEMPLATE_NAME);
    if (result.err())
    {
        LogErrorFormat(L"JoinGame failed: %S\n", result.err_message().c_str());
        m_appState = APP_MAIN_MENU;
    }
}

void Sample::LeaveGameSession()
{
    assert(m_multiplayerManager != nullptr);
    xbox_live_result<void> result = m_multiplayerManager->leave_game();
    if (result.err())
    {
        m_isLeavingGame = false;
        LogErrorFormat(L"LeaveGameSession failed: %S\n", result.err_message().c_str());
    }
}

void Sample::UpdateGameProperties()
{
    assert(m_multiplayerManager != nullptr);
    if (m_multiplayerManager->game_session() != nullptr)
    {
        Concurrency::critical_section::scoped_lock lock(m_stateLock);
        xbox_live_result<void> commitResult;

        commitResult = m_multiplayerManager->game_session()->set_synchronized_properties(
            L"GameMode", web::json::value::string(g_gameModeStrings[GetRandomizedGameModeIndex()]), (void*)InterlockedIncrement(&m_multiplayerContext)
        );

        commitResult = m_multiplayerManager->game_session()->set_synchronized_properties(
            L"Map", web::json::value::string(g_mapStrings[GetRandomizedMapIndex()]), (void*)InterlockedIncrement(&m_multiplayerContext)
        );

        if (commitResult.err())
        {
            LogErrorFormat(L"UpdateGameProperties failed: %S\n", commitResult.err_message().c_str());
        }
    }
}

void Sample::SetGameHost()
{
    assert(m_multiplayerManager != nullptr);
    if (m_multiplayerManager->game_session() != nullptr)
    {
        Concurrency::critical_section::scoped_lock lock(m_stateLock);

        // Set the first local member to be the host.
        xbox_live_result<void> commitResult = m_multiplayerManager->game_session()->set_synchronized_host(m_multiplayerManager->lobby_session()->local_members().at(0), (void*)InterlockedIncrement(&m_multiplayerContext));
        if (commitResult.err())
        {
            LogErrorFormat(L"SetGameHost failed: %S\n", commitResult.err_message().c_str());
        }
    }
}

void Sample::GetActivitiesForSocialGroup()
{
    auto context = m_liveResources->GetLiveContext();
    if (context != nullptr)
    {
        context->multiplayer_service().get_activities_for_social_group(GAME_SERVICE_CONFIG_ID, context->xbox_live_user_id(), xbox::services::social::social_group_constants::people())
        .then([this](xbox_live_result<std::vector<multiplayer_activity_details>> activityDetails)
        {
            if (activityDetails.err())
            {
                LogErrorFormat(L"GetActivitiesForSocialGroup failed: %S\n", activityDetails.err_message().c_str());
            }
        }).wait();
    }
}
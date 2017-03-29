// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Multiplayer.h"
#include "Utils\PerformanceCounters.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::multiplayer::manager;

#define EventQueueLen 17

void Sample::RenderMultiplayerSessionDetails(
    string_t sessionName,
    std::shared_ptr<multiplayer_member> hostMember,
    std::vector<std::shared_ptr<multiplayer_member>> members,
    web::json::value properties,
    multiplayer_session_type sessionType,
    std::shared_ptr<SessionView> sessionView
    )
{
    Concurrency::critical_section::scoped_lock lock(m_stateLock);

    if (sessionName.empty())
    {
        return;
    }

    string_t gameModeStr = L"Mode: ";
    string_t mapStr = L"Map: ";
    if (properties.size() > 0)
    {
        if (properties.has_field(_T("GameMode")))
        {
            gameModeStr += properties.at(_T("GameMode")).as_string();
        }
        if (properties.has_field(_T("Map")))
        {
            mapStr += properties.at(_T("Map")).as_string();
        }
    }

    string_t sessionNameStr = L"LOBBY SESSION: ";
    if (sessionType == multiplayer_session_type::game_session)
    {
        sessionNameStr = L"GAME SESSION: ";
    }

    sessionNameStr += sessionName.substr(0, 18).c_str();
    sessionView->m_sessionNameLabel->SetText(sessionNameStr.c_str());
    sessionView->m_gameModeLabel->SetText(gameModeStr.c_str());
    sessionView->m_mapLabel->SetText(mapStr.c_str());

    int rowCount = 0;
    for (auto& member : members)
    {
        if (member != nullptr)
        {
            bool isHost = false;
            if (hostMember != nullptr)
            {
                if (_wcsicmp(hostMember->xbox_user_id().c_str(), member->xbox_user_id().c_str()) == 0)
                {
                    isHost = true;
                }
            }

            string_t xuidStr = member->xbox_user_id();
            if (member->is_local())
            {
                xuidStr = xuidStr + L" (L)";
            }

            string_t healthStr = L"";
            string_t skillStr = L"";
            auto memberProp = member->properties();
            if (memberProp.size() > 0)
            {
                if (memberProp.has_field(_T("Health")))
                {
                    healthStr = memberProp.at(_T("Health")).as_number().to_uint32().ToString()->Data();
                }

                if (memberProp.has_field(_T("Skill")))
                {
                    skillStr = memberProp.at(_T("Skill")).as_number().to_uint32().ToString()->Data();
                }
            }

            sessionView->m_rows[rowCount].m_xuid->SetText(xuidStr.c_str());
            sessionView->m_rows[rowCount].m_skill->SetText(skillStr.c_str());
            sessionView->m_rows[rowCount].m_health->SetText(healthStr.c_str());
            sessionView->m_rows[rowCount].m_hostIcon->SetVisible(isHost);
            sessionView->m_rows[rowCount].m_gamerpic->SetVisible(true);
            rowCount++;
        }
    }

    sessionView->Show();
}

void Sample::RenderMultiplayerEventQueue()
{
    std::vector<multiplayer_event> eventQueue = m_multiplayerEventQueue;

    // Render event queue notifications
    for (auto& multiplayerEvent : eventQueue)
    {
        stringstream_t ss;
        if (multiplayerEvent.context() != nullptr)
        {
            ss << (uint64_t)multiplayerEvent.context() << _T(" - ");
        }

        switch (multiplayerEvent.event_type())
        {
        case multiplayer_event_type::user_added:
        {
            auto userAddedArgs = std::dynamic_pointer_cast<user_added_event_args>(multiplayerEvent.event_args());
            if (multiplayerEvent.err())
            {
                ss << _T("Failed adding User '");
                ss << userAddedArgs->xbox_user_id();
            }
            else
            {
                ss << _T("User '");
                ss << userAddedArgs->xbox_user_id();
                ss << _T("' added");
            }
            break;
        }

        case multiplayer_event_type::user_removed:
        {
            auto userRemovedArgs = std::dynamic_pointer_cast<user_removed_event_args>(multiplayerEvent.event_args());
            if (multiplayerEvent.err())
            {
                ss << _T("Failed removing User '");
                ss << userRemovedArgs->xbox_user_id();
            }
            else
            {
                ss << _T("User '");
                ss << userRemovedArgs->xbox_user_id();
                ss << _T("' removed");
            }

            m_appState = APP_MAIN_MENU;
            ChangeAppStates();
            break;
        }

        case multiplayer_event_type::member_joined:
        {
            auto memberJoinedArgs = std::dynamic_pointer_cast<member_joined_event_args>(multiplayerEvent.event_args());
            for (auto& member : memberJoinedArgs->members())
            {
                ss << _T("Member '");
                ss << member->xbox_user_id();
                ss << _T("' joined");
                if (multiplayerEvent.session_type() == multiplayer_session_type::lobby_session)
                {
                    ss << _T(" the lobby.");
                }
                else
                {
                    ss << _T(" the game.");
                }
            }
            break;
        }

        case multiplayer_event_type::member_left:
        {
            auto memberLeftArgs = std::dynamic_pointer_cast<member_left_event_args>(multiplayerEvent.event_args());
            for (auto& member : memberLeftArgs->members())
            {
                ss << _T("Member '");
                ss << member->xbox_user_id();
                ss << _T("' left");
                if (multiplayerEvent.session_type() == multiplayer_session_type::lobby_session)
                {
                    ss << _T(" the lobby.");
                }
                else
                {
                    ss << _T(" the game.");
                }
            }
            break;
        }

        case multiplayer_event_type::member_property_changed:
        {
            auto memberPropChangedArgs = std::dynamic_pointer_cast<member_property_changed_event_args>(multiplayerEvent.event_args());
            auto member = memberPropChangedArgs->member();
            ss << _T("Member '");
            ss << member->xbox_user_id();
            ss << _T("' property changed.");
            break;
        }

        case multiplayer_event_type::session_property_changed:
        {
            auto gamePropChangedArgs = std::dynamic_pointer_cast<session_property_changed_event_args>(multiplayerEvent.event_args());
            if (multiplayerEvent.session_type() == multiplayer_session_type::lobby_session)
            {
                ss << _T("Lobby property changed.");
            }
            else
            {
                ss << _T("Game property changed.");
            }
            break;
        }

        case multiplayer_event_type::local_member_property_write_completed:
        {
            if (!multiplayerEvent.err())
            {
                ss << _T("Local member prop write complete.");
            }
            else
            {
                ss << _T("Local member prop write failed.");
            }
            break;
        }

        case multiplayer_event_type::local_member_connection_address_write_completed:
        {
            if (!multiplayerEvent.err())
            {
                ss << _T("Member conn addr write complete");
            }
            else
            {
                ss << _T("Member conn addr write failed.");
            }
            break;
        }

        case multiplayer_event_type::session_property_write_completed:
        {
            if (!multiplayerEvent.err())
            {
                ss << _T("Session property write complete.");
            }
            else
            {
                ss << _T("Session property write failed.");
            }
            break;
        }

        case multiplayer_event_type::session_synchronized_property_write_completed:
        {
            if (!multiplayerEvent.err())
            {
                ss << _T("Synchronized prop write complete.");
            }
            else
            {
                ss << _T("Synchronized prop write failed.");
            }
            break;
        }

        case multiplayer_event_type::synchronized_host_write_completed:
        {
            if (!multiplayerEvent.err())
            {
                ss << _T("Synchronized host write complete.");
            }
            else
            {
                ss << _T("Synchronized host write failed.");
            }
            break;
        }

        case multiplayer_event_type::host_changed:
        {
            auto hostChangedArgs = std::dynamic_pointer_cast<host_changed_event_args>(multiplayerEvent.event_args());
            if (multiplayerEvent.session_type() == multiplayer_session_type::lobby_session)
            {
                ss << _T("Lobby Host changed.");
            }
            else
            {
                ss << _T("Game Host changed.");
            }
            break;
        }

        case multiplayer_event_type::join_game_completed:
        {
            if (!multiplayerEvent.err())
            {
                ss << _T("Join game completed.");
            }
            else
            {
            ss << _T("Join game failed.");
            }
            break;
        }

        case multiplayer_event_type::leave_game_completed:
        {
            if (!multiplayerEvent.err())
            {
                ss << _T("Leave game completed.");
            }
            else
            {
                ss << _T("Leave game failed.");
            }
            break;
        }

        case multiplayer_event_type::join_lobby_completed:
        {
            auto joinLobbyCompletedArgs = std::dynamic_pointer_cast<join_lobby_completed_event_args>(multiplayerEvent.event_args());
            if (!multiplayerEvent.err())
            {
                ss << _T("Join lobby completed.");
            }
            else
            {
                ss << _T("Join lobby failed.");
            }
            break;
        }

        case multiplayer_event_type::invite_sent:
        {
            if (!multiplayerEvent.err())
            {
                ss << _T("Invite sent.");
            }
            else
            {
                ss << _T("Invite failed.");
            }
            break;
        }

        case multiplayer_event_type::joinability_state_changed:
        {
            if (!multiplayerEvent.err())
            {
                ss << _T("Joinability change completed.");
            }
            else
            {
                ss << _T("Joinability change failed.");
            }
            break;
        }

        case multiplayer_event_type::client_disconnected_from_multiplayer_service:
        {
            ss << _T("Client disconnected from MP.");
            break;
        }

        default:
            break;
        }

        m_displayEventQueue.push_back(ss.str());
        if (m_displayEventQueue.size() > EventQueueLen)
        {
            m_displayEventQueue.erase(m_displayEventQueue.begin());
        }
    }

    for (unsigned int i = 0; i < m_displayEventQueue.size(); ++i)
    {
        m_eventQueueConsole->WriteLine(m_displayEventQueue[i].c_str());
    }
}

int32_t Sample::GetActiveMemberCount(
    std::vector<std::shared_ptr<multiplayer_member>> members
    )
{
    int32_t numOfActiveMembers = 0;
    for (auto& member : members)
    {
        if (member != nullptr)
        {
            if (member->status() == multiplayer_session_member_status::active)
            {
                numOfActiveMembers++;
            }
        }
    }
    return numOfActiveMembers;
}

void Sample::RenderPerfCounters()
{
    auto perfInstance = performance_counters::get_singleton_instance();

    m_perfConsole->Format(L"%-14s %8s %8s %8s\n", L"TYPE", L"AVG", L"MIN", L"MAX");
    m_perfConsole->Format(L"-----------------------------------------\n");

    auto noUpdateInstance = perfInstance->get_capture_instace(L"no_updates");
    if (noUpdateInstance != nullptr)
    {
        m_perfConsole->Format(
            L"%-14s %8.5f %8.5f %8.5f\n", 
            L"No updates:", 
            noUpdateInstance->average_time(),
            noUpdateInstance->min_time(),
            noUpdateInstance->max_time()
            );
    }

    auto updatedInstance = perfInstance->get_capture_instace(L"updates");
    if (updatedInstance != nullptr)
    {
        m_perfConsole->Format(
            L"%-14s %8.5f %8.5f %8.5f\n",
            L"With updates:",
            updatedInstance->average_time(),
            updatedInstance->min_time(),
            updatedInstance->max_time()
        );
    }
}
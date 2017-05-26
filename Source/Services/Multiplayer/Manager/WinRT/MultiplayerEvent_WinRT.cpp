// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "MultiplayerEvent_WinRT.h"
#include "UserAddedEventArgs_WinRT.h"
#include "UserRemovedEventArgs_WinRT.h"
#include "MemberJoinedEventArgs_WinRT.h"
#include "MemberLeftEventArgs_WinRT.h"
#include "MemberPropertyChangedEventArgs_WinRT.h"
#include "SessionPropertyChangedEventArgs_WinRT.h"
#include "HostChangedEventArgs_WinRT.h"
#include "JoinLobbyCompletedEventArgs_WinRT.h"
#include "FindMatchCompletedEventArgs_WinRT.h"
#include "PerformQosMeasurementsEventArgs_WinRT.h"
#include "TournamentRegistrationStateChangedEventArgs_WinRT.h"
#include "TournamentGameSessionReadyEventArgs_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::multiplayer::manager;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_BEGIN

MultiplayerEvent::MultiplayerEvent(
    _In_ multiplayer_event cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    if (m_cppObj.err())
    {
        m_errorCode = ref new Platform::Exception(xbox::services::utils::convert_xbox_live_error_code_to_hresult(m_cppObj.err()));
    }

    m_errorMessage = ref new Platform::String(utility::conversions::utf8_to_utf16(m_cppObj.err_message()).c_str());
    m_context = m_cppObj.context();
}

const multiplayer_event&
MultiplayerEvent::GetCppObj() const
{
    return m_cppObj;
}

int
MultiplayerEvent::ErrorCode::get()
{
    if (m_errorCode == nullptr)
    {
        return 0;
    }

    return m_errorCode->HResult;
}

Platform::String^
MultiplayerEvent::ErrorMessage::get()
{
    return m_errorMessage;
}

context_t
MultiplayerEvent::Context::get()
{
    return m_context;
}

MultiplayerEventArgs^
MultiplayerEvent::EventArgs::get()
{
    switch (m_cppObj.event_type())
    {
    case multiplayer_event_type::user_added:
        {
            return ref new UserAddedEventArgs(
                std::dynamic_pointer_cast<user_added_event_args>(m_cppObj.event_args())
                );
        }

    case multiplayer_event_type::user_removed:
        {
            return ref new UserRemovedEventArgs(
                std::dynamic_pointer_cast<user_removed_event_args>(m_cppObj.event_args())
                );
        }

    case multiplayer_event_type::member_joined:
        {
            return ref new MemberJoinedEventArgs(
                std::dynamic_pointer_cast<member_joined_event_args>(m_cppObj.event_args())
                );
        }

    case multiplayer_event_type::member_left:
        {
            return ref new MemberLeftEventArgs(
                std::dynamic_pointer_cast<member_left_event_args>(m_cppObj.event_args())
                );
        }

    case multiplayer_event_type::member_property_changed:
        {
            return ref new MemberPropertyChangedEventArgs(
                std::dynamic_pointer_cast<member_property_changed_event_args>(m_cppObj.event_args())
                );
        }

    case multiplayer_event_type::session_property_changed:
        {
            return ref new SessionPropertyChangedEventArgs(
                std::dynamic_pointer_cast<session_property_changed_event_args>(m_cppObj.event_args())
                );
        }

    case multiplayer_event_type::host_changed:
        {
            return ref new HostChangedEventArgs(
                std::dynamic_pointer_cast<host_changed_event_args>(m_cppObj.event_args())
                );
        }

    case multiplayer_event_type::join_lobby_completed:
        {
            return ref new JoinLobbyCompletedEventArgs(
                std::dynamic_pointer_cast<join_lobby_completed_event_args>(m_cppObj.event_args())
                );
        }

    case multiplayer_event_type::find_match_completed:
        {
            return ref new FindMatchCompletedEventArgs(
                std::dynamic_pointer_cast<find_match_completed_event_args>(m_cppObj.event_args())
                );
        }

    case multiplayer_event_type::perform_qos_measurements:
        {
            return ref new PerformQosMeasurementsEventArgs(
                std::dynamic_pointer_cast<perform_qos_measurements_event_args>(m_cppObj.event_args())
                );
        }

    case multiplayer_event_type::tournament_registration_state_changed:
    {
        return ref new TournamentRegistrationStateChangedEventArgs(
            std::dynamic_pointer_cast<tournament_registration_state_changed_event_args>(m_cppObj.event_args())
            );
    }

    case multiplayer_event_type::tournament_game_session_ready:
        {
            return ref new TournamentGameSessionReadyEventArgs(
                std::dynamic_pointer_cast<tournament_game_session_ready_event_args>(m_cppObj.event_args())
                );
        }

    default:
        {
            return nullptr;
        }
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_END
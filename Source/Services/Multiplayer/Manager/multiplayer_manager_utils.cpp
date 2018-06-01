// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

bool
multiplayer_manager_utils::do_sessions_match(
    _In_ const std::shared_ptr<multiplayer_session>& session1,
    _In_ const std::shared_ptr<multiplayer_session>& session2
    )
{
    if (session1 == nullptr || session2 == nullptr)
    {
        return false;
    }

    return do_session_references_match(session1->session_reference(), session2->session_reference());
}

bool
multiplayer_manager_utils::is_host(
    _In_ const string_t& memberDeviceToken,
    _In_ const std::shared_ptr<multiplayer_session>& session
    )
{
    if(memberDeviceToken.empty() || session == nullptr || session->session_properties() == nullptr)
    {
        return false;
    }

    return utils::str_icmp(memberDeviceToken, session->session_properties()->host_device_token()) == 0;
}

bool
multiplayer_manager_utils::is_player_in_session(
    _In_ const string_t& xboxUserId,
    _In_ const std::shared_ptr<multiplayer_session>& session
    )
{
    if(xboxUserId.empty() || session == nullptr)
    {
        return false;
    }

    for (const auto& member : session->members())
    {
        if (utils::str_icmp(xboxUserId, member->xbox_user_id()) == 0)
        {
            return true;
        }
    }

    return false;
}

std::shared_ptr<multiplayer_session_member>
    multiplayer_manager_utils::get_player_in_session(
    _In_ const string_t& xboxUserId,
    _In_ const std::shared_ptr<multiplayer_session>& session
    )
{
    if(xboxUserId.empty()) return nullptr;

    if(session == nullptr)
    {
        return nullptr;
    }

    for (const auto& member : session->members())
    {
        if (utils::str_icmp(xboxUserId, member->xbox_user_id()) == 0)
        {
            return member;
        }
    }

    return nullptr;
}

std::shared_ptr<multiplayer_session_member>
multiplayer_manager_utils::host_member(
    _In_ const std::shared_ptr<multiplayer_session>& session
    )
{
    if(session == nullptr || session->session_properties() == nullptr)
    {
        return nullptr;
    }

    for (const auto& member : session->members())
    {
        if (utils::str_icmp(member->device_token(), session->session_properties()->host_device_token() ) == 0)
        {
            return member;
        }
    }

    return nullptr;
}

string_t
multiplayer_manager_utils::get_local_user_xbox_user_id(
    _In_ xbox_live_user_t user
    )
{
    if (user == nullptr)
    {
        return string_t();
    }

    return utils::string_t_from_internal_string(user_context::get_user_id(user));
}

bool
multiplayer_manager_utils::has_session_property_changed(
    _In_ const std::shared_ptr<multiplayer_session>& session1,
    _In_ const std::shared_ptr<multiplayer_session>& session2,
    _In_ const string_t& propertyName
    )
{
    if (session1 == nullptr && session2 == nullptr)
    {
        return true;
    }

    if (session1 == nullptr || session2 == nullptr)
    {
        return false;
    }

    const auto& customProp1 = session1->session_properties()->session_custom_properties_json();
    const auto& customProp2 = session2->session_properties()->session_custom_properties_json();
    if ( (customProp1.has_field(propertyName) && !customProp2.has_field(propertyName)) ||
         (!customProp1.has_field(propertyName) && customProp2.has_field(propertyName)))
    {
        return true;
    }

    if (customProp1.has_field(propertyName) && customProp2.has_field(propertyName))
    {
        const auto& prop1 = customProp1.at(propertyName).as_string();
        const auto& prop2 = customProp2.at(propertyName).as_string();
        return utils::str_icmp(prop1, prop2) != 0;
    }

    return false;
}

bool
multiplayer_manager_utils::is_local(
    _In_ const string_t& xboxUserId,
    _In_ const std::map<string_t, std::shared_ptr<multiplayer_local_user>>& xboxLiveContextMap
    )
{
    if(xboxUserId.empty()) return false;

    for(const auto& xboxLiveContext : xboxLiveContextMap)
    {
        const auto& localUser =  xboxLiveContext.second;
        if (localUser != nullptr && utils::str_icmp(xboxUserId, localUser->xbox_user_id()) == 0)
        {
            return true;
        }
    }

    return false;
}

std::shared_ptr<multiplayer_member>
multiplayer_manager_utils::convert_to_game_member(
    _In_ const std::shared_ptr<multiplayer_session_member>& member,
    _In_ const std::shared_ptr<multiplayer_session>& lobbySession,
    _In_ const std::shared_ptr<multiplayer_session>& gameSession,
    _In_ const std::map<string_t, std::shared_ptr<multiplayer_local_user>>& xboxLiveContextMap
    )
{
    return std::make_shared<multiplayer_member>(
        member,
        is_local(member->xbox_user_id(), xboxLiveContextMap),           // isLocal
        is_host(member->device_token(), gameSession),                   // isGameHost
        is_host(member->device_token(), lobbySession),                  // isLobbyHost
        is_player_in_session(member->xbox_user_id(), lobbySession),     // isinLobby
        is_player_in_session(member->xbox_user_id(), gameSession)       // isinGame
        );
}

std::shared_ptr<multiplayer_member>
multiplayer_manager_utils::convert_to_game_member(
    _In_ const std::shared_ptr<multiplayer_session_member>& member,
    _In_ const std::shared_ptr<multiplayer_session>& lobbySession,
    _In_ const std::shared_ptr<multiplayer_session>& gameSession,
    _In_ bool isLocal
    )
{
    return std::make_shared<multiplayer_member>(
        member,
        isLocal,                                                        // isLocal
        is_host(member->device_token(), gameSession),                   // isGameHost
        is_host(member->device_token(), lobbySession),                  // isLobbyHost
        is_player_in_session(member->xbox_user_id(), lobbySession),     // isinLobby
        is_player_in_session(member->xbox_user_id(), gameSession)       // isinGame
        );
}

bool
multiplayer_manager_utils::compare_sessions(
    _In_ const std::shared_ptr<multiplayer_session>& session1,
    _In_ const std::shared_ptr<multiplayer_session>& session2
    )
{
    if (session1 == nullptr && session2 == nullptr)
    {
        return true;
    }

    if ( (session1 == nullptr && session2 != nullptr) ||
         (session1 != nullptr && session2 == nullptr) ||
         session1->change_number() != session2->change_number())
    {
        return false;
    }

    return true;
}


// joinable_by_friends                 : !closed & JoinRestricion::Followed
// invite_only                         : !closed & JoinRestricion::Local
// disable_while_game_in_progress      : closed & JoinRestricion::Local (when in game)
// closed                              : closed & JoinRestricion::Local

void
multiplayer_manager_utils::set_joinability(
    _In_ joinability value,
    _In_ std::shared_ptr<multiplayer_session> sessionToCommit,
    _In_ bool isGameInProgress
    )
{
    sessionToCommit->set_closed(false);
    sessionToCommit->session_properties()->set_join_restriction(multiplayer_session_restriction::local);
    if (value == joinability::joinable_by_friends)
    {
        sessionToCommit->session_properties()->set_join_restriction(multiplayer_session_restriction::followed);
    }
    else if (value == joinability::closed ||
            (isGameInProgress && value == joinability::disable_while_game_in_progress))
    {
        sessionToCommit->set_closed(true);
    }

    string_t jsonValueStr = convert_joinability_to_string(value);
    sessionToCommit->set_session_custom_property_json(multiplayer_lobby_client::c_joinabilityPropertyName, web::json::value::string(jsonValueStr));
}

joinability
multiplayer_manager_utils::get_joinability(
    _In_ std::shared_ptr<multiplayer_session_properties> sessionProperties
    )
{
    string_t joinableStr;
    auto customPropertiesjson = sessionProperties->session_custom_properties_json();
    if (customPropertiesjson.has_field(multiplayer_lobby_client::c_joinabilityPropertyName))
    {
        joinableStr = customPropertiesjson.at(multiplayer_lobby_client::c_joinabilityPropertyName).as_string();
    }

    return convert_string_to_joinability(joinableStr);
}

joinability
multiplayer_manager_utils::convert_string_to_joinability(
    _In_ const string_t& value
    )
{
    if (utils::str_icmp(value, _T("joinable_by_friends")) == 0)
    {
        return joinability::joinable_by_friends;
    }
    else if (utils::str_icmp(value, _T("invite_only")) == 0)
    {
        return joinability::invite_only;
    }
    else if (utils::str_icmp(value, _T("disable_while_game_in_progress")) == 0)
    {
        return joinability::disable_while_game_in_progress;
    }
    else if (utils::str_icmp(value, _T("closed")) == 0)
    {
        return joinability::closed;
    }

    return joinability::none;
}

string_t
multiplayer_manager_utils::convert_joinability_to_string(
    _In_ joinability value
    )
{
    switch (value)
    {
        case xbox::services::multiplayer::manager::joinability::joinable_by_friends: return _T("joinable_by_friends");
        case xbox::services::multiplayer::manager::joinability::invite_only: return _T("invite_only");
        case xbox::services::multiplayer::manager::joinability::disable_while_game_in_progress: return _T("disable_while_game_in_progress");
        case xbox::services::multiplayer::manager::joinability::closed: return _T("closed");
        default: return _T("none");
    }
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "pplx/pplxtasks.h"
#include "xsapi/services.h"
#include "user_context.h"
#include "multiplayer_manager_internal.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::real_time_activity;
using namespace pplx;
#if UWP_API || TV_API || UNIT_TEST_SERVICES
using namespace Windows::Foundation::Collections;
#endif
NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

multiplayer_client_manager::multiplayer_client_manager(const multiplayer_client_manager& other) 
{
    std::lock_guard<std::mutex> lock(other.m_clientRequestLock);

    m_sessionChangedContext = other.m_sessionChangedContext;
    m_subscriptionLostContext = other.m_subscriptionLostContext;
    m_rtaResyncContext = other.m_rtaResyncContext;
    m_primaryXboxLiveContext = other.m_primaryXboxLiveContext == nullptr ? nullptr : other.m_primaryXboxLiveContext;
    m_lastPendingRead = other.m_lastPendingRead == nullptr ? nullptr : other.m_lastPendingRead;
    m_latestPendingRead = other.m_latestPendingRead == nullptr ? nullptr : other.m_latestPendingRead;
}

multiplayer_client_manager::multiplayer_client_manager(
    _In_ string_t lobbySessionTemplateName
    ) :
    m_lobbySessionTemplateName(std::move(lobbySessionTemplateName)),
    m_sessionChangedContext(0),
    m_subscriptionLostContext(0),
    m_rtaResyncContext(0),
    m_subscriptionsLostFired(false),
    m_autoFillMembers(false)
{
    m_multiplayerLocalUserManager = std::make_shared<multiplayer_local_user_manager>();
}

void
multiplayer_client_manager::register_local_user_manager_events()
{
    std::weak_ptr<multiplayer_client_manager> thisWeakPtr = shared_from_this();
    m_sessionChangedContext = m_multiplayerLocalUserManager->add_multiplayer_session_changed_handler([thisWeakPtr](_In_ const multiplayer_session_change_event_args& args)
    {
        std::shared_ptr<multiplayer_client_manager> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->on_session_changed(args);
        }
    });

    m_subscriptionLostContext = m_multiplayerLocalUserManager->add_multiplayer_subscription_lost_handler([thisWeakPtr](void)
    {
        std::shared_ptr<multiplayer_client_manager> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->on_multiplayer_subscriptions_lost();
        }
    });

    m_rtaResyncContext = m_multiplayerLocalUserManager->add_rta_resync_handler([thisWeakPtr](void)
    {
        std::shared_ptr<multiplayer_client_manager> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->on_resync_message_received();
        }
    });
}

void
multiplayer_client_manager::initialize()
{
    if (m_multiplayerLocalUserManager == nullptr)
    {
        m_multiplayerLocalUserManager = std::make_shared<multiplayer_local_user_manager>();
        register_local_user_manager_events();
    }

    m_latestPendingRead = std::make_shared<multiplayer_client_pending_reader>(
        m_lobbySessionTemplateName, 
        m_multiplayerLocalUserManager
        );

    m_lastPendingRead = std::make_shared<multiplayer_client_pending_reader>();
    m_subscriptionsLostFired.store(false);
    m_latestPendingRead->set_auto_fill_members_during_matchmaking(m_autoFillMembers);
}

void multiplayer_client_manager::shutdown()
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    destroy();
}

void
multiplayer_client_manager::destroy()
{
    m_latestPendingRead.reset();
    m_lastPendingRead.reset();
    if (m_multiplayerLocalUserManager != nullptr)
    {
        m_multiplayerLocalUserManager->remove_multiplayer_session_changed_handler(m_sessionChangedContext);
        m_multiplayerLocalUserManager->remove_multiplayer_subscription_lost_handler(m_subscriptionLostContext);
        m_multiplayerLocalUserManager->remove_rta_resync_handler(m_rtaResyncContext);
        m_multiplayerLocalUserManager.reset();
    }
}

std::shared_ptr<multiplayer_local_user_manager>
multiplayer_client_manager::local_user_manager()
{
    return m_multiplayerLocalUserManager;
}

xbox_live_result<void>
multiplayer_client_manager::set_properties(
    _In_ const multiplayer_session_reference& sessionRef,
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
    )
{
    // Note: sessionRef can be empty for the lobby initially as we may have not created one yet.
    RETURN_CPP_IF(name.empty(), void, xbox_live_error_code::invalid_argument, "Name was empty");

    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    auto latestPending = latest_pending_read();
    RETURN_CPP_IF(latestPending == nullptr || get_xbox_live_context_map().size() == 0, void, xbox_live_error_code::logic_error, "Call add_local_user() before writing lobby properties.");

    latestPending->set_properties(sessionRef, name, valueJson, context);
    return xbox_live_result<void>();
}

xbox_live_result<void>
multiplayer_client_manager::set_joinability(
    _In_ xbox::services::multiplayer::manager::joinability value,
    _In_opt_ context_t context
    )
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    auto latestPending = latest_pending_read();
    RETURN_CPP_IF(latestPending == nullptr || get_xbox_live_context_map().size() == 0, void, xbox_live_error_code::logic_error, "Call add_local_user() before writing lobby properties.");

    return latestPending->lobby_client()->set_joinability(value, context);
}

xbox_live_result<void>
multiplayer_client_manager::set_synchronized_host(
    _In_ const multiplayer_session_reference& sessionRef,
    _In_ const string_t& hostDeviceToken,
    _In_opt_ context_t context
    )
{
    // Note: sessionRef can be empty for the lobby initially as we may have not created one yet.
    RETURN_CPP_IF(hostDeviceToken.empty(), void, xbox_live_error_code::invalid_argument, "HostDeviceToken was empty");

    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    auto latestPending = latest_pending_read();
    RETURN_CPP_IF(latestPending == nullptr || get_xbox_live_context_map().size() == 0, void, xbox_live_error_code::logic_error, "Call add_local_user() before writing host properties.");

    latestPending->set_synchronized_host(sessionRef, hostDeviceToken, context);
    return xbox_live_result<void>();
}

xbox_live_result<void>
multiplayer_client_manager::set_synchronized_properties(
    _In_ const multiplayer_session_reference& sessionRef,
    _In_ const string_t& name,
    _In_ const web::json::value& valueJson,
    _In_opt_ context_t context
    )
{
    // Note: sessionRef can be empty for the lobby initially as we may have not created one yet.
    RETURN_CPP_IF(name.empty(), void, xbox_live_error_code::invalid_argument, "Name was empty");

    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    auto latestPending = latest_pending_read();
    RETURN_CPP_IF(latestPending == nullptr || get_xbox_live_context_map().size() == 0, void, xbox_live_error_code::logic_error, "Call add_local_user() before writing lobby properties.");

    latestPending->set_synchronized_properties(sessionRef, name, valueJson, context);
    return xbox_live_result<void>();
}

void
multiplayer_client_manager::synchronized_write_completed(
    _In_ std::error_code errorCode,
    _In_ std::string errorMessage,
    _In_ multiplayer_event_type eventType,
    _In_ multiplayer_session_type sessionType
    )
{
    add_multiplayer_event(eventType, sessionType, errorCode, errorMessage);
}

xbox_live_result<void>
multiplayer_client_manager::join_lobby_by_handle(
    _In_ const string_t& handleId,
    _In_ std::vector<xbox_live_user_t> users
    )
{
    if(handleId.empty()) return xbox_live_result<void>(xbox_live_error_code::invalid_argument);
    if (users.size() == 0) return xbox_live_result<void>(xbox_live_error_code::invalid_argument);

    auto latestPending = latest_pending_read();
    if (latestPending == nullptr)
    {
        initialize();
        latestPending = latest_pending_read();
    }

    latestPending->lobby_client()->add_local_users(users, handleId);
    return xbox_live_result<void>();
}

xbox_live_result<void>
multiplayer_client_manager::join_lobby_by_session_reference(
    _In_ const multiplayer_session_reference& sessionRef,
    _In_ std::vector<xbox_live_user_t> users
    )
{
    if(sessionRef.is_null()) return xbox_live_result<void>(xbox_live_error_code::invalid_argument);
    if (users.size() == 0) return xbox_live_result<void>(xbox_live_error_code::invalid_argument);

    auto latestPending = latest_pending_read();
    if (latestPending == nullptr)
    {
        initialize();
        latestPending = latest_pending_read();
    }

    latestPending->lobby_client()->add_local_users(users, sessionRef);
    return xbox_live_result<void>();
}

#if UWP_API || TV_API || UNIT_TEST_SERVICES
xbox_live_result<void>
multiplayer_client_manager::join_lobby(
    _In_ Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ eventArgs,
    _In_ std::vector<xbox_live_user_t> users
    )
{
    if (users.size() == 0) return xbox_live_error_code::invalid_argument;

    auto url = ref new Windows::Foundation::Uri(eventArgs->Uri->RawUri);
    return join_lobby(url, users);
}

xbox_live_result<void>
multiplayer_client_manager::join_lobby(
    _In_ Windows::Foundation::Uri^ url,
    _In_ std::vector<xbox_live_user_t> users
    )
{
    string_t handleId;
    string_t invitedXuid;
    multiplayer_session_reference teamSessionRef;
    if (utils::str_icmp(url->Host->Data(), _T("inviteHandleAccept")) == 0)
    {
        handleId = url->QueryParsed->GetFirstValueByName("handle")->Data();
        invitedXuid = url->QueryParsed->GetFirstValueByName("invitedXuid")->Data();
    }
    else if(utils::str_icmp(url->Host->Data(), _T("activityHandleJoin")) == 0)
    {
        handleId = url->QueryParsed->GetFirstValueByName("handle")->Data();
        invitedXuid = url->QueryParsed->GetFirstValueByName("joinerXuid")->Data();
    }
    else if (utils::str_icmp(url->Host->Data(), _T("tournament")) == 0)
    {
        // Only used for Tournament MPM integration support.
        string_t action = url->QueryParsed->GetFirstValueByName("action")->Data();
        if (utils::str_icmp(action, _T("joinGame")) != 0)
        {
            return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Invalid tournament protocol passed into join_lobby() API.");
        }

        invitedXuid = url->QueryParsed->GetFirstValueByName("joinerXuid")->Data();

        string_t scid = url->QueryParsed->GetFirstValueByName("scid")->Data();
        string_t templateName = url->QueryParsed->GetFirstValueByName("templateName")->Data();
        string_t sessionName = url->QueryParsed->GetFirstValueByName("name")->Data();
        teamSessionRef = multiplayer_session_reference(scid, templateName, sessionName);
    }
    else
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Invalid protocol passed into join_lobby() API.");
    }

    // Check if the xuid matches with the sent users.
    bool invitedUserFound = false;
    int invitedUserIndex = 0;
    for (auto& user: users)
    {
        if (utils::str_icmp(invitedXuid, utils::string_t_from_internal_string(user_context::get_user_id(user))) == 0)
        {
            invitedUserFound = true;
            break;
        }
        invitedUserIndex++;
    }

    if (!invitedUserFound)
    {
        // The invited user hasn't been added.
        std::shared_ptr<join_lobby_completed_event_args> joinLobbyEventArgs = std::make_shared<join_lobby_completed_event_args>(invitedXuid);

        multiplayer_event multiplayerEvent(
            xbox_live_error_code::logic_error,
            "InvitedXuid's user hasn't been added. Pass in the invited user into join_lobby() API.",
            multiplayer_event_type::join_lobby_completed,
            std::dynamic_pointer_cast<join_lobby_completed_event_args>(joinLobbyEventArgs),
            multiplayer_session_type::lobby_session
        );

        // Since m_latestPendingRead hasn't been initialized yet, this will ensure 
        // the event is still returned correctly through multiplayer_manager::do_work();
        m_multiplayerEventQueue.push_back(multiplayerEvent);

        return xbox_live_result<void>(xbox_live_error_code::logic_error, "Pass in the invited user into join_lobby() API.");
    }
    else if (invitedUserFound && invitedUserIndex > 0)
    {
        auto invitedUser = users[0];
        users[0] = users[invitedUserIndex];
        users[invitedUserIndex] = invitedUser;
    }

    // This will also join any game that is associated with the lobby.
    if (!handleId.empty())
    {
        return join_lobby_by_handle(handleId, users);
    }

    return join_lobby_by_session_reference(teamSessionRef, users);
}
#endif

xbox_live_result<void>
multiplayer_client_manager::join_game_from_lobby(
    _In_ const string_t& sessionTemplateName
    )
{
    auto primaryContext = get_primary_context();
    auto latestPending = latest_pending_read();
    if( primaryContext == nullptr || latestPending == nullptr || latestPending->lobby_client()->session() == nullptr)
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "No lobby session exists. Call add_local_user() to create a lobby first.");
    }

    if (latestPending->game_client()->session() != nullptr)
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "A game session already exists. Call leave_game() to leave existing game before creating a new one.");
    }

    if (latestPending->match_client()->match_status() > match_status::none)
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "Matchmaking is currently in progress. Call cancel_match() before joining a game.");
    }

    latestPending->game_client()->set_game_session_template(sessionTemplateName);
    return latestPending->game_client()->join_game_from_lobby_helper();
}

xbox_live_result<void>
multiplayer_client_manager::join_game(
    _In_ const string_t& sessionName,
    _In_ const string_t& sessionTemplateName,
    _In_ const std::vector<string_t>& xboxUserIds
    )
{
    if(sessionName.empty()) return xbox_live_result<void>(xbox_live_error_code::invalid_argument);

    std::shared_ptr<xbox_live_context_impl> primaryContext = get_primary_context();
    RETURN_CPP_IF(primaryContext == nullptr, void, xbox_live_error_code::logic_error, "Call add_local_user() before joining.");
    auto latestPending = latest_pending_read();
    if (latestPending == nullptr)
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "No lobby session exists. Call add_local_user() to create a lobby first.");
    }

    if (latestPending->match_client()->match_status() > match_status::none)
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "Matchmaking is currently in progress. Call cancel_match() before joining a game.");
    }

    std::weak_ptr<multiplayer_client_manager> thisWeakPtr = shared_from_this();
    create_task([thisWeakPtr, primaryContext, sessionName, sessionTemplateName, xboxUserIds]()
    {
        std::shared_ptr<multiplayer_client_manager> pThis(thisWeakPtr.lock());
        RETURN_CPP_IF(pThis == nullptr, void, xbox_live_error_code::generic_error, "multiplayer_client_manager class was destroyed.");
        auto latestPending2 = pThis->latest_pending_read();
        RETURN_CPP_IF(latestPending2 == nullptr, void, xbox_live_error_code::generic_error, "multiplayer_client_manager class was destroyed.");
        auto gameClient = latestPending2->game_client();
        RETURN_CPP_IF(gameClient == nullptr, void, xbox_live_error_code::generic_error, "multiplayer_game_client class was destroyed.");

        if (xboxUserIds.size() > 0)
        {
            // Create a session with reservations.
            multiplayer_session_reference gameSessionRef(utils::string_t_from_internal_string(utils::try_get_override_scid()), sessionTemplateName, sessionName);
            auto gameSession = std::make_shared<multiplayer_session>(
                utils::string_t_from_internal_string(primaryContext->xbox_live_user_id()),
                gameSessionRef,
                xboxUserIds
                );

            gameSession->join(web::json::value::null(), false);
            for (const auto& memberXuid : xboxUserIds)
            {
                if (utils::str_icmp(utils::internal_string_from_string_t(memberXuid), primaryContext->xbox_live_user_id()) != 0)
                {
                    gameSession->add_member_reservation(memberXuid);
                }
            }
            auto result = primaryContext->multiplayer_service().write_session(gameSession, multiplayer_session_write_mode::update_or_create_new).get();
            if (result.err())
            {
                multiplayer_event multiplayerEvent(
                    result.err(),
                    result.err_message(),
                    multiplayer_event_type::join_game_completed,
                    std::make_shared<multiplayer_event_args>(),
                    multiplayer_session_type::game_session
                    );

                pThis->add_to_multiplayer_event_queue(multiplayerEvent);
                return xbox_live_result<void>();
            }

            // Continue joining the session for all local users.
        }

        gameClient->set_game_session_template(sessionTemplateName);
        RETURN_EXCEPTION_FREE_XBOX_LIVE_RESULT(gameClient->join_game_helper(sessionName), void);
    });

    return xbox_live_result<void>();
}

xbox_live_result<void>
multiplayer_client_manager::leave_game()
{
    std::shared_ptr<xbox_live_context_impl> primaryContext = get_primary_context();
    auto latestPendingRead = latest_pending_read();
    if (latestPendingRead == nullptr || primaryContext == nullptr)
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "Call add_local_user() before committing.");
    }

    auto gameSession = latestPendingRead->game_client()->session();
    if (gameSession != nullptr)
    {
        latestPendingRead->game_client()->leave_remote_session(gameSession, true, true);
    }

    if (latestPendingRead->match_client()->match_status() != match_status::none)
    {
        latestPendingRead->match_client()->cancel_match();
        latestPendingRead->match_client()->set_match_status(match_status::canceled);
        latestPendingRead->match_client()->handle_find_match_completed(xbox_live_error_code::generic_error, "Matchmaking request was canceled since leave_game() was called.");
    }

    m_multiplayerLocalUserManager->change_all_local_user_game_state(multiplayer_local_user_game_state::unknown);
    return xbox_live_result<void>(xbox_live_error_code::no_error);
}

pplx::task<xbox_live_result<std::vector<multiplayer_activity_details>>>
multiplayer_client_manager::get_activities_for_social_group(
    _In_ xbox_live_user_t user,
    _In_ const string_t& socialGroup
    )
{
    RETURN_TASK_CPP_INVALIDARGUMENT_IF(user == nullptr, std::vector<multiplayer_activity_details>, "Invalid xboxLiveContext argument passed.");

    return get_multiplayer_service(user).get_activities_for_social_group(
        utils::string_t_from_internal_string(utils::try_get_override_scid()),
        multiplayer_manager_utils::get_local_user_xbox_user_id(user),
        socialGroup);
}

xbox_live_result<void>
multiplayer_client_manager::invite_friends(
    _In_ xbox_live_user_t user,
    _In_ const string_t& contextStringId,
    _In_ const string_t& customActivationContext
    )
{
    RETURN_CPP_IF(user == nullptr, void, xbox_live_error_code::invalid_argument, "Invalid user argument passed.");
    auto latestPendingRead = latest_pending_read();
    RETURN_CPP_IF(latestPendingRead == nullptr, void, xbox_live_error_code::logic_error, "Call add_local_user() and wait for user_added completion event before sending invites.");
    RETURN_CPP_IF(latestPendingRead->lobby_client()->session() == nullptr, void, xbox_live_error_code::logic_error, "Call add_local_user() and wait for user_added completion event before sending invites.");

    std::weak_ptr<multiplayer_client_manager> thisWeakPtr = shared_from_this();

#if TV_API

    auto sessionRef = latestPendingRead->lobby_client()->session()->session_reference();
    auto sessionReferenceToInviteTo = ref new Windows::Xbox::Multiplayer::MultiplayerSessionReference(
        ref new Platform::String(sessionRef.session_name().c_str()),
        ref new Platform::String(sessionRef.service_configuration_id().c_str()),
        ref new Platform::String(sessionRef.session_template_name().c_str())
        );

    auto asyncOp = Windows::Xbox::UI::SystemUI::ShowSendGameInvitesAsync(
        user,
        sessionReferenceToInviteTo,
        ref new Platform::String(contextStringId.c_str()),
        ref new Platform::String(customActivationContext.c_str())
        );

    pplx::create_task(asyncOp)
    .then([thisWeakPtr](pplx::task<void> t)
    {
        std::shared_ptr<multiplayer_client_manager> pThis(thisWeakPtr.lock());
        if(pThis != nullptr)
        {
            try
            {
                t.get();
                pThis->add_multiplayer_event(multiplayer_event_type::invite_sent, multiplayer_session_type::lobby_session);
            }
            catch (...)
            {
                xbox_live_error_code err = utils::convert_exception_to_xbox_live_error_code();
                pThis->add_multiplayer_event(multiplayer_event_type::invite_sent, multiplayer_session_type::lobby_session, err, "Failed sending invites.");
            }
        }
    });
#else
    UNREFERENCED_PARAMETER(contextStringId);
    UNREFERENCED_PARAMETER(customActivationContext);
#if !UNIT_TEST_SERVICES

    auto asyncOp = xbox::services::system::title_callable_ui::show_game_invite_ui(
        latestPendingRead->lobby_client()->session()->session_reference(),
        contextStringId
        );

    pplx::create_task(asyncOp)
    .then([thisWeakPtr](xbox_live_result<void> result)
    {
        std::shared_ptr<multiplayer_client_manager> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->add_multiplayer_event(multiplayer_event_type::invite_sent, multiplayer_session_type::lobby_session, result.err(), result.err_message());
        }
    });
#endif
#endif

    return xbox_live_result<void>();
}

xbox_live_result<void>
multiplayer_client_manager::invite_users(
    _In_ xbox_live_user_t user,
    _In_ const std::vector<string_t>& xboxUserIds,
    _In_ const string_t& contextStringId,
    _In_ const string_t& customActivationContext
    )
{
    RETURN_CPP_IF(user == nullptr, void, xbox_live_error_code::invalid_argument, "Invalid user argument passed.");
    auto latestPendingRead = latest_pending_read();
    RETURN_CPP_IF(latestPendingRead == nullptr, void, xbox_live_error_code::logic_error, "Call add_local_user() and wait for user_added completion event before sending invites.");
    RETURN_CPP_IF(latestPendingRead->lobby_client()->session() == nullptr, void, xbox_live_error_code::logic_error, "Call add_local_user() and wait for user_added completion event before sending invites.");

    std::weak_ptr<multiplayer_client_manager> thisWeakPtr = shared_from_this();

    auto task = get_multiplayer_service(user).send_invites(
            latestPendingRead->lobby_client()->session()->session_reference(),
            xboxUserIds,
            utils::try_get_master_title_id(),
            contextStringId,
            customActivationContext
            )
    .then([thisWeakPtr](xbox_live_result<std::vector<string_t>> result)
    {
        std::shared_ptr<multiplayer_client_manager> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->add_multiplayer_event(multiplayer_event_type::invite_sent, multiplayer_session_type::lobby_session, result.err(), result.err_message());
        }
    });

    return xbox_live_result<void>();
}

multiplayer_service&
multiplayer_client_manager::get_multiplayer_service(
    _In_ xbox_live_user_t user
    )
{
    auto localUser = m_multiplayerLocalUserManager->get_local_user(user);
    if (localUser != nullptr)
    {
        m_clientManagerMultiplayerService = localUser->context()->multiplayer_service();
    }
    else
    {
        std::shared_ptr<xbox::services::user_context> userContext;

        userContext = std::make_shared<xbox::services::user_context>(user);
        std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings = std::make_shared<xbox::services::xbox_live_context_settings>();
        std::shared_ptr<xbox_live_app_config> appConfig = xbox::services::xbox_live_app_config::get_app_config_singleton();
        m_clientManagerMultiplayerService = multiplayer_service(userContext, xboxLiveContextSettings, appConfig, nullptr);
    }

    return m_clientManagerMultiplayerService;
}

std::shared_ptr<multiplayer_client_pending_reader>
multiplayer_client_manager::latest_pending_read() const
{
    return m_latestPendingRead;
}

std::shared_ptr<multiplayer_client_pending_reader>
multiplayer_client_manager::last_pending_read() const
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    return m_lastPendingRead;
}

std::shared_ptr<multiplayer_lobby_client>
multiplayer_client_manager::lobby_client() const
{
    return m_latestPendingRead->lobby_client();
}

bool
multiplayer_client_manager::is_request_in_progress()
{
    if (m_latestPendingRead->lobby_client()->is_request_in_progress() ||
        m_latestPendingRead->game_client()->is_request_in_progress())
    {
        return true;
    }

    return false;
}

bool
multiplayer_client_manager::is_update_avaialable()
{
    if (m_latestPendingRead == nullptr || m_lastPendingRead == nullptr)
    {
        return false;
    }

    if (m_lastPendingRead->is_update_avaialable(*m_latestPendingRead))
    {
        return true;
    }

    if (get_xbox_live_context_map().size() == 0 && is_request_in_progress())
    {
        return true;
    }

    // Always do work for match
    m_latestPendingRead->process_match_events();

    return false;
}

std::vector<multiplayer_event>
multiplayer_client_manager::do_work()
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);

    if (m_latestPendingRead == nullptr)
    {
        return std::vector<multiplayer_event>();
    }

    m_latestPendingRead->do_work();

    process_events(m_latestPendingRead->lobby_client()->session(), m_lastPendingRead->lobby_client()->session(), multiplayer_session_type::lobby_session);
    process_events(m_latestPendingRead->game_client()->session(), m_lastPendingRead->game_client()->session(), multiplayer_session_type::game_session);
    process_events(m_latestPendingRead->match_client()->session(), m_lastPendingRead->match_client()->session(), multiplayer_session_type::match_session);

    m_lastPendingRead->deep_copy_if_updated(*m_latestPendingRead);
    auto eventQueue = m_lastPendingRead->multiplayer_event_queue();

    if (get_xbox_live_context_map().size() == 0 && !is_request_in_progress())
    {
        if (!m_subscriptionsLostFired)
        {
            // Force client disconnected event to fire for consistent developer behavior.
            on_multiplayer_subscriptions_lost();
        }
        else
        {
            // If the last person just left and no more events left, destroy all objects.
            destroy();
            return eventQueue;
        }
    }

    m_latestPendingRead->clear_multiplayer_event_queue();
    m_lastPendingRead->clear_multiplayer_event_queue();

    return eventQueue;
}

std::map<string_t, std::shared_ptr<multiplayer_local_user>>
multiplayer_client_manager::get_xbox_live_context_map()
{
    return m_multiplayerLocalUserManager->get_local_user_map();
}

std::shared_ptr<xbox_live_context_impl>
multiplayer_client_manager::get_primary_context()
{
    return m_multiplayerLocalUserManager->get_primary_context();
}

void
multiplayer_client_manager::on_multiplayer_subscriptions_lost()
{
    std::weak_ptr<multiplayer_client_manager> thisWeakPtr = shared_from_this();
    create_task([thisWeakPtr]()
    {
        std::shared_ptr<multiplayer_client_manager> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            std::lock_guard<std::mutex> guard(pThis->m_clientRequestLock);

            bool expected = false;
            if (pThis->m_subscriptionsLostFired.compare_exchange_strong(expected, true))
            {
                // Fired when the title's connection to MPSD using the real-time activity service is lost. 
                // When this event occurs, the title should shut down the multiplayer.

                auto lobbyClient = pThis->lobby_client();
                if (lobbyClient != nullptr)
                {
                    lobbyClient->remove_all_local_users();
                }

                pThis->add_multiplayer_event_helper(multiplayer_event_type::client_disconnected_from_multiplayer_service, multiplayer_session_type::lobby_session);
            }
        }
    });
}

void
multiplayer_client_manager::on_resync_message_received()
{
    // Upon receiving RTA resync message, re-fetch all multiplayer sessions.
    // Note: You could get multiple re-sync messages. It's recommended that you only fetch once every 30 secs.
    if (m_latestPendingRead != nullptr)
    {
        auto latestPendingReadSharedPtr = m_latestPendingRead;
        create_task([latestPendingReadSharedPtr]()
        {
            latestPendingReadSharedPtr->lobby_client()->session_writer()->on_resync_message_received();
            latestPendingReadSharedPtr->game_client()->session_writer()->on_resync_message_received();
        });
    }
}

void
multiplayer_client_manager::on_session_changed(
    _In_ const multiplayer_session_change_event_args& args
    )
{
    std::lock_guard<std::mutex> guard(m_synchronizeWriteWithTapLock);

    if (m_latestPendingRead != nullptr)
    {
        multiplayer_session_reference sessionRef = args.session_reference();
        if (m_latestPendingRead->is_match(sessionRef))
        {
            m_latestPendingRead->match_client()->on_session_changed(args);
        }
        
        if (m_latestPendingRead->is_lobby(sessionRef))
        {
            m_latestPendingRead->lobby_client()->session_writer()->on_session_changed(args);
        }
        else if (m_latestPendingRead->is_game(sessionRef))
        {
            m_latestPendingRead->game_client()->session_writer()->on_session_changed(args);
        }
    }
}

std::vector<multiplayer_event>
multiplayer_client_manager::event_queue() const
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    return m_multiplayerEventQueue;
}

void
multiplayer_client_manager::clear_event_queue()
{
    std::lock_guard<std::mutex> lock(m_clientRequestLock);
    m_multiplayerEventQueue.clear();
}

void
multiplayer_client_manager::add_to_multiplayer_event_queue(
    _In_ multiplayer_event multiplayerEvent
    )
{
    // Note: This function does not require a lock. Caller already has a m_clientRequestLock

    if (m_latestPendingRead != nullptr)
    {
        m_latestPendingRead->add_to_multiplayer_event_queue(multiplayerEvent);
    }
}

void
multiplayer_client_manager::add_multiplayer_event_helper(
    _In_ multiplayer_event_type eventType,
    _In_ multiplayer_session_type sessionType,
    _In_ std::error_code errorCode,
    _In_ std::string errorMessage
    )
{
    multiplayer_event multiplayerEvent(
        errorCode,
        errorMessage,
        eventType,
        std::make_shared<multiplayer_event_args>(),
        sessionType
        );

    add_to_multiplayer_event_queue(multiplayerEvent);
}

void
multiplayer_client_manager::add_multiplayer_event(
    _In_ multiplayer_event_type eventType,
    _In_ multiplayer_session_type sessionType,
    _In_ std::error_code errorCode,
    _In_ std::string errorMessage
    )
{
    std::lock_guard<std::mutex> guard(m_clientRequestLock);
    add_multiplayer_event_helper(eventType, sessionType, errorCode, errorMessage);
}

multiplayer_session_type
multiplayer_client_manager::get_session_type(
    _In_ std::shared_ptr<multiplayer_session> session
    )
{
    multiplayer_session_type sessionType = multiplayer_session_type::unknown;
    auto latestPendingRead = latest_pending_read();
    if (latestPendingRead != nullptr)
    {
        if (latestPendingRead->is_lobby(session->session_reference()))
        {
            sessionType = multiplayer_session_type::lobby_session;
        }
        else if (latestPendingRead->is_game(session->session_reference()))
        {
            sessionType = multiplayer_session_type::game_session;
        }
    }

    return sessionType;
}

void
multiplayer_client_manager::process_events(
    _In_ std::shared_ptr<multiplayer_session> currentSession,
    _In_ std::shared_ptr<multiplayer_session> oldSession,
    _In_ multiplayer_session_type sessionType
    )
{
    if (oldSession == nullptr || currentSession == nullptr || oldSession->change_number() == currentSession->change_number())
    {
        return;
    }

    xbox_live_result<multiplayer_session_change_types> diff = multiplayer_session::compare_multiplayer_sessions(currentSession, oldSession);
    if (!diff.err() && diff.payload() == multiplayer_session_change_types::none)
    {
        return;
    }

    multiplayer_session_change_types diffType = diff.payload();

    if (sessionType != multiplayer_session_type::match_session)
    {
        if (multiplayer_manager_utils::is_multiplayer_session_change_type(diffType, multiplayer_session_change_types::host_device_token_change))
        {
            handle_host_changed(currentSession, sessionType);
        }

        if (multiplayer_manager_utils::is_multiplayer_session_change_type(diffType, multiplayer_session_change_types::member_list_change))
        {
            handle_member_list_changed(currentSession, oldSession, sessionType);
        }

        if (multiplayer_manager_utils::is_multiplayer_session_change_type(diffType, multiplayer_session_change_types::custom_property_change))
        {
            handle_session_properties_changed(currentSession, oldSession, sessionType);
        }

        if (multiplayer_manager_utils::is_multiplayer_session_change_type(diffType, multiplayer_session_change_types::member_custom_property_change))
        {
            handle_member_properties_changed(currentSession, oldSession, sessionType);
        }
    }
    
    if (sessionType != multiplayer_session_type::game_session)
    {
        // Don't need to process these for game. The match will take care of handling these events.
        if (multiplayer_manager_utils::is_multiplayer_session_change_type(diffType, multiplayer_session_change_types::matchmaking_status_change))
        {
            m_latestPendingRead->match_client()->handle_match_status_changed(currentSession);
        }
    }

    if (multiplayer_manager_utils::is_multiplayer_session_change_type(diffType, multiplayer_session_change_types::tournament_property_change))
    {
        handle_tournament_properties_changed(currentSession, oldSession, sessionType);
    }

    if (multiplayer_manager_utils::is_multiplayer_session_change_type(diffType, multiplayer_session_change_types::arbitration_property_change))
    {
        if (currentSession->arbitration_server().result_state() != oldSession->arbitration_server().result_state() &&
            currentSession->arbitration_server().result_state() == tournaments::tournament_arbitration_state::completed)
        {
            add_multiplayer_event_helper(multiplayer_event_type::arbitration_complete, sessionType);
        }
    }
}

void
multiplayer_client_manager::handle_member_list_changed(
    _In_ std::shared_ptr<multiplayer_session> currentSession,
    _In_ std::shared_ptr<multiplayer_session> oldSession,
    _In_ multiplayer_session_type sessionType
    )
{
    std::map<string_t, std::shared_ptr<multiplayer_session_member>> currentSessionMembers;
    std::map<string_t, std::shared_ptr<multiplayer_session_member>> oldSessionMembers;

    for (const auto& currentSessionMember : currentSession->members())
    {
        currentSessionMembers[currentSessionMember->xbox_user_id()] = currentSessionMember;
    }

    for (const auto& oldSessionMember : oldSession->members())
    {
        oldSessionMembers[oldSessionMember->xbox_user_id()] = oldSessionMember;
    }

    bool haveMembersJoined = false;
    bool haveMembersLeft = false;

    // See if any new members joined
    std::vector<std::shared_ptr<multiplayer_session_member>> membersJoined;
    for (const auto& currentSessionMember : currentSession->members())
    {
        if (oldSessionMembers.find(currentSessionMember->xbox_user_id()) == oldSessionMembers.end())
        {
            haveMembersJoined = true;
            membersJoined.push_back(currentSessionMember);
        }
    }

    // See if any members left
    std::vector<std::shared_ptr<multiplayer_session_member>> membersLeft;
    for (const auto& oldSessionMember : oldSession->members())
    {
        if (currentSessionMembers.find(oldSessionMember->xbox_user_id()) == currentSessionMembers.end())
        {
            haveMembersLeft = true;
            membersLeft.push_back(oldSessionMember);
        }
    }

    if (haveMembersJoined || haveMembersLeft)
    {
        auto latestPendingRead = latest_pending_read();
        if (latestPendingRead == nullptr)
        {
            return;
        }

        if (haveMembersJoined)
        {
            std::vector<std::shared_ptr<multiplayer_member>> gameMembers;
            for (const auto& member : membersJoined)
            {
                gameMembers.push_back(latestPendingRead->convert_to_game_member(member));
            }

            std::shared_ptr<member_joined_event_args> memberJoinedEventArgs = std::make_shared<member_joined_event_args>(
                gameMembers
                );

            multiplayer_event multiplayerEvent(
                xbox_live_error_code::no_error,
                std::string(),
                multiplayer_event_type::member_joined,
                std::dynamic_pointer_cast<member_joined_event_args>(memberJoinedEventArgs),
                sessionType
                );

            add_to_multiplayer_event_queue(multiplayerEvent);
        }

        if (haveMembersLeft)
        {
            std::vector<std::shared_ptr<multiplayer_member>> gameMembers;
            for (const auto& member : membersLeft)
            {
                gameMembers.push_back(latestPendingRead->convert_to_game_member(member));
            }

            std::shared_ptr<member_left_event_args> memberLeftEventArgs = std::make_shared<member_left_event_args>(
                gameMembers
                );

            multiplayer_event multiplayerEvent(
                xbox_live_error_code::no_error,
                std::string(),
                multiplayer_event_type::member_left,
                std::dynamic_pointer_cast<member_left_event_args>(memberLeftEventArgs),
                sessionType
                );

            add_to_multiplayer_event_queue(multiplayerEvent);
        }
    }
}

void
multiplayer_client_manager::handle_member_properties_changed(
    _In_ std::shared_ptr<multiplayer_session> currentSession,
    _In_ std::shared_ptr<multiplayer_session> oldSession,
    _In_ multiplayer_session_type sessionType
    )
{
    std::map<string_t, std::shared_ptr<multiplayer_session_member>> oldSessionMembers;
    for (const auto& oldSessionMember : oldSession->members())
    {
        oldSessionMembers[oldSessionMember->xbox_user_id()] = oldSessionMember;
    }

    // See if properties changed and add them to the queue.
    std::vector<std::shared_ptr<multiplayer_session_member>> memberPropertiesChanged;
    for (const auto& currentSessionMember : currentSession->members())
    {
        if (oldSessionMembers.find(currentSessionMember->xbox_user_id()) != oldSessionMembers.end())
        {
            std::shared_ptr<multiplayer_session_member> oldSessionMember = oldSessionMembers[currentSessionMember->xbox_user_id()];
            if (utils::str_icmp(currentSessionMember->member_custom_properties_json().serialize(),
                oldSessionMember->member_custom_properties_json().serialize()) != 0)
            {
                memberPropertiesChanged.push_back(currentSessionMember);
            }
        }
    }

    if (memberPropertiesChanged.size() > 0)
    {
        std::vector<std::shared_ptr<multiplayer_member>> gameMembers;
        const auto& localUsersMap = m_multiplayerLocalUserManager->get_local_user_map();
        for (const auto& member : memberPropertiesChanged)
        {
            auto iter = localUsersMap.find(member->xbox_user_id());
            if (iter != localUsersMap.end())
            {
                // Don't trigger member property changed events for local users.
                continue;
            }

            auto latestPendingRead = latest_pending_read();
            if (latestPendingRead == nullptr)
            {
                continue;
            }
            std::shared_ptr<member_property_changed_event_args> memberPropertiesChangedArgs = std::make_shared<member_property_changed_event_args>(
                latestPendingRead->convert_to_game_member(member),
                member->member_custom_properties_json()
                );

            multiplayer_event multiplayerEvent(
                xbox_live_error_code::no_error,
                std::string(),
                multiplayer_event_type::member_property_changed,
                std::dynamic_pointer_cast<member_property_changed_event_args>(memberPropertiesChangedArgs),
                sessionType
                );

            add_to_multiplayer_event_queue(multiplayerEvent);
        }
    }
}

void
multiplayer_client_manager::handle_session_properties_changed(
    _In_ std::shared_ptr<multiplayer_session> currentSession,
    _In_ std::shared_ptr<multiplayer_session> oldSession,
    _In_ multiplayer_session_type sessionType
    )
{
    if (sessionType == multiplayer_session_type::lobby_session &&
        m_multiplayerLocalUserManager->is_local_user_game_state(multiplayer_local_user_game_state::pending_join))
    {
        // Don't join the game if matchmaking is in progress.
        auto latestPendingRead = latest_pending_read();
        if (latestPendingRead != nullptr && latestPendingRead->match_client()->match_status() == match_status::none)
        {
            // If state is completed, or transfer handle was removed.
            if (latestPendingRead->lobby_client()->is_transfer_handle_state(_T("completed")) ||
                (multiplayer_manager_utils::has_session_property_changed(currentSession, oldSession, multiplayer_lobby_client::c_transferHandlePropertyName) &&
                latestPendingRead->lobby_client()->get_transfer_handle().empty())
                )
            {
                m_multiplayerLocalUserManager->change_all_local_user_game_state(multiplayer_local_user_game_state::join);

                // Join the game session using the handleId.
                latestPendingRead->game_client()->join_game_from_lobby_helper();
            }
        }
    }

    // Don't trigger property changed event if the transfer handle property changes.
    if (multiplayer_manager_utils::has_session_property_changed(currentSession, oldSession, multiplayer_lobby_client::c_transferHandlePropertyName) ||
        multiplayer_manager_utils::has_session_property_changed(currentSession, oldSession, multiplayer_lobby_client::c_joinabilityPropertyName))
    {
        return;
    }

    auto gamePropertiesChangedArgs = std::make_shared<session_property_changed_event_args>(
        currentSession->session_properties()->session_custom_properties_json()
        );

    multiplayer_event multiplayerEvent(
        xbox_live_error_code::no_error,
        std::string(),
        multiplayer_event_type::session_property_changed,
        std::dynamic_pointer_cast<session_property_changed_event_args>(gamePropertiesChangedArgs),
        sessionType
        );

    add_to_multiplayer_event_queue(multiplayerEvent);
}

void
multiplayer_client_manager::handle_host_changed(
    _In_ std::shared_ptr<multiplayer_session> currentSession,
    _In_ multiplayer_session_type sessionType
    )
{
    /// A host may have left, and there may be no new host.
    std::shared_ptr<multiplayer_member> hostMember = nullptr;
    std::shared_ptr<multiplayer_session_member> host = multiplayer_manager_utils::host_member(currentSession);
    if (host != nullptr)
    {
        auto latestPendingRead = latest_pending_read();
        if (latestPendingRead != nullptr)
        {
            hostMember = latestPendingRead->convert_to_game_member(host);
        }
    }

    std::shared_ptr<host_changed_event_args> hostChangedEventArgs = std::make_shared<host_changed_event_args>(
        hostMember
        );

    multiplayer_event multiplayerEvent(
        xbox_live_error_code::no_error,
        std::string(),
        multiplayer_event_type::host_changed,
        std::dynamic_pointer_cast<host_changed_event_args>(hostChangedEventArgs),
        sessionType
        );

    add_to_multiplayer_event_queue(multiplayerEvent);
}

void
multiplayer_client_manager::handle_tournament_properties_changed(
    _In_ std::shared_ptr<multiplayer_session> currentSession,
    _In_ std::shared_ptr<multiplayer_session> oldSession,
    _In_ multiplayer_session_type sessionType
    )
{
    auto currTournamentsServer = currentSession->tournaments_server();
    auto oldTournamentsServer = oldSession->tournaments_server();
    if (currTournamentsServer.registration_state() != oldTournamentsServer.registration_state() ||
        currTournamentsServer.registration_reason() != oldTournamentsServer.registration_reason())
    {
        auto registrationStateChangedEventArgs = std::make_shared<tournament_registration_state_changed_event_args>(
            currTournamentsServer.registration_state(),
            currTournamentsServer.registration_reason()
            );

        multiplayer_event multiplayerEvent(
            xbox_live_error_code::no_error,
            std::string(),
            multiplayer_event_type::tournament_registration_state_changed,
            registrationStateChangedEventArgs,
            sessionType
        );

        add_to_multiplayer_event_queue(multiplayerEvent);
    }

    auto currentGameSessionRef = currTournamentsServer.next_game_session_reference();
    if ( sessionType == multiplayer_session_type::lobby_session &&
         currentSession->session_constants()->capabilities_team() &&
         !currentGameSessionRef.is_null() &&
         currentGameSessionRef._Serialize() != oldTournamentsServer.next_game_session_reference()._Serialize())
    {
        auto latestPendingRead = latest_pending_read();
        if (latestPendingRead != nullptr)
        {
            latestPendingRead->lobby_client()->handle_game_session_ready_event(currentSession);
        }
    }
}

std::shared_ptr<multiplayer_match_client>
multiplayer_client_manager::match_client()
{
    auto latestPendingRead = latest_pending_read();
    if (latestPendingRead == nullptr)
    {
        return nullptr;
    }

    return latestPendingRead->match_client();
}

xbox_live_result<void>
multiplayer_client_manager::find_match(
    _In_ const string_t& hopperName,
    _In_ const web::json::value& attributes,
    _In_ const std::chrono::seconds& timeout
)
{
    auto latestPendingRead = latest_pending_read();
    RETURN_CPP_IF(latestPendingRead == nullptr || latestPendingRead->lobby_client()->session() == nullptr, void, xbox_live_error_code::logic_error, "No local user added. Call add_local_user() first.");
    return latestPendingRead->find_match(hopperName, attributes, timeout);
}

void
multiplayer_client_manager::set_auto_fill_members_during_matchmaking(
    _In_ bool autoFillMembers
    )
{
    m_autoFillMembers = autoFillMembers;
    auto latestPendingRead = latest_pending_read();
    if (latestPendingRead != nullptr)
    {
        latestPendingRead->set_auto_fill_members_during_matchmaking(autoFillMembers);
    }
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
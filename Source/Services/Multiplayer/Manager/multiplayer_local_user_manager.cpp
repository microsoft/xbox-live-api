// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"
#include "xsapi/services.h"
#include "user_context.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::real_time_activity;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

multiplayer_local_user_manager::multiplayer_local_user_manager() :
    m_multiplayerSubscriptionLostEventHandlerCounter(0),
    m_sessionChangeEventHandlerCounter(0),
    m_rtaResyncEventHandlerCounter(0)
{
}

multiplayer_local_user_manager::~multiplayer_local_user_manager()
{
    change_all_local_user_lobby_state(multiplayer_local_user_lobby_state::remove);
    remove_stale_local_users_from_map();

    m_sessionChangeEventHandler.clear();
    m_multiplayerSubscriptionLostEventHandler.clear();
    m_rtaResyncEventHandler.clear();
}

std::map<string_t, std::shared_ptr<multiplayer_local_user>>
multiplayer_local_user_manager::get_local_user_map()
{
    std::lock_guard<std::mutex> lock(m_lock.get());
    return m_localUserRequestMap;
}

std::shared_ptr<xbox_live_context_impl>
multiplayer_local_user_manager::get_context(
    _In_ const string_t& xboxUserId
    )
{
    if (xboxUserId.empty())
    {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(m_lock.get());

    auto iter = m_localUserRequestMap.find(xboxUserId);
    if (iter != m_localUserRequestMap.end())
    {
        return iter->second->context();
    }
    return nullptr;
}

std::shared_ptr<multiplayer_local_user>
multiplayer_local_user_manager::get_local_user(
    _In_ xbox_live_user_t user
    )
{
    if (user == nullptr)
    {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(m_lock.get());
    return get_local_user_helper(user);
}

std::shared_ptr<multiplayer_local_user>
multiplayer_local_user_manager::get_local_user(
    _In_ const string_t& xboxUserId
    )
{
    std::lock_guard<std::mutex> lock(m_lock.get());
    return get_local_user_helper(xboxUserId);
}

std::shared_ptr<multiplayer_local_user>
multiplayer_local_user_manager::get_local_user_helper(
    _In_ xbox_live_user_t user
    )
{
    if (user == nullptr)
    {
        return nullptr;
    }

    string_t xboxUserId = multiplayer_manager_utils::get_local_user_xbox_user_id(user);
    return get_local_user_helper(xboxUserId);
}

std::shared_ptr<multiplayer_local_user>
multiplayer_local_user_manager::get_local_user_helper(
    _In_ const string_t& xboxUserId
    )
{
    if (xboxUserId.empty())
    {
        return nullptr;
    }

    auto iter = m_localUserRequestMap.find(xboxUserId);
    if (iter != m_localUserRequestMap.end())
    {
        return iter->second;
    }
    return nullptr;
}

std::shared_ptr<xbox_live_context_impl>
multiplayer_local_user_manager::get_primary_context()
{
    std::lock_guard<std::mutex> lock(m_lock.get());
    return m_primaryXboxLiveContext;
}

void
multiplayer_local_user_manager::change_all_local_user_lobby_state(
    _In_ multiplayer_local_user_lobby_state state
    )
{
    std::lock_guard<std::mutex> lock(m_lock.get());

    for(const auto& user : m_localUserRequestMap)
    {
        const auto& localUser =  user.second;
        if (localUser != nullptr)
        {
            localUser->set_lobby_state(state);
        }
    }
}

void
multiplayer_local_user_manager::change_all_local_user_game_state(
    _In_ multiplayer_local_user_game_state state
    )
{
    std::lock_guard<std::mutex> lock(m_lock.get());

    for(const auto& user : m_localUserRequestMap)
    {
        const auto& localUser =  user.second;
        if (localUser != nullptr)
        {
            localUser->set_game_state(state);
        }
    }
}

bool
multiplayer_local_user_manager::is_local_user_game_state(
    _In_ multiplayer_local_user_game_state state
    )
{
    std::lock_guard<std::mutex> lock(m_lock.get());

    for(const auto& user : m_localUserRequestMap)
    {
        const auto& localUser =  user.second;
        if (localUser != nullptr && localUser->game_state() != state)
        {
            return false;
        }
    }

    return true;
}

const std::shared_ptr<multiplayer_local_user>&
multiplayer_local_user_manager::add_user_to_xbox_live_context_to_map(
    _In_ xbox_live_user_t user
    )
{
    XSAPI_ASSERT(user != nullptr);

    string_t xboxUserId = utils::string_t_from_internal_string(user_context::get_user_id(user));

    bool isPrimary = m_localUserRequestMap.size() == 0 ? true : false;

    auto iter = m_localUserRequestMap.find(xboxUserId);
    if (iter == m_localUserRequestMap.end())
    {
        auto localUser = std::make_shared<multiplayer_local_user>(
            user,
            xboxUserId,
            isPrimary
            );

        auto ret = m_localUserRequestMap.insert(std::pair<string_t, std::shared_ptr<multiplayer_local_user>>(xboxUserId, localUser));
        iter = ret.first;

        if (isPrimary)
        {
            m_primaryXboxLiveContext = localUser->context();
        }

        // Activate events only for all users
        activate_multiplayer_events(localUser);
    }

    return iter->second;
}

void
multiplayer_local_user_manager::remove_stale_local_users_from_map()
{
    std::lock_guard<std::mutex> lock(m_lock.get());

    bool swtichPrimaryXboxLiveContext = false;
    for(auto iter = m_localUserRequestMap.begin(); iter != m_localUserRequestMap.end(); )
    {
        const auto& localUser =  iter->second;
        if (localUser != nullptr && localUser->lobby_state() == multiplayer_local_user_lobby_state::remove)
        {
            // De-activate rta events for the old user.
            deactivate_multiplayer_events(localUser);

            swtichPrimaryXboxLiveContext = localUser->is_primary_xbox_live_context();
            m_localUserRequestMap.erase(iter++);
        }
        else
        {
            ++iter;
        }
    }

    if (m_localUserRequestMap.size() == 0)
    {
        m_primaryXboxLiveContext = nullptr;
        return;
    }

    if (swtichPrimaryXboxLiveContext)
    {
        // Assign the first guy in the map to be the primary user.
        std::shared_ptr<multiplayer_local_user> user = m_localUserRequestMap.begin()->second;
        if (user != nullptr)
        {
            user->set_is_primary_xbox_live_context(true);
            m_primaryXboxLiveContext = user->context();
        }
    }
}

void
multiplayer_local_user_manager::activate_multiplayer_events(
    _In_ const std::shared_ptr<multiplayer_local_user>& localUser
    )
{
    XSAPI_ASSERT(localUser != nullptr);
    if (localUser == nullptr) return;

    std::weak_ptr<multiplayer_local_user_manager> thisWeakPtr = shared_from_this();

    auto xboxUserId = localUser->xbox_user_id();
    localUser->context()->real_time_activity_service()->activate();
    function_context rtaStateChanged = localUser->context()->real_time_activity_service()->add_connection_state_change_handler([thisWeakPtr, xboxUserId](_In_ real_time_activity_connection_state state)
    {
        std::shared_ptr<multiplayer_local_user_manager> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->on_connection_state_changed(xboxUserId, state);
        }
    });
    localUser->set_rta_state_changed_context(rtaStateChanged);

    function_context rtaResyncContext = localUser->context()->real_time_activity_service()->add_resync_handler([thisWeakPtr]()
    {
        std::shared_ptr<multiplayer_local_user_manager> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->on_resync_message_received();
        }
    });
    localUser->set_rta_resync_context(rtaResyncContext);

    if (!localUser->context()->multiplayer_service().subscriptions_enabled())
    {
        localUser->context()->multiplayer_service().enable_multiplayer_subscriptions();
        function_context sessionChangedContext = localUser->context()->multiplayer_service().add_multiplayer_session_changed_handler([thisWeakPtr](_In_ const multiplayer_session_change_event_args& args)
        {
            std::shared_ptr<multiplayer_local_user_manager> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->on_session_changed(args);
            }
        });
        localUser->set_session_changed_context(sessionChangedContext);

        function_context subscriptionLostContext = localUser->context()->multiplayer_service().add_multiplayer_subscription_lost_handler([thisWeakPtr, xboxUserId](void)
        {
            std::shared_ptr<multiplayer_local_user_manager> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->on_subscriptions_lost(xboxUserId);
            }
        });
        localUser->set_subscription_lost_context(subscriptionLostContext);
    }
}

void
multiplayer_local_user_manager::deactivate_multiplayer_events(
    _In_ const std::shared_ptr<multiplayer_local_user>& localUser
    )
{
    XSAPI_ASSERT(localUser != nullptr);
    if (localUser == nullptr) return;

    pplx::create_task([localUser]()
    {
        if (localUser->context()->multiplayer_service().subscriptions_enabled())
        {
            localUser->context()->multiplayer_service().remove_multiplayer_session_changed_handler(localUser->session_changed_context());
            localUser->context()->multiplayer_service().disable_multiplayer_subscriptions();
        }

        localUser->context()->real_time_activity_service()->remove_connection_state_change_handler(localUser->rta_state_changed_context());
        localUser->context()->real_time_activity_service()->remove_resync_handler(localUser->rta_resync_context());
        localUser->context()->real_time_activity_service()->deactivate();
    });
}

void
multiplayer_local_user_manager::on_connection_state_changed(
    _In_ const string_t& xboxUserId,
    _In_ real_time_activity_connection_state state
    )
{
    if (state == real_time_activity_connection_state::disconnected)
    {
        on_subscriptions_lost(xboxUserId);
    }
}

function_context
multiplayer_local_user_manager::add_multiplayer_session_changed_handler(
    _In_ std::function<void(const multiplayer_session_change_event_args&)> handler
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_sessionChangeEventHandlerCounter;
        m_sessionChangeEventHandler[m_sessionChangeEventHandlerCounter] = std::move(handler);
    }

    return context;
}

void
multiplayer_local_user_manager::remove_multiplayer_session_changed_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
    m_sessionChangeEventHandler.erase(context);
}

void
multiplayer_local_user_manager::on_session_changed(
    _In_ const multiplayer_session_change_event_args& args
    )
{
    std::unordered_map<uint32_t, std::function<void(const multiplayer_session_change_event_args&)>> sessionChangeEventHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
        sessionChangeEventHandlerCopy = m_sessionChangeEventHandler;
    }

    for(auto& handler : sessionChangeEventHandlerCopy)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(args);
            }
            catch (...)
            {
                LOG_ERROR("multiplayer_local_user_manager::on_session_changed call threw an exception");
            }
        }
    }
}

function_context
multiplayer_local_user_manager::add_multiplayer_subscription_lost_handler(
    _In_ std::function<void()> handler
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_multiplayerSubscriptionLostEventHandlerCounter;
        m_multiplayerSubscriptionLostEventHandler[m_multiplayerSubscriptionLostEventHandlerCounter] = std::move(handler);
    }

    return context;
}

void
multiplayer_local_user_manager::remove_multiplayer_subscription_lost_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
    m_multiplayerSubscriptionLostEventHandler.erase(context);
}

void
multiplayer_local_user_manager::on_subscriptions_lost(
    _In_ const string_t& xboxUserId
    )
{
    {
        // Only fire this for the last user.
        // Note: This will be fired from the previous user's deactivation.
        std::lock_guard<std::mutex> lock(m_lock.get());
        auto user = get_local_user_helper(xboxUserId);
        if (user == nullptr && m_localUserRequestMap.size() > 0)
        {
            return;
        }
    }

    std::unordered_map<uint32_t, std::function<void()>> multiplayerSubscriptionLostEventHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
        multiplayerSubscriptionLostEventHandlerCopy = m_multiplayerSubscriptionLostEventHandler;
    }

    for(auto& handler : multiplayerSubscriptionLostEventHandlerCopy)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second();
            }
            catch (...)
            {
                LOG_ERROR("multiplayer_local_user_manager::on_subscriptions_lost call threw an exception");
            }
        }
    }
}

function_context
multiplayer_local_user_manager::add_rta_resync_handler(
    _In_ std::function<void()> handler
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_rtaResyncEventHandlerCounter;
        m_rtaResyncEventHandler[m_rtaResyncEventHandlerCounter] = std::move(handler);
    }

    return context;
}

void
multiplayer_local_user_manager::remove_rta_resync_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
    m_rtaResyncEventHandler.erase(context);
}

void
multiplayer_local_user_manager::on_resync_message_received()
{
    std::unordered_map<uint32_t, std::function<void()>> rtaResyncEventHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
        rtaResyncEventHandlerCopy = m_rtaResyncEventHandler;
    }

    for(auto& handler : rtaResyncEventHandlerCopy)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second();
            }
            catch (...)
            {
                LOG_ERROR("multiplayer_local_user_manager::on_resync_message_received call threw an exception");
            }
        }
    }
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
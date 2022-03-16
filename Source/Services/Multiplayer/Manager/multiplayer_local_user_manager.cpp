// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "pch.h"
#include "multiplayer_manager_internal.h"
#include "real_time_activity_manager.h"

using namespace xbox::services;
using namespace xbox::services::multiplayer;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_BEGIN

MultiplayerLocalUserManager::~MultiplayerLocalUserManager()
{
    ChangeAllLocalUserLobbyState(MultiplayerLocalUserLobbyState::Remove);
    RemoveStaleLocalUsersFromMap();

    m_sessionChangeEventHandler.clear();
    m_multiplayerSubscriptionLostEventHandler.clear();
    m_rtaResyncEventHandler.clear();
}

const xsapi_internal_map<uint64_t, std::shared_ptr<MultiplayerLocalUser>>&
MultiplayerLocalUserManager::GetLocalUserMap()
{
    std::lock_guard<std::mutex> lock(m_lock);
    return m_localUserRequestMap;
}

std::shared_ptr<XblContext>
MultiplayerLocalUserManager::GetContext(
    _In_ uint64_t xboxUserId
)
{
    std::lock_guard<std::mutex> lock(m_lock);

    auto iter = m_localUserRequestMap.find(xboxUserId);
    if (iter != m_localUserRequestMap.end())
    {
        return iter->second->Context();
    }
    return nullptr;
}

std::shared_ptr<MultiplayerLocalUser>
MultiplayerLocalUserManager::GetLocalUser(
    _In_ xbox_live_user_t user
    )
{
    if (user == nullptr)
    {
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(m_lock);
    return GetLocalUserHelper(user);
}

std::shared_ptr<MultiplayerLocalUser>
MultiplayerLocalUserManager::GetLocalUser(
    _In_ uint64_t xuid
    )
{
    std::lock_guard<std::mutex> lock(m_lock);
    return GetLocalUserHelper(xuid);
}

std::shared_ptr<MultiplayerLocalUser>
MultiplayerLocalUserManager::GetLocalUserHelper(
    _In_ xbox_live_user_t user
    )
{
    auto wrapUserResult{ User::WrapHandle(user) };
    if (Failed(wrapUserResult))
    {
        return nullptr;
    }

    return GetLocalUserHelper(wrapUserResult.Payload().Xuid());
}

std::shared_ptr<MultiplayerLocalUser>
MultiplayerLocalUserManager::GetLocalUserHelper(
    _In_ uint64_t xuid
    )
{
    auto iter = m_localUserRequestMap.find(xuid);
    if (iter != m_localUserRequestMap.end())
    {
        return iter->second;
    }
    return nullptr;
}

std::shared_ptr<XblContext>
MultiplayerLocalUserManager::GetPrimaryContext()
{
    std::lock_guard<std::mutex> lock(m_lock);
    return m_primaryXboxLiveContext;
}

void
MultiplayerLocalUserManager::ChangeAllLocalUserLobbyState(
    _In_ MultiplayerLocalUserLobbyState state
    )
{
    std::lock_guard<std::mutex> lock(m_lock);

    for(const auto& user : m_localUserRequestMap)
    {
        const auto& localUser =  user.second;
        if (localUser != nullptr)
        {
            localUser->SetLobbyState(state);
        }
    }
}

void
MultiplayerLocalUserManager::ChangeAllLocalUserGameState(
    _In_ MultiplayerLocalUserGameState state
    )
{
    std::lock_guard<std::mutex> lock(m_lock);

    for(const auto& user : m_localUserRequestMap)
    {
        const auto& localUser =  user.second;
        if (localUser != nullptr)
        {
            localUser->SetGameState(state);
        }
    }
}

bool
MultiplayerLocalUserManager::IsLocalUserGameState(
    _In_ MultiplayerLocalUserGameState state
    )
{
    std::lock_guard<std::mutex> lock(m_lock);

    for(const auto& user : m_localUserRequestMap)
    {
        const auto& localUser =  user.second;
        if (localUser != nullptr && localUser->GameState() != state)
        {
            return false;
        }
    }

    return true;
}

Result<const std::shared_ptr<MultiplayerLocalUser>>
MultiplayerLocalUserManager::AddUserToXboxLiveContextToMap(
    _In_ xbox_live_user_t user
    )
{
    auto wrapUserResult{ User::WrapHandle(user) };
    if (Failed(wrapUserResult))
    {
        return wrapUserResult.Hresult();
    }

    uint64_t xboxUserId = wrapUserResult.Payload().Xuid();

    bool isPrimary = m_localUserRequestMap.size() == 0 ? true : false;

    auto iter = m_localUserRequestMap.find(xboxUserId);
    if (iter == m_localUserRequestMap.end())
    {
        auto innerWrapUserResult{ User::WrapHandle(user) };
        if (Failed(innerWrapUserResult))
        {
            return innerWrapUserResult.Hresult();
        }

        auto localUser = MakeShared<MultiplayerLocalUser>(
            innerWrapUserResult.ExtractPayload(),
            xboxUserId,
            isPrimary
            );

        auto ret = m_localUserRequestMap.insert(std::pair<uint64_t, std::shared_ptr<MultiplayerLocalUser>>(xboxUserId, localUser));
        iter = ret.first;

        if (isPrimary)
        {
            m_primaryXboxLiveContext = localUser->Context();
        }

        // Activate events only for all users
        ActivateMultiplayerEvents(localUser);
    }

    return iter->second;
}

void
MultiplayerLocalUserManager::RemoveStaleLocalUsersFromMap()
{
    std::lock_guard<std::mutex> lock(m_lock);

    bool swtichPrimaryXboxLiveContext = false;
    for(auto iter = m_localUserRequestMap.begin(); iter != m_localUserRequestMap.end(); )
    {
        const auto& localUser =  iter->second;
        if (localUser != nullptr && localUser->LobbyState() == MultiplayerLocalUserLobbyState::Remove)
        {
            // De-activate rta events for the old user.
            DeactivateMultiplayerEvents(localUser);

            swtichPrimaryXboxLiveContext = localUser->IsPrimaryXboxLiveContext();
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
        std::shared_ptr<MultiplayerLocalUser> user = m_localUserRequestMap.begin()->second;
        if (user != nullptr)
        {
            user->SetIsPrimaryXboxLiveContext(true);
            m_primaryXboxLiveContext = user->Context();
        }
    }
}

void
MultiplayerLocalUserManager::ActivateMultiplayerEvents(
    _In_ const std::shared_ptr<MultiplayerLocalUser>& localUser
    )
{
    XSAPI_ASSERT(localUser != nullptr);
    if (localUser == nullptr) return;

    std::weak_ptr<MultiplayerLocalUserManager> weakThis = shared_from_this();
    auto xboxUserId = localUser->Xuid();

    if (auto globalState{ GlobalState::Get() })
    {
        globalState->RTAManager()->Activate(localUser->Context()->User());

        XblFunctionContext rtaResyncContext = globalState->RTAManager()->AddResyncHandler(localUser->Context()->User(), 
            [weakThis]
        {
            std::shared_ptr<MultiplayerLocalUserManager> pThis(weakThis.lock());
            if (pThis != nullptr)
            {
                pThis->OnResyncMessageReceived();
            }
        });
        localUser->SetRtaResyncContext(rtaResyncContext);
    }

    if (!localUser->Context()->MultiplayerService()->SubscriptionsEnabled())
    {
        localUser->Context()->MultiplayerService()->EnableMultiplayerSubscriptions();
        XblFunctionContext sessionChangedContext = localUser->Context()->MultiplayerService()->AddMultiplayerSessionChangedHandler([weakThis](_In_ XblMultiplayerSessionChangeEventArgs args)
        {
            std::shared_ptr<MultiplayerLocalUserManager> pThis(weakThis.lock());
            if (pThis != nullptr)
            {
                pThis->OnSessionChanged(args);
            }
        });
        localUser->SetSessionChangedContext(sessionChangedContext);

        XblFunctionContext connectionIdChangedContext = localUser->Context()->MultiplayerService()->AddMultiplayerConnectionIdChangedHandler([weakThis](const String&)
        {
            std::shared_ptr<MultiplayerLocalUserManager> pThis(weakThis.lock());
            if (pThis != nullptr)
            {
                pThis->OnConnectionIdChanged();
            }
        });
        localUser->SetConnectionIdChangedContext(connectionIdChangedContext);

        XblFunctionContext subscriptionLostContext = localUser->Context()->MultiplayerService()->AddMultiplayerSubscriptionLostHandler([weakThis, xboxUserId](void)
        {
            std::shared_ptr<MultiplayerLocalUserManager> pThis(weakThis.lock());
            if (pThis != nullptr)
            {
                pThis->OnSubscriptionsLost(xboxUserId);
            }
        });
        localUser->SetSubscriptionLostContext(subscriptionLostContext);
    }
}

void MultiplayerLocalUserManager::DeactivateMultiplayerEvents(
    _In_ const std::shared_ptr<MultiplayerLocalUser>& localUser
)
{
    XSAPI_ASSERT(localUser != nullptr);
    if (localUser == nullptr)
    {
        return;
    }

    // TODO allow setting queue
    HRESULT hr = m_queue.RunWork([localUser = std::shared_ptr<MultiplayerLocalUser>{ localUser }]
    {
        if (localUser->Context()->MultiplayerService()->SubscriptionsEnabled())
        {
            localUser->Context()->MultiplayerService()->RemoveMultiplayerSessionChangedHandler(localUser->SessionChangedContext());
            localUser->Context()->MultiplayerService()->DisableMultiplayerSubscriptions();
            localUser->Context()->MultiplayerService()->RemoveMultiplayerConnectionIdChangedHandler(localUser->ConnectionIdChangedContext());
        }
        auto globalState{ GlobalState::Get() };
        if (globalState != nullptr)
        {
            globalState->RTAManager()->RemoveResyncHandler(localUser->Context()->User(), localUser->RtaResyncContext());
            globalState->RTAManager()->Deactivate(localUser->Context()->User());
        }
    });

    if (FAILED(hr))
    {
        LOGS_INFO << __FUNCTION__ << " RunAsync failed with hr=" << hr;
    }
}

XblFunctionContext
MultiplayerLocalUserManager::AddMultiplayerSessionChangedHandler(
    _In_ Callback<XblMultiplayerSessionChangeEventArgs> handler
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock);

    XblFunctionContext context = 0;
    if (handler != nullptr)
    {
        context = m_sessionChangeEventHandlerCounter;
        m_sessionChangeEventHandler[m_sessionChangeEventHandlerCounter++] = std::move(handler);
    }

    return context;
}

void
MultiplayerLocalUserManager::RemoveMultiplayerSessionChangedHandler(
    _In_ XblFunctionContext context
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock);
    m_sessionChangeEventHandler.erase(context);
}

void
MultiplayerLocalUserManager::OnSessionChanged(
    _In_ const XblMultiplayerSessionChangeEventArgs& args
    )
{
    xsapi_internal_unordered_map<uint32_t, Callback<XblMultiplayerSessionChangeEventArgs>> sessionChangeEventHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionLock);
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
                LOG_ERROR("MultiplayerLocalUserManager::on_session_changed call threw an exception");
            }
        }
    }
}

XblFunctionContext
MultiplayerLocalUserManager::AddMultiplayerConnectionIdChangedHandler(
    _In_ Function<void()> handler
)
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock);

    XblFunctionContext context = 0;
    if (handler != nullptr)
    {
        context = m_multiplayerConnectionIdChangedEventHandlerCounter;
        m_multiplayerConnectionIdChangedEventHandler[m_multiplayerConnectionIdChangedEventHandlerCounter++] = std::move(handler);
    }

    return context;
}

void
MultiplayerLocalUserManager::RemoveMultiplayerConnectionIdChangedHandler(
    _In_ XblFunctionContext context
)
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock);
    m_multiplayerConnectionIdChangedEventHandler.erase(context);
}

void
MultiplayerLocalUserManager::OnConnectionIdChanged(
)
{
    xsapi_internal_unordered_map<uint32_t, Function<void()>> multiplayerConnectionIdChangedEventHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionLock);
        multiplayerConnectionIdChangedEventHandlerCopy = m_multiplayerConnectionIdChangedEventHandler;
    }

    for (auto& handler : multiplayerConnectionIdChangedEventHandlerCopy)
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
                LOG_ERROR("MultiplayerLocalUserManager::on_connection_id_changed call threw an exception");
            }
        }
    }
}

XblFunctionContext
MultiplayerLocalUserManager::AddMultiplayerSubscriptionLostHandler(
    _In_ Function<void()> handler
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock);

    XblFunctionContext context = 0;
    if (handler != nullptr)
    {
        context = m_multiplayerSubscriptionLostEventHandlerCounter;
        m_multiplayerSubscriptionLostEventHandler[m_multiplayerSubscriptionLostEventHandlerCounter++] = std::move(handler);
    }

    return context;
}

void
MultiplayerLocalUserManager::RemoveMultiplayerSubscriptionLostHandler(
    _In_ XblFunctionContext context
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock);
    m_multiplayerSubscriptionLostEventHandler.erase(context);
}

void
MultiplayerLocalUserManager::OnSubscriptionsLost(
    _In_ uint64_t xuid
    )
{
    {
        // Only fire this for the last user.
        // Note: This will be fired from the previous user's deactivation.
        std::lock_guard<std::mutex> lock(m_lock);
        auto user = GetLocalUserHelper(xuid);
        if (user == nullptr && m_localUserRequestMap.size() > 0)
        {
            return;
        }
    }

    xsapi_internal_unordered_map<uint32_t, Function<void()>> multiplayerSubscriptionLostEventHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionLock);
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
                LOG_ERROR("MultiplayerLocalUserManager::on_subscriptions_lost call threw an exception");
            }
        }
    }
}

XblFunctionContext
MultiplayerLocalUserManager::AddRtaResyncHandler(
    _In_ Function<void()> handler
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock);

    XblFunctionContext context = 0;
    if (handler != nullptr)
    {
        context = m_rtaResyncEventHandlerCounter;
        m_rtaResyncEventHandler[m_rtaResyncEventHandlerCounter++] = std::move(handler);
    }

    return context;
}

void
MultiplayerLocalUserManager::RemoveRtaResyncHandler(
    _In_ XblFunctionContext context
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock);
    m_rtaResyncEventHandler.erase(context);
}

void
MultiplayerLocalUserManager::OnResyncMessageReceived()
{
    xsapi_internal_unordered_map<uint32_t, Function<void()>> rtaResyncEventHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionLock);
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
                LOG_ERROR("MultiplayerLocalUserManager::on_resync_message_received call threw an exception");
            }
        }
    }
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_MULTIPLAYER_MANAGER_CPP_END
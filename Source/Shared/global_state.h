// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "service_call_routed_handler.h"
#include "local_storage.h"

#if HC_PLATFORM == HC_PLATFORM_GDK
#include <appnotify.h>
#endif

// When TRACK_ASYNC is enabled, we will monitor the progress of each XAsync operation, asserting
// if tasks are started or outstanding when XblCleanup completes. By default only do async tracking
// for UnitTests, but it can be enabled for debugging in other situations as well.
#define TRACK_ASYNC XSAPI_UNIT_TESTS

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// Forward declarations
namespace achievements
{
    namespace manager
    {
        class AchievementsManager;
    }
}

namespace multiplayer 
{
    namespace manager
    {
        class MultiplayerManager;
    }
}

namespace social
{
    namespace manager
    {
        class SocialManager;
    }
}

namespace real_time_activity
{
    class RealTimeActivityManager;
}

#if HC_PLATFORM == HC_PLATFORM_GDK
typedef Function<void(bool isSuspended)> AppChangeNotificationHandler;
#endif

class GlobalState : public RefCounter, public std::enable_shared_from_this<GlobalState>
{
public:
    virtual ~GlobalState() noexcept = default;

    static HRESULT Create(_In_ const XblInitArgs* args);
    static HRESULT CleanupAsync(_In_ XAsyncBlock* async) noexcept;
    static std::shared_ptr<GlobalState> Get() noexcept;

    const TaskQueue& Queue() const noexcept;

    std::shared_ptr<achievements::manager::AchievementsManager> AchievementsManager() const noexcept;
    std::shared_ptr<multiplayer::manager::MultiplayerManager> MultiplayerManager() const noexcept;
    std::shared_ptr<social::manager::SocialManager> SocialManager() const noexcept;
    std::shared_ptr<real_time_activity::RealTimeActivityManager> RTAManager() const noexcept;

    void SetUserChangeHandler(uint64_t token, std::shared_ptr<UserChangeEventHandler> context) noexcept;
    size_t EraseUserChangeHandler(uint64_t token) noexcept;

    size_t EraseUserExpiredToken(uint64_t xuid) noexcept;
    void InsertUserExpiredToken(uint64_t xuid) noexcept;

    XblFunctionContext AddServiceCallRoutedHandler(
        _In_ XblCallRoutedHandler handler,
        _In_opt_ void* context
    ) noexcept;

    void RemoveServiceCallRoutedHandler(
        _In_ XblFunctionContext token
    ) noexcept;

    std::shared_ptr<system::LocalStorage> LocalStorage() const noexcept;

#if HC_PLATFORM == HC_PLATFORM_GDK
    XblFunctionContext AddAppChangeNotificationHandler(
        _In_ AppChangeNotificationHandler routine);

    static void AppStateChangeNotificationReceived(
        BOOLEAN quiesced,
        PVOID context
    );

    void RemoveAppChangeNotificationHandler(
        _In_ XblFunctionContext token
    ) noexcept;
#endif

    // TODO making this a wrapper of xsapi_singleton for now. It properly manages the lifetime of the global state
    // and enables asyncronous cleanup. Goal is to eventually move the necessary state from xsapi_singleton into this class
    // and get rid of xsapi_singleton altogether.
    std::shared_ptr<struct xsapi_singleton> singleton;

#if TRACK_ASYNC
    std::mutex asyncBlocksMutex{};
    UnorderedMap<XAsyncBlock*, const char*> asyncBlocks{};
#endif

private:
    GlobalState(_In_ const XblInitArgs* args);
    GlobalState(const GlobalState&) = delete;
    GlobalState& operator=(const GlobalState&) = delete;

    mutable std::mutex m_mutex;
    TaskQueue m_taskQueue{ nullptr };
    std::shared_ptr<achievements::manager::AchievementsManager> m_achievementsManager;
    std::shared_ptr<multiplayer::manager::MultiplayerManager> m_multiplayerManager;
    std::shared_ptr<social::manager::SocialManager> m_socialManager;
    std::shared_ptr<real_time_activity::RealTimeActivityManager> m_rtaManager;
    std::shared_ptr<system::LocalStorage> m_localStorage;
    Set<uint64_t> m_userExpiredTokens;
    UnorderedMap<uint64_t, std::shared_ptr<UserChangeEventHandler>> m_userChangeHandlers;
    XblFunctionContext m_nextHandlerToken{ 0 };
    xsapi_internal_unordered_map<XblFunctionContext, std::shared_ptr<ServiceCallRoutedHandler>> m_callRoutedHandlers;

#if HC_PLATFORM == HC_PLATFORM_GDK
    XblFunctionContext m_nextAppChangeHandlerToken{ 0 };
    xsapi_internal_unordered_map<XblFunctionContext, AppChangeNotificationHandler> m_appChangeNotificationHandlers;
#endif

    enum class AccessMode
    {
        GET,
        SET,
    };

    // In order to avoid uncertainty of global static initialization order, keep the static state pointer to a function scope
    static std::shared_ptr<GlobalState> AccessHelper(
        _In_ AccessMode mode,
        _In_opt_ std::shared_ptr<GlobalState> state = nullptr
    ) noexcept;

    // RefCounter
    std::shared_ptr<RefCounter> GetSharedThis() override;

#if HC_PLATFORM == HC_PLATFORM_GDK
    // Holds the registration ID for receiving App State Notifications (aka Quick Resume)
     PAPPSTATE_REGISTRATION m_registrationID;
#endif
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
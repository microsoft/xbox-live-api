// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "global_state.h"
#include "Achievements/Manager/achievements_manager_internal.h"
#include "multiplayer_manager_internal.h"
#include "social_manager_internal.h"
#include "real_time_activity_manager.h"
#include "Logger/log_hc_output.h"
#if HC_PLATFORM == HC_PLATFORM_ANDROID
#include "a/utils_a.h"
#include "a/java_interop.h"
#endif

HC_DEFINE_TRACE_AREA(XSAPI, HCTraceLevel::Verbose);

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

// Local Storage in Android is dependent on the JNI Initialization done in GlobalState::Create
// Only on Android the initialization of m_localStorage is done outside of the constructor.
GlobalState::GlobalState(
    _In_ const XblInitArgs* args
) :
    m_taskQueue{ args ? TaskQueue::DeriveWorkerQueue(args->queue) : nullptr },
    m_achievementsManager{ MakeShared<achievements::manager::AchievementsManager>() },
    m_multiplayerManager{ MakeShared<multiplayer::manager::MultiplayerManager>() },
    m_socialManager{ MakeShared<social::manager::SocialManager>() },
    m_rtaManager{ MakeShared<real_time_activity::RealTimeActivityManager>(m_taskQueue) },
#if HC_PLATFORM != HC_PLATFORM_ANDROID
    m_localStorage{ MakeShared<system::LocalStorage>(m_taskQueue) },
#endif
    m_appConfig{ MakeShared<xbox::services::AppConfig>() },
    m_logger{ MakeShared<logger>() }
{
    m_logger->add_log_output(MakeShared<log_hc_output>());

#if _DEBUG && XSAPI_UNIT_TESTS && XSAPI_PROFILE
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

HRESULT GlobalState::Create(
    _In_ const XblInitArgs* args
)
{
    RETURN_HR_INVALIDARGUMENT_IF_NULL(args);

#if !(HC_PLATFORM == HC_PLATFORM_XDK || HC_PLATFORM == HC_PLATFORM_UWP)
    RETURN_HR_INVALIDARGUMENT_IF_NULL(args->scid);
    RETURN_HR_INVALIDARGUMENT_IF_EMPTY_STRING(args->scid);
#endif

    XTaskQueueHandle processQueue{ nullptr };
    bool haveProcessQueue = XTaskQueueGetCurrentProcessTaskQueue(&processQueue);
    if (!haveProcessQueue && args->queue == nullptr)
    {
        return E_NO_TASK_QUEUE;
    }

    if (AccessHelper(AccessMode::GET))
    {
        return E_XBL_ALREADY_INITIALIZED;
    }

    // Global state depends on libHttpClient state so call HCInitialize before creating our state.
#if HC_PLATFORM == HC_PLATFORM_ANDROID
    HCInitArgs hcArgs{};
    hcArgs.javaVM = args->javaVM;
    hcArgs.applicationContext = args->applicationContext;
    RETURN_HR_IF_FAILED(HCInitialize(&hcArgs));
#else
    RETURN_HR_IF_FAILED(HCInitialize(nullptr));
#endif

    auto state = std::shared_ptr<GlobalState>(
        new (Alloc(sizeof(GlobalState))) GlobalState(args),
        Deleter<GlobalState>(),
        Allocator<GlobalState>()
    );

#if HC_PLATFORM == HC_PLATFORM_ANDROID
    // TODO Should make the java interop singleton a member of GlobalState rather than a separate static
    auto javaInteropInitResult = xbox::services::java_interop::get_java_interop_singleton()->initialize(
        args->javaVM,
        args->applicationContext
    );
    if (javaInteropInitResult.err())
    {
        return utils::convert_xbox_live_error_code_to_hresult(javaInteropInitResult.err());
    }
#endif

#if HC_PLATFORM == HC_PLATFORM_ANDROID
    // Local Storage in Android is dependent on the JNI Initialization so create LocalStorage now.
    state->m_localStorage = MakeShared<system::LocalStorage>(state->m_taskQueue);
#endif

#if HC_PLATFORM == HC_PLATFORM_WIN32
    // On Win32, set Local Storage path using init args
    RETURN_HR_IF_FAILED(state->m_localStorage->SetStoragePath(args->localStoragePath));
#endif

#if HC_PLATFORM != HC_PLATFORM_XDK && HC_PLATFORM != HC_PLATFORM_UWP
    RETURN_HR_INVALIDARGUMENT_IF_NULL(args->scid);
    RETURN_HR_IF_FAILED(state->m_appConfig->Initialize(args->scid));
#else
    RETURN_HR_IF_FAILED(state->m_appConfig->Initialize());
#endif

#if HC_PLATFORM_IS_EXTERNAL
    state->m_appConfig->SetAppId(args->appId);
    state->m_appConfig->SetAppVer(args->appVer);
    state->m_appConfig->SetOsName(args->osName);
    state->m_appConfig->SetOsVersion(args->osVersion);
    state->m_appConfig->SetOsLocale(args->osLocale);
    state->m_appConfig->SetDeviceClass(args->deviceClass);
    state->m_appConfig->SetDeviceId(args->deviceId);
#endif

#if HC_PLATFORM == HC_PLATFORM_IOS
    if (args->apnsEnvironment)
    {
        state->m_appConfig->SetAPNSEnvironment(args->apnsEnvironment);
    }
#endif

#if HC_PLATFORM == HC_PLATFORM_XDK
    // Initialize achievements provider properties
    RETURN_HR_IF_FAILED(CoCreateGuid(&state->m_achievementsSessionId));

    CHAR strTitleId[16] = "";
    sprintf_s(strTitleId, "%0.8X", state->m_appConfig->TitleId());

    Stringstream achievementsProviderName;
    achievementsProviderName << "XSAPI_" << strTitleId;
    state->m_achivementsEventProviderName = achievementsProviderName.str();
#endif

    state->m_locales = utils::generate_locales();

    // GlobalState object has been created and initialized successfully at this point so store it.
    (void)AccessHelper(AccessMode::SET, state);

#if HC_PLATFORM == HC_PLATFORM_GDK
    RegisterAppStateChangeNotification(AppStateChangeNotificationReceived, nullptr, &state->m_registrationID);
#endif

    // Leak a ref until Cleanup is called
    state->AddRef();

    return S_OK;
}

HRESULT GlobalState::CleanupAsync(
    _In_ XAsyncBlock* async
) noexcept
{
    auto state{ AccessHelper(AccessMode::SET, nullptr) };
    if (!state)
    {
        return E_XBL_NOT_INITIALIZED;
    }

#if HC_PLATFORM == HC_PLATFORM_GDK
    // In case of GDK, make sure to unregister from App Change Notifications before disconnecting
    UnregisterAppStateChangeNotification(state->m_registrationID);
#endif

    return XAsyncBegin(
        async,
        state.get(),
        reinterpret_cast<void*>(CleanupAsync),
        __FUNCTION__,
        [](XAsyncOp op, const XAsyncProviderData* data)
    {

        switch(op)
        {
        case XAsyncOp::Begin:
        {
            return XAsyncSchedule(data->async, 0);
        }
        case XAsyncOp::DoWork:
        {
            //Limit shared_ptr scope so that it never lives beyond the XTaskQueueTerminate callback,
            //even if XTaskQueueTerminate completes synchronously. This ensures the call to DecRef
            //in the callback releases the last remaining reference.
            XTaskQueueHandle stateTaskQueueHandle;
            {
                // Create a shared_ptr to the state so we can track the use_count here
                auto state{ static_cast<GlobalState*>(data->context)->shared_from_this() };

                stateTaskQueueHandle = state->Queue().GetHandle();

                // Wait for all other references to the GlobalState to go away.
                // Note that the use count check here is only valid because we never create
                // a weak_ptr to the singleton. If we did that could cause the use count
                // to increase even though we are the only strong reference
                if (state.use_count() > 2)
                {
                    // Wait for async tasks which require the GlobalState to finish.
                    RETURN_HR_IF_FAILED(XAsyncSchedule(data->async, 10));
                    return E_PENDING;
                }

#if XSAPI_UNIT_TESTS
                // As a sanity check, ensure there aren't other references to RTA manager.
                // Don't always block on this - unclosed XblContexts would then prevent XblCleanup from completing.
                auto rtaManagerUseCount{ state->m_rtaManager.use_count() };
                if (rtaManagerUseCount > 1)
                {
                    HC_TRACE_VERBOSE(XSAPI, "RTAManager still in use (use_count=%d)", rtaManagerUseCount);
                    RETURN_HR_IF_FAILED(XAsyncSchedule(data->async, 10));
                    return E_PENDING;
                }
#if TRACK_ASYNC
                // Wait for any outstanding XAsync operations to be cleaned up. XAsync makes no guarantee 
                // that XAsyncOp::Cleanup will be called & completed before prior to signaling to the client that the
                // operation has completed. This leads to a race condition where titles could be told XblCleanupAsync
                // has finished while we still have XAsync provider contexts that need to be cleaned up. We don't want
                // to always block on this since XAsync operations whose result is never consumed by the client would
                // stop XblCleanupAsync from ever returning.
                {
                    std::lock_guard<std::mutex> lock{ state->asyncBlocksMutex };
                    if (!state->asyncBlocks.empty())
                    {
                        Stringstream ss;
                        ss << "Awaiting outstanding XAsync operations:";
                        for (auto& pair : state->asyncBlocks)
                        {
                            ss << "\nXAsyncBlock[" << pair.first << "] Identity=" << pair.second;
                        }

                        HC_TRACE_VERBOSE(XSAPI, ss.str().data());
                        RETURN_HR_IF_FAILED(XAsyncSchedule(data->async, 10));
                        return E_PENDING;
                    }
                }
#endif // TRACK_ASYNC
#endif // XSAPI_UNIT_TESTS

                // Cleanup RTA state and open connections
                state->m_rtaManager->Cleanup();
            }

            // Terminate all pending/running async tasks on the global background queue.
            // We don't use TaskQueue::Terminate here because control will return to the client before
            // the Callback is cleaned up, giving them freedom to clean up custom memory hooks.
            auto hr = XTaskQueueTerminate(stateTaskQueueHandle, false, const_cast<XAsyncProviderData*>(data), [](void* context)
            {
                HC_TRACE_VERBOSE(XSAPI, "Cleaning up GlobalState");

                auto data{ static_cast<const XAsyncProviderData*>(context) };
                auto state{ static_cast<GlobalState*>(data->context) };

#if TRACK_ASYNC
                {
                    // Log warning for unfinished async operations
                    std::lock_guard<std::mutex> lock{ state->asyncBlocksMutex };
                    for (auto& pair : state->asyncBlocks)
                    {
                        HC_TRACE_WARNING(XSAPI, "Unfinished XAsyncBlock[%llu], Identity=%s", pair.first, pair.second);
                    }
                }
#endif

                // Release the leaked reference from Create
                state->DecRef();

                // Don't call HCCleanup until after GlobalState is destroyed since some of our state
                // depends on HC state.
                HCCleanup();

                XAsyncComplete(data->async, S_OK, 0);
            });

            RETURN_HR_IF_FAILED(hr);
            return E_PENDING;
        }
        default:
        {
            return S_OK;
        }
        }
    });
}

std::shared_ptr<GlobalState> GlobalState::Get() noexcept
{
    return AccessHelper(AccessMode::GET);
}

std::shared_ptr<GlobalState> GlobalState::AccessHelper(
    _In_ AccessMode mode,
    _In_opt_ std::shared_ptr<GlobalState> state
) noexcept
{
    static std::mutex s_mutex;
    static std::shared_ptr<GlobalState> s_state{ nullptr };

    std::lock_guard<std::mutex> lock{ s_mutex };

    switch (mode)
    {
    case AccessMode::GET:
    {
        assert(!state);
        return s_state;
    }

    case AccessMode::SET:
    {
        auto previousValue{ s_state };
        s_state = state;
        return previousValue;
    }
    }

    return nullptr;
}

const TaskQueue& GlobalState::Queue() const noexcept
{
    return m_taskQueue;
}

std::shared_ptr<multiplayer::manager::MultiplayerManager> GlobalState::MultiplayerManager() const noexcept
{
    return m_multiplayerManager;
}

std::shared_ptr<social::manager::SocialManager> GlobalState::SocialManager() const noexcept
{
    return m_socialManager;
}

std::shared_ptr<achievements::manager::AchievementsManager> GlobalState::AchievementsManager() const noexcept
{
    return m_achievementsManager;
}

std::shared_ptr<real_time_activity::RealTimeActivityManager> GlobalState::RTAManager() const noexcept
{
    return m_rtaManager;
}

void GlobalState::SetUserChangeHandler(uint64_t token, std::shared_ptr<UserChangeEventHandler> context) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_userChangeHandlers[token] = context;
}

size_t GlobalState::EraseUserChangeHandler(uint64_t token) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    return m_userChangeHandlers.erase(token);
}

size_t GlobalState::EraseUserExpiredToken(uint64_t xuid) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    return m_userExpiredTokens.erase(xuid);
}

void GlobalState::InsertUserExpiredToken(uint64_t xuid) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_userExpiredTokens.insert(xuid);
}

XblFunctionContext GlobalState::AddServiceCallRoutedHandler(
    _In_ XblCallRoutedHandler callback,
    _In_opt_ void* context
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
 
    m_callRoutedHandlers.emplace(m_nextHandlerToken, MakeShared<ServiceCallRoutedHandler>(callback, context));
    return m_nextHandlerToken++;
}

void GlobalState::RemoveServiceCallRoutedHandler(
    _In_ XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_callRoutedHandlers.erase(token);
}

std::shared_ptr<system::LocalStorage> GlobalState::LocalStorage() const noexcept
{
    return m_localStorage;
}

std::shared_ptr<xbox::services::AppConfig> GlobalState::AppConfig() const noexcept
{
    return m_appConfig;
}

std::shared_ptr<logger> GlobalState::Logger() const noexcept
{
    return m_logger;
}

std::shared_ptr<RefCounter> GlobalState::GetSharedThis()
{
    return shared_from_this();
}

#if HC_PLATFORM == HC_PLATFORM_GDK

XblFunctionContext GlobalState::AddAppChangeNotificationHandler(
    _In_ AppChangeNotificationHandler appChangeNotificationHandler
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_appChangeNotificationHandlers.emplace(m_nextAppChangeHandlerToken, appChangeNotificationHandler);
    return m_nextAppChangeHandlerToken++;
}

/* static */ void GlobalState::AppStateChangeNotificationReceived(
    BOOLEAN quiesced,
    PVOID context
)
{
    UNREFERENCED_PARAMETER(context);
    auto state{ GlobalState::Get() };

    if (state)
    {
        // Before moving to the handlers, we need to unlock the mutex or run into a deadlock inside
        // the erase_token_and_signature function. For now, copying the handlers locally and unlocking
        std::unique_lock<std::mutex> lock{ state->m_mutex };
        auto localAppChangeNotificationHandlers = state->m_appChangeNotificationHandlers;

        lock.unlock();
        for (const auto& pair : localAppChangeNotificationHandlers)
        {
            auto handler = pair.second;
            handler(quiesced);
        }
    }
}

void GlobalState::RemoveAppChangeNotificationHandler(
    _In_ XblFunctionContext token
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    m_appChangeNotificationHandlers.erase(token);
}

#endif

#if HC_PLATFORM == HC_PLATFORM_XDK
const String& GlobalState::AchievementsProviderName() const noexcept
{
    return m_achivementsEventProviderName;
}

const GUID& GlobalState::AchievementsSessionId() const noexcept
{
    return m_achievementsSessionId;
}
#endif

const String& GlobalState::Locales() const noexcept
{
    return m_locales;
}

void GlobalState::OverrideLocale(const xsapi_internal_string& locale) noexcept
{
    m_locales = utils::generate_locales(locale);
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
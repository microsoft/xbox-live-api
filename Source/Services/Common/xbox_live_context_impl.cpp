// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "shared_macros.h"
#include "xsapi/services.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "xsapi/system.h"
#include "xbox_live_context_impl.h"
#if !TV_API && !UNIT_TEST_SERVICES
#include "notification_service.h"
#endif
#include "presence_internal.h"
#include "real_time_activity_internal.h"

#include "Achievements\achievements_internal.h"
#include "profile_internal.h"
#include "social_internal.h"
#include "xbox_live_app_config_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

using namespace xbox::services::system;

#if XSAPI_XDK_AUTH
xbox_live_context_impl::xbox_live_context_impl(
    _In_ Windows::Xbox::System::User^ user
    ) :
    m_signInContext(0),
    m_signOutContext(0)
{
    m_userContext = std::make_shared<xbox::services::user_context>(user);
}

Windows::Xbox::System::User^
xbox_live_context_impl::user()
{
    return m_userContext->user();
}
#endif

#if XSAPI_NONXDK_CPP_AUTH && !UNIT_TEST_SERVICES
xbox_live_context_impl::xbox_live_context_impl(
    _In_ std::shared_ptr<system::xbox_live_user> user
    ) :
    m_signInContext(0),
    m_signOutContext(0)
{
    user->_User_impl()->set_user_pointer(user);
    m_userContext = std::make_shared<xbox::services::user_context>(user);
}

std::shared_ptr<system::xbox_live_user>
xbox_live_context_impl::user()
{
    return m_userContext->user();
}
#endif

#if XSAPI_NONXDK_WINRT_AUTH
xbox_live_context_impl::xbox_live_context_impl(
    _In_ std::shared_ptr<system::xbox_live_user> user
) :
    m_signInContext(0),
    m_signOutContext(0)
{
    user->_User_impl()->set_user_pointer(user);
    m_userContext = std::make_shared<xbox::services::user_context>(user);
}

xbox_live_context_impl::xbox_live_context_impl(
    _In_ Microsoft::Xbox::Services::System::XboxLiveUser^ user
    ) :
    m_signInContext(0),
    m_signOutContext(0)
{
    m_userContext = std::make_shared<xbox::services::user_context>(user);
}

Microsoft::Xbox::Services::System::XboxLiveUser^
xbox_live_context_impl::user()
{
    return m_userContext->user();
}
#endif

xbox_live_context_impl::~xbox_live_context_impl()
{
#if XSAPI_NONXDK_CPP_AUTH
    if (m_userContext->user() != nullptr)
    {
        auto userImpl = m_userContext->user()->_User_impl();

        userImpl->remove_sign_in_completed_handler(
            m_signInContext
            );

        userImpl->remove_sign_out_completed_handler(
            m_signOutContext
            );
    }
#endif

    real_time_activity::real_time_activity_service_factory::get_singleton_instance()->remove_user_from_rta_map(m_userContext);
}


void xbox_live_context_impl::init()
{
    xbox_live_result<void> servicesConfigFileReadResult;

    m_appConfig = xbox_live_app_config::get_app_config_singleton();
    m_appConfigInternal = xbox_live_app_config_internal::get_app_config_singleton();
    m_xboxLiveContextSettings = std::make_shared<xbox::services::xbox_live_context_settings>();
    init_real_time_activity_service_instance();

#if UWP_API || TV_API
    AsyncBlock* async = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    async->queue = get_xsapi_singleton()->m_asyncQueue;
    async->callback = [](AsyncBlock* async)
    {
        xsapi_memory::mem_free(async);
    };

    BeginAsync(async, nullptr, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        if (op == AsyncOp_DoWork)
        {
            auto dispatcher = xbox_live_context_settings::_s_dispatcher;
            if (dispatcher == nullptr)
            {
                try
                {
                    auto mainView = Windows::ApplicationModel::Core::CoreApplication::MainView;
                    if (mainView != nullptr)
                    {
                        auto coreWindow = mainView->CoreWindow;
                        if (coreWindow != nullptr)
                        {
                            dispatcher = coreWindow->Dispatcher;
                        }
                    }
                }
                catch (...)
                {
                    LOG_ERROR("Protocol activation failed due to inability to acquire a CoreWindow");
                }

                if (dispatcher != nullptr)
                {
                    dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                        ref new Windows::UI::Core::DispatchedHandler([]()
                    {
                        xbox::services::service_call_logging_config::get_singleton_instance()->_Register_for_protocol_activation();
                    }));
                }
            }
            xbox::services::service_call_logging_config::get_singleton_instance()->_ReadLocalConfig();
            CompleteAsync(data->async, S_OK, 0);
        }
        return S_OK;
    });
    ScheduleAsync(async, 0);
#endif

    std::weak_ptr<xbox_live_context_impl> thisWeakPtr = shared_from_this();

    m_achievementServiceInternal = xsapi_allocate_shared<xbox::services::achievements::achievement_service_internal>(m_userContext, m_xboxLiveContextSettings, m_appConfigInternal, thisWeakPtr);
    m_profileServiceImpl = xsapi_allocate_shared<xbox::services::social::profile_service_impl>(m_userContext, m_xboxLiveContextSettings, m_appConfigInternal);
    m_reputationServiceImpl = xsapi_allocate_shared<xbox::services::social::reputation_service_impl>(m_userContext, m_xboxLiveContextSettings, m_appConfigInternal);
    m_leaderboardService = xbox::services::leaderboard::leaderboard_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    m_matchmakingService = xbox::services::matchmaking::matchmaking_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    m_gameServerPlatformService = xbox::services::game_server_platform::game_server_platform_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    m_titleStorageService = xbox::services::title_storage::title_storage_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    m_privacyService = xbox::services::privacy::privacy_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    m_presenceService = xsapi_allocate_shared<xbox::services::presence::presence_service_internal>(m_realTimeActivityService, m_userContext, m_xboxLiveContextSettings, m_appConfigInternal);
    m_userStatisticsService = xbox::services::user_statistics::user_statistics_service(m_userContext, m_xboxLiveContextSettings, m_appConfig, m_realTimeActivityService);
    m_multiplayerService = xbox::services::multiplayer::multiplayer_service(m_userContext, m_xboxLiveContextSettings, m_appConfig, m_realTimeActivityService);
    m_tournamentService = xbox::services::tournaments::tournament_service(m_userContext, m_xboxLiveContextSettings, m_appConfig, m_realTimeActivityService);
    m_socialServiceImpl = std::make_shared<xbox::services::social::social_service_impl>(m_userContext, m_xboxLiveContextSettings, m_appConfigInternal, m_realTimeActivityService);
    m_contextualSearchService = xbox::services::contextual_search::contextual_search_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    m_stringService = xbox::services::system::string_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    m_clubsService = xbox::services::clubs::clubs_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);

#if (UWP_API || XSAPI_U)
    m_eventsService = events::events_service(m_userContext, m_appConfig);
#endif 

#if TV_API || UNIT_TEST_SERVICES
    // These services are only on XDK
    m_catalogService = marketplace::catalog_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    m_inventoryService = marketplace::inventory_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    m_entertainmentProfileService = entertainment_profile::entertainment_profile_list_service(m_userContext, m_xboxLiveContextSettings, m_appConfig);
#else
    // Only start the presence writer on UAP
    presence::presence_writer::get_presence_writer_singleton()->start_writer(m_presenceService);

    auto notificationService = notification::notification_service::get_notification_service_singleton();
    notificationService->subscribe_to_notifications(
        m_userContext,
        m_xboxLiveContextSettings,
        m_appConfig
        );


#if XSAPI_NONXDK_CPP_AUTH || XSAPI_NONXDK_WINRT_AUTH
    if (m_userContext->user() != nullptr)
    {
        m_signInContext = m_userContext->user()->_User_impl()->add_sign_in_completed_handler(
        [thisWeakPtr](const xsapi_internal_string& xboxUserId)
        {
            std::shared_ptr<xbox_live_context_impl> pThis(thisWeakPtr.lock());
            if (pThis != nullptr && utils::str_icmp(pThis->xbox_live_user_id(), xboxUserId) == 0)
            {
                presence::presence_writer::get_presence_writer_singleton()->start_writer(pThis->m_presenceService);
            }
        });

        m_signOutContext = m_userContext->user()->_User_impl()->add_sign_out_completed_handler(
            [thisWeakPtr](const system::sign_out_completed_event_args&)
        {
            std::shared_ptr<xbox_live_context_impl> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->real_time_activity_service()->_Close_websocket();
                presence::presence_writer::get_presence_writer_singleton()->stop_writer(pThis->xbox_live_user_id());
            }
        });
    }
#endif
#endif
}

std::shared_ptr<user_context> xbox_live_context_impl::user_context()
{
    return m_userContext;
}

xsapi_internal_string xbox_live_context_impl::xbox_live_user_id()
{
    return m_userContext->xbox_user_id();
}

std::shared_ptr<social::profile_service_impl>
xbox_live_context_impl::profile_service_impl()
{
    return m_profileServiceImpl;
}

std::shared_ptr<social::social_service_impl>
xbox_live_context_impl::social_service_impl()
{
    return m_socialServiceImpl;
}

std::shared_ptr<social::reputation_service_impl>
xbox_live_context_impl::reputation_service_impl()
{
    return m_reputationServiceImpl;
}

leaderboard::leaderboard_service&
xbox_live_context_impl::leaderboard_service()
{
    return m_leaderboardService;
}

std::shared_ptr<achievements::achievement_service_internal>&
xbox_live_context_impl::achievement_service_internal()
{
    return m_achievementServiceInternal;
}

multiplayer::multiplayer_service&
xbox_live_context_impl::multiplayer_service()
{
    return m_multiplayerService;
}

matchmaking::matchmaking_service&
xbox_live_context_impl::matchmaking_service()
{
    return m_matchmakingService;
}

tournaments::tournament_service&
xbox_live_context_impl::tournament_service()
{
    return m_tournamentService;
}

user_statistics::user_statistics_service&
xbox_live_context_impl::user_statistics_service()
{
    return m_userStatisticsService;
}

void
xbox_live_context_impl::init_real_time_activity_service_instance()
{
    if (m_userContext->caller_context_type() == caller_context_type::title)
    {
        m_realTimeActivityService = std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service>(new xbox::services::real_time_activity::real_time_activity_service(m_userContext, m_xboxLiveContextSettings, m_appConfig));
    }
    else
    {
        m_realTimeActivityService = real_time_activity::real_time_activity_service_factory::get_singleton_instance()->get_rta_instance(m_userContext, m_xboxLiveContextSettings, m_appConfig);
    }
}

const std::shared_ptr<real_time_activity::real_time_activity_service>&
xbox_live_context_impl::real_time_activity_service()
{
    return m_realTimeActivityService;
}

std::shared_ptr<presence::presence_service_internal>
xbox_live_context_impl::presence_service()
{
    return m_presenceService;
}

game_server_platform::game_server_platform_service&
xbox_live_context_impl::game_server_platform_service()
{
    return m_gameServerPlatformService;
}

title_storage::title_storage_service&
xbox_live_context_impl::title_storage_service()
{
    return m_titleStorageService;
}

privacy::privacy_service&
xbox_live_context_impl::privacy_service()
{
    return m_privacyService;
}

contextual_search::contextual_search_service&
xbox_live_context_impl::contextual_search_service()
{
    return m_contextualSearchService;
}

system::string_service&
xbox_live_context_impl::string_service()
{
    return m_stringService;
}

clubs::clubs_service&
xbox_live_context_impl::clubs_service()
{
    return m_clubsService;
}

#if (UWP_API || XSAPI_U)
events::events_service&
xbox_live_context_impl::events_service()
{
    return m_eventsService;
}
#endif

#if TV_API || UNIT_TEST_SERVICES
marketplace::catalog_service&
xbox_live_context_impl::catalog_service()
{
    return m_catalogService;
}

marketplace::inventory_service&
    xbox_live_context_impl::inventory_service()
{
    return m_inventoryService;
}
entertainment_profile::entertainment_profile_list_service&
xbox_live_context_impl::entertainment_profile_list_service()
{
    return m_entertainmentProfileService;
}
#endif

std::shared_ptr<xbox_live_context_settings> 
xbox_live_context_impl::settings()
{
    return m_xboxLiveContextSettings;
}

std::shared_ptr<xbox_live_app_config> 
xbox_live_context_impl::application_config()
{
    return m_appConfig;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END
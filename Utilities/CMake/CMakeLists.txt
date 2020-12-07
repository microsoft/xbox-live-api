cmake_minimum_required (VERSION 3.6)

if( SHIP )
    set (PROJECT_NAME Microsoft.Xbox.Services.Ship)
elseif( UNITTEST )
    if( TAEF )
        set (PROJECT_NAME Microsoft.Xbox.Services.UnitTest.141.TAEF)
    elseif( TE )
        set (PROJECT_NAME Microsoft.Xbox.Services.UnitTest.141.TE)
    endif()
elseif( GDK )
    set (PROJECT_NAME Microsoft.Xbox.Services.GDK.C)
elseif( PCWIN32 )
    set (PROJECT_NAME Microsoft.Xbox.Services.Win32.Cpp)
elseif( XDK )
    set (PROJECT_NAME Microsoft.Xbox.Services.XDK.Cpp)
elseif( UWP )
    set (PROJECT_NAME Microsoft.Xbox.Services.UWP.Cpp)
elseif (ANDROID)
    set(PROJECT_NAME Microsoft.Xbox.Services.Android)
endif()


project (${PROJECT_NAME})

if(MSVC_VERSION GREATER 1909)
  set(COMPILER_VERSION "15")
elseif(MSVC_VERSION GREATER 1899)
  set(COMPILER_VERSION "14")
elseif(MSVC_VERSION GREATER 1700)
  set(COMPILER_VERSION "12")
elseif(MSVC_VERSION GREATER 1600)
  set(COMPILER_VERSION "11")
endif()

if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES Debug)
  set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Reset the configurations to what we need" FORCE)
endif()

set(CMAKE_USE_RELATIVE_PATHS TRUE)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
  set(PLATFORM STORE)
endif()

set (XSAPI_VERSION 2017.04.20170508.01)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_compile_options(/Zm300 /bigobj)
if (WINDOWS_STORE OR WINDOWS_PHONE)
    add_compile_options(/ZW)
endif()

add_definitions(-D_NO_ASYNCRTIMP -D_NO_PPLXIMP -D_NO_XSAPIIMP -DXSAPI_BUILD)

if( ANDROID)
    set_source_files_properties(../../Source/Services/Common/Unix/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
else()
    set_source_files_properties(../../Source/Services/Common/cpp/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
endif()

if (NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio .*")
    set_property(SOURCE ../../Source/Services/Common/cpp/pch.cpp APPEND PROPERTY OBJECT_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/pch.pch")
    set_property(SOURCE ${SOURCES} APPEND PROPERTY OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pch.pch")
endif()

include_directories(
    $(ProjectDir)
    $(ProjectDir)../../Source/Services
    $(ProjectDir)../../Source/Services/Common
    $(ProjectDir)../../Source/Services/Social
    $(ProjectDir)../../Source/Services/Multiplayer
    $(ProjectDir)../../Source/Services/Presence
    $(ProjectDir)../../Source/Services/Notification
    $(ProjectDir)../../Source/Services/TCUI
    $(ProjectDir)../../Source/Services/Social/Manager
    $(ProjectDir)../../Source/Services/RealTimeActivity
    $(ProjectDir)../../Source/Shared
    $(ProjectDir)../../include
    $(ProjectDir)../../External/cpprestsdk/Release/include
    $(ProjectDir)../../Source/System
    )

set(CMAKE_SUPPRESS_REGENERATION true)

set(Public_Source_Files
    ../../include/xsapi-cpp/achievements.h
    ../../include/xsapi-cpp/leaderboard.h
    ../../include/xsapi-cpp/matchmaking.h
    ../../include/xsapi-cpp/multiplayer.h
    ../../include/xsapi-cpp/privacy.h
    ../../include/xsapi-cpp/profile.h
    ../../include/xsapi-cpp/services.h
    ../../include/xsapi-cpp/social.h
    ../../include/xsapi-cpp/system.h
    ../../include/xsapi-cpp/types.h
    ../../include/xsapi-cpp/user_statistics.h
    ../../include/xsapi-cpp/xbox_live_context.h
    ../../include/xsapi-cpp/xbox_service_call_routed_event_args.h
    ../../include/xsapi-cpp/errors.h
    ../../include/xsapi-cpp/http_call.h
    ../../include/xsapi-cpp/http_call_request_message.h
    ../../include/xsapi-cpp/mem.h
    ../../include/xsapi-cpp/social_manager.h
    ../../include/xsapi-cpp/service_call_logging_config.h
    ../../include/xsapi-cpp/multiplayer_manager.h
    ../../include/xsapi-cpp/presence.h
    ../../include/xsapi-cpp/real_time_activity.h
    ../../include/xsapi-cpp/title_storage.h
    ../../include/xsapi-cpp/xbox_live_app_config.h
    ../../include/xsapi-cpp/xbox_live_context_settings.h
    ../../include/xsapi-cpp/string_verify.h
    )

set(Public_Source_Files_C
    ../../include/xsapi-c/achievements_c.h
    ../../include/xsapi-c/achievements_manager_c.h
    ../../include/xsapi-c/errors_c.h
    ../../include/xsapi-c/presence_c.h
    ../../include/xsapi-c/profile_c.h
    ../../include/xsapi-c/services_c.h
    ../../include/xsapi-c/social_c.h
    ../../include/xsapi-c/social_manager_c.h
    ../../include/xsapi-c/string_verify_c.h
    ../../include/xsapi-c/pal.h
    ../../include/xsapi-c/xbox_live_context_c.h
    ../../include/xsapi-c/xbox_live_context_settings_c.h
    ../../include/xsapi-c/xbox_live_global_c.h
    ../../include/xsapi-c/matchmaking_c.h
    ../../include/xsapi-c/multiplayer_c.h
    ../../include/xsapi-c/multiplayer_activity_c.h
    ../../include/xsapi-c/multiplayer_manager_c.h
    ../../include/xsapi-c/notification_c.h
    ../../include/xsapi-c/privacy_c.h
    ../../include/xsapi-c/title_managed_statistics_c.h
    ../../include/xsapi-c/user_statistics_c.h
    ../../include/xsapi-c/leaderboard_c.h
    ../../include/xsapi-c/events_c.h
    ../../include/xsapi-c/real_time_activity_c.h
    ../../include/xsapi-c/types_c.h
    ../../include/xsapi-c/http_call_c.h
    ../../include/xsapi-c/title_storage_c.h
    ../../include/xsapi-c/game_invite_c.h
    ../../include/xsapi-c/platform_c.h
    )

message(STATUS "Test")

if( UNITTEST )
    message(STATUS "Test public")
    list(APPEND
        Public_Source_Files
        ../../include/xsapi-cpp/title_callable_ui.h
        ../../include/xsapi-cpp/events.h
        )
else()
    if( XDK )
        message(STATUS "XDK public")
    else()
        message(STATUS "Windows public")
        list(APPEND
            Public_Source_Files
            ../../include/xsapi-cpp/events.h
            )
        if( PCWIN32 OR ANDROID OR UWP )
            list(APPEND
                Public_Source_Files
                ../../include/xsapi-cpp/notification_service.h
                )
        elseif( UWP OR ANDROID )
            message(STATUS "UWP public")
            list(APPEND
                Public_Source_Files
                ../../include/xsapi-cpp/title_callable_ui.h
                )
        endif()
    endif()
endif()

set(Common_Source_Files
    ../../Source/Services/Common/pch_common.h
    ../../Source/Services/Common/xbox_live_context.cpp
    ../../Source/Services/Common/xbox_live_context_internal.h
    ../../Source/Services/Common/xbox_live_context_settings_internal.h
    ../../Source/Services/Common/xbox_live_context_settings.cpp
    ../../Source/Services/Common/xbox_live_context_api.cpp
    ../../Source/Services/Common/xbox_live_global_api.cpp
    )

if( ANDROID )
    list(APPEND
        Common_Source_Files
        ../../Source/Services/Common/Unix/pch.cpp
        ../../Source/Services/Common/Unix/pch.h
        )
else()
    list(APPEND
        Common_Source_Files
        ../../Source/Services/Common/cpp/pch.cpp
        ../../Source/Services/Common/cpp/pch.h
        )
endif()

set(System_Source_Files
    ../../Source/System/ppltasks_extra.h
    ../../Source/System/client_operation.h
    ../../Source/System/local_storage.h
    ../../Source/System/local_storage.cpp
    )

if ( UNITTEST OR UWP OR PCWIN32 OR GDK )
    list(APPEND
        System_Source_Files
        ../../Source/System/auth_config.cpp
        ../../Source/System/auth_config.h
        )
endif()

if( UNITTEST OR PCWIN32 OR ANDROID )
    list(APPEND
        System_Source_Files
        ../../Source/System/platform_api.cpp
        )
endif()

if( UNITTEST OR PCWIN32 )
    list(APPEND
        System_Source_Files
        ../../Source/System/Win32/local_storage_win32.cpp
        )
endif()

if ( ANDROID )
    list(APPEND
        System_Source_Files
        ../../Source/System/a/java_interop.cpp
        ../../Source/System/a/java_interop.h
        ../../Source/System/Android/local_storage_android.cpp
    )
endif()

set(Shared_Logger_Source_Files
    ../../Source/Shared/Logger/log.cpp
    ../../Source/Shared/Logger/log.h
    ../../Source/Shared/Logger/log_entry.cpp
    ../../Source/Shared/Logger/log_output.cpp
    ../../Source/Shared/Logger/log_hc_output.cpp
    ../../Source/Shared/Logger/log_hc_output.h
    )

set(Shared_Source_Files
    ../../Source/Shared/xsapi_utils.cpp
    ../../Source/Shared/xsapi_json_utils.cpp
    ../../Source/Shared/service_call_routed_handler.h
    ../../Source/Shared/service_call_routed_handler.cpp
    ../../Source/Shared/errors_legacy.h
    ../../Source/Shared/errors.cpp
    ../../Source/Shared/http_call_request_message.cpp
    ../../Source/Shared/build_version.h
    ../../Source/Shared/service_call_logger.cpp
    ../../Source/Shared/service_call_logger_data.cpp
    ../../Source/Shared/service_call_logging_config.h
    ../../Source/Shared/service_call_logging_config.cpp
    ../../Source/Shared/utils_locales.cpp
    ../../Source/Shared/web_socket.cpp
    ../../Source/Shared/xbox_live_app_config_internal.h
    ../../Source/Shared/xbox_live_app_config.cpp
    ../../Source/Shared/xbox_live_services_settings.h
    ../../Source/Shared/xbox_live_services_settings.cpp
    ../../Source/Shared/xbox_system_factory.cpp
    ../../Source/Shared/Shared_macros.h
    ../../Source/Shared/xsapi_utils.h
    ../../Source/Shared/xsapi_json_utils.h
    ../../Source/Shared/perf_tester.h
    ../../Source/Shared/service_call_logger.h
    ../../Source/Shared/service_call_logger_data.h
    ../../Source/Shared/service_call_logger_protocol.h
    ../../Source/Shared/web_socket.h  
    ../../Source/Shared/xbox_system_factory.h
    ../../Source/Shared/http_headers.h
    ../../Source/Shared/http_call_request_message_internal.h
    ../../Source/Shared/telemetry.h
    ../../Source/Shared/telemetry.cpp
    ../../Source/Shared/internal_mem.h
    ../../Source/Shared/internal_mem.cpp
    ../../Source/Shared/async_helpers.h
    ../../Source/Shared/async_helpers.cpp
    ../../Source/Shared/internal_errors.h
    ../../Source/Shared/ref_counter.h
    ../../Source/Shared/ref_counter.cpp
    ../../Source/Shared/http_call_api.cpp
    ../../Source/Shared/http_call_wrapper_internal.h
    ../../Source/Shared/http_call_wrapper_internal.cpp
    ../../Source/Shared/global_state.h
    ../../Source/Shared/global_state.cpp
    ../../Source/Shared/user.h
    ../../Source/Shared/string_array.h
    ../../Source/Shared/user.cpp
    ../../Source/Shared/enum_traits.h
    ../../Source/Shared/http_utils.h
    ../../Source/Shared/http_utils.cpp
    ../../Source/Shared/internal_types.h
    ../../Source/Shared/http_call_legacy.h
    ../../Source/Shared/http_call_legacy.cpp
    ../../Source/Shared/public_utils_legacy.h
    ../../Source/Shared/public_utils_legacy.cpp
    )

if( UWP OR PCWIN32 OR GDK OR UNITTEST )
    list(APPEND
        Shared_Source_Files
        ../../Source/Shared/WinRT/local_config_winrt.cpp
        )
endif()

if( UWP )
    list(APPEND
        Shared_Source_Files
        ../../Source/Shared/local_config.h
        ../../Source/Shared/local_config.cpp
        )
endif()

if ( UWP OR XDK OR UNITTEST )
    list(APPEND
        Shared_Source_Files
        ../../Source/Shared/service_call_logger_protocol.cpp
        )
endif()

if ( ANDROID )
    list(APPEND
        Shared_Source_Files
        ../../Source/Shared/a/android_utils.h
        ../../Source/Shared/a/android_utils.cpp
        ../../Source/Shared/u/xbl_guid.h
        ../../Source/Shared/a/guid.cpp
        ../../Source/Shared/a/http_call_jni.cpp
        ../../Source/Shared/a/http_call_jni.h
        ../../Source/Shared/a/http_call_static_glue.h
        ../../Source/Shared/a/http_call_static_glue.cpp
        ../../Source/Shared/a/interop_jni.h
        ../../Source/Shared/a/interop_jni.cpp
        ../../Source/Shared/a/jni_utils.h
        ../../Source/Shared/a/rwlock_guard.h
        ../../Source/Shared/a/rwlock_guard.cpp
        ../../Source/Shared/a/utils_a.h
        ../../Source/Shared/a/utils_a.cpp
        ../../Source/Shared/a/xbox_live_app_config_jni.cpp
        ../../Source/Shared/a/xbox_live_app_config_static_glue.h
        ../../Source/Shared/a/telemetry_a.cpp
        )
endif()

set(TCUI_Source_Files
    )

if ( UWP )
    list(APPEND
        TCUI_Source_Files
        ../../Source/Services/TCUI/UWP/title_callable_ui_uwp.cpp
        )
endif()

if ( ANDROID )
    list(APPEND
        TCUI_Source_Files
        ../../Source/Services/TCUI/Android/title_callable_static_glue.h
        ../../Source/Services/TCUI/Android/title_callable_ui_static_glue.cpp
        ../../Source/Services/TCUI/Android/title_callable_ui_android.cpp
        )
endif()

set(Notification_Source_Files
    )

if ( (NOT XDK) AND (NOT UNITTEST) )
    list(APPEND
        Notification_Source_Files
        ../../Source/Services/Notification/notification_internal.h
        ../../Source/Services/Notification/notification_service.cpp
        ../../Source/Services/Notification/notification_api.cpp
        )
endif()

if ( UWP )
    list(APPEND
        Notification_Source_Files
        ../../Source/Services/Notification/UWP/notification_service_uwp.cpp
        )
endif()

if ( PCWIN32 )
    list(APPEND
        Notification_Source_Files
        ../../Source/Services/Notification/RTA/notification_service_rta.cpp
        ../../Source/Services/Notification/RTA/game_invite_subscription.cpp
        ../../Source/Services/Notification/RTA/achievement_unlock_subscription.h
        ../../Source/Services/Notification/RTA/achievement_unlock_subscription.cpp
        ../../Source/Services/Notification/RTA/spop_kick_subscription.h
        ../../Source/Services/Notification/RTA/spop_kick_subscription.cpp
        )
endif()

if ( ANDROID )
    list(APPEND
        Notification_Source_Files
        ../../Source/Services/Notification/Mobile/notification_service_mobile.cpp
        )
endif()

set(Achievements_Source_Files
    ../../Source/Services/Achievements/achievement_service_internal.cpp
    ../../Source/Services/Achievements/achievements_internal.h
    ../../Source/Services/Achievements/achievements_result.cpp
    ../../Source/Services/Achievements/achievements_api.cpp
    ../../Source/Services/Achievements/achievements_subscription.cpp
    )
    
set(Achievements_Manager_Source_Files
    ../../Source/Services/Achievements/Manager/achievements_manager_internal.h
    ../../Source/Services/Achievements/Manager/achievements_manager_internal.cpp
    ../../Source/Services/Achievements/Manager/achievements_manager_api.cpp
    )

if( (NOT UNITTEST) AND (NOT XDK) )
    set(Events_Source_Files
        ../../Source/Services/Events/events_service.h
        ../../Source/Services/Events/events_service_api.cpp
        )
endif()

if ( GDK )
    list(APPEND
        Events_Source_Files
        ../../Source/Services/Events/events_service_gdk.h
        ../../Source/Services/Events/events_service_gdk.cpp
        ../../Source/Services/Events/events_service_etw.h
        ../../Source/Services/Events/events_service_etw.cpp
        )
elseif ( UWP )
# UWP and GDK still using ETW based events service
    list(APPEND
        Events_Source_Files
        ../../Source/Services/Events/events_service_etw.h
        ../../Source/Services/Events/events_service_etw.cpp
        )
elseif ( PCWIN32 OR ANDROID )
    list(APPEND
        Events_Source_Files
        ../../Source/Services/Events/event.cpp
        ../../Source/Services/Events/event_queue.cpp
        ../../Source/Services/Events/event_upload_payload.cpp
        ../../Source/Services/Events/events_service_xsapi.h
        ../../Source/Services/Events/events_service_xsapi.cpp
        )
endif()

if( PCWIN32 )
    list(APPEND
        Events_Source_Files
        ../../Source/Services/Events/Windows/events_service_windows.cpp
        )
endif()

if( ANDROID )
    list(APPEND
        Events_Source_Files
        ../../Source/Services/Events/Android/events_service_android.cpp
        )
endif()


set(Leaderboard_Source_Files
    ../../Source/Services/Leaderboard/Leaderboard_column.cpp
    ../../Source/Services/Leaderboard/Leaderboard_internal.h
    ../../Source/Services/Leaderboard/Leaderboard_result.cpp
    ../../Source/Services/Leaderboard/Leaderboard_row.cpp
    ../../Source/Services/Leaderboard/Leaderboard_service.cpp
    )

set(Privacy_Source_Files
    ../../Source/Services/Privacy/privacy_service_internal.h
    ../../Source/Services/Privacy/Privacy_service.cpp
    ../../Source/Services/Privacy/privacy_api.cpp
    ../../Source/Services/Privacy/permission_check_result.cpp
    )

set(Presence_Source_Files
    ../../Source/Services/Presence/device_presence_change_subscription.cpp
    ../../Source/Services/Presence/presence_internal.h
    ../../Source/Services/Presence/presence_device_record.cpp
    ../../Source/Services/Presence/presence_record.cpp
    ../../Source/Services/Presence/presence_service.cpp
    ../../Source/Services/Presence/presence_title_request.cpp
    ../../Source/Services/Presence/presence_user_batch_request.cpp
    ../../Source/Services/Presence/title_presence_change_subscription.cpp
    ../../Source/Services/Presence/presence_api.cpp
    )

set(Titlestorage_Source_Files
    ../../Source/Services/TitleStorage/title_storage_internal.h
    ../../Source/Services/TitleStorage/title_storage_api.cpp
    ../../Source/Services/TitleStorage/title_storage_blob_metadata_result.cpp
    ../../Source/Services/TitleStorage/title_storage_service.cpp
    )

set(Social_Source_Files
    ../../Source/Services/Social/reputation_feedback_request.cpp
    ../../Source/Services/Social/reputation_service.cpp
    ../../Source/Services/Social/social_relationship_result.cpp
    ../../Source/Services/Social/social_relationship_change_subscription.cpp
    ../../Source/Services/Social/social_service.cpp
    ../../Source/Services/Social/social_api.cpp
    ../../Source/Services/Social/social_internal.h
    ../../Source/Services/Social/profile_internal.h
    ../../Source/Services/Social/profile_service.cpp
    ../../Source/Services/Social/profile_api.cpp
    )

set(Social_Manager_Source_Files
    ../../Source/Services/Social/Manager/peoplehub_service.h
    ../../Source/Services/Social/Manager/peoplehub_service.cpp
    ../../Source/Services/Social/Manager/social_graph.h
    ../../Source/Services/Social/Manager/social_graph.cpp
    ../../Source/Services/Social/Manager/social_manager.cpp
    ../../Source/Services/Social/Manager/social_manager_api.cpp
    ../../Source/Services/Social/Manager/social_manager_internal.h
    ../../Source/Services/Social/Manager/social_manager_user_group.h
    ../../Source/Services/Social/Manager/social_manager_user_group.cpp
    )

set(Stats_Source_Files
    ../../Source/Services/Stats/requested_statistics.cpp
    ../../Source/Services/Stats/service_configuration_statistic.cpp
    ../../Source/Services/Stats/statistic.cpp
    ../../Source/Services/Stats/user_statistics_result.cpp
    ../../Source/Services/Stats/user_statistics_service.cpp
    ../../Source/Services/Stats/user_statistics_api.cpp
    ../../Source/Services/Stats/statistic_change_subscription.cpp
    ../../Source/Services/Stats/user_statistics_internal.h
    ../../Source/Services/Stats/title_managed_statistics_internal.h
    ../../Source/Services/Stats/title_managed_statistics_service.cpp
    ../../Source/Services/Stats/title_managed_statistics_api.cpp
    )

set(Matchmaking_Source_Files
    ../../Source/Services/Matchmaking/hopper_statistics_response.cpp
    ../../Source/Services/Matchmaking/matchmaking_service.cpp
    ../../Source/Services/Matchmaking/match_ticket_details_response.cpp
    ../../Source/Services/Matchmaking/matchmaking_internal.h
    )

set (Multiplayer_Source_Files
    ../../Source/Services/Multiplayer/multiplayer_activity_handle_post_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_activity_query_post_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_invite_handle_post_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_service.cpp
    ../../Source/Services/Multiplayer/multiplayer_search_handle_details.cpp
    ../../Source/Services/Multiplayer/multiplayer_search_handle_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_query_search_handle_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_session.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_member.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_reference.cpp
    ../../Source/Services/Multiplayer/multiplayer_service.cpp
    ../../Source/Services/Multiplayer/multiplayer_transfer_handle_post_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_subscription.cpp
    ../../Source/Services/Multiplayer/multiplayer_internal.h
    ../../Source/Services/Multiplayer/multiplayer_serializers.cpp
    ../../Source/Services/Multiplayer/multiplayer_api.cpp
    )

set (Multiplayer_Manager_Source_Files
    ../../Source/Services/Multiplayer/Manager/multiplayer_match_client.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_session_writer.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_client_manager.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_client_pending_reader.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_client_pending_request.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_game_client.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_member.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_game_session.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_lobby_client.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_local_user_manager.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_lobby_session.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_local_user.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_manager.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_manager_utils.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_manager_internal.h
    ../../Source/Services/Multiplayer/Manager/multiplayer_event_queue.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_manager_api.cpp
    )

set(StringVerify_Source_Files
    ../../Source/Services/StringVerify/string_service.cpp
    ../../Source/Services/StringVerify/string_service_internal.h
    ../../Source/Services/StringVerify/verify_string_result.cpp
    )

set(MultiplayerActivity_Source_Files
    ../../Source/Services/MultiplayerActivity/multiplayer_activity_api.cpp
    ../../Source/Services/MultiplayerActivity/multiplayer_activity_internal.h
    ../../Source/Services/MultiplayerActivity/multiplayer_activity_service.cpp
    ../../Source/Services/MultiplayerActivity/multiplayer_activity_info.cpp
    )
    
set(RealTimeActivityManager_Source_Files
    ../../Source/Services/RealTimeActivityManager/real_time_activity_manager.h
    ../../Source/Services/RealTimeActivityManager/real_time_activity_manager.cpp
    ../../Source/Services/RealTimeActivityManager/real_time_activity_connection.h
    ../../Source/Services/RealTimeActivityManager/real_time_activity_connection.cpp
    ../../Source/Services/RealTimeActivityManager/real_time_activity_subscription.h
	../../Source/Services/RealTimeActivityManager/real_time_activity_api.cpp
    )
    
set(Ship_Source_Files
    ../../Source/Services/Common/cpp/pch.cpp
    build.cpp
    )
    
if( SHIP )
    message(STATUS "SHIP add_library")
    source_group("Build" FILES ${Ship_Source_Files})
    set( SOURCE_FILES
        ${Ship_Source_Files}
        )
else()
    source_group("Header Files" FILES ${Public_Source_Files})
    source_group("C Public Includes" FILES ${Public_Source_Files_C})
    source_group("C++ Source\\System" FILES ${System_Source_Files})
    source_group("C++ Source\\Shared" FILES ${Shared_Source_Files})
    source_group("C++ Source\\Shared\\Logger" FILES ${Shared_Logger_Source_Files})
    source_group("C++ Source\\Services\\TCUI" FILES ${TCUI_Source_Files})
    source_group("C++ Source\\Services\\Notification" FILES ${Notification_Source_Files})
    source_group("C++ Source\\Services\\Common" FILES ${Common_Source_Files})
    source_group("C++ Source\\Services\\Achievements" FILES ${Achievements_Source_Files})
    source_group("C++ Source\\Services\\Achievements" FILES ${Achievements_Source_Files_Cpp})
    source_group("C++ Source\\Services\\Achievements\\Manager" FILES ${Achievements_Manager_Source_Files})
    source_group("C++ Source\\Services\\Privacy" FILES ${Privacy_Source_Files})
    source_group("C++ Source\\Services\\Presence" FILES ${Presence_Source_Files})
    source_group("C++ Source\\Services\\TitleStorage" FILES ${Titlestorage_Source_Files})
    source_group("C++ Source\\Services\\Social" FILES ${Social_Source_Files})
    source_group("C++ Source\\Services\\Social\\Manager" FILES ${Social_Manager_Source_Files})
    source_group("C++ Source\\Services\\Stats" FILES ${Stats_Source_Files})
    source_group("C++ Source\\Services\\Leaderboard" FILES ${Leaderboard_Source_Files})
    source_group("C++ Source\\Services\\Matchmaking" FILES ${Matchmaking_Source_Files})
    source_group("C++ Source\\Services\\Multiplayer" FILES ${Multiplayer_Source_Files})
    source_group("C++ Source\\Services\\Multiplayer\\Manager" FILES ${Multiplayer_Manager_Source_Files})
    source_group("C++ Source\\Services\\StringVerify" FILES ${StringVerify_Source_Files})
    source_group("C++ Source\\Services\\MultiplayerActivity" FILES ${MultiplayerActivity_Source_Files})
    source_group("C++ Source\\Services\\RealTimeActivityManager" FILES ${RealTimeActivityManager_Source_Files})

    if( XDK OR UNITTEST )
        message(STATUS "XDK source group")
    endif()

    if ( NOT XDK )
        message(STATUS "Non XDK source group")
        source_group("C++ Source\\Services\\Events" FILES ${Events_Source_Files})
    endif()

    if( NOT WINRT )
        message(STATUS "Flat C source group")
        source_group("C++ Source\\Services\\Multiplayer\\C" FILES ${Multiplayer_Source_Files_C})
        source_group("C++ Source\\Services\\Multiplayer\\Manager\\C" FILES ${Multiplayer_Manager_Source_Files_C})
    endif()

    set( SOURCE_FILES
        ${Common_Source_Files}
        ${Public_Source_Files}
        ${System_Source_Files}
        ${Shared_Source_Files}
        ${Shared_Logger_Source_Files}
        ${Achievements_Source_Files}
        ${Achievements_Manager_Source_Files}
        ${Presence_Source_Files}
        ${Social_Source_Files}
        ${Social_Manager_Source_Files}
        ${StringVerify_Source_Files}
        ${Matchmaking_Source_Files}
        ${Multiplayer_Source_Files}
        ${Multiplayer_Manager_Source_Files}
        ${Stats_Source_Files}
        ${Privacy_Source_Files}
        ${Stats_Source_Files}
        ${Leaderboard_Source_Files}
        ${Titlestorage_Source_Files}
        ${MultiplayerActivity_Source_Files}
        ${RealTimeActivityManager_Source_Files}
        )

    if( NOT GDK )
        list(APPEND
            SOURCE_FILES
            ${Notification_Source_Files}
            ${TCUI_Source_Files}
            ${Leaderboard_Source_Files}
            ${Matchmaking_Source_Files}
            )
    endif()

    if( NOT XDK )
        list(APPEND
            SOURCE_FILES
            ${Events_Source_Files}
            )
    endif()

    if( NOT WINRT )
        message(STATUS "Adding flat C source")
        list(APPEND
            SOURCE_FILES
            ${Public_Source_Files_C}
            ${Multiplayer_Source_Files_C}
            ${Multiplayer_Manager_Source_Files_C}
            )
    endif()
endif() # SHIP

set(UnitTests_Source_Files_Mocks
    ../../Tests/UnitTests/Mocks/mock_web_socket.h
    ../../Tests/UnitTests/Mocks/mock_web_socket.cpp
    ../../Tests/UnitTests/Mocks/mock_user.cpp
    ../../Tests/UnitTests/Mocks/xal_mocks.cpp
    ../../Tests/UnitTests/Mocks/http_mock.h
    ../../Tests/UnitTests/Mocks/http_mock.cpp
    ../../Tests/UnitTests/Mocks/mock_rta_service.h
    ../../Tests/UnitTests/Mocks/mock_rta_service.cpp
    )

set(UnitTests_Source_Files_Support
    ../../Tests/UnitTests/Support/iso8601.cpp
    ../../Tests/UnitTests/Support/DefineTestMacros.h
    ../../Tests/UnitTests/Support/iso8601.h
    ../../Tests/UnitTests/Support/UnitTestIncludes.h
    ../../Tests/UnitTests/Support/event.h
    ../../Tests/UnitTests/Support/event.cpp
    ../../Tests/UnitTests/Support/unit_test_helpers.h
    ../../Tests/UnitTests/Support/unit_test_helpers.cpp
    )

if ( TAEF )
    list(APPEND
        UnitTests_Source_Files_Support
        ../../Tests/UnitTests/Support/TAEF/UnitTestBase.h
        ../../Tests/UnitTests/Support/TAEF/UnitTestIncludes_TAEF.h
        ../../Tests/UnitTests/Support/TAEF/UnitTestBase.cpp
        )
endif()

if ( TE )
    list(APPEND
        UnitTests_Source_Files_Support
        ../../Tests/UnitTests/Support/TE/unittesthelpers_te.cpp
        ../../Tests/UnitTests/Support/TE/unittesthelpers_te.h
        ../../Tests/UnitTests/Support/TE/unittestincludes_te.h
        )
endif()
    
set(UnitTests_Source_Files_Tests
    ../../Tests/UnitTests/Tests/Services/AchievementsTests.cpp
    ../../Tests/UnitTests/Tests/Services/AchievementsManagerTests.cpp
    ../../Tests/UnitTests/Tests/Services/ErrorTests.cpp
    ../../Tests/UnitTests/Tests/Services/LeaderboardTests.cpp
    ../../Tests/UnitTests/Tests/Services/MatchmakingTests.cpp
    ../../Tests/UnitTests/Tests/Services/MultiplayerManagerTests.cpp
    ../../Tests/UnitTests/Tests/Services/MultiplayerTests.cpp
    ../../Tests/UnitTests/Tests/Services/PeoplehubTests.cpp
    ../../Tests/UnitTests/Tests/Services/PresenceTests.cpp
    ../../Tests/UnitTests/Tests/Services/PrivacyTests.cpp
    ../../Tests/UnitTests/Tests/Services/ProfileTests.cpp
    ../../Tests/UnitTests/Tests/Services/RealTimeActivityManagerTests.cpp
    ../../Tests/UnitTests/Tests/Services/ReputationTests.cpp
    ../../Tests/UnitTests/Tests/Services/SocialManagerTests.cpp
    ../../Tests/UnitTests/Tests/Services/SocialTests.cpp
    ../../Tests/UnitTests/Tests/Services/StatsTests.cpp
    ../../Tests/UnitTests/Tests/Services/TitleManagedStatsTests.cpp
    ../../Tests/UnitTests/Tests/Services/StringVerifyTests.cpp
    ../../Tests/UnitTests/Tests/Services/TitleStorageTests.cpp
    ../../Tests/UnitTests/Tests/Services/MultiplayerActivityTests.cpp
    ../../Tests/UnitTests/Tests/Shared/HttpCallTests.cpp
    ../../Tests/UnitTests/Tests/Shared/HttpCallSettingsTests.cpp
    ../../Tests/UnitTests/Tests/Shared/LogTests.cpp
    ../../Tests/UnitTests/Tests/Shared/ServiceCallLoggerTests.cpp
    ../../Tests/UnitTests/Tests/Shared/XboxLiveContextTests.cpp
    ../../Tests/UnitTests/Tests/Shared/XboxLiveCallbackTests.cpp
    ../../Tests/UnitTests/Tests/Shared/GlobalTests.cpp
    ../../Tests/UnitTests/Tests/Shared/PlatformTests.cpp
    )

set(TestJson_Files
    ../../Tests/UnitTests/Tests/Services/TestResponses/Multiplayer.json
    ../../Tests/UnitTests/Tests/Services/TestResponses/Matchmaking.json
    ../../Tests/UnitTests/Tests/Services/TestResponses/MultiplayerManager.json
    )

set_property(SOURCE ${TestJson_Files} PROPERTY VS_DEPLOYMENT_CONTENT 1)

if( TAEF OR TE )
    source_group("C++ Source\\UnitTests\\Mocks" FILES ${UnitTests_Source_Files_Mocks})
    source_group("C++ Source\\UnitTests\\Support" FILES ${UnitTests_Source_Files_Support})
    source_group("C++ Source\\UnitTests\\Tests" FILES ${UnitTests_Source_Files_Tests})
    source_group("C++ Source\\UnitTests\\Tests\\TestResponses" FILES ${TestJson_Files})
    list(APPEND
        SOURCE_FILES
        ${UnitTests_Source_Files_Mocks}
        ${UnitTests_Source_Files_Support}
        ${UnitTests_Source_Files_Tests}
        ${TestJson_Files}
        )
endif()

list( SORT SOURCE_FILES )
add_library(${PROJECT_NAME} ${SOURCE_FILES})

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES VS_WINRT_EXTENSIONS TRUE)
set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS "/INCREMENTAL:NO")
set(CMAKE_STATIC_LINKER_FLAGS "/INCREMENTAL:NO")

message(STATUS "CMAKE_SYSTEM_VERSION='${CMAKE_SYSTEM_VERSION}'")
message(STATUS "CMAKE_SYSTEM_NAME='${CMAKE_SYSTEM_NAME}'")
message(STATUS "SHORT_VERSION='${SHORT_VERSION}'")

cmake_minimum_required (VERSION 3.6)

if( SHIP )
    set (PROJECT_NAME Microsoft.Xbox.Services.Ship)
elseif( UNITTEST )
    if( TAEF )
        set (PROJECT_NAME Microsoft.Xbox.Services.UnitTest.140.TAEF)
    elseif( TE )
        set (PROJECT_NAME Microsoft.Xbox.Services.UnitTest.140.TE)
    endif()
elseif( XDK )
    if( WINRT )
        set (PROJECT_NAME Microsoft.Xbox.Services.XDK.WinRT)
    else()
        set (PROJECT_NAME Microsoft.Xbox.Services.XDK.Cpp)
    endif()
elseif( UWP )
    if( WINRT )
        set (PROJECT_NAME Microsoft.Xbox.Services.UWP.WinRT)
    else()
        set (PROJECT_NAME Microsoft.Xbox.Services.UWP.Cpp)
    endif()
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

if( WINRT OR UNITTEST )
    set_source_files_properties(../../Source/Services/Common/WinRT/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
else()
    set_source_files_properties(../../Source/Services/Common/Desktop/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
endif()

if (NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio .*")
    set_property(SOURCE ../../Source/Services/Common/Desktop/pch.cpp APPEND PROPERTY OBJECT_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/pch.pch")
    set_property(SOURCE ${SOURCES} APPEND PROPERTY OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pch.pch")
endif()

include_directories(
    $(ProjectDir)
    $(ProjectDir)../../Source/Services
    $(ProjectDir)../../Source/Services/Common
    $(ProjectDir)../../Source/Services/Common/Desktop
    $(ProjectDir)../../Source/Services/Social
    $(ProjectDir)../../Source/Services/Multiplayer
    $(ProjectDir)../../Source/Services/Presence
    $(ProjectDir)../../Source/Services/Misc
    $(ProjectDir)../../Source/Services/Social/Manager
    $(ProjectDir)../../Source/Services/RealTimeActivity
    $(ProjectDir)../../Source/Services/Clubs
    $(ProjectDir)../../Source/Shared
    $(ProjectDir)../../Source/Shared/WinRT
    $(ProjectDir)../../include
    $(ProjectDir)../../External/cpprestsdk/Release/include
    $(ProjectDir)../../Source/System/WinRT
    $(ProjectDir)../../Source/System
    )

set(CMAKE_SUPPRESS_REGENERATION true)

set(Public_Source_Files
    ../../include/xsapi/achievements.h
    ../../include/xsapi/leaderboard.h
    ../../include/xsapi/matchmaking.h
    ../../include/xsapi/multiplayer.h
    ../../include/xsapi/privacy.h
    ../../include/xsapi/profile.h
    ../../include/xsapi/services.h
    ../../include/xsapi/social.h
    ../../include/xsapi/system.h
    ../../include/xsapi/types.h
    ../../include/xsapi/user_statistics.h
    ../../include/xsapi/xbox_live_context.h
    ../../include/xsapi/xbox_service_call_routed_event_args.h
    ../../include/xsapi/contextual_search_service.h
    ../../include/xsapi/errors.h
    ../../include/xsapi/http_call.h
    ../../include/xsapi/http_call_request_message.h
    ../../include/xsapi/game_server_platform.h
    ../../include/xsapi/mem.h
    ../../include/xsapi/social_manager.h
    ../../include/xsapi/service_call_logging_config.h
    ../../include/xsapi/stats_manager.h
    ../../include/xsapi/multiplayer_manager.h
    ../../include/xsapi/presence.h
    ../../include/xsapi/real_time_activity.h
    ../../include/xsapi/title_storage.h
    ../../include/xsapi/tournaments.h
    ../../include/xsapi/xbox_live_app_config.h
    ../../include/xsapi/xbox_live_context_settings.h
    ../../include/xsapi/clubs.h
    )

set(Public_Source_Files_C
    ../../include/xsapi-c/achievements_c.h
    ../../include/xsapi-c/errors_c.h
    ../../include/xsapi-c/presence_c.h
    ../../include/xsapi-c/profile_c.h
    ../../include/xsapi-c/services_c.h
    ../../include/xsapi-c/social_c.h
    ../../include/xsapi-c/social_manager_c.h
    ../../include/xsapi-c/pal.h
    ../../include/xsapi-c/xbox_live_app_config_c.h
    ../../include/xsapi-c/xbox_live_context_c.h
    ../../include/xsapi-c/xbox_live_context_settings_c.h
    ../../include/xsapi-c/xbox_live_global_c.h
    )

message(STATUS "Test")

if( UNITTEST )
    message(STATUS "Test public")
    list(APPEND
        Public_Source_Files
        ../../include/xsapi/entertainment_profile.h
        ../../include/xsapi/xbox_cll.h
        ../../include/xsapi/marketplace.h
        ../../include/xsapi/title_callable_ui.h
        ../../include/xsapi/events.h
        )
else()
    if( XDK )
        message(STATUS "XDK public")
        list(APPEND
            Public_Source_Files
            ../../include/xsapi/entertainment_profile.h
            ../../include/xsapi/xbox_cll.h
            ../../include/xsapi/marketplace.h
            )
    else()
        message(STATUS "Windows public")
        list(APPEND
            Public_Source_Files
            ../../include/xsapi/events.h
            )

        if( UWP )
            message(STATUS "UWP public")
            list(APPEND
                Public_Source_Files
                ../../include/xsapi/title_callable_ui.h
                )
        endif()
    endif()
endif()

set(Common_Source_Files
    ../../Source/Services/Common/xbox_live_context.cpp
    ../../Source/Services/Common/xbox_live_context_impl.cpp
    ../../Source/Services/Common/pch_common.h
    ../../Source/Services/Common/xbox_live_context_impl.h
    )

if( WINRT OR UNITTEST )
    list(APPEND
        Common_Source_Files
        ../../Source/Services/Common/WinRT/pch.cpp
        ../../Source/Services/Common/WinRT/pch.h
        )
else()
    list(APPEND
        Common_Source_Files
        ../../Source/Services/Common/Desktop/pch.cpp
        ../../Source/Services/Common/Desktop/pch.h
        )
endif()

set(System_Source_Files
    ../../Source/System/xbox_live_mutex.cpp
    ../../Source/System/ppltasks_extra.h
    ../../Source/System/system_internal.h
    )

if ( UWP OR UNITTEST )
    list(APPEND
        System_Source_Files
        ../../Source/System/token_and_signature_result.cpp
        ../../Source/System/user.cpp
        ../../Source/System/user_impl.cpp
        ../../Source/System/user_impl.h
        ../../Source/System/auth_config.cpp
        ../../Source/System/auth_config.h
        )
endif()

if ( UWP )
    list(APPEND
        System_Source_Files
        ../../Source/System/user_impl_idp.cpp
        )
endif()

set(Shared_Logger_Source_Files
    ../../Source/Shared/Logger/debug_output.cpp
    ../../Source/Shared/Logger/etw_output.cpp
    ../../Source/Shared/Logger/etw_output.h
    ../../Source/Shared/Logger/log.cpp
    ../../Source/Shared/Logger/log_entry.cpp
    ../../Source/Shared/Logger/custom_output.cpp
    ../../Source/Shared/Logger/log_output.cpp
    ../../Source/Shared/Logger/custom_output.h
    ../../Source/Shared/Logger/debug_output.h
    ../../Source/Shared/Logger/etw_output.h
    ../../Source/Shared/Logger/log.h
    )

if( UNITTEST )
    list(APPEND
        Shared_Logger_Source_Files
        ../../Source/Shared/Logger/console_output.cpp
        ../../Source/Shared/Logger/console_output.h
        )
endif()

set(Shared_Source_Files
    ../../Source/Shared/http_call_impl.cpp
    ../../Source/Shared/http_call_response.cpp
    ../../Source/Shared/user_context.cpp
    ../../Source/Shared/utils.cpp
    ../../Source/Shared/xbox_service_call_routed_event_args_internal.h
    ../../Source/Shared/xbox_service_call_routed_event_args.cpp
    ../../Source/Shared/call_buffer_timer.cpp
    ../../Source/Shared/errors.cpp
    ../../Source/Shared/http_call_request_message.cpp
    ../../Source/Shared/initiator.cpp
    ../../Source/Shared/local_config.cpp
    ../../Source/Shared/mem.cpp
    ../../Source/Shared/build_version.h
    ../../Source/Shared/service_call_logger.cpp
    ../../Source/Shared/service_call_logger_data.cpp
    ../../Source/Shared/service_call_logging_config.cpp
    ../../Source/Shared/utils_locales.cpp
    ../../Source/Shared/web_socket_client.cpp
    ../../Source/Shared/web_socket_connection.cpp
    ../../Source/Shared/xbox_live_app_config_internal.h
    ../../Source/Shared/xbox_live_app_config.cpp
    ../../Source/Shared/xbox_live_context_settings.cpp
    ../../Source/Shared/xbox_live_services_settings.cpp
    ../../Source/Shared/xbox_system_factory.cpp
    ../../Source/Shared/http_call_impl.h
    ../../Source/Shared/http_call_response_internal.h
    ../../Source/Shared/local_config.h
    ../../Source/Shared/Shared_macros.h
    ../../Source/Shared/user_context.h
    ../../Source/Shared/utils.h
    ../../Source/Shared/call_buffer_timer.h
    ../../Source/Shared/initiator.h
    ../../Source/Shared/perf_tester.h
    ../../Source/Shared/service_call_logger.h
    ../../Source/Shared/service_call_logger_data.h
    ../../Source/Shared/service_call_logger_protocol.h
    ../../Source/Shared/web_socket_client.h
    ../../Source/Shared/web_socket_connection.h
    ../../Source/Shared/web_socket_connection_state.h
    ../../Source/Shared/xbox_system_factory.h
    ../../Source/Shared/http_headers.h
    ../../Source/Shared/http_call_request_message_internal.h
    ../../Source/Shared/service_call_logger_protocol.cpp
    ../../Source/Shared/WinRT/Event_WinRT.cpp
    ../../Source/Shared/WinRT/Event_WinRT.h
    )
    
if( NOT XDK )
    list(APPEND
        Shared_Source_Files
        ../../Source/Shared/telemetry.h
        ../../Source/Shared/telemetry.cpp
        ../../Source/Shared/WinRT/local_config_winrt.cpp
        )
endif()

if( UWP AND (NOT WINRT) )
    list(APPEND
        Shared_Source_Files
        ../../Source/Shared/user_context_cpp.cpp
        )
endif()

if( ( UWP AND WINRT ) OR UNITTEST )
    list(APPEND
        Shared_Source_Files
        ../../Source/Shared/user_context_winrt.cpp
        )
endif()

if ( XDK )
    list(APPEND
        Shared_Source_Files
        ../../Source/Shared/user_context_xdk.cpp
        )
endif()

set(Misc_Source_Files
    ../../Source/Services/Misc/contextual_config_result.cpp
    ../../Source/Services/Misc/contextual_search_broadcast.cpp
    ../../Source/Services/Misc/contextual_search_configured_stat.cpp
    ../../Source/Services/Misc/contextual_search_game_clip.cpp
    ../../Source/Services/Misc/contextual_search_game_clips_result.cpp
    ../../Source/Services/Misc/contextual_search_game_clip_stat.cpp
    ../../Source/Services/Misc/contextual_search_game_clip_thumbnail.cpp
    ../../Source/Services/Misc/contextual_search_game_clip_uri_info.cpp
    ../../Source/Services/Misc/contextual_search_service.cpp
    ../../Source/Services/Misc/string_service.cpp
    ../../Source/Services/Misc/verify_string_result.cpp
    ../../Source/Services/Misc/contextual_config_result.h
    )

if ( NOT XDK )
    list(APPEND
        Misc_Source_Files
        ../../Source/Services/Misc/notification_service.cpp
        ../../Source/Services/Misc/notification_service.h
        )
endif()

if ( UWP OR UNITTEST )
    list(APPEND
        Misc_Source_Files
        ../../Source/Services/Misc/UWP/title_callable_ui.cpp
        )
endif()

if ( UWP )
    list(APPEND
        Misc_Source_Files
        ../../Source/Services/Misc/Windows/notification_service_windows.cpp
        )
endif()

set(Achievements_Source_Files
    ../../Source/Services/Achievements/achievement.cpp
    ../../Source/Services/Achievements/achievement_media_asset.cpp
    ../../Source/Services/Achievements/achievement_progression.cpp
    ../../Source/Services/Achievements/achievement_requirement.cpp
    ../../Source/Services/Achievements/achievement_reward.cpp
    ../../Source/Services/Achievements/achievement_service.cpp
    ../../Source/Services/Achievements/achievement_service_internal.cpp
    ../../Source/Services/Achievements/achievement_time_window.cpp
    ../../Source/Services/Achievements/achievement_title_association.cpp
    ../../Source/Services/Achievements/achievements_internal.h
    ../../Source/Services/Achievements/achievements_result.cpp
    )

if( UWP OR UNITTEST )
    set(Events_Source_Files
        ../../Source/Services/Events/events_service_uwp.cpp
        )
endif()

set(Leaderboard_Source_Files
    ../../Source/Services/Leaderboard/Leaderboard_column.cpp
    ../../Source/Services/Leaderboard/Leaderboard_result.cpp
    ../../Source/Services/Leaderboard/Leaderboard_row.cpp
    ../../Source/Services/Leaderboard/Leaderboard_serializers.cpp
    ../../Source/Services/Leaderboard/Leaderboard_service.cpp
    ../../Source/Services/Leaderboard/Leaderboard_query.h
    ../../Source/Services/Leaderboard/Leaderboard_serializers.h
    )

set(Privacy_Source_Files
    ../../Source/Services/Privacy/multiple_permissions_check_result.cpp
    ../../Source/Services/Privacy/permission_check_result.cpp
    ../../Source/Services/Privacy/permission_deny_reason.cpp
    ../../Source/Services/Privacy/Privacy_service.cpp
    )

set(Presence_Source_Files
    ../../Source/Services/Presence/device_presence_change_event_args.cpp
    ../../Source/Services/Presence/device_presence_change_subscription.cpp
    ../../Source/Services/Presence/presence_internal.h
    ../../Source/Services/Presence/device_presence_change_event_args.cpp
    ../../Source/Services/Presence/device_presence_change_subscription.cpp
    ../../Source/Services/Presence/presence_internal.h
    ../../Source/Services/Presence/media_presence_data.cpp
    ../../Source/Services/Presence/presence_activity_data.cpp
    ../../Source/Services/Presence/presence_broadcast_record.cpp
    ../../Source/Services/Presence/presence_data.cpp
    ../../Source/Services/Presence/presence_device_record.cpp
    ../../Source/Services/Presence/presence_media_record.cpp
    ../../Source/Services/Presence/presence_record.cpp
    ../../Source/Services/Presence/presence_service.cpp
    ../../Source/Services/Presence/presence_service_internal.cpp
    ../../Source/Services/Presence/presence_title_record.cpp
    ../../Source/Services/Presence/presence_title_request.cpp
    ../../Source/Services/Presence/presence_user_batch_request.cpp
    ../../Source/Services/Presence/title_presence_change_event_args.cpp
    ../../Source/Services/Presence/title_presence_change_subscription.cpp
    )

if ( NOT XDK ) 
    list(APPEND
        Presence_Source_Files
        ../../Source/Services/Presence/presence_writer.cpp
        )
endif()

set(Gameserverplatform_Source_Files
    ../../Source/Services/GameServerPlatform/allocation_result.cpp
    ../../Source/Services/GameServerPlatform/cluster_result.cpp
    ../../Source/Services/GameServerPlatform/game_server_image_set.cpp
    ../../Source/Services/GameServerPlatform/game_server_metadata_result.cpp
    ../../Source/Services/GameServerPlatform/game_server_platform_service.cpp
    ../../Source/Services/GameServerPlatform/game_server_port_mapping.cpp
    ../../Source/Services/GameServerPlatform/game_server_ticket_status.cpp
    ../../Source/Services/GameServerPlatform/game_variant.cpp
    ../../Source/Services/GameServerPlatform/game_variant_schema.cpp
    ../../Source/Services/GameServerPlatform/quality_of_service_server.cpp
    )

set(Realtimeactivity_Source_Files
    ../../Source/Services/RealTimeActivity/real_time_activity_service.cpp
    ../../Source/Services/RealTimeActivity/real_time_activity_service_factory.cpp
    ../../Source/Services/RealTimeActivity/real_time_activity_subscription.cpp
    ../../Source/Services/RealTimeActivity/real_time_activity_subscription_error_event_args.cpp
    ../../Source/Services/RealTimeActivity/real_time_activity_internal.h
    )

set(Titlestorage_Source_Files
    ../../Source/Services/TitleStorage/title_storage_blob_metadata.cpp
    ../../Source/Services/TitleStorage/title_storage_blob_metadata_result.cpp
    ../../Source/Services/TitleStorage/title_storage_blob_result.cpp
    ../../Source/Services/TitleStorage/title_storage_quota.cpp
    ../../Source/Services/TitleStorage/title_storage_service.cpp
    )

set(Social_Source_Files
    ../../Source/Services/Social/profile_service.cpp
    ../../Source/Services/Social/reputation_feedback_request.cpp
    ../../Source/Services/Social/reputation_service.cpp
    ../../Source/Services/Social/reputation_service_impl.cpp
    ../../Source/Services/Social/Social_service.cpp
    ../../Source/Services/Social/xbox_Social_relationship.cpp
    ../../Source/Services/Social/xbox_Social_relationship_result.cpp
    ../../Source/Services/Social/xbox_user_profile.cpp
    ../../Source/Services/Social/social_relationship_change_event_args.cpp
    ../../Source/Services/Social/social_relationship_change_subscription.cpp
    ../../Source/Services/Social/social_service_impl.cpp
    ../../Source/Services/Social/social_internal.h
    ../../Source/Services/Social/profile_internal.h
    ../../Source/Services/Social/profile_service_impl.cpp
    )

set(Social_Manager_Source_Files
    ../../Source/Services/Social/Manager/internal_Social_event.cpp
    ../../Source/Services/Social/Manager/peoplehub_service.cpp
    ../../Source/Services/Social/Manager/preferred_color.cpp
    ../../Source/Services/Social/Manager/Social_event.cpp
    ../../Source/Services/Social/Manager/Social_graph.cpp
    ../../Source/Services/Social/Manager/Social_manager.cpp
    ../../Source/Services/Social/Manager/Social_manager_presence_title_record.cpp
    ../../Source/Services/Social/Manager/Social_manager_presence_record.cpp
    ../../Source/Services/Social/Manager/Social_user_group_loaded_event_args.cpp
    ../../Source/Services/Social/Manager/title_history.cpp
    ../../Source/Services/Social/Manager/xbox_Social_user.cpp
    ../../Source/Services/Social/Manager/xbox_Social_user_group.cpp
    ../../Source/Services/Social/Manager/xbox_user_id_container.cpp
    ../../Source/Services/Social/Manager/Social_manager_internal.h
    ../../Source/Services/Social/Manager/Social_manager_internal.cpp
    )

set(Stats_Source_Files
    ../../Source/Services/Stats/requested_statistics.cpp
    ../../Source/Services/Stats/service_configuration_statistic.cpp
    ../../Source/Services/Stats/statistic.cpp
    ../../Source/Services/Stats/user_statistics_result.cpp
    ../../Source/Services/Stats/user_statistics_service.cpp
    ../../Source/Services/Stats/statistic_change_event_args.cpp
    ../../Source/Services/Stats/statistic_change_subscription.cpp
    ../../Source/Services/Stats/user_statistics_service_impl.cpp
    ../../Source/Services/Stats/user_statistics_internal.h
    )

set(Stats_Manager_Source_Files
    ../../Source/Services/Stats/Manager/Leaderboard_query.cpp
    ../../Source/Services/Stats/Manager/Leaderboard_result_event_args.cpp
    ../../Source/Services/Stats/Manager/Stats_manager.cpp
    ../../Source/Services/Stats/Manager/Stats_manager_impl.cpp
    ../../Source/Services/Stats/Manager/Stats_service.cpp
    ../../Source/Services/Stats/Manager/Stats_value_document.cpp
    ../../Source/Services/Stats/Manager/stat_event.cpp
    ../../Source/Services/Stats/Manager/stat_value.cpp
    ../../Source/Services/Stats/Manager/Stats_manager_internal.h
    )

set(Matchmaking_Source_Files
    ../../Source/Services/Matchmaking/create_match_ticket_response.cpp
    ../../Source/Services/Matchmaking/hopper_statistics_response.cpp
    ../../Source/Services/Matchmaking/matchmaking_service.cpp
    ../../Source/Services/Matchmaking/match_ticket_details_response.cpp
    )

set (Multiplayer_Source_Files
    ../../Source/Services/Multiplayer/multiplayer_activity_details.cpp
    ../../Source/Services/Multiplayer/multiplayer_activity_handle_post_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_activity_query_post_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_invite.cpp
    ../../Source/Services/Multiplayer/multiplayer_invite_handle_post_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_managed_initialization.cpp
    ../../Source/Services/Multiplayer/multiplayer_member_initialization.cpp
    ../../Source/Services/Multiplayer/multiplayer_peer_to_host_requirements.cpp
    ../../Source/Services/Multiplayer/multiplayer_peer_to_peer_requirements.cpp
    ../../Source/Services/Multiplayer/multiplayer_quality_of_service_measurement.cpp
    ../../Source/Services/Multiplayer/multiplayer_service.cpp
    ../../Source/Services/Multiplayer/multiplayer_search_handle_details.cpp
    ../../Source/Services/Multiplayer/multiplayer_search_handle_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_query_search_handle_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_role_types.cpp
    ../../Source/Services/Multiplayer/multiplayer_role_info.cpp
    ../../Source/Services/Multiplayer/multiplayer_role_type.cpp
    ../../Source/Services/Multiplayer/multiplayer_session.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_capabilities.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_constants.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_member.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_member_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_properties.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_reference.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_states.cpp
    ../../Source/Services/Multiplayer/multiplayer_get_sessions_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_service_impl.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_arbitration_server.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_change_event_args.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_matchmaking_server.cpp
    ../../Source/Services/Multiplayer/multiplayer_session_tournaments_server.cpp
    ../../Source/Services/Multiplayer/multiplayer_transfer_handle_post_request.cpp
    ../../Source/Services/Multiplayer/multiplayer_subscription.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_manager_internal.h
    ../../Source/Services/Multiplayer/multiplayer_internal.h
    )

set (Multiplayer_Manager_Source_Files
    ../../Source/Services/Multiplayer/Manager/find_match_completed_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/host_changed_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/join_lobby_completed_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_match_client.cpp
    ../../Source/Services/Multiplayer/Manager/member_joined_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/member_left_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/member_property_changed_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_session_writer.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_client_manager.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_client_pending_reader.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_client_pending_request.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_event.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_game_client.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_member.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_game_session.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_lobby_client.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_local_user_manager.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_lobby_session.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_local_user.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_manager.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_manager_utils.cpp
    ../../Source/Services/Multiplayer/Manager/session_property_changed_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/tournament_game_session_ready_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/tournament_registration_state_changed_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/user_added_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/user_removed_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/perform_qos_measurements_event_args.cpp
    ../../Source/Services/Multiplayer/Manager/multiplayer_manager_internal.h
    )

set(Tournaments_Source_Files
    ../../Source/Services/Tournaments/current_match_metadata.cpp
    ../../Source/Services/Tournaments/match_metadata.cpp
    ../../Source/Services/Tournaments/previous_match_metadata.cpp
    ../../Source/Services/Tournaments/team_change_event_args.cpp
    ../../Source/Services/Tournaments/team_change_subscription.cpp
    ../../Source/Services/Tournaments/tournament_service_impl.cpp
    ../../Source/Services/Tournaments/tournament_change_event_args.cpp
    ../../Source/Services/Tournaments/tournament_change_subscription.cpp
    ../../Source/Services/Tournaments/team_info.cpp
    ../../Source/Services/Tournaments/team_request.cpp
    ../../Source/Services/Tournaments/team_request_result.cpp
    ../../Source/Services/Tournaments/team_summary.cpp
    ../../Source/Services/Tournaments/tournament.cpp
    ../../Source/Services/Tournaments/tournament_reference.cpp
    ../../Source/Services/Tournaments/tournament_request.cpp
    ../../Source/Services/Tournaments/tournament_request_result.cpp
    ../../Source/Services/Tournaments/tournament_service.cpp
    ../../Source/Services/Tournaments/tournament_team_result.cpp
    ../../Source/Services/Tournaments/tournament_service_internal.h
    )

set(EntertainmentProfile_Source_Files
    ../../Source/Services/EntertainmentProfile/entertainment_profile.cpp
    ../../Source/Services/EntertainmentProfile/entertainment_profile_list_contains_item_result.cpp
    ../../Source/Services/EntertainmentProfile/entertainment_profile_list_xbox_one_pins.cpp
    )

set(Marketplace_Source_Files
    ../../Source/Services/Marketplace/browse_catalog_result.cpp
    ../../Source/Services/Marketplace/catalog_item.cpp
    ../../Source/Services/Marketplace/catalog_item_availability.cpp
    ../../Source/Services/Marketplace/catalog_item_details.cpp
    ../../Source/Services/Marketplace/catalog_item_image.cpp
    ../../Source/Services/Marketplace/catalog_service.cpp
    ../../Source/Services/Marketplace/catalog_total.cpp
    ../../Source/Services/Marketplace/consume_inventory_item_request.cpp
    ../../Source/Services/Marketplace/consume_inventory_item_result.cpp
    ../../Source/Services/Marketplace/inventory_item.cpp
    ../../Source/Services/Marketplace/inventory_items_result.cpp
    ../../Source/Services/Marketplace/inventory_service.cpp
    )

set(Clubs_Source_Files
    ../../Source/Services/Clubs/club.cpp
    ../../Source/Services/Clubs/club_action_settings.cpp
    ../../Source/Services/Clubs/club_presence_counts.cpp
    ../../Source/Services/Clubs/club_profile.cpp
    ../../Source/Services/Clubs/club_role_record.cpp
    ../../Source/Services/Clubs/club_roster.cpp
    ../../Source/Services/Clubs/club_search_auto_complete.cpp
    ../../Source/Services/Clubs/club_search_facet_result.cpp
    ../../Source/Services/Clubs/club_user_presence_record.cpp
    ../../Source/Services/Clubs/clubs_owned_result.cpp
    ../../Source/Services/Clubs/clubs_search_result.cpp
    ../../Source/Services/Clubs/clubs_serializers.cpp
    ../../Source/Services/Clubs/clubs_serializers.h
    ../../Source/Services/Clubs/clubs_service.cpp
    ../../Source/Services/Clubs/clubs_service_impl.cpp
    ../../Source/Services/Clubs/clubs_service_impl.h
    )

set(System_Source_Files_C
    ../../Source/System/C/user_internal_c.h
    ../../Source/System/C/user_internal_c.cpp
    )

if( NOT XDK )
    list(APPEND
        System_Source_Files_C
        ../../Source/System/C/system_c.h
        ../../Source/System/C/user_c.cpp
        )
endif()

set(Shared_Source_Files_C
    ../../Source/Shared/C/xbox_live_app_config_c.cpp
    ../../Source/Shared/C/xbox_live_context_settings_c.cpp
    )

set(Common_Source_Files_C
    ../../Source/Services/Common/C/xbox_live_context_internal_c.h
    ../../Source/Services/Common/C/xbox_live_context_c.cpp
    ../../Source/Services/Common/C/xbox_live_global_c.cpp
    )

set(Social_Source_Files_C
    ../../Source/Services/Social/C/profile.cpp
    ../../Source/Services/Social/C/social.cpp
    )

set(Social_Manager_Source_Files_C
    ../../Source/Services/Social/Manager/C/social_manager_c.cpp
    )

set(Achievements_Source_Files_C
    ../../Source/Services/Achievements/C/achievements.cpp
    ../../Source/Services/Achievements/C/achievements_internal_c.h
    ../../Source/Services/Achievements/C/achievements_internal_c.cpp
    )

set(EntertainmentProfile_WinRT_Source_Files
    ../../Source/Services/EntertainmentProfile/WinRT/EntertainmentProfileListContainsItemResult_WinRT.cpp
    ../../Source/Services/EntertainmentProfile/WinRT/EntertainmentProfileListContainsItemResult_WinRT.h
    ../../Source/Services/EntertainmentProfile/WinRT/EntertainmentProfileListService_WinRT.cpp
    ../../Source/Services/EntertainmentProfile/WinRT/EntertainmentProfileListService_WinRT.h
    ../../Source/Services/EntertainmentProfile/WinRT/EntertainmentProfileListVideoQueue_WinRT.cpp
    ../../Source/Services/EntertainmentProfile/WinRT/EntertainmentProfileListVideoQueue_WinRT.h
    )

set(Marketplace_WinRT_Source_Files
    ../../Source/Services/Marketplace/WinRT/BundleRelationshipType_WinRT.h
    ../../Source/Services/Marketplace/WinRT/InventoryItemState_WinRT.h
    ../../Source/Services/Marketplace/WinRT/CatalogSortOrder_WinRT.h
    ../../Source/Services/Marketplace/WinRT/InventoryItemAvailability_WinRT.h
    ../../Source/Services/Marketplace/WinRT/InventoryService_WinRT.h
    ../../Source/Services/Marketplace/WinRT/MediaItemType_WinRT.h
    ../../Source/Services/Marketplace/WinRT/BrowseCatalogResult_WinRT.h
    ../../Source/Services/Marketplace/WinRT/CatalogItemAvailability_WinRT.h
    ../../Source/Services/Marketplace/WinRT/CatalogItemDetails_WinRT.h
    ../../Source/Services/Marketplace/WinRT/CatalogItemImage_WinRT.h
    ../../Source/Services/Marketplace/WinRT/CatalogItem_WinRT.h
    ../../Source/Services/Marketplace/WinRT/CatalogService_WinRT.h
    ../../Source/Services/Marketplace/WinRT/CatalogTotal_WinRT.h
    ../../Source/Services/Marketplace/WinRT/ConsumeInventoryItemResult_WinRT.h
    ../../Source/Services/Marketplace/WinRT/InventoryItemsResult_WinRT.h
    ../../Source/Services/Marketplace/WinRT/InventoryItem_WinRT.h
    ../../Source/Services/Marketplace/WinRT/InventoryService_WinRT.h
    ../../Source/Services/Marketplace/WinRT/BrowseCatalogResult_WinRT.cpp
    ../../Source/Services/Marketplace/WinRT/CatalogItemAvailability_WinRT.cpp
    ../../Source/Services/Marketplace/WinRT/CatalogItemDetails_WinRT.cpp
    ../../Source/Services/Marketplace/WinRT/CatalogItemImage_WinRT.cpp
    ../../Source/Services/Marketplace/WinRT/CatalogItem_WinRT.cpp
    ../../Source/Services/Marketplace/WinRT/CatalogService_WinRT.cpp
    ../../Source/Services/Marketplace/WinRT/CatalogTotal_WinRT.cpp
    ../../Source/Services/Marketplace/WinRT/ConsumeInventoryItemResult_WinRT.cpp
    ../../Source/Services/Marketplace/WinRT/InventoryItemsResult_WinRT.cpp
    ../../Source/Services/Marketplace/WinRT/InventoryItem_WinRT.cpp
    ../../Source/Services/Marketplace/WinRT/InventoryService_WinRT.cpp
    )

set(Achievements_WinRT_Source_Files
    ../../Source/Services/Achievements/WinRT/AchievementMediaAssetType_WinRT.h
    ../../Source/Services/Achievements/WinRT/AchievementOrderBy_WinRT.h
    ../../Source/Services/Achievements/WinRT/AchievementProgression_WinRT.cpp
    ../../Source/Services/Achievements/WinRT/AchievementProgression_WinRT.h
    ../../Source/Services/Achievements/WinRT/AchievementRequirement_WinRT.h
    ../../Source/Services/Achievements/WinRT/AchievementRewardType_WinRT.h
    ../../Source/Services/Achievements/WinRT/AchievementService_WinRT.cpp
    ../../Source/Services/Achievements/WinRT/AchievementService_WinRT.h
    ../../Source/Services/Achievements/WinRT/AchievementType_WinRT.h
    ../../Source/Services/Achievements/WinRT/Achievement_WinRT.cpp
    ../../Source/Services/Achievements/WinRT/Achievement_WinRT.h
    ../../Source/Services/Achievements/WinRT/AchievementsResult_WinRT.cpp
    ../../Source/Services/Achievements/WinRT/AchievementsResult_WinRT.h
    ../../Source/Services/Achievements/WinRT/MediaAsset_WinRT.cpp
    ../../Source/Services/Achievements/WinRT/MediaAsset_WinRT.h
    ../../Source/Services/Achievements/WinRT/ParticipationType_WinRT.h
    ../../Source/Services/Achievements/WinRT/ProgressState_WinRT.h
    ../../Source/Services/Achievements/WinRT/Reward_WinRT.cpp
    ../../Source/Services/Achievements/WinRT/Reward_WinRT.h
    ../../Source/Services/Achievements/WinRT/TimeWindow_WinRT.cpp
    ../../Source/Services/Achievements/WinRT/TimeWindow_WinRT.h
    ../../Source/Services/Achievements/WinRT/TitleAssociation_WinRT.h
    )

set(Common_WinRT_Source_Files
    ../../Source/Services/Common/WinRT/XboxLiveContext_WinRT.cpp
    ../../Source/Services/Common/WinRT/XboxLiveContext_WinRT.h
    )

set(Events_WinRT_Source_Files
    ../../Source/Services/Events/WinRT/EventsService_WinRT.cpp
    ../../Source/Services/Events/WinRT/EventsService_WinRT.h
    )

set(GameServerPlatform_WinRT_Source_Files
    ../../Source/Services/GameServerPlatform/WinRT/AllocationResult_WinRT.cpp
    ../../Source/Services/GameServerPlatform/WinRT/AllocationResult_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/ClusterResult_WinRT.cpp
    ../../Source/Services/GameServerPlatform/WinRT/ClusterResult_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/GameServerFulfillmentState_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/GameServerHostStatus_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/GameServerImageSet_WinRT.cpp
    ../../Source/Services/GameServerPlatform/WinRT/GameServerImageSet_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/GameServerMetadataResult_WinRT.cpp
    ../../Source/Services/GameServerPlatform/WinRT/GameServerMetadataResult_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/GameServerPlatformService_WinRT.cpp
    ../../Source/Services/GameServerPlatform/WinRT/GameServerPlatformService_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/GameServerPortMapping_WinRT.cpp
    ../../Source/Services/GameServerPlatform/WinRT/GameServerPortMapping_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/GameServerTicketStatus_WinRT.cpp
    ../../Source/Services/GameServerPlatform/WinRT/GameServerTicketStatus_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/GameVariantSchema_WinRT.cpp
    ../../Source/Services/GameServerPlatform/WinRT/GameVariantSchema_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/GameVariant_WinRT.cpp
    ../../Source/Services/GameServerPlatform/WinRT/GameVariant_WinRT.h
    ../../Source/Services/GameServerPlatform/WinRT/QualityOfServiceServer_WinRT.cpp
    ../../Source/Services/GameServerPlatform/WinRT/QualityOfServiceServer_WinRT.h
    )

set(Leaderboard_WinRT_Source_Files
    ../../Source/Services/Leaderboard/WinRT/LeaderboardColumn.cpp
    ../../Source/Services/Leaderboard/WinRT/LeaderboardColumn.h
    ../../Source/Services/Leaderboard/WinRT/LeaderboardResult.cpp
    ../../Source/Services/Leaderboard/WinRT/LeaderboardResult.h
    ../../Source/Services/Leaderboard/WinRT/LeaderboardRow.cpp
    ../../Source/Services/Leaderboard/WinRT/LeaderboardRow.h
    ../../Source/Services/Leaderboard/WinRT/LeaderboardService.cpp
    ../../Source/Services/Leaderboard/WinRT/LeaderboardService.h
    )

set(Matchmaking_WinRT_Source_Files
    ../../Source/Services/Matchmaking/WinRT/CreateMatchTicketResponse_WinRT.h
    ../../Source/Services/Matchmaking/WinRT/HopperStatisticsResponse_WinRT.h
    ../../Source/Services/Matchmaking/WinRT/MatchTicketDetailsResponse_WinRT.cpp
    ../../Source/Services/Matchmaking/WinRT/MatchTicketDetailsResponse_WinRT.h
    ../../Source/Services/Matchmaking/WinRT/MatchmakingService_WinRT.cpp
    ../../Source/Services/Matchmaking/WinRT/MatchmakingService_WinRT.h
    ../../Source/Services/Matchmaking/WinRT/PreserveSessionMode_WinRT.h
    ../../Source/Services/Matchmaking/WinRT/TicketStatus_WinRT.h
    )

set(Misc_WinRT_Source_Files
    ../../Source/Services/Misc/WinRT/ContextualSearchBroadcast_WinRT.cpp
    ../../Source/Services/Misc/WinRT/ContextualSearchBroadcast_WinRT.h
    ../../Source/Services/Misc/WinRT/ContextualSearchConfiguredStat_WinRT.cpp
    ../../Source/Services/Misc/WinRT/ContextualSearchConfiguredStat_WinRT.h
    ../../Source/Services/Misc/WinRT/ContextualSearchFilterOperator_WinRT.h
    ../../Source/Services/Misc/WinRT/ContextualSearchGameClipStat_WinRT.cpp
    ../../Source/Services/Misc/WinRT/ContextualSearchGameClipStat_WinRT.h
    ../../Source/Services/Misc/WinRT/ContextualSearchGameClipThumbnail_WinRT.cpp
    ../../Source/Services/Misc/WinRT/ContextualSearchGameClipThumbnail_WinRT.h
    ../../Source/Services/Misc/WinRT/ContextualSearchGameClipUriInfo_WinRT.cpp
    ../../Source/Services/Misc/WinRT/ContextualSearchGameClipUriInfo_WinRT.h
    ../../Source/Services/Misc/WinRT/ContextualSearchGameClip_WinRT.cpp
    ../../Source/Services/Misc/WinRT/ContextualSearchGameClip_WinRT.h
    ../../Source/Services/Misc/WinRT/ContextualSearchGameClipsResult_WinRT.cpp
    ../../Source/Services/Misc/WinRT/ContextualSearchGameClipsResult_WinRT.h
    ../../Source/Services/Misc/WinRT/ContextualSearchService_WinRT.cpp
    ../../Source/Services/Misc/WinRT/ContextualSearchService_WinRT.h
    ../../Source/Services/Misc/WinRT/ContextualSearchStatDisplayType_WinRT.h
    ../../Source/Services/Misc/WinRT/ContextualSearchStatVisibility_WinRT.h
    ../../Source/Services/Misc/WinRT/StringService_WinRT.cpp
    ../../Source/Services/Misc/WinRT/StringService_WinRT.h
    ../../Source/Services/Misc/WinRT/VerifyStringResultCode_WinRT.h
    ../../Source/Services/Misc/WinRT/VerifyStringResult_WinRT.h
    ../../Source/Services/Misc/WinRT/XboxLiveHttpCall_WinRT.cpp
    ../../Source/Services/Misc/WinRT/XboxLiveHttpCall_WinRT.h
    )
    
if( NOT XDK )
    list(APPEND
        Misc_WinRT_Source_Files
        ../../Source/Services/Misc/WinRT/TitleCallableUI_WinRT.cpp
        ../../Source/Services/Misc/WinRT/TitleCallableUI_WinRT.h
        )
endif()

set(MultiplayerManager_WinRT_Source_Files
    ../../Source/Services/Multiplayer/Manager/WinRT/FindMatchCompletedEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/FindMatchCompletedEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/HostChangedEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/HostChangedEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/JoinInProgressState_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/JoinLobbyCompletedEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/JoinLobbyCompletedEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MatchStatus_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MemberJoinedEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/MemberJoinedEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MemberLeftEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/MemberLeftEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MemberPropertyChangedEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/MemberPropertyChangedEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerEventType_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerEvent_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerEvent_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerGameSession_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerGameSession_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerLobbySession_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerLobbySession_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerManager_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerManager_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerMember_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerMember_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/MultiplayerSessionType_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/PerformQosMeasurementsEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/PerformQosMeasurementsEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/SessionPropertyChangedEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/SessionPropertyChangedEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/TournamentGameSessionReadyEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/TournamentGameSessionReadyEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/TournamentRegistrationStateChangedEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/TournamentRegistrationStateChangedEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/UserAddedEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/UserAddedEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/Manager/WinRT/UserRemovedEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/Manager/WinRT/UserRemovedEventArgs_WinRT.h
    )

set(Multiplayer_WinRT_Source_Files
    ../../Source/Services/Multiplayer/WinRT/MatchmakingStatus_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayMetrics_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerActivityDetails_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerActivityDetails_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerGetSessionsRequest_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerGetSessionsRequest_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerInitializationStage_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerManagedInitialization_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerManagedInitialization_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerMeasurementFailure_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerMemberInitialization_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerMemberInitialization_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerPeerToHostRequirements_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerPeerToHostRequirements_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerPeerToPeerRequirements_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerPeerToPeerRequirements_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerQualityOfServiceMeasurements_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerQualityOfServiceMeasurements_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerQuerySearchHandleRequest_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerQuerySearchHandleRequest_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerRoleInfo_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerRoleInfo_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerRoleType_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerRoleType_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSearchHandleDetails_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSearchHandleDetails_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSearchHandleRequest_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSearchHandleRequest_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerServiceMatchmakingServer_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerServiceMatchmakingServer_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerService_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerService_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionArbitrationServer_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionArbitrationServer_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionCapabilities_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionCapabilities_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionChangeEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionChangeEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionChangeTypes.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionConstants_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionConstants_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionMemberStatus_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionMember_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionMember_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionProperties_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionProperties_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionReference_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionReference_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionRestriction_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionRoleTypes_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionRoleTypes_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionStates_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionStates_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionStatus_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionTournamentsServer_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionTournamentsServer_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionVisibility_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSessionWriteMode_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSession_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSession_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSubscriptionLostEventArgs_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/MultiplayerSubscriptionLostEventArgs_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/MutableRoleSetting_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/NetworkAddressTranslationSetting_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/WriteSessionResult_WinRT.cpp
    ../../Source/Services/Multiplayer/WinRT/WriteSessionResult_WinRT.h
    ../../Source/Services/Multiplayer/WinRT/WriteSessionStatus_WinRT.h
    )

set(Presence_WinRT_Source_Files
    ../../Source/Services/Presence/WinRT/DevicePresenceChangeEventArgs_WinRT.cpp
    ../../Source/Services/Presence/WinRT/DevicePresenceChangeEventArgs_WinRT.h
    ../../Source/Services/Presence/WinRT/DevicePresenceChangeSubscription_WinRT.cpp
    ../../Source/Services/Presence/WinRT/DevicePresenceChangeSubscription_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceBroadcastRecord_WinRT.cpp
    ../../Source/Services/Presence/WinRT/PresenceBroadcastRecord_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceData_WinRT.cpp
    ../../Source/Services/Presence/WinRT/PresenceData_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceDetailLevel_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceDeviceRecord_WinRT.cpp
    ../../Source/Services/Presence/WinRT/PresenceDeviceRecord_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceDeviceType_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceMediaIdType_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceMediaRecord_WinRT.cpp
    ../../Source/Services/Presence/WinRT/PresenceMediaRecord_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceRecord_WinRT.cpp
    ../../Source/Services/Presence/WinRT/PresenceRecord_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceService_WinRT.cpp
    ../../Source/Services/Presence/WinRT/PresenceService_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceTitleRecord_WinRT.cpp
    ../../Source/Services/Presence/WinRT/PresenceTitleRecord_WinRT.h
    ../../Source/Services/Presence/WinRT/PresenceTitleViewState_WinRT.h
    ../../Source/Services/Presence/WinRT/TitlePresenceChangeEventArgs_WinRT.cpp
    ../../Source/Services/Presence/WinRT/TitlePresenceChangeEventArgs_WinRT.h
    ../../Source/Services/Presence/WinRT/TitlePresenceChangeSubscription_WinRT.cpp
    ../../Source/Services/Presence/WinRT/TitlePresenceChangeSubscription_WinRT.h
    ../../Source/Services/Presence/WinRT/TitlePresenceState_WinRT.h
    ../../Source/Services/Presence/WinRT/UserPresenceState_WinRT.h
    )

set(Privacy_WinRT_Source_Files
    ../../Source/Services/Privacy/WinRT/MultiplePermissionsCheckResult_WinRT.cpp
    ../../Source/Services/Privacy/WinRT/MultiplePermissionsCheckResult_WinRT.h
    ../../Source/Services/Privacy/WinRT/PermissionCheckResult_WinRT.cpp
    ../../Source/Services/Privacy/WinRT/PermissionCheckResult_WinRT.h
    ../../Source/Services/Privacy/WinRT/PermissionDenyReason_WinRT.cpp
    ../../Source/Services/Privacy/WinRT/PermissionDenyReason_WinRT.h
    ../../Source/Services/Privacy/WinRT/PermissionIdConstants_WinRT.cpp
    ../../Source/Services/Privacy/WinRT/PermissionIdConstants_WinRT.h
    ../../Source/Services/Privacy/WinRT/PrivacyService_WinRT.cpp
    ../../Source/Services/Privacy/WinRT/PrivacyService_WinRT.h
    )

set(RealTimeActivity_WinRT_Source_Files
    ../../Source/Services/RealTimeActivity/WinRT/RealTimeActivityConnectionState_WinRT.h
    ../../Source/Services/RealTimeActivity/WinRT/RealTimeActivityResyncEventArgs_WinRT.cpp
    ../../Source/Services/RealTimeActivity/WinRT/RealTimeActivityResyncEventArgs_WinRT.h
    ../../Source/Services/RealTimeActivity/WinRT/RealTimeActivityService_WinRT.cpp
    ../../Source/Services/RealTimeActivity/WinRT/RealTimeActivityService_WinRT.h
    ../../Source/Services/RealTimeActivity/WinRT/RealTimeActivitySubscriptionErrorEventArgs_WinRT.cpp
    ../../Source/Services/RealTimeActivity/WinRT/RealTimeActivitySubscriptionErrorEventArgs_WinRT.h
    ../../Source/Services/RealTimeActivity/WinRT/RealTimeActivitySubscriptionError_WinRT.h
    ../../Source/Services/RealTimeActivity/WinRT/RealTimeActivitySubscriptionState_WinRT.h
    )

set(SocialManager_WinRT_Source_Files
    ../../Source/Services/Social/Manager/WinRT/PreferredColor_WinRT.cpp
    ../../Source/Services/Social/Manager/WinRT/PreferredColor_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/SocialEventArgs_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/SocialEventType_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/SocialEvent_WinRT.cpp
    ../../Source/Services/Social/Manager/WinRT/SocialEvent_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/SocialManagerExtraDetailLevel_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/SocialManagerPresenceRecord_WinRT.cpp
    ../../Source/Services/Social/Manager/WinRT/SocialManagerPresenceRecord_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/SocialManagerPresenceTitleRecord_WinRT.cpp
    ../../Source/Services/Social/Manager/WinRT/SocialManagerPresenceTitleRecord_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/SocialManager_WinRT.cpp
    ../../Source/Services/Social/Manager/WinRT/SocialManager_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/SocialUserGroupLoadedEventArgs_WinRT.cpp
    ../../Source/Services/Social/Manager/WinRT/SocialUserGroupLoadedEventArgs_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/SocialUserGroupType_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/TitleHistory_WinRT.cpp
    ../../Source/Services/Social/Manager/WinRT/TitleHistory_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/XboxSocialUserGroup_WinRT.cpp
    ../../Source/Services/Social/Manager/WinRT/XboxSocialUserGroup_WinRT.h
    ../../Source/Services/Social/Manager/WinRT/XboxSocialUser_WinRT.cpp
    ../../Source/Services/Social/Manager/WinRT/XboxSocialUser_WinRT.h
    )

set(Social_WinRT_Source_Files
    ../../Source/Services/Social/WinRT/PresenceFilter_WinRT.h
    ../../Source/Services/Social/WinRT/ProfileService_WinRT.cpp
    ../../Source/Services/Social/WinRT/ProfileService_WinRT.h
    ../../Source/Services/Social/WinRT/RelationshipFilter_WinRT.h
    ../../Source/Services/Social/WinRT/ReputationFeedbackType_WinRT.h
    ../../Source/Services/Social/WinRT/ReputationService_WinRT.cpp
    ../../Source/Services/Social/WinRT/ReputationService_WinRT.h
    ../../Source/Services/Social/WinRT/SocialGroupConstants_WinRT.h
    ../../Source/Services/Social/WinRT/SocialNotificationType_WinRT.h
    ../../Source/Services/Social/WinRT/SocialRelationshipChangeEventArgs_WinRT.cpp
    ../../Source/Services/Social/WinRT/SocialRelationshipChangeEventArgs_WinRT.h
    ../../Source/Services/Social/WinRT/SocialRelationshipChangeSubscription_WinRT.cpp
    ../../Source/Services/Social/WinRT/SocialRelationshipChangeSubscription_WinRT.h
    ../../Source/Services/Social/WinRT/SocialRelationship_WinRT.h
    ../../Source/Services/Social/WinRT/SocialService_WinRT.cpp
    ../../Source/Services/Social/WinRT/SocialService_WinRT.h
    ../../Source/Services/Social/WinRT/XboxSocialRelationshipResult_WinRT.cpp
    ../../Source/Services/Social/WinRT/XboxSocialRelationshipResult_WinRT.h
    ../../Source/Services/Social/WinRT/XboxSocialRelationship_WinRT.cpp
    ../../Source/Services/Social/WinRT/XboxSocialRelationship_WinRT.h
    ../../Source/Services/Social/WinRT/XboxUserProfile_WinRT.cpp
    ../../Source/Services/Social/WinRT/XboxUserProfile_WinRT.h
    )

set(StatsManager_WinRT_Source_Files
    ../../Source/Services/Stats/Manager/WinRT/LeaderboardQuery_WinRT.cpp
    ../../Source/Services/Stats/Manager/WinRT/LeaderboardQuery_WinRT.h
    ../../Source/Services/Stats/Manager/WinRT/LeaderboardResultEventArgs_WinRT.cpp
    ../../Source/Services/Stats/Manager/WinRT/LeaderboardResultEventArgs_WinRT.h
    ../../Source/Services/Stats/Manager/WinRT/SortOrder_WinRT.h
    ../../Source/Services/Stats/Manager/WinRT/StatisticDataType_WinRT.h
    ../../Source/Services/Stats/Manager/WinRT/StatisticEventArgs_WinRT.h
    ../../Source/Services/Stats/Manager/WinRT/StatisticEventType_WinRT.h
    ../../Source/Services/Stats/Manager/WinRT/StatisticEvent_WinRT.cpp
    ../../Source/Services/Stats/Manager/WinRT/StatisticEvent_WinRT.h
    ../../Source/Services/Stats/Manager/WinRT/StatisticManager_WinRT.cpp
    ../../Source/Services/Stats/Manager/WinRT/StatisticManager_WinRT.h
    ../../Source/Services/Stats/Manager/WinRT/StatisticValue_WinRT.cpp
    ../../Source/Services/Stats/Manager/WinRT/StatisticValue_WinRT.h
    )

set(Stats_WinRT_Source_Files
    ../../Source/Services/Stats/WinRT/RequestedStatistics_WinRT.cpp
    ../../Source/Services/Stats/WinRT/RequestedStatistics_WinRT.h
    ../../Source/Services/Stats/WinRT/ServiceConfigurationStatistic_WinRT.cpp
    ../../Source/Services/Stats/WinRT/ServiceConfigurationStatistic_WinRT.h
    ../../Source/Services/Stats/WinRT/StatisticChangeEventArgs_WinRT.cpp
    ../../Source/Services/Stats/WinRT/StatisticChangeEventArgs_WinRT.h
    ../../Source/Services/Stats/WinRT/StatisticChangeSubscription_WinRT.cpp
    ../../Source/Services/Stats/WinRT/StatisticChangeSubscription_WinRT.h
    ../../Source/Services/Stats/WinRT/Statistic_WinRT.cpp
    ../../Source/Services/Stats/WinRT/Statistic_WinRT.h
    ../../Source/Services/Stats/WinRT/UserStatisticsResult_WinRT.cpp
    ../../Source/Services/Stats/WinRT/UserStatisticsResult_WinRT.h
    ../../Source/Services/Stats/WinRT/UserStatisticsService_WinRT.cpp
    ../../Source/Services/Stats/WinRT/UserStatisticsService_WinRT.h
    )

set(TitleStorage_WinRT_Source_Files
    ../../Source/Services/TitleStorage/WinRT/TitleStorageBlobMetadataResult_WinRT.cpp
    ../../Source/Services/TitleStorage/WinRT/TitleStorageBlobMetadataResult_WinRT.h
    ../../Source/Services/TitleStorage/WinRT/TitleStorageBlobMetadata_WinRT.cpp
    ../../Source/Services/TitleStorage/WinRT/TitleStorageBlobMetadata_WinRT.h
    ../../Source/Services/TitleStorage/WinRT/TitleStorageBlobResult_WinRT.cpp
    ../../Source/Services/TitleStorage/WinRT/TitleStorageBlobResult_WinRT.h
    ../../Source/Services/TitleStorage/WinRT/TitleStorageBlobType_WinRT.h
    ../../Source/Services/TitleStorage/WinRT/TitleStorageETagMatchCondition_WinRT.h
    ../../Source/Services/TitleStorage/WinRT/TitleStorageQuota_WinRT.cpp
    ../../Source/Services/TitleStorage/WinRT/TitleStorageQuota_WinRT.h
    ../../Source/Services/TitleStorage/WinRT/TitleStorageService_WinRT.cpp
    ../../Source/Services/TitleStorage/WinRT/TitleStorageService_WinRT.h
    ../../Source/Services/TitleStorage/WinRT/TitleStorageType_WinRT.h
    )

set(Tournaments_WinRT_Source_Files
    ../../Source/Services/Tournaments/WinRT/CurrentMatchMetadata_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/CurrentMatchMetadata_WinRT.h
    ../../Source/Services/Tournaments/WinRT/MatchMetadata_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/MatchMetadata_WinRT.h
    ../../Source/Services/Tournaments/WinRT/PreviousMatchMetadata_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/PreviousMatchMetadata_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TeamChangeEventArgs_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TeamChangeEventArgs_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TeamChangeSubscription_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TeamChangeSubscription_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TeamCompletedReason_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TeamInfo_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TeamInfo_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TeamOrderBy_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TeamRequestResult_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TeamRequestResult_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TeamRequest_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TeamRequest_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TeamState_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TeamSummary_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TeamSummary_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentArbitrationState_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentArbitrationStatus_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentChangeEventArgs_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TournamentChangeEventArgs_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentChangeSubscription_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TournamentChangeSubscription_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentGameResultSource_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentGameResultState_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentOrderBy_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentReference_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TournamentReference_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentRegistrationReason_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentRegistrationState_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentRequestResult_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TournamentRequestResult_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentRequest_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TournamentRequest_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentService_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TournamentService_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentSortOrder_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentState_WinRT.h
    ../../Source/Services/Tournaments/WinRT/TournamentTeamResult_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/TournamentTeamResult_WinRT.h
    ../../Source/Services/Tournaments/WinRT/Tournament_WinRT.cpp
    ../../Source/Services/Tournaments/WinRT/Tournament_WinRT.h
    )

set(Shared_WinRT_Source_Files
    ../../Source/Shared/WinRT/AppConfiguration_WinRT.cpp
    ../../Source/Shared/WinRT/AppConfiguration_WinRT.h
    ../../Source/Shared/WinRT/HttpCallRequestMessage_WinRT.cpp
    ../../Source/Shared/WinRT/HttpCallRequestMessage_WinRT.h
    ../../Source/Shared/WinRT/HttpRequestMessageType_WinRT.h
    ../../Source/Shared/WinRT/Macros_WinRT.h
    ../../Source/Shared/WinRT/ServiceCallLoggingConfig_WinRT.cpp
    ../../Source/Shared/WinRT/ServiceCallLoggingConfig_WinRT.h
    ../../Source/Shared/WinRT/Utils_WinRT.cpp
    ../../Source/Shared/WinRT/Utils_WinRT.h
    ../../Source/Shared/WinRT/XboxLiveContextSettings_WinRT.cpp
    ../../Source/Shared/WinRT/XboxLiveContextSettings_WinRT.h
    ../../Source/Shared/WinRT/XboxLiveLogCallEventArgs_WinRT.cpp
    ../../Source/Shared/WinRT/XboxLiveLogCallEventArgs_WinRT.h
    ../../Source/Shared/WinRT/XboxLiveServicesSettings_WinRT.cpp
    ../../Source/Shared/WinRT/XboxLiveServicesSettings_WinRT.h
    ../../Source/Shared/WinRT/XboxServiceCallRoutedEventArgs_WinRT.cpp
    ../../Source/Shared/WinRT/XboxServiceCallRoutedEventArgs_WinRT.h
    ../../Source/Shared/WinRT/XboxServicesDiagnosticsTraceLevel_WinRT.h
    )

if ( NOT XDK )
    list(APPEND
        Shared_WinRT_Source_Files
        ../../Source/Shared/WinRT/XboxLiveWnsEventArgs_WinRT.h
        )
else()
    list(APPEND
        Shared_WinRT_Source_Files
        ../../Source/Shared/WinRT/local_config_winrt.cpp
        )
endif()

set(System_WinRT_Source_Files
    ../../Source/System/WinRT/GetTokenAndSignatureResult_WinRT.cpp
    ../../Source/System/WinRT/GetTokenAndSignatureResult_WinRT.h
    ../../Source/System/WinRT/SignInResult_WinRT.h
    ../../Source/System/WinRT/SignInStatus_WinRT.h
    ../../Source/System/WinRT/User_WinRT.cpp
    ../../Source/System/WinRT/User_WinRT.h
    )

set(Clubs_WinRT_Source_Files
    ../../Source/Services/Clubs/WinRT/Club_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/Club_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubActionSettings_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubActionSettings_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubPresenceCounts_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubPresenceCounts_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubProfile_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubProfile_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubRecommendation_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubRecommendation_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubRole_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubRoleRecord_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubRoleRecord_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubRoster_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubRoster_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubSearchAutoComplete_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubSearchAutoComplete_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubSearchFacetResult_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubSearchFacetResult_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubSetting_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubSetting_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubsOwnedResult_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubsOwnedResult_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubsSearchResult_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubsSearchResult_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubsService_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubsService_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubType_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubUserPresence_WinRT.h
    ../../Source/Services/Clubs/WinRT/ClubUserPresenceRecord_WinRT.cpp
    ../../Source/Services/Clubs/WinRT/ClubUserPresenceRecord_WinRT.h
    )

set(Ship_Source_Files
    ../../Source/Services/Common/Desktop/pch.cpp
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
    source_group("C++ Source\\Services\\Misc" FILES ${Misc_Source_Files})
    source_group("C++ Source\\Services\\Common" FILES ${Common_Source_Files})
    source_group("C++ Source\\Services\\Achievements" FILES ${Achievements_Source_Files})
    source_group("C++ Source\\Services\\Leaderboard" FILES ${Leaderboard_Source_Files})
    source_group("C++ Source\\Services\\Privacy" FILES ${Privacy_Source_Files})
    source_group("C++ Source\\Services\\Presence" FILES ${Presence_Source_Files})
    source_group("C++ Source\\Services\\GameServerPlatform" FILES ${Gameserverplatform_Source_Files})
    source_group("C++ Source\\Services\\RTA" FILES ${Realtimeactivity_Source_Files})
    source_group("C++ Source\\Services\\TitleStorage" FILES ${Titlestorage_Source_Files})
    source_group("C++ Source\\Services\\Social" FILES ${Social_Source_Files})
    source_group("C++ Source\\Services\\Social\\Manager" FILES ${Social_Manager_Source_Files})
    source_group("C++ Source\\Services\\Stats" FILES ${Stats_Source_Files})
    source_group("C++ Source\\Services\\Stats\\Manager" FILES ${Stats_Manager_Source_Files})
    source_group("C++ Source\\Services\\Matchmaking" FILES ${Matchmaking_Source_Files})
    source_group("C++ Source\\Services\\Multiplayer" FILES ${Multiplayer_Source_Files})
    source_group("C++ Source\\Services\\Multiplayer\\Manager" FILES ${Multiplayer_Manager_Source_Files})
    source_group("C++ Source\\Services\\Tournaments" FILES ${Tournaments_Source_Files})
    source_group("C++ Source\\Services\\Clubs" FILES ${Clubs_Source_Files})

    if( XDK OR UNITTEST )
        message(STATUS "XDK source group")
        source_group("C++ Source\\Services\\EntertainmentProfile" FILES ${EntertainmentProfile_Source_Files})
        source_group("C++ Source\\Services\\Marketplace" FILES ${Marketplace_Source_Files})
    endif()
    
    if ( NOT XDK )
        message(STATUS "Non XDK source group")
        source_group("C++ Source\\Services\\Events" FILES ${Events_Source_Files})
    endif()

    if( WINRT OR UNITTEST )
        message(STATUS "WINRT source group")
        source_group("C++ Source\\Shared\\WinRT" FILES ${Shared_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Common\\WinRT" FILES ${Common_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Achievements\\WinRT" FILES ${Achievements_WinRT_Source_Files})
        source_group("C++ Source\\Services\\GameServerPlatform\\WinRT" FILES ${GameServerPlatform_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Leaderboard\\WinRT" FILES ${Leaderboard_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Matchmaking\\WinRT" FILES ${Matchmaking_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Misc\\WinRT" FILES ${Misc_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Multiplayer\\Manager\\WinRT" FILES ${MultiplayerManager_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Multiplayer\\WinRT" FILES ${Multiplayer_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Presence\\WinRT" FILES ${Presence_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Privacy\\WinRT" FILES ${Privacy_WinRT_Source_Files})
        source_group("C++ Source\\Services\\RTA\\WinRT" FILES ${RealTimeActivity_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Social\\Manager\\WinRT" FILES ${SocialManager_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Social\\WinRT" FILES ${Social_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Stats\\Manager\\WinRT" FILES ${StatsManager_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Stats\\WinRT" FILES ${Stats_WinRT_Source_Files})
        source_group("C++ Source\\Services\\TitleStorage\\WinRT" FILES ${TitleStorage_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Tournaments\\WinRT" FILES ${Tournaments_WinRT_Source_Files})
        source_group("C++ Source\\Services\\Clubs\\WinRT" FILES ${Clubs_WinRT_Source_Files})
        
        if( XDK OR UNITTEST )
            message(STATUS "XDK WinRT source group")
            source_group("C++ Source\\Services\\EntertainmentProfile\\WinRT" FILES ${EntertainmentProfile_WinRT_Source_Files})
            source_group("C++ Source\\Services\\Marketplace\\WinRT" FILES ${Marketplace_WinRT_Source_Files})
        endif()
        
        if( UWP OR UNITTEST )
            source_group("C++ Source\\System\\WinRT" FILES ${System_WinRT_Source_Files})
            source_group("C++ Source\\Services\\Events\\WinRT" FILES ${Events_WinRT_Source_Files})
        endif()
    endif()

    if( NOT WINRT )
        message(STATUS "Flat C source group")
        source_group("C++ Source\\System\\C" FILES ${System_Source_Files_C})
        source_group("C++ Source\\Shared\\C" FILES ${Shared_Source_Files_C})
        source_group("C++ Source\\Services\\Achievements\\C" FILES ${Achievements_Source_Files_C})
        source_group("C++ Source\\Services\\Common\\C" FILES ${Common_Source_Files_C})
        source_group("C++ Source\\Services\\Social\\C" FILES ${Social_Source_Files_C})
        source_group("C++ Source\\Services\\Social\\Manager\\C" FILES ${Social_Manager_Source_Files_C})
    endif()

    set( SOURCE_FILES
        ${Common_Source_Files}
        ${Public_Source_Files}
        ${System_Source_Files}
        ${Shared_Source_Files}
        ${Shared_Logger_Source_Files}
        ${Misc_Source_Files}
        ${Achievements_Source_Files}
        ${Leaderboard_Source_Files}
        ${Privacy_Source_Files}
        ${Presence_Source_Files}
        ${Gameserverplatform_Source_Files}
        ${Realtimeactivity_Source_Files}
        ${Titlestorage_Source_Files}
        ${Social_Source_Files}
        ${Social_Manager_Source_Files}
        ${Stats_Source_Files}
        ${Stats_Manager_Source_Files}
        ${Matchmaking_Source_Files}
        ${Multiplayer_Source_Files}
        ${Multiplayer_Manager_Source_Files}
        ${Tournaments_Source_Files}
        ${Clubs_Source_Files}
        )

    if( XDK OR UNITTEST )
        list(APPEND
            SOURCE_FILES
            ${EntertainmentProfile_Source_Files}
            ${Marketplace_Source_Files}
            )
    endif()
    
    if( NOT XDK )
        list(APPEND
            SOURCE_FILES
            ${Events_Source_Files}
            )
    endif()

    if( WINRT OR UNITTEST )
        message(STATUS "WINRT add_library")
        list(APPEND
            SOURCE_FILES
            ${Achievements_WinRT_Source_Files}
            ${Common_WinRT_Source_Files}
            ${GameServerPlatform_WinRT_Source_Files}
            ${Leaderboard_WinRT_Source_Files}
            ${Matchmaking_WinRT_Source_Files}
            ${Misc_WinRT_Source_Files}
            ${MultiplayerManager_WinRT_Source_Files}
            ${Multiplayer_WinRT_Source_Files}
            ${Presence_WinRT_Source_Files}
            ${Privacy_WinRT_Source_Files}
            ${RealTimeActivity_WinRT_Source_Files}
            ${SocialManager_WinRT_Source_Files}
            ${Social_WinRT_Source_Files}
            ${StatsManager_WinRT_Source_Files}
            ${Stats_WinRT_Source_Files}
            ${TitleStorage_WinRT_Source_Files}
            ${Tournaments_WinRT_Source_Files}
            ${Shared_WinRT_Source_Files}
            ${Clubs_WinRT_Source_Files}
            )
        
        if( XDK OR UNITTEST )
            list(APPEND
                SOURCE_FILES
                ${EntertainmentProfile_WinRT_Source_Files}
                ${Marketplace_WinRT_Source_Files}
                )
        endif()
        
        if ( UWP OR UNITTEST )
            list(APPEND
                SOURCE_FILES
                ${Events_WinRT_Source_Files}
                ${System_WinRT_Source_Files}
                )
        endif()
    endif()

    if( NOT WINRT )
        message(STATUS "Adding flat C source")
        list(APPEND
            SOURCE_FILES
            ${Public_Source_Files_C}
            ${Achievements_Source_Files_C}
            ${Social_Manager_Source_Files_C}
            ${Social_Source_Files_C}
            ${Common_Source_Files_C}
            ${Shared_Source_Files_C}
            ${System_Source_Files_C}
            )
    endif()
endif() # SHIP

set(UnitTests_Source_Files_Mocks
    ../../Tests/UnitTests/Mocks/MockHttpCall.cpp
    ../../Tests/UnitTests/Mocks/MockLocalConfig.cpp
    ../../Tests/UnitTests/Mocks/MockMultiplayer.cpp
    ../../Tests/UnitTests/Mocks/MockSocialManager.cpp
    ../../Tests/UnitTests/Mocks/MockUser.cpp
    ../../Tests/UnitTests/Mocks/MockXboxSystemFactory.cpp
    ../../Tests/UnitTests/Mocks/StockMocks.cpp
    ../../Tests/UnitTests/Mocks/MockHttpCall.h
    ../../Tests/UnitTests/Mocks/MockLocalConfig.h
    ../../Tests/UnitTests/Mocks/MockMultiplayer.h
    ../../Tests/UnitTests/Mocks/MockSocialManager.h
    ../../Tests/UnitTests/Mocks/MockUser.h
    ../../Tests/UnitTests/Mocks/MockXboxSystemFactory.h
    ../../Tests/UnitTests/Mocks/MockWebSocketClient.h
    ../../Tests/UnitTests/Mocks/StockMocks.h
    )

set(UnitTests_Source_Files_Support
    ../../Tests/UnitTests/Support/iso8601.cpp
    ../../Tests/UnitTests/Support/unittest_output.cpp
    ../../Tests/UnitTests/Support/DefineTestMacros.h
    ../../Tests/UnitTests/Support/iso8601.h
    ../../Tests/UnitTests/Support/UnitTestIncludes.h
    ../../Tests/UnitTests/Support/unittest_output.h
    )

set(UnitTests_Source_Files_Tests
    ../../Tests/UnitTests/Tests/Services/AchievementsTests.cpp
    ../../Tests/UnitTests/Tests/Services/ContextualSearchTests.cpp
    ../../Tests/UnitTests/Tests/Services/EntertainmentProfileTests.cpp
    ../../Tests/UnitTests/Tests/Services/ErrorTests.cpp
    ../../Tests/UnitTests/Tests/Services/GameServerPlatformTests.cpp
    ../../Tests/UnitTests/Tests/Services/LeaderboardTests.cpp
    ../../Tests/UnitTests/Tests/Services/MarketplaceTests.cpp
    ../../Tests/UnitTests/Tests/Services/MatchmakingTests.cpp
    ../../Tests/UnitTests/Tests/Services/MultiplayerManagerTests.cpp
    ../../Tests/UnitTests/Tests/Services/MultiplayerTests.cpp
    ../../Tests/UnitTests/Tests/Services/PeoplehubTests.cpp
    ../../Tests/UnitTests/Tests/Services/PresenceTests.cpp
    ../../Tests/UnitTests/Tests/Services/PrivacyTests.cpp
    ../../Tests/UnitTests/Tests/Services/ProfileTests.cpp
    ../../Tests/UnitTests/Tests/Services/RealTimeActivityTests.cpp
    ../../Tests/UnitTests/Tests/Services/ReputationTests.cpp
    ../../Tests/UnitTests/Tests/Services/RtaTestHelper.cpp
    ../../Tests/UnitTests/Tests/Services/SimplifiedStatServiceTests.cpp
    ../../Tests/UnitTests/Tests/Services/SocialManagerTests.cpp
    ../../Tests/UnitTests/Tests/Services/SocialTests.cpp
    ../../Tests/UnitTests/Tests/Services/StatsManagerTests.cpp
    ../../Tests/UnitTests/Tests/Services/StatsTests.cpp
    ../../Tests/UnitTests/Tests/Services/StringVerifyTests.cpp
    ../../Tests/UnitTests/Tests/Services/TitleStorageTests.cpp
    ../../Tests/UnitTests/Tests/Services/TournamentsTests.cpp
    ../../Tests/UnitTests/Tests/Shared/EventTests_WinRT.cpp
    ../../Tests/UnitTests/Tests/Shared/HttpCallResponseTests.cpp
    ../../Tests/UnitTests/Tests/Shared/HttpCallSettingsTests.cpp
    ../../Tests/UnitTests/Tests/Shared/HttpCallSettingsTests_WinRT.cpp
    ../../Tests/UnitTests/Tests/Shared/LogTests.cpp
    ../../Tests/UnitTests/Tests/Shared/ServiceCallLoggerTests.cpp
    ../../Tests/UnitTests/Tests/Shared/WebsocketTests.cpp
    ../../Tests/UnitTests/Tests/Shared/XboxLiveContextTests.cpp
    ../../Tests/UnitTests/Tests/Shared/XboxLiveCallbackTests.cpp
    ../../Tests/UnitTests/Tests/Services/RtaTestHelper.h
    ../../Tests/UnitTests/Tests/Services/SocialManagerHelper.h
    ../../Tests/UnitTests/Tests/Services/StatsManagerHelper.h
    ../../Tests/UnitTests/Tests/Services/ClubsTests.cpp
    )

set(Taef_UnitTests_Source_Files
    ../../Tests/UnitTests/Support/TAEF/UnitTestBase.h
    ../../Tests/UnitTests/Support/TAEF/UnitTestBase_winrt.h
    ../../Tests/UnitTests/Support/TAEF/UnitTestIncludes_TAEF.h
    ../../Tests/UnitTests/Support/TAEF/UnitTestBase.cpp
    ../../Tests/UnitTests/Support/TAEF/UnitTestBase_winrt.cpp
    )

set(TE_UnitTests_Source_Files
    ../../Tests/UnitTests/Support/TE/unittesthelpers.cpp
    ../../Tests/UnitTests/Support/TE/unittesthelpers.h
    ../../Tests/UnitTests/Support/TE/unittestincludes_te.h
    )

if( TAEF )
    source_group("C++ Source\\UnitTests\\Mocks" FILES ${UnitTests_Source_Files_Mocks})
    source_group("C++ Source\\UnitTests\\Support" FILES ${UnitTests_Source_Files_Support})
    source_group("C++ Source\\UnitTests\\Tests" FILES ${UnitTests_Source_Files_Tests})
    source_group("C++ Source\\UnitTests\\Support" FILES ${Taef_UnitTests_Source_Files})
    list(APPEND
        SOURCE_FILES
        ${UnitTests_Source_Files_Mocks}
        ${UnitTests_Source_Files_Support}
        ${UnitTests_Source_Files_Tests}
        ${Taef_UnitTests_Source_Files}
        )
elseif( TE )
    source_group("C++ Source\\UnitTests\\Mocks" FILES ${UnitTests_Source_Files_Mocks})
    source_group("C++ Source\\UnitTests\\Support" FILES ${UnitTests_Source_Files_Support})
    source_group("C++ Source\\UnitTests\\Tests" FILES ${UnitTests_Source_Files_Tests})
    source_group("C++ Source\\UnitTests\\Support" FILES ${TE_UnitTests_Source_Files})
    list(APPEND
        SOURCE_FILES
        ${UnitTests_Source_Files_Mocks}
        ${UnitTests_Source_Files_Support}
        ${UnitTests_Source_Files_Tests}
        ${TE_UnitTests_Source_Files}
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


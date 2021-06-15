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


get_filename_component(PATH_TO_XSAPI_ROOT "../../" ABSOLUTE)

if(ANDROID)
    set_source_files_properties(${PATH_TO_XSAPI_ROOT}/Source/Services/Common/Unix/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
else()
    set_source_files_properties(${PATH_TO_XSAPI_ROOT}/Source/Services/Common/cpp/pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
endif()

if (NOT ${CMAKE_GENERATOR} MATCHES "Visual Studio .*")
    set_property(SOURCE ${PATH_TO_XSAPI_ROOT}/Source/Services/Common/cpp/pch.cpp APPEND PROPERTY OBJECT_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/pch.pch")
    set_property(SOURCE ${SOURCES} APPEND PROPERTY OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/pch.pch")
endif()

include_directories(
    $(ProjectDir)
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/Services
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/Services/Common
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/Services/Social
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/Services/Multiplayer
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/Services/Presence
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/Services/Notification
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/Services/TCUI
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/Services/Social/Manager
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/Services/RealTimeActivity
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/Shared
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Include
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/External/cpprestsdk/Release/Include
    $(ProjectDir)${PATH_TO_XSAPI_ROOT}/Source/System
    )

set(CMAKE_SUPPRESS_REGENERATION true)

include("GetCommonXSAPISourceFiles.cmake")
get_common_xsapi_source_files(
    Public_Source_Files
    Public_Source_Files_C
    Common_Source_Files
    System_Source_Files
    Shared_Logger_Source_Files
    Shared_Source_Files
    Achievements_Source_Files
    Achievements_Manager_Source_Files
    Leaderboard_Source_Files
    Privacy_Source_Files
    Presence_Source_Files
    TitleStorage_Source_Files
    Social_Source_Files
    Social_Manager_Source_Files
    Stats_Source_Files
    Matchmaking_Source_Files
    Multiplayer_Source_Files
    Multiplayer_Manager_Source_Files
    StringVerify_Source_Files
    MultiplayerActivity_Source_Files
    RealTimeActivityManager_Source_Files
    ${PATH_TO_XSAPI_ROOT}
    )

if( UNITTEST )
    message(STATUS "Test public")
    list(APPEND
        Public_Source_Files
        ${PATH_TO_XSAPI_ROOT}/include/xsapi-cpp/title_callable_ui.h
        ${PATH_TO_XSAPI_ROOT}/include/xsapi-cpp/events.h
        )
else()
    if( XDK )
        message(STATUS "XDK public")
    else()
        message(STATUS "Windows public")
        list(APPEND
            Public_Source_Files
            ${PATH_TO_XSAPI_ROOT}/include/xsapi-cpp/events.h
            )
        if( PCWIN32 OR ANDROID OR UWP )
            list(APPEND
                Public_Source_Files
                ${PATH_TO_XSAPI_ROOT}/include/xsapi-cpp/notification_service.h
                )
        elseif( UWP OR ANDROID )
            message(STATUS "UWP public")
            list(APPEND
                Public_Source_Files
                ${PATH_TO_XSAPI_ROOT}/include/xsapi-cpp/title_callable_ui.h
                )
        endif()
    endif()
endif()

if(NOT ANDROID )
    list(APPEND
        Common_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Common/cpp/pch.cpp
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Common/cpp/pch.h
        )
endif()

if( UNITTEST OR PCWIN32)
    list(APPEND
        System_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/System/platform_api.cpp
        )
endif()

if( UNITTEST OR PCWIN32 )
    list(APPEND
        System_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/System/Win32/local_storage_win32.cpp
        )
endif()


if( UWP OR PCWIN32 OR GDK OR UNITTEST )
    list(APPEND
        Shared_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Shared/WinRT/local_config_winrt.cpp
        )
endif()

if( UWP )
    list(APPEND
        Shared_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Shared/local_config.h
        ${PATH_TO_XSAPI_ROOT}/Source/Shared/local_config.cpp
        )
endif()


set(TCUI_Source_Files
    )

if ( UWP )
    list(APPEND
        TCUI_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Services/TCUI/UWP/title_callable_ui_uwp.cpp
        )
endif()

set(Notification_Source_Files
    )

if ( (NOT XDK) AND (NOT UNITTEST) AND (NOT ANDROID) )
    list(APPEND
        Notification_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Notification/notification_internal.h
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Notification/notification_service.cpp
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Notification/notification_api.cpp
        )
endif()

if ( UWP )
    list(APPEND
        Notification_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Notification/UWP/notification_service_uwp.cpp
        )
endif()

if ( PCWIN32 )
    list(APPEND
        Notification_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Notification/RTA/notification_service_rta.cpp
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Notification/RTA/notification_subscription.h
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Notification/RTA/notification_subscription.cpp
        )
endif()


if( (NOT UNITTEST) AND (NOT XDK) AND (NOT ANDROID))
    set(Events_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/events_service.h
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/events_service_api.cpp
        )
endif()

if ( GDK )
    list(APPEND
        Events_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/events_service_gdk.h
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/events_service_gdk.cpp
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/events_service_etw.h
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/events_service_etw.cpp
        )
elseif ( UWP )
# UWP and GDK still using ETW based events service
    list(APPEND
        Events_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/events_service_etw.h
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/events_service_etw.cpp
        )
elseif ( PCWIN32)
    list(APPEND
        Events_Source_Files
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/event.cpp
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/event_queue.cpp
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/event_upload_payload.cpp
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/events_service_xsapi.h
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/events_service_xsapi.cpp
        ${PATH_TO_XSAPI_ROOT}/Source/Services/Events/Windows/events_service_windows.cpp
        )
endif()

set(Ship_Source_Files
    ${PATH_TO_XSAPI_ROOT}/Source/Services/Common/cpp/pch.cpp
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
        ${TitleStorage_Source_Files}
        ${MultiplayerActivity_Source_Files}
        ${RealTimeActivityManager_Source_Files}
        )


    if (NOT ANDROID)
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
    else()
        # if Android
        include("GetAndroidXSAPISourceFiles.cmake")
        
        get_android_xsapi_source_files(
            Common_Android_Source_Files
            System_Android_Source_Files
            Shared_Android_Source_Files
            TCUI_Android_Source_Files
            Notification_Android_Source_Files
            Events_Android_Source_Files
            ${PATH_TO_XSAPI_ROOT}
        )
        
        source_group("C++ Source\\System" FILES ${System_Android_Source_Files})
        source_group("C++ Source\\Shared" FILES ${Shared_Android_Source_Files})
        source_group("C++ Source\\Services\\TCUI" FILES ${TCUI_Android_Source_Files})
        source_group("C++ Source\\Services\\Notification" FILES ${Notification_Android_Source_Files})
        source_group("C++ Source\\Services\\Common" FILES ${Common_Android_Source_Files})
        source_group("C++ Source\\Services\\Events" FILES ${Events_Android_Source_Files})
        
        list(APPEND
            SOURCE_FILES
            ${System_Android_Source_Files}
            ${Shared_Android_Source_Files}
            ${TCUI_Android_Source_Files}
            ${Notification_Android_Source_Files}
            ${Common_Android_Source_Files}
            ${Events_Android_Source_Files}
            )
    endif()
endif() # SHIP

set(UnitTests_Source_Files_Mocks
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Mocks/mock_web_socket.h
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Mocks/mock_web_socket.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Mocks/mock_user.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Mocks/xal_mocks.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Mocks/http_mock.h
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Mocks/http_mock.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Mocks/mock_rta_service.h
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Mocks/mock_rta_service.cpp
    )

set(UnitTests_Source_Files_Support
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/iso8601.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/DefineTestMacros.h
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/iso8601.h
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/UnitTestIncludes.h
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/event.h
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/event.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/unit_test_helpers.h
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/unit_test_helpers.cpp
    )

if ( TAEF )
    list(APPEND
        UnitTests_Source_Files_Support
        ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/TAEF/UnitTestBase.h
        ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/TAEF/UnitTestIncludes_TAEF.h
        ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/TAEF/UnitTestBase.cpp
        )
endif()

if ( TE )
    list(APPEND
        UnitTests_Source_Files_Support
        ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/TE/unittesthelpers_te.cpp
        ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/TE/unittesthelpers_te.h
        ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Support/TE/unittestincludes_te.h
        )
endif()
    
set(UnitTests_Source_Files_Tests
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/AchievementsTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/AchievementsManagerTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/ErrorTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/LeaderboardTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/MatchmakingTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/MultiplayerManagerTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/MultiplayerTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/PeoplehubTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/PresenceTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/PrivacyTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/ProfileTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/RealTimeActivityManagerTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/ReputationTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/SocialManagerTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/SocialTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/StatsTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/TitleManagedStatsTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/StringVerifyTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/TitleStorageTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/MultiplayerActivityTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Shared/HttpCallTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Shared/HttpCallSettingsTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Shared/LogTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Shared/XboxLiveContextTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Shared/XboxLiveCallbackTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Shared/GlobalTests.cpp
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Shared/PlatformTests.cpp
    )

set(TestJson_Files
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/TestResponses/Multiplayer.json
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/TestResponses/Matchmaking.json
    ${PATH_TO_XSAPI_ROOT}/Tests/UnitTests/Tests/Services/TestResponses/MultiplayerManager.json
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

cmake_minimum_required (VERSION 3.6)

project("Microsoft.Xbox.Services.Android")

get_filename_component(PATH_TO_XSAPI_ROOT "../../.." ABSOLUTE)

set(PATH_TO_XAL_ROOT "${PATH_TO_XSAPI_ROOT}/External/xal")
set(PATH_TO_XAL "${PATH_TO_XAL_ROOT}/Source/Xal")

set(CMAKE_STATIC_LIBRARY_PREFIX "")

#######################################
### Set up source and include files ###
#######################################

include("../GetCommonXSAPISourceFiles.cmake")
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
    "${PATH_TO_XSAPI_ROOT}"
    )

set(SOURCE_FILES
    "${Public_Source_Files}"
    "${Common_Source_Files}"
    "${Public_Source_Files_C}"
    "${System_Source_Files}"
    "${Shared_Logger_Source_Files}"
    "${Shared_Source_Files}"
    "${Achievements_Source_Files}"
    "${Achievements_Manager_Source_Files}"
    "${Leaderboard_Source_Files}"
    "${Privacy_Source_Files}"
    "${Presence_Source_Files}"
    "${TitleStorage_Source_Files}"
    "${Social_Source_Files}"
    "${Social_Manager_Source_Files}"
    "${Stats_Source_Files}"
    "${Matchmaking_Source_Files}"
    "${Multiplayer_Source_Files}"
    "${Multiplayer_Manager_Source_Files}"
    "${StringVerify_Source_Files}"
    "${MultiplayerActivity_Source_Files}"
    "${RealTimeActivityManager_Source_Files}"
    )
    
include("../GetAndroidXSAPISourceFiles.cmake")
get_android_xsapi_source_files(
    Common_Android_Source_Files
    System_Android_Source_Files
    Shared_Android_Source_Files
    TCUI_Android_Source_Files
    Notification_Android_Source_Files
    Events_Android_Source_Files
    "${PATH_TO_XSAPI_ROOT}"
)

set(ANDROID_SOURCE_FILES
    "${Common_Android_Source_Files}"
    "${System_Android_Source_Files}"
    "${Shared_Android_Source_Files}"
    "${TCUI_Android_Source_Files}"
    "${Notification_Android_Source_Files}"
    "${Events_Android_Source_Files}"
    )
    
 set(COMMON_INCLUDE_DIRS
    "${PATH_TO_XSAPI_ROOT}/Include"
    "${PATH_TO_XSAPI_ROOT}/Include/xsapi-c"
    "${PATH_TO_XSAPI_ROOT}/Include/xsapi-cpp"
    "${PATH_TO_XSAPI_ROOT}/Include/xsapi-cpp/impl"
    "${PATH_TO_XSAPI_ROOT}/Include/cpprestinclude"
    "${PATH_TO_XSAPI_ROOT}/Source/Services"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Achievements"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Common"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Common/Unix"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Events"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Leaderboard"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Matchmaking"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Multiplayer"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Multiplayer/Manager"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/MultiplayerActivity"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Notification"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/RealTimeActivityManager"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Presence"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Privacy"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Social"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Social/Manager"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/Stats"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/StringVerify"
    "${PATH_TO_XSAPI_ROOT}/Source/System"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/TitleStorage"
    "${PATH_TO_XSAPI_ROOT}/Source/Services/TCUI"
    "${PATH_TO_XSAPI_ROOT}/Source/Shared"
    "${PATH_TO_XSAPI_ROOT}/Source/Shared/u"
    "${PATH_TO_XAL}/Include"
    "${PATH_TO_XSAPI_ROOT}/External/rapidjson/include"
    "${PATH_TO_XAL_ROOT}/External/libHttpClient/Include"
    "${PATH_TO_XAL_ROOT}/External/libHttpClient/Include/httpClient"
    "${PATH_TO_XAL_ROOT}/External/libHttpClient/External/asio/asio/include"
    "${PATH_TO_XAL_ROOT}/External/libHttpClient/External/asio/asio/include/asio"
    "${PATH_TO_XAL_ROOT}/External/libHttpClient/External/asio/asio/include/asio/detail"
    "${PATH_TO_XAL_ROOT}/External/libHttpClient/External/asio/asio/include/asio/impl"
    "${PATH_TO_XAL_ROOT}/External/libHttpClient/External/websocketpp/websocketpp"
    "${PATH_TO_XAL_ROOT}/External/libHttpClient/External/websocketpp/websocketpp/config"
    "${PATH_TO_XAL_ROOT}/External/libHttpClient/External/websocketpp/websocketpp/transport"
    "${PATH_TO_XAL_ROOT}/External/CompactCoreCLL"
    )

#########################
### Set up static lib ###
#########################

add_library(
    "${PROJECT_NAME}"
    STATIC
    "${SOURCE_FILES}"
    "${ANDROID_SOURCE_FILES}"
)

target_include_directories(
    "${PROJECT_NAME}"
    PRIVATE
    "${COMMON_INCLUDE_DIRS}"
)

#########################
### Set up flags ###
#########################
include("GetXSAPIFlags.cmake")
get_xsapi_android_flags(FLAGS FLAGS_DEBUG FLAGS_RELEASE)

include("${PATH_TO_XAL_ROOT}/External/libHttpClient/Utilities/CMake/Android/TargetSetFlags.cmake")
target_set_flags(
    "${PROJECT_NAME}"
    "${FLAGS}"
    "${FLAGS_DEBUG}"
    "${FLAGS_RELEASE}"
)
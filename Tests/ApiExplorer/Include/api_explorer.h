// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#pragma once
#if HC_PLATFORM == HC_PLATFORM_ANDROID
#include "multidevice.h"
#endif

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// C++ is not currently supported in IOS Framework. In the future, this should be updated to allow C++ tests in iOS API Explorer
#define IOS_CPP_ENABLED 0
#define CPP_TESTS_ENABLED (HC_PLATFORM != HC_PLATFORM_IOS || IOS_CPP_ENABLED)

struct ApiCommand;
typedef void (*CommandCallbackType)(const std::vector<std::string>& cmdLineTokens);
typedef HRESULT (*ApiCallbackType)(ApiCommand*);

struct Command
{
    char name[256];
    CommandCallbackType cmdCallback;
    bool hideFromHelp;
};

struct SubCommand
{
    char name[256];
    char subname[256];
    CommandCallbackType cmdCallback;
    bool hideFromHelp;
};

struct ApiExplorerData
{
    ~ApiExplorerData()
    {
        if (L != nullptr)
        {
            lua_close(L); // cleanup Lua
        }

        if (queue)
        {
            XTaskQueueCloseHandle(queue);
        }
    }

    std::recursive_mutex m_luaLock;
    lua_State* L{ nullptr }; // the Lua interpreter 
    bool m_quit{ false };
    bool m_socialDoWorkDone{ true };
    bool m_mpmDoWorkDone{ true };
    bool m_achievementsDoWorkDone{ true };

    std::vector<Command> m_commands;
    std::vector<std::string> m_cmdLineLog;
    std::vector<std::string> m_tutorial;
    std::string m_tutorialWaitFor;
    std::vector<HRESULT> m_ignoreHRs;
    std::string m_testsPath;
    HRESULT m_testHR{ S_OK };
    bool m_stopTest{ false };
    bool m_callUpdate{ false };
    HRESULT m_lastError{ S_OK };
    bool m_tutorialWait{ false };
    bool m_runningTests{ false };
    int m_onlyFileNumber{ 0 };
    int m_minFileNumber{ 0 };
    int m_maxFileNumber{ 0 };
    bool m_checkHR{ true };
    HRESULT m_runTestsHR{ S_OK };
    bool m_isXalInitialized{ false };
    std::string m_onXalTryAddFirstUserSilentlyAsync;
    std::string m_onTaskQueueTerminateWithAsyncWait;
    bool m_repeatJsonCmds{ false };
    bool m_wasTestSkipped{ false };
    bool m_trackUnhookedMemory{ false };

    std::mutex m_catMessageLock;
    std::string m_catMessage;

    // ApiRunnerState
    XalUserHandle xalUser{ nullptr };
    bool gotXalUser{ false };
    XTaskQueueHandle queue{ nullptr };
    XblContextHandle xboxLiveContext{ nullptr };
    std::vector<XblRealTimeActivitySubscriptionHandle> subscriptionHandleDeviceList;
    std::vector<XblRealTimeActivitySubscriptionHandle> subscriptionHandleTitleList;
    std::string gamertag;
    uint64_t xboxUserId{ 0 };
    uint32_t titleId{ 0 };
    const char* scid{ nullptr };
    XblFunctionContext fnAddServiceCallRoutedHandler{ 0 };
    HCMockCallHandle nsalMockCall{ nullptr };
    bool libHttpClientInit{ false };

    // Android Specific Data
#if HC_PLATFORM == HC_PLATFORM_ANDROID
    JavaVM *javaVM;
    jobject applicationContext;
    HCInitArgs initArgs;

    /// <summary>The Java Application Context.</summary>
    jclass m_mainActivityClass;
    jobject m_mainActivityClassInstance;
    jmethodID m_getApplicationContext;
#endif

    // Achievements Data
    XblAchievementsResultHandle achievementsResult{ nullptr };
#if CPP_TESTS_ENABLED
    xbox::services::achievements::achievements_result achievementsResultCpp{};
#endif

    // libHttpClient Data
    HCCallHandle httpCall{ nullptr };
    HCMockCallHandle mockHttpCall{ nullptr };
    HCWebsocketHandle websocket{ nullptr };

    // XblHttpCall Data
    XblHttpCallHandle xblHttpCall{ nullptr };

    // Leaderboard Data
    std::vector<char> leaderboardBuffer;
    XblLeaderboardResult* leaderboardResult{ nullptr };
#if  CPP_TESTS_ENABLED
    xbox::services::leaderboard::leaderboard_result leaderboardResultCpp{};
#endif

    //Social Data
#if CPP_TESTS_ENABLED
    xbox::services::social::xbox_social_relationship_result socialRelationshipResult;
    std::shared_ptr<xbox::services::social::social_relationship_change_subscription> socialRelationshipChangeSubscription;
    function_context socialRelationshipChangedHandlerContext;
#endif

    // Matchmaking Data
    XblCreateMatchTicketResponse* matchTicketResponse;
#if CPP_TESTS_ENABLED
    xbox::services::matchmaking::create_match_ticket_response matchTicketResponseCpp;
#endif

    // User Statistic Data
    XblRealTimeActivitySubscriptionHandle statisticChangeSubscriptionHandle{ nullptr };
    XblFunctionContext statisticChangedFunctionContext{ 0 };
    long long lastUserStat = 0;
#if CPP_TESTS_ENABLED
    std::shared_ptr<xbox::services::user_statistics::statistic_change_subscription> statisticChangeSubscriptionCpp{ nullptr };
    function_context statisticChangedFunctionContextCpp{ nullptr };
#endif

    // Title Storage Data
    XblTitleStorageBlobMetadataResultHandle blobMetadataResultHandle{ nullptr };
    XblTitleStorageBlobMetadata binaryBlobMetadata{};
    XblTitleStorageBlobMetadata jsonBlobMetadata{};
#if CPP_TESTS_ENABLED
    xbox::services::title_storage::title_storage_blob_metadata_result blobMetadataResultCpp{};
    xbox::services::title_storage::title_storage_blob_metadata blobMetadataCpp{};
#endif

    // Title Storage Rest API Calls Data
    std::vector<std::string> blobResponseStrings;
    std::string metadataResponseString;
    std::vector<XblHttpCallHandle> titleStorageHttpCalls;
    size_t titleStorageCompletedHttpCalls = 0;
    size_t filesToDownload = 0;

    // MP
    std::string inviteHandle;

#if HC_PLATFORM == HC_PLATFORM_IOS
    std::string apnsToken;
#endif

#if HC_PLATFORM == HC_PLATFORM_UWP
    Windows::UI::Core::CoreDispatcher^ m_dispatcher;
#endif

#if HC_PLATFORM == HC_PLATFORM_WIN32
    XblRealTimeActivitySubscriptionHandle gameInviteNotificationSubscriptionHandle{ nullptr };
    XblFunctionContext gameInviteNotificationFunctionContext{ 0 };
#endif

    std::shared_ptr<ApiRunnerMultiDeviceManager> m_multiDeviceManager;
    XblFunctionContext m_achievementProgressNotificationFunctionContext{ 0 };
    
};

enum TypeType
{
    TypeType_None,
    TypeType_Command,
    TypeType_Event,
    TypeType_API
};

std::vector<std::string> TokenizeString(const std::string& input);
void LogCat(bool logToFile, _Printf_format_string_ char const* format, ...);
ApiExplorerData* Data();

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Renderer.h"
#include "Utils\Input.h"
#include "GameLogic\GameData.h"

#include <iostream>
#include <fstream>

using namespace std;

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
enum ButtonPress
{
    SignIn,
    ToggleSocialGroup1,
    ToggleSocialGroup2,
    ToggleSocialGroup3,
    ToggleSocialGroup4,
    ToggleSocialGroup5,
    ImportCustomList,
    GetUserProfile,
    GetFriends,
    GetAchievementsForTitle,
    StartTests,
    StartTestsFast
};

#define NUM_OF_TEST_AREAS 4
enum TestArea
{
    Achievements,
    Profile,
    Social,
    SocialManger
};

#define PERF_COUNTERS 0    // Enable this for capturing performance counters

namespace LongHaulTestApp
{
    class Game : public DX::IDeviceNotify, public std::enable_shared_from_this<Game>
    {
    public:
        Game() {}
        Game(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~Game();
        void CreateWindowSizeDependentResources();
        void Update();
        bool Render();
        void Init(Windows::UI::Core::CoreWindow^ window);

        static void HandleSignout(_In_ xbl_user_handle user);

        void OnProtocolActivation(Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ args);

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

        void OnMainMenu();
        void OnGameUpdate();

        void SignIn();
        void SignInSilently();

        void HandleSignInResult(_In_ XblSignInResult signInResult);

        void RegisterInputKeys();

        std::shared_ptr<GameData> GetGameData() { return m_gameData; }

        /// UI elements
        std::vector< std::wstring > m_displayEventQueue;
        size_t m_previousDisplayQueueSize;
        static std::mutex m_displayEventQueueLock;

        void Log(std::wstring log, bool showOnUI = true);
        void Log(std::string log, bool showOnUI = true);

        xbl_user_handle GetUser() { return m_user; }
        string_t GetGamertag()
        {
            char gamertag[GamertagMaxBytes];
            XblUserGetGamertag(m_user, GamertagMaxBytes, gamertag, nullptr);
            return utility::conversions::utf8_to_utf16(gamertag);
        }

        Windows::System::Diagnostics::ProcessMemoryUsageReport^ m_initMemReport;
        Windows::System::Diagnostics::ProcessMemoryUsageReport^ m_curMemReport;
        unsigned long long m_curDeltaMem;
        unsigned long long m_lastDeltaMem;
        uint32_t m_testDelay;
        uint64_t m_testsRun;

    private:
        xbl_user_handle m_user;
        uint64_t m_xuid;
        xbl_context_handle m_xboxLiveContext;
        async_queue_handle_t m_queue;
        uint32_t m_callbackToken;
        const XblAppConfig* m_config;

        function_context m_signOutContext;

        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::unique_ptr<Renderer> m_sceneRenderer;
        DX::StepTimer m_timer;
        bool bInitialized;

        Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ m_protocolActivatedEventArgs;
        Windows::Gaming::Input::IGamepad^ m_lastGamepadInputUsed;
        std::shared_ptr<GameData> m_gameData;
        Input^ m_input;
        bool m_gotInitMemReport;

        Concurrency::critical_section m_stateLock;

        HANDLE m_hBackgroundThread;

        bool m_testsStarted;

        ///////////////////////////////////////
        //////       Test Framework      //////
        ///////////////////////////////////////
        
        // Methods
        void InitializeTestFramework();
        void InitializeTests();

        void HandleTests();
        void BeginTest();
        void EndTest();

        void PrintMemoryUsage();

        string TaceLevelToString(xbox::services::xbox_services_diagnostics_trace_level traceLevel);

        // Vars
        TestArea m_testArea;
        bool m_testing;
        time_t m_time;

        ofstream m_logFile;
        std::string m_logFileName;

        ///////////////////////////////////////
        //////        Achievements       //////
        ///////////////////////////////////////

        // Tests
        void TestAchievementsFlow();

        // Utils
        void GetAchievmentsForTitle();
        void AchievementResultsGetNext(xbl_achievements_result_handle result);
        void GetAchievement(PCSTR scid, PCSTR achievementId);
        void UpdateAchievement(PCSTR scid, PCSTR achievementId);

        // Vars
        uint32_t m_progress;

        ///////////////////////////////////////
        //////           Profile         //////
        ///////////////////////////////////////

        // Tests
        void TestProfileFlow();

        // Utils
        void TestGetUserProfile();
        void TestGetUserProfiles();
        void TestGetUserProfilesForSocialGroup();

        // Vars
        uint32_t m_test;

        ///////////////////////////////////////
        //////           Soclal          //////
        ///////////////////////////////////////

        // Tests
        void TestSocialFlow();

        // Utils
        void GetSocialRelationship();
        void SocialRelationshipGetNext(xbl_social_relationship_result_handle relationshipResult);
        void TestResputationFeedback();
        
        ///////////////////////////////////////
        //////       Soclal Manager      //////
        ///////////////////////////////////////

        // Tests
        void TestSocialManagerFlow();

        // Test Utils
        void AddLocalUserToSocialManager();
        void RemoveLocalUserFromSocialManager();
        void CreateSocialUserGroup();
        void TestSocialUserGroup();

        // Manager Utils
        void SocialManagerIntegrationUpdate();
        void NextSocialManagerTestConfig();

        void WaitForSocialEvent(XblSocialManagerEventType eventType, std::function<void(XblSocialManagerEvent)> callback);

        void PrintSocialManagerDoWork(XblSocialManagerEvent* events, uint32_t size);
        string GroupTypeToString();
        string SocialEventTypeToString(XblSocialManagerEventType eventType);
        string PresenceFilterToString();
        string RelationshipFilterToString();

        // Vars
        XblSocialUserGroupType m_testGroupType;

        XblPresenceFilter m_presenceFilter = XblPresenceFilter_TitleOnline;
        XblRelationshipFilter m_relationshipFilter;
        XblSocialManagerUserGroup* m_socialUserGroup;

        bool m_waitingForEvent;
        XblSocialManagerEventType m_eventType;
        std::function<void(XblSocialManagerEvent)> m_eventTypeCallback;
    };
}

extern LongHaulTestApp::Game* g_sampleInstance;



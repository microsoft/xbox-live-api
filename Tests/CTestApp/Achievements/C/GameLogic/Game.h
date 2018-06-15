// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Renderer.h"
#include "Utils\Input.h"
#include "GameLogic\GameData.h"

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
enum ButtonPress
{
    SignIn,
    GetAchievementsForTitle,
    AchievementResultsGetNext,
    GetAchievement,
    UpdateAchievement
};

#define PERF_COUNTERS 0    // Enable this for capturing performance counters

namespace Sample
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


        void OnProtocolActivation(Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ args);

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

        void OnMainMenu();
        void OnGameUpdate();

        // SignInIntegration.cpp
        void SignIn();
        void SignInSilently();
        void HandleSignInResult(_In_ XblSignInResult signInResult);
        static void HandleSignout(_In_ xbl_user_handle user);
        function_context m_signOutContext;

        void RegisterInputKeys();

        std::shared_ptr<GameData> GetGameData() { return m_gameData; }

        /// UI elements
        std::vector< std::wstring > m_displayEventQueue;
        size_t m_previousDisplayQueueSize;
        static std::mutex m_displayEventQueueLock;

        void Log(std::wstring log);
        void Log(std::string log);
        
        xbl_user_handle GetUser() { return m_user; }
        string_t GetGamertag()
        {
            char gamertag[GamertagMaxBytes];
            XblUserGetGamertag(m_user, GamertagMaxBytes, gamertag, nullptr);
            return utility::conversions::utf8_to_utf16(gamertag);
        }

        bool IsAchievementsResultSet() { return m_achievementsResultSet; }
        xbl_achievements_result_handle GetAchievementsResultHandle() { return m_achievementsResult; }

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::unique_ptr<Renderer> m_sceneRenderer;
        DX::StepTimer m_timer;
        bool bInitialized;

        Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ m_protocolActivatedEventArgs;
        Windows::Gaming::Input::IGamepad^ m_lastGamepadInputUsed;
        std::shared_ptr<GameData> m_gameData;
        Input^ m_input;

        Concurrency::critical_section m_stateLock;

    private:
        xbl_user_handle m_user;
        async_queue_handle_t m_queue;
        uint32_t m_asyncQueueCallbackToken;
        HANDLE m_hBackgroundThread;

        xbl_context_handle m_xboxLiveContext;
        uint64_t m_xuid;
        const XblAppConfig* m_config;

        void InitializeXboxLive();
        void CleanupXboxLive();

    public:
        xbl_achievements_result_handle m_achievementsResult;
        bool m_achievementsResultSet;
        void SetAchievementsResult(
            _In_ xbl_achievements_result_handle achievementsResult
        );

        void GetAchievementsForTitle(
            _In_ uint32_t skipItems,
            _In_ uint32_t maxItems
        );

        void AchievementResultsGetNext(
            _In_ xbl_achievements_result_handle resultHandle,
            _In_ uint32_t maxItems
        );

        void GetAchievement(
            _In_ const char* achievementId
        );

        void UpdateAchievement(
            _In_ const char* achievementId,
            _In_ uint32_t percentComplete
        );
    };
}

extern Sample::Game* g_sampleInstance;



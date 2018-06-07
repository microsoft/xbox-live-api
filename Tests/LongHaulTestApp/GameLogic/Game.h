// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "Common\StepTimer.h"
#include "Common\DeviceResources.h"
#include "Renderer.h"
#include "Utils\Input.h"
#include "GameLogic\GameData.h"
#include "Tests.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace pplx;

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
enum ButtonPress
{
    SignIn,
    StartTests,
    StartTestsFast
};

#define BACKGROUND_THREADS 4
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

        ofstream m_logFile;
        std::string m_logFileName;
        void Log(std::wstring log, bool showOnUI = true);
        void Log(std::string log, bool showOnUI = true);
        string TaceLevelToString(xbox::services::xbox_services_diagnostics_trace_level traceLevel);
        void PrintMemoryUsage();

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

        xbl_user_handle m_user;
        uint64_t m_xuid;
        xbl_context_handle m_xboxLiveContext;
        async_queue_handle_t m_queue;
        uint32_t m_asyncQueueCallbackToken;
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

        TestsManager m_testsManager;
    };
}

extern LongHaulTestApp::Game* g_sampleInstance;



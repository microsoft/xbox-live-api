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
    ToggleSocialGroup1,
    ToggleSocialGroup2,
    ToggleSocialGroup3,
    ToggleSocialGroup4,
    ToggleSocialGroup5,
    ImportCustomList,
    GetUserProfile,
    GetFriends
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

        static void HandleSignout(_In_ XBL_XBOX_LIVE_USER *user);

        void OnProtocolActivation(Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ args);

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

        void OnMainMenu();
        void OnGameUpdate();

        void SignIn();
        void SignInSilently();

        static void HandleSignInResult(
            _In_ XBL_RESULT result,
            _In_ XSAPI_SIGN_IN_RESULT payload,
            _In_opt_ void* context);

        void RegisterInputKeys();

        std::shared_ptr<GameData> GetGameData() { return m_gameData; }

        /// UI elements
        std::vector< std::wstring > m_displayEventQueue;
        size_t m_previousDisplayQueueSize;
        static std::mutex m_displayEventQueueLock;

        void Log(std::wstring log);

        XBL_XBOX_LIVE_USER *GetUser() { return m_user; }
    private:
        XBL_XBOX_LIVE_USER *m_user;
        XBL_XBOX_LIVE_CONTEXT_HANDLE m_xboxLiveContext;

        function_context m_signOutContext;

        std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::unique_ptr<Renderer> m_sceneRenderer;
        DX::StepTimer m_timer;
        bool bInitialized;

        Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ m_protocolActivatedEventArgs;
        Windows::Gaming::Input::IGamepad^ m_lastGamepadInputUsed;
        std::shared_ptr<GameData> m_gameData;
        Input^ m_input;

        Concurrency::critical_section m_stateLock;

        void ReadLastCsv();
        void ReadCsvFile(_In_ Windows::Storage::StorageFile^ file);

        void GetUserProfile();
        void GetSocialRelationships();

        HANDLE m_hBackgroundThread;
    };
}

extern Sample::Game* g_sampleInstance;



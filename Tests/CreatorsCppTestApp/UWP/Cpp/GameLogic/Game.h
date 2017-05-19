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

        void HandleSignout(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user);

        void OnProtocolActivation(Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ args);

        // IDeviceNotify
        virtual void OnDeviceLost();
        virtual void OnDeviceRestored();

        void OnMainMenu();
        void OnGameUpdate();

        void SignIn();
        void SignInSilently();

        void HandleSignInResult(_In_ const xbox::services::system::sign_in_result &result);

        void RegisterInputKeys();

        std::shared_ptr<GameData> GetGameData() { return m_gameData; }
        std::shared_ptr<xbox::services::social::manager::social_manager> GetSocialManager() { return m_socialManager;  }

        /// UI elements
        std::vector< std::wstring > m_displayEventQueue;
        size_t m_previousDisplayQueueSize;
        static std::mutex m_displayEventQueueLock;

        void Log(string_t log);

        int GetNumberOfUserInGraph() { return (m_user == nullptr) ? 0 : 1; }
        bool GetAllFriends() { return m_allFriends; }
        bool GetOnlineFriends() { return m_onlineFriends; }
        bool GetAllFavs() { return m_allFavs; }
        bool GetOnlineInTitle() { return m_onlineInTitle; }
        bool GetCustomList() { return m_customList; }

        std::shared_ptr<xbox::services::system::xbox_live_user> GetUser() { return m_user; }
        std::vector<std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>> GetSocialGroups();

        static std::mutex m_socialManagerLock;
    private:
        std::shared_ptr<xbox::services::xbox_live_context> m_xboxLiveContext;
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
        std::unique_ptr<Renderer> m_sceneRenderer;
        DX::StepTimer m_timer;
        bool bInitialized;

        bool m_allFriends;
        bool m_onlineFriends;
        bool m_allFavs;
        bool m_onlineInTitle;
        bool m_customList;
        std::vector<string_t> m_xuidList;

        std::shared_ptr<xbox::services::social::manager::social_manager> m_socialManager;
        std::vector<std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>> m_socialGroups;

        Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ m_protocolActivatedEventArgs;
        Windows::Gaming::Input::IGamepad^ m_lastGamepadInputUsed;
        std::shared_ptr<GameData> m_gameData;
        Input^ m_input;

        Concurrency::critical_section m_stateLock;
        std::shared_ptr<xbox::services::system::xbox_live_user> m_user;
        function_context m_signOutContext;

        void ReadLastCsv();
        void ReadCsvFile(_In_ Windows::Storage::StorageFile^ file);
        void UpdateCustomList(_In_ const std::vector<string_t>& xuidList);

        void InitializeSocialManager();
        void AddUserToSocialManager(
            _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user
            );
        void RemoveUserFromSocialManager(
            _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user
            );
        void CreateOrUpdateSocialGroupFromList(
            _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
            _In_ std::vector<string_t> xuidList
            );
        void DestorySocialGroupFromList(
            _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user
            );
        void CreateSocialGroupFromFilters(
            _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
            _In_ xbox::services::social::manager::presence_filter presenceDetailLevel,
            _In_ xbox::services::social::manager::relationship_filter filter
            );
        void DestroySocialGroup(
            _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
            _In_ xbox::services::social::manager::presence_filter presenceDetailLevel,
            _In_ xbox::services::social::manager::relationship_filter filter
            );
        void UpdateSocialManager();
        void LogSocialEventList(std::vector<xbox::services::social::manager::social_event> eventList);
        void CreateSocialGroupsBasedOnUI(
            _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user
            );

        void UpdateSocialGroupForAllUsers(
            _In_ bool toggle,
            _In_ xbox::services::social::manager::presence_filter presenceFilter,
            _In_ xbox::services::social::manager::relationship_filter relationshipFilter
            );
        void UpdateSocialGroup(
            _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
            _In_ bool toggle,
            _In_ xbox::services::social::manager::presence_filter presenceFilter,
            _In_ xbox::services::social::manager::relationship_filter relationshipFilter
            );

        void UpdateSocialGroupOfListForAllUsers(
            _In_ bool toggle
            );
        void UpdateSocialGroupOfList(
            _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
            _In_ bool toggle
            );

    };
}

extern Sample::Game* g_sampleInstance;



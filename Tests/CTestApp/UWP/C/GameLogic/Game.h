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
    GetFriends,
    GetAchievementsForTitle
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

        void Log(std::wstring log);
        void Log(std::string log);

        int GetNumberOfUserInGraph() { return (m_xboxLiveContext == nullptr) ? 0 : 1; }
        bool GetAllFriends() { return m_allFriends; }
        bool GetOnlineFriends() { return m_onlineFriends; }
        bool GetAllFavs() { return m_allFavs; }
        bool GetOnlineInTitle() { return m_onlineInTitle; }
        bool GetCustomList() { return m_customList; }

        xbl_user_handle GetUser() { return m_user; }
        string_t GetGamertag()
        {
            char gamertag[GamertagMaxBytes];
            XblUserGetGamertag(m_user, GamertagMaxBytes, gamertag, nullptr);
            return utility::conversions::utf8_to_utf16(gamertag);
        }
        std::vector<XblSocialManagerUserGroup*> GetSocialGroups();

    private:
        xbl_user_handle m_user;
        uint64_t m_xuid;
        xbl_context_handle m_xboxLiveContext;
        async_queue_handle_t m_queue;
        registration_token_t m_asyncQueueCallbackToken;
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

        Concurrency::critical_section m_stateLock;

        void ReadLastCsv();
        void ReadCsvFile(_In_ Windows::Storage::StorageFile^ file);
        void UpdateCustomList(_In_ const std::vector<uint64_t>& xuidList);

        // Social Tests
        void GetUserProfile();
        void GetSocialRelationships();

        bool m_allFriends;
        bool m_onlineFriends;
        bool m_allFavs;
        bool m_onlineInTitle;
        bool m_customList;
        std::vector<uint64_t> m_xuidList;

        std::vector<XblSocialManagerUserGroup*> m_socialGroups;

        // SocialManagerIntegration.cpp
        void AddUserToSocialManager(xbl_user_handle user);
        void RemoveUserFromSocialManager(xbl_user_handle user);
        void CreateOrUpdateSocialGroupFromList(
            xbl_user_handle user,
            std::vector<uint64_t> xuidList
        );
        void DestroySocialGroup(xbl_user_handle user);
        void CreateSocialUserGroupFromFilters(
            xbl_user_handle user,
            XblPresenceFilter presenceFilter,
            XblRelationshipFilter relationshipFilter
        );
        void DestroySocialGroup(
            xbl_user_handle user,
            XblPresenceFilter presenceFilter,
            XblRelationshipFilter relationshipFilter
        );
        void UpdateSocialManager();

        void LogSocialEventList(
            XblSocialManagerEvent* eventList,
            uint32_t eventListCount
        );

        void CreateSocialGroupsBasedOnUI(xbl_user_handle user);

        void UpdateSocialGroupForAllUsers(
            bool toggle,
            XblPresenceFilter presenceFilter,
            XblRelationshipFilter relationshipFilter
        );

        void UpdateSocialGroup(
            xbl_user_handle user,
            bool toggle,
            XblPresenceFilter presenceFilter,
            XblRelationshipFilter relationshipFilter
        );

        void UpdateSocialGroupOfListForAllUsers(bool toggle);

        void UpdateSocialGroupOfList(
            xbl_user_handle user,
            bool toggle
        );

        // Achievement Tests
        void GetAchievmentsForTitle();
        void AchievementResultsGetNext(xbl_achievements_result_handle resultHandle);
        void GetAchievement(std::string scid, std::string achievementId);
        void UpdateAchievement(std::string scid, std::string achievementId);
    };
}

extern Sample::Game* g_sampleInstance;



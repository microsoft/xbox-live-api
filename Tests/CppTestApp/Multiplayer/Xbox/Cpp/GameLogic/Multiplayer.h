// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "SampleGUI.h"
#include "..\LiveResources.h"
#include "..\Utils\SessionListView.h"

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#define XPRIVILEGE_MULTIPLAYER_SESSIONS     254
#define TITLE_ID                            0x169056CF
#define GAME_SERVICE_CONFIG_ID              L"097d0100-e05c-4d37-8420-46f1169056cf"
#define GAME_SESSION_TEMPLATE_NAME          L"SessionBrowseTemplate"
#define LOBBY_TEMPLATE_NAME                 L"LobbySession"
#define OWNER_MANAGED_LFG_TEMPLATE_NAME     L"OwnerManagedLFGTest"
#define NON_OWNER_MANAGED_LFG_TEMPLATE_NAME L"NonOwnerManagedLFGTest"

// Enable this for capturing performance counters
#define PERF_COUNTERS                       0

enum APPSTATE
{
    APP_MAIN_MENU,
    APP_CREATE_LOBBY,
    APP_CREATE_LFG_LOBBY,
    APP_JOIN_LOBBY,
    APP_JOIN_GAME,
    APP_JOINING_LOBBY,
    APP_IN_GAME,
    APP_SHOW_FRIEND_GAMES,
    APP_SHOW_SEARCH_HANDLES,
};

// list of game modes
static WCHAR* g_gameModeStrings[] =
{
    L"Deathmatch",
    L"Team Battle",
    L"Capture the Flag",
    L"Heist",
    L"Conquest",
    L"Team Assault"
};

// list of maps
static WCHAR* g_mapStrings[] =
{
    L"Kunar Base",
    L"Leyte Gulf",
    L"Ardennes",
    L"Normandy",
    L"Helmand Valley"
};

// A basic sample implementation that creates a D3D11 device and
// provides a render loop.
class Sample
{
public:

    Sample();

    // Initialization and management
    void Initialize(IUnknown* window);

    // Basic game loop
    void Tick();
    void Render();

    // Rendering helpers
    void Clear();

    // Messages
    void OnSuspending();
    void OnResuming();
    void OnProtocolActivated(Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ protocolArgs);
    
private:

    // Multiplayer Manager logic
    uint64_t m_multiplayerContext;
    Concurrency::critical_section m_stateLock;
    std::vector<xbox::services::multiplayer::manager::multiplayer_event> m_multiplayerEventQueue;
    std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_manager> m_multiplayerManager;
    Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ m_protocolActivatedEventArgs;

    // Multiplayer Manager Methods
    void InitializeMultiplayerManager(_In_ const string_t& templateName = LOBBY_TEMPLATE_NAME);
    void DoWork();
    void AddLocalUser();
    void RemoveLocalUser();
    void UpdateLocalMemberProperties();
    void UpdateLobbyProperties();
    void SetLobbyHost();
    void UpdateJoinability();
    void UpdateGameProperties();
    void SetGameHost();
    void JoinGameFromLobby();
    void JoinGame();
    void LeaveGameSession();
    void InviteFriends();
    void HandleProtocolActivation();

    // Multiplayer helper methods
    void GetActivitiesForSocialGroup();

    // Search Handle methods
    void PublishSearchHandle();
    void SetRoleInfo();
    void BrowseSearchHandles();

    void SetupUI();
    void Update(DX::StepTimer const& timer);
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>        m_deviceResources;

    // Rendering loop timer.
    uint64_t                                    m_frame;
    DX::StepTimer                               m_timer;

    // Input devices.
    std::unique_ptr<DirectX::GamePad>           m_gamePad;
    DirectX::GamePad::ButtonStateTracker        m_gamePadButtons;

    // DirectXTK objects.
    std::unique_ptr<DirectX::GraphicsMemory>    m_graphicsMemory;

    // Xbox Live objects
    std::unique_ptr<ATG::LiveResources>         m_liveResources;

    std::unique_ptr<ATG::UIManager>         m_ui;
    std::shared_ptr<DX::TextConsole>       m_eventQueueConsole;
    std::shared_ptr<DX::TextConsole>       m_errorMsgConsole;
    std::shared_ptr<DX::TextConsole>       m_perfConsole;
    ATG::IPanel*                           m_hudPanel;
    ATG::IPanel*                           m_mainMenuPanel;
    ATG::IPanel*                           m_sessionDetailsPanel;
    std::vector< std::wstring >            m_displayEventQueue;
    std::shared_ptr<SessionView>           m_lobbyView;
    std::shared_ptr<SessionView>           m_gameView;
    ATG::TextLabel*                        m_joinabilityLabel;

    // Sample utility helpers 
    APPSTATE m_appState;
    UINT m_mapIndex;
    UINT m_gameModeIndex;
    bool m_isLeavingGame;
    float m_clearErrorMsgTimer;
    bool m_isProtocolActivated;

    // Search Handle
    std::vector<xbox::services::multiplayer::multiplayer_search_handle_details> m_searchHandles;

    void ChangeAppStates();
    string_t CreateGuid();
    int32_t GetRandomizedGameModeIndex();
    int32_t GetRandomizedMapIndex();
    void RenderMultiplayerEventQueue();
    void RenderPerfCounters();
    void LogErrorFormat(const wchar_t* strMsg, ...);
    xbox::services::multiplayer::manager::joinability GetRandJoinabilityValue();
    int32_t GetActiveMemberCount(std::vector<std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_member>> members);
    void RenderMultiplayerSessionDetails(
        string_t sessionName,
        std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_member> hostMember,
        std::vector<std::shared_ptr<xbox::services::multiplayer::manager::multiplayer_member>> members,
        web::json::value properties,
        xbox::services::multiplayer::manager::multiplayer_session_type sessionType,
        std::shared_ptr<SessionView> sessionView
    );
};

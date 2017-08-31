// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "SampleGUI.h"
#include "DeviceResources.h"
#include "LiveResources.h"
#include "StepTimer.h"


// A basic sample implementation that creates a D3D11 device and
// provides a render loop.
class Sample : public DX::IDeviceNotify, public std::enable_shared_from_this<Sample>
{
public:

    Sample();

    // Initialization and management
    void Initialize(IUnknown* window, int width, int height, DXGI_MODE_ROTATION rotation);
    void HandleSignin(
        _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
        _In_ xbox::services::system::sign_in_status result
        );
    void HandleSignout(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user);

    void SetupDefaultUser(Windows::System::User^ systemUser);

    // Basic render loop
    void Tick();
    void Render();

    // Rendering helpers
    void Clear();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height, DXGI_MODE_ROTATION rotation);
    void ValidateDevice();

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    // Leaderboard Methods
    void SetStatForUser(
        _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user, 
        _In_ const string_t& name,
        _In_ int64_t value);

    void GetStatForUser(
        _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
        _In_ const string_t& statName
        );

    void PrintLeaderboard(const xbox::services::leaderboard::leaderboard_result& leaderboard);

    void SetupUI();
    void Update(DX::StepTimer const& timer);
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    void InitializeStatsManager();
    void AddUserToStatsManager(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user);
    void RemoveUserFromStatsManager(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user);
    void UpdateStatsManager();

    void GetLeaderboard(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user, _In_ const string_t& statName);
    void GetLeaderboardSkipToRank(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user, _In_ const string_t& statName);
    void GetLeaderboardSkipToSelf(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user, _In_ const string_t& statName);
    void GetLeaderboardForSocialGroup(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user, _In_ const string_t& statName, _In_ const string_t& socialGroup);

    void ProcessLeaderboards(
        _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
        _In_ xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result
        );

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

    // Input devices.
    std::unique_ptr<DirectX::GamePad>       m_gamePad;
    DirectX::GamePad::ButtonStateTracker    m_gamePadButtons[DirectX::GamePad::MAX_PLAYER_COUNT];
    std::unique_ptr<DirectX::Keyboard>      m_keyboard;
    std::unique_ptr<DirectX::Mouse>         m_mouse;

    // UI
    std::shared_ptr<ATG::UIManager>         m_ui;
    std::unique_ptr<DX::TextConsole>        m_console;

    // Xbox Live objects
    std::unique_ptr<ATG::LiveResources>     m_liveResources;
    function_context m_signInContext;
    function_context m_signOutContext;
    std::shared_ptr<xbox::services::stats::manager::stats_manager> m_statsManager;
    int m_score;

    DirectX::Keyboard::KeyboardStateTracker m_keyboardButtons;
};
//--------------------------------------------------------------------------------------
// Leaderboards.h
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "SampleGUI.h"
#include "LiveResources.h"

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

private:

    // Leaderboard Methods
    void WriteEvent();
    void GetLeaderboard();
    void GetLeaderboardSkipToRank();
    void GetLeaderboardSkipToXuid();
    void GetLeaderboardForSocialGroup();
    void GetLeaderboardForSocialGroupWithSort();
    void ProcessLeaderboards(xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result);
    void PrintLeaderboard(const xbox::services::leaderboard::leaderboard_result& leaderboard);

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
    bool m_multiColumnEnabled;
    std::vector<string_t> m_columnNames;
    std::unique_ptr<ATG::LiveResources>         m_liveResources;

    // UI Objects
    std::unique_ptr<ATG::UIManager>             m_ui;

    std::unique_ptr<DX::TextConsole>       m_console;
};

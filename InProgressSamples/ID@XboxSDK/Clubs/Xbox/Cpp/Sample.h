// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "SampleGUI.h"
#include "ClubRepeater.h"
#include "LiveResourcesXDK.h"
#include "LiveInfoHUD.h"

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

    // Messages
    void OnSuspending();
    void OnResuming();

    // SocialManager
    std::vector<std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>> GetSocialGroups();

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    void SetupUI();
    void RefreshClubList();

    // Clubs
    std::vector<xbox::services::clubs::club> m_clubs;
    xbox::services::clubs::club *m_selectedClub;

    void GetUsersClubAssociations(_In_ Windows::Xbox::System::User^ user);
    void RequerySelectedClub();
    void CreateClub();
    void InviteToClub();
    void JoinClub();
    void LeaveClub();
    void RenameClub();
    void DeleteClub();

    void PrintClubRoles(const std::vector<xbox::services::clubs::club_role>& roles);
    void PrintClub(const xbox::services::clubs::club& club);

    void ShowOrHideButton(int id, bool show);
    void HideAllButtons();
    void HandleClubSelected(unsigned index);

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
    std::shared_ptr<ATG::LiveResources>         m_liveResources;
    std::unique_ptr<ATG::LiveInfoHUD>           m_liveInfoHud;    

    // UI Objects
    std::shared_ptr<ATG::UIManager>             m_ui;
    ATG::UIConfig                               m_uiConfig;
    std::unique_ptr<DX::TextConsole>            m_console;
    std::unique_ptr<ClubRepeater>               m_clubList;
};

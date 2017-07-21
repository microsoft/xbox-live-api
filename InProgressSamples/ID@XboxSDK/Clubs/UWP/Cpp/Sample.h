// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "SampleGUI.h"
#include "ClubRepeater.h"
#include "LiveResources.h"

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

    // Basic game loop
    void Tick();

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

    void GetUsersClubAssociations(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user);
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
    std::unique_ptr<DirectX::Keyboard>          m_keyboard;
    std::unique_ptr<DirectX::Mouse>             m_mouse;

    DirectX::GamePad::ButtonStateTracker        m_gamePadButtons;
    DirectX::Keyboard::KeyboardStateTracker     m_keyboardButtons;

    // DirectXTK objects.
    //std::unique_ptr<DirectX::GraphicsMemory>    m_graphicsMemory;

    // Xbox Live objects
    std::shared_ptr<ATG::LiveResources>         m_liveResources;
    function_context m_signInContext;
    function_context m_signOutContext;

    // UI Objects
    std::shared_ptr<ATG::UIManager>             m_ui;
    ATG::UIConfig                               m_uiConfig;
    std::unique_ptr<DX::TextConsole>            m_console;   
    std::unique_ptr<ClubRepeater>               m_clubList;
};

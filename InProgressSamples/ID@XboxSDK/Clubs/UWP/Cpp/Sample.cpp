// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Sample.h"

#include "ATGColors.h"

using Microsoft::WRL::ComPtr;

using namespace DirectX;
using namespace ATG;
using namespace xbox::services::clubs;

namespace
{
    const int c_sampleUIPanel = 2000;
    const int c_debugLog = 202;
    const int c_createClub = 203;
    const int c_inviteToClub = 204;
    const int c_joinClub = 205;
    const int c_leaveClub = 206;
    const int c_deleteClub = 207;
    const int c_renameClub = 208;
    const int c_xuidLabel = 210;
}

Sample::Sample() :
    m_frame(0)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_liveResources = std::make_shared<ATG::LiveResources>();
   
    m_ui = std::make_shared<ATG::UIManager>(m_uiConfig);
}

void Sample::HandleSignin(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
    _In_ xbox::services::system::sign_in_status result
)
{
    stringstream_t ss;
    ss << "Xuid: " << user->xbox_user_id();

    m_ui->FindControl<ATG::TextLabel>(c_sampleUIPanel, c_xuidLabel)->SetText(ss.str().data());

    if (result == xbox::services::system::sign_in_status::success)
    {
        this->GetUsersClubAssociations(user);
    }
}

void Sample::HandleSignout(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user)
{
    m_console->Clear();
}

// Initialize the Direct3D resources required to run.
void Sample::Initialize(IUnknown* window, int width, int height, DXGI_MODE_ROTATION rotation)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();
    m_keyboard->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));

    m_ui->LoadLayout(L".\\Assets\\SampleUI.csv", L".\\Assets");
    m_liveResources->Initialize(m_ui, m_ui->FindPanel<ATG::Overlay>(c_sampleUIPanel));

    m_deviceResources->SetWindow(window, width, height, rotation);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    SetupUI();

    std::weak_ptr<Sample> thisWeakPtr = shared_from_this();
    m_signInContext = m_liveResources->add_signin_handler([thisWeakPtr](
        _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
        _In_ xbox::services::system::sign_in_status result
        )
    {
        std::shared_ptr<Sample> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->HandleSignin(user, result);
        }
    });

    m_signOutContext = xbox::services::system::xbox_live_user::add_sign_out_completed_handler(
        [thisWeakPtr](const xbox::services::system::sign_out_completed_event_args& args)
    {
        UNREFERENCED_PARAMETER(args);
        std::shared_ptr<Sample> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->HandleSignout(args.user());
        }
    });
}

#pragma region UI Methods
void Sample::SetupUI()
{
    // Setup buttons
    m_ui->FindControl<Button>(c_sampleUIPanel, c_createClub)->SetCallback([this](IPanel*, IControl*)
    {
        this->CreateClub();
    });

    m_ui->FindControl<Button>(c_sampleUIPanel, c_deleteClub)->SetCallback([this](IPanel*, IControl*)
    {
        this->DeleteClub();
    });

    m_ui->FindControl<Button>(c_sampleUIPanel, c_inviteToClub)->SetCallback([this](IPanel*, IControl*)
    {
        this->InviteToClub();
    });

    m_ui->FindControl<Button>(c_sampleUIPanel, c_joinClub)->SetCallback([this](IPanel*, IControl*)
    {
        this->JoinClub();
    });

    m_ui->FindControl<Button>(c_sampleUIPanel, c_renameClub)->SetCallback([this](IPanel*, IControl*)
    {
        this->RenameClub();
    });

    m_ui->FindControl<Button>(c_sampleUIPanel, c_leaveClub)->SetCallback([this](IPanel*, IControl*)
    {
        this->LeaveClub();
    });
    
    HideAllButtons();

    // Setup club list repeater
    auto loc = POINT{ 100, 200 };
    auto pos = SIZE{ 700, 40 };

    m_clubList = std::make_unique<ClubRepeater>(m_ui, loc, pos, 8000);
    m_clubList->SetSelectedCallback([this](unsigned index)
    {
        if (index < m_clubs.size())
        {
            this->HandleClubSelected(index);
        }
    });

    // Populate club list with empty items to start
    auto clubsListSource = std::vector<std::shared_ptr<ClubListItem>>();

    for (auto x = 0; x < 15; x++)
    {
        clubsListSource.push_back(std::make_shared<ClubListItem>(nullptr));
    }
    m_clubList->GenerateList(c_sampleUIPanel, clubsListSource, 2);
}

void Sample::ShowOrHideButton(int id, bool show)
{
    auto button = m_ui->FindControl<Button>(c_sampleUIPanel, id);
    button->SetEnabled(show);
    button->SetVisible(show);
}

void Sample::HideAllButtons()
{
    ShowOrHideButton(c_joinClub, false);
    ShowOrHideButton(c_leaveClub, false);
    ShowOrHideButton(c_inviteToClub, false);
    ShowOrHideButton(c_deleteClub, false);
    ShowOrHideButton(c_renameClub, false);
}

void Sample::HandleClubSelected(unsigned index)
{
    PrintClub(m_clubs[index]);
    m_selectedClub = &m_clubs[index];
    m_clubList->ShadeItem(index, m_uiConfig);

    HideAllButtons();
    bool owner = false;

    for (const auto& role : m_selectedClub->viewer_roles())
    {
        switch (role)
        {
        case club_role::invited:
            ShowOrHideButton(c_joinClub, true);
            break;
        case club_role::owner:
            owner = true;
            ShowOrHideButton(c_renameClub, true);
            //ShowOrHideButton(c_leaveClub, false); // Owner can only delete club, not leave it
            ShowOrHideButton(c_deleteClub, true); 
            ShowOrHideButton(c_inviteToClub, true); 
            break;
        case club_role::member:
            if (!owner)
            {
                ShowOrHideButton(c_leaveClub, true);
            }
            break;
        case club_role::moderator:
            ShowOrHideButton(c_inviteToClub, true);
            break;
        default:
            break;
        }
    }
}

void Sample::RefreshClubList()
{
    auto clubListSource = std::vector<std::shared_ptr<ClubListItem>>();
    for (unsigned x = 0; x < 15; x++)
    {
        if (x < m_clubs.size())
        {
            clubListSource.push_back(std::make_shared<ClubListItem>(&m_clubs[x]));
        }
        else
        {
            clubListSource.push_back(std::make_shared<ClubListItem>(nullptr));
        }
    }
    
    // when the list is updated, reset the selected club
    m_selectedClub = nullptr;
    m_clubList->ShadeItem(16, m_uiConfig);
    m_clubList->UpdateList(clubListSource);    
}
#pragma endregion

#pragma region Frame Update
// Executes basic render loop.
void Sample::Tick()
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Frame %I64u", m_frame);

    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();

    PIXEndEvent();
    m_frame++;
}

// Updates the world.
void Sample::Update(DX::StepTimer const& timer)
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Update");

    float elapsedTime = float(timer.GetElapsedSeconds());
    auto pad = m_gamePad->GetState(0);

    if (pad.IsConnected())
    {
        m_gamePadButtons.Update(pad);

        m_ui->Update(elapsedTime, pad);

        if (pad.IsViewPressed() || pad.IsBPressed())
        {
            Windows::ApplicationModel::Core::CoreApplication::Exit();
        }
    }
    else
    {
        m_gamePadButtons.Reset();
    }

    auto mouse = m_mouse->GetState();
    mouse;

    auto kb = m_keyboard->GetState();
    m_keyboardButtons.Update(kb);

    if (!m_ui->Update(elapsedTime, *m_mouse, *m_keyboard))
    {
        // UI is not consuming input, so implement sample mouse & keyboard controls
    }

    if (kb.Escape)
    {
        Windows::ApplicationModel::Core::CoreApplication::Exit();
    }

    if (m_keyboardButtons.IsKeyPressed(Keyboard::A))
    {
        m_liveResources->SignIn();
    }

    PIXEndEvent();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Sample::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    auto context = m_deviceResources->GetD3DDeviceContext();
    PIXBeginEvent(context, PIX_COLOR_DEFAULT, L"Render");
    m_ui->Render();
    m_console->Render();
    PIXEndEvent(context);

    // Show the new frame.
    PIXBeginEvent(context, PIX_COLOR_DEFAULT, L"Present");
    m_deviceResources->Present();
    PIXEndEvent(context);
}

// Helper method to clear the back buffers.
void Sample::Clear()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    PIXBeginEvent(context, PIX_COLOR_DEFAULT, L"Clear");

    // Clear the views.
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, ATG::Colors::Background);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    PIXEndEvent(context);
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Sample::OnSuspending()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    context->ClearState();

    m_deviceResources->Trim();
}

void Sample::OnResuming()
{
    m_timer.ResetElapsedTime();
    m_gamePadButtons.Reset();
    m_keyboardButtons.Reset();

    // Reset UI on return from suspend.
    m_ui->Reset();
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Sample::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    m_console = std::make_unique<DX::TextConsole>(context, L"SegoeUI_18.spritefont");

    // Let UI create Direct3D resources.
    m_ui->RestoreDevice(context);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Sample::CreateWindowSizeDependentResources()
{
    // Let UI know the size of our rendering viewport.
    RECT fullscreen = m_deviceResources->GetOutputSize();
    m_ui->SetWindow(fullscreen);

    const RECT* label = m_ui->FindControl<ATG::Image>(c_sampleUIPanel, c_debugLog)->GetRectangle();

    RECT console = { 0 };
    console.top = label->top;
    console.left = label->left;
    console.bottom = console.top + 600;
    console.right = console.left + 800;

    m_console->SetWindow(console);
}

void Sample::OnDeviceLost()
{
    m_ui->ReleaseDevice();
}

void Sample::OnActivated()
{
}

void Sample::OnDeactivated()
{
}

void Sample::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}

void Sample::OnWindowSizeChanged(int width, int height, DXGI_MODE_ROTATION rotation)
{
    if (!m_deviceResources->WindowSizeChanged(width, height, rotation))
        return;

    CreateWindowSizeDependentResources();
}

void Sample::ValidateDevice()
{
    m_deviceResources->ValidateDevice();
}

void Sample::GetDefaultSize(int& width, int& height) const
{
    width = 1600;
    height = 1050;
}
#pragma endregion

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Sample.h"

#include "ATGColors.h"

using Microsoft::WRL::ComPtr;

using namespace DirectX;
using namespace ATG;
using namespace xbox::services::clubs;
using namespace Windows::Xbox::System;

namespace
{
    const int c_sampleUIPanel = 200;
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
    m_liveInfoHud = std::make_unique<ATG::LiveInfoHUD>(L"Social Sample");
    
    m_ui = std::make_shared<ATG::UIManager>(m_uiConfig);
}

// Initialize the Direct3D resources required to run.
void Sample::Initialize(IUnknown* window)
{
    m_gamePad = std::make_unique<GamePad>();

    m_ui->LoadLayout(L".\\Assets\\SampleUI.csv", L".\\Assets");

    //m_liveResources->Initialize();
    m_deviceResources->SetWindow(window);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // Set the callbacks before initializing the LiveInfoHUD
    m_liveResources->SetUserChangedCallback([this](ATG::XboxLiveUser user)
    {
        m_liveInfoHud->SetUser(m_liveResources->GetLiveContext());
        m_ui->FindPanel<ATG::IPanel>(200)->Show();
    });

    m_liveResources->SetUserSignOutCompletedCallback([this](ATG::XboxLiveUser)
    {
        m_liveInfoHud->SetUser(m_liveResources->GetLiveContext());
        m_ui->FindPanel<ATG::IPanel>(200)->Close();
    });

    // Initialize these after the device and window dependant resources
    m_liveResources->Initialize();
    m_liveInfoHud->Initialize(m_liveResources->GetLiveContext());

    SetupUI();

    auto user = m_liveResources->GetUser();

    if (user != nullptr)
    {
        stringstream_t ss;
        ss << "Xuid: " << user->XboxUserId->Data();

        m_ui->FindControl<ATG::TextLabel>(c_sampleUIPanel, c_xuidLabel)->SetText(ss.str().data());
        GetUsersClubAssociations(user);
    }

    User::UserAdded += ref new Windows::Foundation::EventHandler<UserAddedEventArgs^>(
        [this](Platform::Object^, UserAddedEventArgs^ eventArgs)
    {
        if (m_clubs.size() == 0)
        {
            auto user = m_liveResources->GetUser();

            if (user != nullptr)
            {
                stringstream_t ss;
                ss << "Xuid: " << user->XboxUserId->Data();

                m_ui->FindControl<ATG::TextLabel>(c_sampleUIPanel, c_xuidLabel)->SetText(ss.str().data());
                GetUsersClubAssociations(user);
            }

        }
    });
}

#pragma region UI Methods
void Sample::SetupUI()
{
    using namespace ATG;

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

    // Setup user list repeater
    auto loc = POINT{ 150, 200 };
    auto pos = SIZE{ 610, 40 };

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
        if (pad.IsMenuPressed())
        {
            Windows::Xbox::UI::SystemUI::ShowAccountPickerAsync(nullptr, Windows::Xbox::UI::AccountPickerOptions::None);
        }
    }
    else
    {
        m_gamePadButtons.Reset();
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

    // Prepare the render target to render a new frame.
    m_deviceResources->Prepare();
    Clear();

    auto context = m_deviceResources->GetD3DDeviceContext();
    PIXBeginEvent(context, PIX_COLOR_DEFAULT, L"Render");
    m_liveInfoHud->Render();
    m_ui->Render();
    m_console->Render();
    PIXEndEvent(context);

    // Show the new frame.
    PIXBeginEvent(context, PIX_COLOR_DEFAULT, L"Present");
    m_deviceResources->Present();
    m_graphicsMemory->Commit();
    PIXEndEvent(context);
}

// Helper method to clear the back buffers.
void Sample::Clear()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    PIXBeginEvent(context, PIX_COLOR_DEFAULT, L"Clear");

    // Clear the views.
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    context->ClearRenderTargetView(renderTarget, ATG::Colors::Background);
    context->OMSetRenderTargets(1, &renderTarget, nullptr);

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
    context->Suspend(0);
}

void Sample::OnResuming()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    context->Resume();
    m_timer.ResetElapsedTime();
    m_gamePadButtons.Reset();
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Sample::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    m_graphicsMemory = std::make_unique<GraphicsMemory>(device, m_deviceResources->GetBackBufferCount());
    m_console = std::make_unique<DX::TextConsole>(context, L"Courier_16.spritefont");
    m_liveInfoHud->RestoreDevice(context);

    m_ui->RestoreDevice(context);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Sample::CreateWindowSizeDependentResources()
{
    RECT fullscreen = m_deviceResources->GetOutputSize();

    m_ui->SetWindow(fullscreen);

    RECT console = { 0 };

    console.top = 200;
    console.left = 1120;
    console.bottom = console.top + 700;
    console.right = console.left + 660;

    m_console->SetWindow(console);
}
#pragma endregion

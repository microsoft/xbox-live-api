//--------------------------------------------------------------------------------------
// Leaderboards.cpp
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "SampleGUI.h"
#include "ATGColors.h"
#include "Leaderboards.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace xbox::services;
using namespace xbox::services::leaderboard;
using namespace xbox::services::experimental::stats::manager;

namespace
{
    const WCHAR StatsNameEnemyDefeats[] = L"EnemyDefeats";
    const WCHAR LeaderboardIdEnemyDefeats[] = L"LBEnemyDefeatsDescending";

    const int c_maxLeaderboards          = 10;
    const int c_liveHUD                  = 1000;
    const int c_sampleUIPanel            = 2000;
    const int c_multiColumnCheckBox      = 2200;
    const int c_writeEventBtn            = 2101;
    const int c_getLeaderboardBtn        = 2102;
    const int c_skipToXuidBtn            = 2103;
    const int c_skipToRankBtn = 2104;
    const int c_getLeaderboardForSocialGroupBtn = 2105;
    const int c_getLeaderboardForSocialGroupWithSortBtn = 2106;
}

Sample::Sample()
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);

    m_liveResources = std::make_unique<ATG::LiveResources>();

    // Set up UI using default fonts and colors.
    ATG::UIConfig uiconfig;
    m_ui = std::make_unique<ATG::UIManager>(uiconfig);
}

// Initialize the Direct3D resources required to run.
void Sample::Initialize(IUnknown* window, int width, int height, DXGI_MODE_ROTATION rotation)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();
    m_keyboard->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(reinterpret_cast<ABI::Windows::UI::Core::ICoreWindow*>(window));

    m_ui->LoadLayout(L".\\Assets\\Layout.csv", L".\\Assets");
    m_liveResources->Initialize(m_ui, m_ui->FindPanel<ATG::Overlay>(c_sampleUIPanel));

    m_deviceResources->SetWindow(window, width, height, rotation);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    SetupUI();
}

#pragma region Leaderboard Methods

void Sample::WriteEvent()
{
    stats_manager::get_singleton_instance().set_stat(m_liveResources->GetUser(), L"MultiplayerCorrelationId", L"multiplayer correlation id");
    stats_manager::get_singleton_instance().set_stat(m_liveResources->GetUser(), L"GameplayModeId", L"multiplayer correlation id");
    stats_manager::get_singleton_instance().set_stat(m_liveResources->GetUser(), L"DifficultyLevelId", 100);
    stats_manager::get_singleton_instance().set_stat(m_liveResources->GetUser(), L"RoundId", 1);
    stats_manager::get_singleton_instance().set_stat(m_liveResources->GetUser(), L"PlayerRoleId", 1);
    stats_manager::get_singleton_instance().set_stat(m_liveResources->GetUser(), L"PlayerWeaponId", 2);
    stats_manager::get_singleton_instance().set_stat(m_liveResources->GetUser(), L"EnemyRoleId", 3);
    stats_manager::get_singleton_instance().set_stat(m_liveResources->GetUser(), L"KillTypeId", 4);

    // Write an event with data fields
    web::json::value properties = web::json::value::object();
    properties[L"MultiplayerCorrelationId"] = web::json::value(L"multiplayer correlation id");
    properties[L"GameplayModeId"] = web::json::value(L"gameplay mode id");
    properties[L"DifficultyLevelId"] = 100;
    properties[L"RoundId"] = 1;
    properties[L"PlayerRoleId"] = 1;
    properties[L"PlayerWeaponId"] = 2;
    properties[L"EnemyRoleId"] = 3;
    properties[L"KillTypeId"] = 4;
}

void Sample::GetLeaderboard()
{
    auto& leaderboardService = m_liveResources->GetLiveContext()->leaderboard_service();
    pplx::task<xbox_live_result<leaderboard_result>> asyncTask;
    if (m_multiColumnEnabled)
    {
        asyncTask = leaderboardService.get_leaderboard(m_liveResources->GetServiceConfigId(), LeaderboardIdEnemyDefeats, m_columnNames);
    }
    else
    {
        asyncTask = leaderboardService.get_leaderboard(m_liveResources->GetServiceConfigId(), LeaderboardIdEnemyDefeats);
    }

    asyncTask.then([this](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
    {
        this->ProcessLeaderboards(result);
    });
}

void Sample::GetLeaderboardSkipToRank()
{
    uint32_t skipToRank = 2;
    uint32_t maxItems = 10;
    auto& leaderboardService = m_liveResources->GetLiveContext()->leaderboard_service();
    pplx::task<xbox_live_result<leaderboard_result>> asyncTask;
    if (m_multiColumnEnabled)
    {
        asyncTask = leaderboardService.get_leaderboard(m_liveResources->GetServiceConfigId(), LeaderboardIdEnemyDefeats, skipToRank, maxItems, m_columnNames);
    }
    else
    {
        asyncTask = leaderboardService.get_leaderboard(m_liveResources->GetServiceConfigId(), LeaderboardIdEnemyDefeats, skipToRank);
    }

    asyncTask.then([this](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
    {
        this->ProcessLeaderboards(result);
    });
}

void Sample::GetLeaderboardSkipToXuid()
{
    uint32_t maxItems = 10;
    auto& leaderboardService = m_liveResources->GetLiveContext()->leaderboard_service();
    pplx::task<xbox_live_result<leaderboard_result>> asyncTask;
    if (m_multiColumnEnabled)
    {
        asyncTask = leaderboardService.get_leaderboard_skip_to_xuid(
            m_liveResources->GetServiceConfigId(),
            LeaderboardIdEnemyDefeats,
            m_liveResources->GetUser()->xbox_user_id(),
            maxItems,
            m_columnNames);
    }
    else
    {
        asyncTask = leaderboardService.get_leaderboard_skip_to_xuid(
            m_liveResources->GetServiceConfigId(),
            LeaderboardIdEnemyDefeats,
            m_liveResources->GetUser()->xbox_user_id());
    }

    asyncTask.then([this](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
    {
        this->ProcessLeaderboards(result);
    });
}

void Sample::GetLeaderboardForSocialGroup()
{
    uint32_t maxItems = 10;
    auto& leaderboardService = m_liveResources->GetLiveContext()->leaderboard_service();
    pplx::task<xbox_live_result<leaderboard_result>> asyncTask;
    if (m_multiColumnEnabled)
    {
        asyncTask = leaderboardService.get_leaderboard(
            m_liveResources->GetServiceConfigId(),
            LeaderboardIdEnemyDefeats,
            m_liveResources->GetUser()->xbox_user_id(),
            social::social_group_constants::favorite(),
            maxItems,
            m_columnNames
        );
    }
    else
    {
        asyncTask = leaderboardService.get_leaderboard_for_social_group(
            m_liveResources->GetUser()->xbox_user_id(),
            m_liveResources->GetServiceConfigId(),
            StatsNameEnemyDefeats,
            social::social_group_constants::people()
        );
    }

    asyncTask.then([this](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
    {
        this->ProcessLeaderboards(result);
    });
}

void Sample::GetLeaderboardForSocialGroupWithSort()
{
    uint32_t maxItems = 10;
    auto& leaderboardService = m_liveResources->GetLiveContext()->leaderboard_service();
    pplx::task<xbox_live_result<leaderboard_result>> asyncTask;
    if (m_multiColumnEnabled)
    {
        // Cannot sort with multi-column leaderboard
        asyncTask = leaderboardService.get_leaderboard(
            m_liveResources->GetServiceConfigId(),
            LeaderboardIdEnemyDefeats,
            m_liveResources->GetUser()->xbox_user_id(),
            social::social_group_constants::favorite(),
            maxItems,
            m_columnNames
        );
    }
    else
    {
        asyncTask = leaderboardService.get_leaderboard_for_social_group(
            m_liveResources->GetUser()->xbox_user_id(),
            m_liveResources->GetServiceConfigId(),
            StatsNameEnemyDefeats,
            social::social_group_constants::people(),
            L"descending"
        );
    }

    asyncTask.then([this](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
    {
        this->ProcessLeaderboards(result);
    });
}

void Sample::ProcessLeaderboards(xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
{
    if (!result.err())
    {
        auto leaderboard = result.payload();
        PrintLeaderboard(leaderboard);

        // Keep processing if there are more Leaderboards.
        if (leaderboard.has_next())
        {
            leaderboard.get_next(c_maxLeaderboards).then(
                [this](xbox::services::xbox_live_result<xbox::services::leaderboard::leaderboard_result> result)
            {
                this->ProcessLeaderboards(result);
            });
        }
    }
}

void Sample::PrintLeaderboard(const xbox::services::leaderboard::leaderboard_result& leaderboard)
{
    if (leaderboard.rows().size() > 0)
    {
        m_console->Format(L"%16s %6s %12s %8s\n", L"Gamertag", L"Rank", L"Percentile", L"Values");
    }

    for (const xbox::services::leaderboard::leaderboard_row& row : leaderboard.rows())
    {
        string_t colValues;
        for (auto columnValue : row.column_values())
        {
            colValues = colValues + L" ";
            colValues = colValues + columnValue;
        }
        m_console->Format(L"%16s %6d %12f %8s\n", row.gamertag().c_str(), row.rank(), row.percentile(), colValues.c_str());
    }
}

#pragma endregion

#pragma region UI Methods
void Sample::SetupUI()
{
    using namespace ATG;

    // Write an event
    m_ui->FindControl<Button>(c_sampleUIPanel, c_writeEventBtn)->SetCallback([this](IPanel*, IControl*)
    {
        m_console->Clear();
        this->WriteEvent();
    });

    // Multi-column options
    m_ui->FindControl<ATG::CheckBox>(c_sampleUIPanel, c_multiColumnCheckBox)->SetCallback([this](ATG::IPanel *parent, ATG::IControl *)
    {
        m_multiColumnEnabled = false;
        if (dynamic_cast<ATG::CheckBox*>(parent->Find(c_multiColumnCheckBox))->IsChecked())
        {
            m_multiColumnEnabled = true;
        }
    });

    // Get Leaderboards
    m_ui->FindControl<Button>(c_sampleUIPanel, c_getLeaderboardBtn)->SetCallback([this](IPanel*, IControl*)
    {
        m_console->Clear();
        GetLeaderboard();
    });

    // Skip to a specific rank
    m_ui->FindControl<Button>(c_sampleUIPanel, c_skipToRankBtn)->SetCallback([this](IPanel*, IControl*)
    {
        m_console->Clear();
        GetLeaderboardSkipToRank();
    });

    // Skip to a specific xuid (for e.g. self)
    m_ui->FindControl<Button>(c_sampleUIPanel, c_skipToXuidBtn)->SetCallback([this](IPanel*, IControl*)
    {
        m_console->Clear();
        GetLeaderboardSkipToXuid();
    });

    // Get leaderboard for social group
    m_ui->FindControl<Button>(c_sampleUIPanel, c_getLeaderboardForSocialGroupBtn)->SetCallback([this](IPanel*, IControl*)
    {
        m_console->Clear();
        GetLeaderboardForSocialGroup();
    });

    // Get & sort leaderboard for social group
    m_ui->FindControl<Button>(c_sampleUIPanel, c_getLeaderboardForSocialGroupWithSortBtn)->SetCallback([this](IPanel*, IControl*)
    {
        m_console->Clear();
        GetLeaderboardForSocialGroupWithSort();
    });
}
#pragma endregion

#pragma region Frame Update
// Executes basic render loop.
void Sample::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
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

        if (!m_ui->Update(elapsedTime, pad))
        {
            // UI is not consuming input, so implement sample gamepad controls
        }

        if (pad.IsViewPressed())
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

    if (m_keyboardButtons.IsKeyPressed(Keyboard::Y))
    {
        m_liveResources->SwitchAccount();
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

    // Allow UI to render last
    m_ui->Render();
    m_console->Render();

    PIXEndEvent(context);

    // Show the new frame.
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Present");
    m_deviceResources->Present();
    PIXEndEvent();
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
void Sample::OnActivated()
{
}

void Sample::OnDeactivated()
{
}

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

// Properties
void Sample::GetDefaultSize(int& width, int& height) const
{
    width = 1280;
    height = 720;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Sample::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    m_console = std::make_unique<DX::TextConsole>(context, L"Courier_16.spritefont");

    // Let UI create Direct3D resources.
    m_ui->RestoreDevice(context);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Sample::CreateWindowSizeDependentResources()
{
    // Let UI know the size of our rendering viewport.
    RECT fullscreen = m_deviceResources->GetOutputSize();
    m_ui->SetWindow(fullscreen);

    static const RECT consoleDisplay = { 560, 150, 1250, 705 };
    m_console->SetWindow(consoleDisplay);
}

void Sample::OnDeviceLost()
{
    m_ui->ReleaseDevice();
}

void Sample::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

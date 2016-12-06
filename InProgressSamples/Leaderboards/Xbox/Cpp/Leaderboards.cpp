//--------------------------------------------------------------------------------------
// Leaderboards.cpp
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "StatsSample.h"
#include "Leaderboards.h"
#include "ATGColors.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace xbox::services;
using namespace xbox::services::leaderboard;

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

Sample::Sample() :
    m_frame(0)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_UNKNOWN);
    m_liveResources = std::make_unique<ATG::LiveResources>();

    ATG::UIConfig uiconfig;
    m_ui = std::make_unique<ATG::UIManager>(uiconfig);

}

// Initialize the Direct3D resources required to run.
void Sample::Initialize(IUnknown* window)
{
    //Register the Stats Sample ETW+ Provider
    ULONG result = EventRegisterXDKS_0301D082();

    m_gamePad = std::make_unique<GamePad>();

    m_ui->LoadLayout(L".\\Assets\\SampleUI.csv", L".\\Assets");

    m_liveResources->Initialize(m_ui, m_ui->FindPanel<ATG::Overlay>(c_sampleUIPanel));
    m_deviceResources->SetWindow(window);

    m_deviceResources->CreateDeviceResources();  
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();
    
    SetupUI();
    //init columns
    m_columnNames.push_back(L"GameplayModeId");
    m_columnNames.push_back(L"DifficultyLevelId");
}

#pragma region Leaderboard Methods

void Sample::WriteEvent()
{
    GUID playerSessionId = {};
    GUID roundId = {};

    // We use GameplayModeId & DifficultyLevelId for multi-columns

    auto result = EventWriteEnemyDefeated(
        m_liveResources->GetUser()->XboxUserId->Data(), // UserId
        1,                      // SectionId
        &playerSessionId,       // PlayerSessionId
        L"0",                   // MultiplayerSessionId
        rand() % 5,             // GameplayModeId
        rand() % 100,           // DifficultyLevelId
        &roundId,               // RoundId
        1,                      // PlayerRoleId
        1,                      // PlayerWeaponTypeId
        1,                      // EnemyRoleId
        1,                      // KillTypeId
        1,                      // LocationX
        1,                      // LocationY
        1,                      // LocationZ
        1                       // EnemyWeaponId
    );

    if (result == ERROR_SUCCESS)
    {
        m_console->WriteLine(L"EnemyDefeated event was fired");
    }
    else
    {
        m_console->WriteLine(L"Failed to fire EnemyDefeated event");
    }
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
            m_liveResources->GetUser()->XboxUserId->Data(),
            maxItems,
            m_columnNames);
    }
    else
    {
        asyncTask = leaderboardService.get_leaderboard_skip_to_xuid(
            m_liveResources->GetServiceConfigId(),
            LeaderboardIdEnemyDefeats,
            m_liveResources->GetUser()->XboxUserId->Data());
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
            m_liveResources->GetUser()->XboxUserId->Data(),
            social::social_group_constants::favorite(),
            maxItems,
            m_columnNames
        );
    }
    else
    {
        asyncTask = leaderboardService.get_leaderboard_for_social_group(
            m_liveResources->GetUser()->XboxUserId->Data(),
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
            m_liveResources->GetUser()->XboxUserId->Data(),
            social::social_group_constants::favorite(),
            maxItems,
            m_columnNames
        );
    }
    else
    {
        asyncTask = leaderboardService.get_leaderboard_for_social_group(
            m_liveResources->GetUser()->XboxUserId->Data(), 
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
        m_console->Format(L"%16s %6s %12s %12s\n", L"Gamertag", L"Rank", L"Percentile", L"Values");
    }

    for (const xbox::services::leaderboard::leaderboard_row& row : leaderboard.rows())
    {
        string_t colValues;
        for (auto columnValue : row.column_values())
        {
            colValues = colValues + L" ";
            colValues = colValues + columnValue;
        }
        m_console->Format(L"%16s %6d %12f %12s\n", row.gamertag().c_str(), row.rank(), row.percentile(), colValues.c_str());
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
        if(dynamic_cast<ATG::CheckBox*>(parent->Find(c_multiColumnCheckBox))->IsChecked())
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

        if (!m_ui->Update(elapsedTime, pad))
        {
            if (pad.IsViewPressed())
            {
                Windows::ApplicationModel::Core::CoreApplication::Exit();
            }
            if (pad.IsMenuPressed())
            {
                Windows::Xbox::UI::SystemUI::ShowAccountPickerAsync(nullptr,Windows::Xbox::UI::AccountPickerOptions::None);
            }
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

    PIXEndEvent(context);

    m_ui->Render();
    m_console->Render();
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
    m_ui->Reset();
    m_liveResources->Refresh();
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Sample::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    m_graphicsMemory = std::make_unique<GraphicsMemory>(device, m_deviceResources->GetBackBufferCount());
    m_console = std::make_unique<DX::TextConsole>(context,L"Courier_16.spritefont");

    m_ui->RestoreDevice(context);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Sample::CreateWindowSizeDependentResources()
{
    RECT fullscreen = m_deviceResources->GetOutputSize();
    static const RECT consoleDisplay = { 750, 150, 1838, 950 };

    m_ui->SetWindow(fullscreen);
    m_console->SetWindow(consoleDisplay);
}
#pragma endregion

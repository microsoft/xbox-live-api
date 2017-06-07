// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "SampleGUI.h"
#include "ATGColors.h"
#include "Multiplayer.h"
#include "..\Utils\PerformanceCounters.h"

using namespace DirectX;
using namespace xbox::services;
using namespace xbox::services::multiplayer;
using namespace xbox::services::multiplayer::manager;

namespace
{
    const wchar_t *JoinabilityStateText[] = {
        L"None",
        L"Joinable by friends",
        L"Invite only",
        L"Disable while Game in progress",
        L"Closed"
    };

    const int c_liveHUD                 = 1000;
    const int c_mainMenuPanelId         = 2000;
    const int c_sessionDetailsPanelId   = 3000;
    const int c_joinLobbyBtn            = 2101;
    const int c_joinGameBtn             = 2102;
    const int c_joinLFGBtn              = 2103;

    const float c_clearErrorMsgTimer    = 3.0f;
}

Sample::Sample() :
    m_frame(0),
    m_appState(APP_MAIN_MENU),
    m_gameModeIndex(0),
    m_mapIndex(0),
    m_clearErrorMsgTimer(0.0f),
    m_multiplayerContext(0),
    m_isLeavingGame(false),
    m_isProtocolActivated(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_UNKNOWN);
    m_liveResources = std::make_unique<ATG::LiveResources>();

    // Set up UI using default fonts and colors.
    ATG::UIConfig uiconfig;
    m_ui = std::make_unique<ATG::UIManager>(uiconfig);
}

// Initialize the Direct3D resources required to run.
void Sample::Initialize(IUnknown* window)
{
    m_gamePad = std::make_unique<GamePad>();
    m_ui->LoadLayout(L".\\Assets\\SampleUI.csv", L".\\Assets");

    m_liveResources->Initialize(m_ui, m_ui->FindPanel<ATG::Overlay>(c_mainMenuPanelId));
    m_hudPanel = m_ui->FindPanel<ATG::HUD>(1000);
    m_mainMenuPanel = m_ui->FindPanel<ATG::Overlay>(c_mainMenuPanelId);
    m_sessionDetailsPanel = m_ui->FindPanel<ATG::HUD>(c_sessionDetailsPanelId);
    m_hudPanel->Show();
    m_mainMenuPanel->Show();
    m_sessionDetailsPanel->Close();

    m_lobbyView = std::make_shared<SessionView>();
    m_gameView = std::make_shared<SessionView>();
    m_deviceResources->SetWindow(window);
    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    SetupUI();
    InitializeMultiplayerManager();

    if (m_isProtocolActivated)
    {
        HandleProtocolActivation();
    }
}

#pragma region UI Methods
void Sample::SetupUI()
{
    using namespace ATG;

    m_hudPanel = m_ui->FindPanel<ATG::HUD>(1000);
    m_lobbyView->SetControls(m_sessionDetailsPanel, 3100);
    m_gameView->SetControls(m_sessionDetailsPanel, 3200);
    m_joinabilityLabel = dynamic_cast<ATG::TextLabel*>(m_sessionDetailsPanel->Find(3009));

    // Join Lobby
    m_ui->FindControl<Button>(c_mainMenuPanelId, c_joinLobbyBtn)->SetCallback([this](IPanel*, IControl*)
    {
        m_appState = APPSTATE::APP_CREATE_LOBBY;
        ChangeAppStates();
    });

    // Join Game
    m_ui->FindControl<Button>(c_mainMenuPanelId, c_joinGameBtn)->SetCallback([this](IPanel*, IControl*)
    {
        m_appState = APPSTATE::APP_JOIN_GAME;
        ChangeAppStates();
    });
}

void Sample::ChangeAppStates()
{
    switch (m_appState)
    {
    case APP_MAIN_MENU:
        m_multiplayerContext = 0;
        m_lobbyView->Clear();
        m_gameView->Clear();
        m_displayEventQueue.clear();
        m_errorMsgConsole->Clear();
#if PERF_COUNTERS
        performance_counters::get_singleton_instance()->clear_captured_data();
#endif

        m_hudPanel->Show();
        m_mainMenuPanel->Show();
        m_sessionDetailsPanel->Close();
        break;

    case APP_CREATE_LOBBY:
        m_displayEventQueue.clear();
        m_mainMenuPanel->Close();
        m_sessionDetailsPanel->Show();
        AddLocalUser();
        break;

    case APP_JOIN_LOBBY:
        m_displayEventQueue.clear();
        m_mainMenuPanel->Close();
        m_sessionDetailsPanel->Show();
        break;

    case APP_JOIN_GAME:
        m_displayEventQueue.clear();
        m_mainMenuPanel->Close();
        m_sessionDetailsPanel->Show();
        JoinGame();
        break;

    case APP_CREATE_LFG_LOBBY:
        m_displayEventQueue.clear();
        m_mainMenuPanel->Close();
        m_sessionDetailsPanel->Show();
        InitializeMultiplayerManager(OWNER_MANAGED_LFG_TEMPLATE_NAME);
        AddLocalUser();
        break;

    case APP_SHOW_SEARCH_HANDLES:
        BrowseSearchHandles();
        break;

    case APP_SHOW_FRIEND_GAMES:
        GetActivitiesForSocialGroup();
        break;

    default:
        break;
    }
}

void Sample::LogErrorFormat(const wchar_t* strMsg, ...)
{
    WCHAR strBuffer[2048];
    va_list args;
    va_start(args, strMsg);
    _vsnwprintf_s(strBuffer, 2048, _TRUNCATE, strMsg, args);
    strBuffer[2047] = L'\0';
    va_end(args);

    m_errorMsgConsole->Clear();
    m_errorMsgConsole->Write(strBuffer);
    m_clearErrorMsgTimer = c_clearErrorMsgTimer;
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

    if (m_isProtocolActivated &&
        m_liveResources->GetUser() != nullptr && m_liveResources->GetUser()->IsSignedIn)
    {
        HandleProtocolActivation();
    }

    float elapsedTime = float(timer.GetElapsedSeconds());
    auto pad = m_gamePad->GetState(0);
    if (pad.IsConnected())
    {
        m_gamePadButtons.Update(pad);

        if (!m_ui->Update(elapsedTime, pad))
        {
            if (m_clearErrorMsgTimer > 0.0f)
            {
                m_clearErrorMsgTimer -= elapsedTime;
                if (m_clearErrorMsgTimer < 0.0f)
                {
                    m_clearErrorMsgTimer = 0.0f;
                    m_errorMsgConsole->Clear();
                }
            }

            if (m_appState == APPSTATE::APP_MAIN_MENU)
            {
                if (pad.IsMenuPressed())
                {
                    Windows::Xbox::UI::SystemUI::ShowAccountPickerAsync(nullptr, Windows::Xbox::UI::AccountPickerOptions::None);
                }
            }
            else if (m_appState == APPSTATE::APP_IN_GAME)
            {
                if (m_gamePadButtons.dpadRight == GamePad::ButtonStateTracker::ButtonState::PRESSED)
                {
                    PublishSearchHandle();
                }

                if (m_gamePadButtons.dpadLeft == GamePad::ButtonStateTracker::ButtonState::PRESSED)
                {
                    BrowseSearchHandles();
                }

                if (m_gamePadButtons.leftTrigger == GamePad::ButtonStateTracker::ButtonState::PRESSED)
                {
                    UpdateLobbyProperties();
                }

                if (m_gamePadButtons.rightTrigger == GamePad::ButtonStateTracker::ButtonState::PRESSED)
                {
                    SetLobbyHost();
                }

                if (m_gamePadButtons.leftShoulder == GamePad::ButtonStateTracker::ButtonState::PRESSED)
                {
                    UpdateGameProperties();
                }

                if (m_gamePadButtons.rightShoulder == GamePad::ButtonStateTracker::ButtonState::PRESSED)
                {
                    SetGameHost();
                }

                if (m_gamePadButtons.a == GamePad::ButtonStateTracker::ButtonState::PRESSED)
                {
                    JoinGameFromLobby();
                }

                if (m_gamePadButtons.b == GamePad::ButtonStateTracker::ButtonState::PRESSED)
                {
                    if (m_multiplayerManager->game_session() != nullptr)
                    {
                        LeaveGameSession();
                    }
                    else
                    {
                        RemoveLocalUser();
                    }
                }

                if (m_gamePadButtons.x == GamePad::ButtonStateTracker::ButtonState::PRESSED)
                {
                    InviteFriends();
                }

                if (m_gamePadButtons.y == GamePad::ButtonStateTracker::ButtonState::PRESSED)
                {
                    UpdateJoinability();
                }
            }
        }
    }
    else
    {
        m_gamePadButtons.Reset();
    }

    PIXBeginEvent(PIX_COLOR_DEFAULT, L"MPM::DoWork");
    DoWork();
    PIXEndEvent();
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
    if (m_appState == APP_IN_GAME)
    {
        auto lobbySession = m_multiplayerManager->lobby_session();
        RenderMultiplayerSessionDetails(
            lobbySession->session_reference().session_name(),
            lobbySession->host(),
            lobbySession->members(),
            lobbySession->properties(),
            multiplayer_session_type::lobby_session,
            m_lobbyView);

        auto gameSession = m_multiplayerManager->game_session();
        if (gameSession != nullptr)
        {
            RenderMultiplayerSessionDetails(
                gameSession->session_reference().session_name(),
                gameSession->host(),
                gameSession->members(),
                gameSession->properties(),
                multiplayer_session_type::game_session,
                m_gameView);
        }
        m_joinabilityLabel->SetText(JoinabilityStateText[(int)m_multiplayerManager->joinability()]);
        RenderMultiplayerEventQueue();

#if PERF_COUNTERS
        RenderPerfCounters();
        m_perfConsole->Render();
#endif
    }

    m_ui->Render();

    // Render these after the main UI panel
    m_eventQueueConsole->Render(true);
    m_errorMsgConsole->Render();
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

    m_lobbyView->Clear();
    m_gameView->Clear();
    m_eventQueueConsole->Clear();
    m_perfConsole->Clear();

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

void Sample::OnProtocolActivated(Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ protocolArgs)
{
    m_isProtocolActivated = true;
    m_protocolActivatedEventArgs = protocolArgs;

    if (m_multiplayerManager == nullptr ||
        (m_liveResources != nullptr && m_liveResources->GetUser() != nullptr && !m_liveResources->GetUser()->IsSignedIn))
    {
        // Game hasn't been intialized yet. We will process the protocol after initialization.
        return;
    }

    HandleProtocolActivation();
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Sample::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    m_graphicsMemory = std::make_unique<GraphicsMemory>(device, m_deviceResources->GetBackBufferCount());
    m_eventQueueConsole = std::make_shared<DX::TextConsole>(context, L"Courier_16.spritefont");
    m_errorMsgConsole = std::make_shared<DX::TextConsole>(context, L"Courier_16.spritefont");
    m_perfConsole = std::make_shared<DX::TextConsole>(context, L"Courier_16.spritefont");

    // Let UI create Direct3D resources.
    m_ui->RestoreDevice(context);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Sample::CreateWindowSizeDependentResources()
{
    // Let UI know the size of our rendering viewport.
    RECT fullscreen = m_deviceResources->GetOutputSize();
    m_ui->SetWindow(fullscreen);

    m_eventQueueConsole->SetWindow({ 1175, 225, 1800, 700 });
    m_errorMsgConsole->SetWindow({ 1175, 600, 1750, 900 });
    m_errorMsgConsole->SetForegroundColor(ATG::ColorsLinear::Orange);

    m_perfConsole->SetWindow({ 1100, 600, 1750, 900 });
    m_perfConsole->SetForegroundColor(ATG::Colors::OffWhite);
}
#pragma endregion
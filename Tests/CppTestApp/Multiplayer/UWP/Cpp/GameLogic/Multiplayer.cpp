// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "SampleGUI.h"
#include "ATGColors.h"
#include "Multiplayer.h"

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
    const int c_joinCustomGameBtn       = 2102;
    const int c_joinGameBtnId           = 3203;
    const int c_exitGameBtnId           = 3204;

    const float c_clearErrorMsgTimer    = 3.0f;
}

Sample::Sample():
    m_appState(APP_MAIN_MENU),
    m_gameModeIndex(0),
    m_mapIndex(0),
    m_clearErrorMsgTimer(0.0f),
    m_multiplayerContext(0),
    m_isLeavingGame(false),
    m_isJoiningGame(false),
    m_isProtocolActivated(false)
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
    m_liveResources->Initialize(m_ui, m_ui->FindPanel<ATG::Overlay>(c_mainMenuPanelId));
    m_hudPanel = m_ui->FindPanel<ATG::HUD>(1000);
    m_mainMenuPanel = m_ui->FindPanel<ATG::Overlay>(c_mainMenuPanelId);
    m_sessionDetailsPanel = m_ui->FindPanel<ATG::Overlay>(c_sessionDetailsPanelId);
    m_hudPanel->Show();
    m_mainMenuPanel->Show();
    m_sessionDetailsPanel->Close();

    m_lobbyView = std::make_shared<SessionView>();
    m_gameView = std::make_shared<SessionView>();

    m_deviceResources->SetWindow(window, width, height, rotation);
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

    m_lobbyView->SetControls(m_sessionDetailsPanel, 3100);
    m_gameView->SetControls(m_sessionDetailsPanel, 3200);
    m_joinabilityLabel = dynamic_cast<ATG::TextLabel*>(m_sessionDetailsPanel->Find(3009));
    m_joinGameBtn = dynamic_cast<ATG::ImageButton*>(m_sessionDetailsPanel->Find(3203));

    // Join Lobby
    m_ui->FindControl<ImageButton>(c_mainMenuPanelId, c_joinLobbyBtn)->SetCallback([this](IPanel*, IControl*)
    {
        m_appState = APPSTATE::APP_CREATE_LOBBY;
        ChangeAppStates();
    });

    // Join Custom Game
    m_ui->FindControl<ImageButton>(c_mainMenuPanelId, c_joinCustomGameBtn)->SetCallback([this](IPanel*, IControl*)
    {
        m_appState = APPSTATE::APP_JOIN_GAME;
        ChangeAppStates();
    });

    // Exit Lobby
    m_ui->FindControl<ImageButton>(c_sessionDetailsPanelId, 3103)->SetCallback([this](IPanel*, IControl*) { RemoveLocalUser(); });

    // Update Lobby properties
    m_ui->FindControl<ImageButton>(c_sessionDetailsPanelId, 3104)->SetCallback([this](IPanel*, IControl*) { UpdateLobbyProperties(); });

    // Update Lobby host
    m_ui->FindControl<ImageButton>(c_sessionDetailsPanelId, 3105)->SetCallback([this](IPanel*, IControl*) { SetLobbyHost(); });

    // Join Game
    m_ui->FindControl<ImageButton>(c_sessionDetailsPanelId, 3203)->SetCallback([this](IPanel*, IControl*)
    {
        if (m_joinGameBtn->GetImageId() == c_joinGameBtnId)
        {
            if (!m_isJoiningGame && m_multiplayerManager->game_session() == nullptr)
            {
                m_isJoiningGame = true;
                JoinGameFromLobby();
            }
        }

        if (m_joinGameBtn->GetImageId() == c_exitGameBtnId)
        {
            if (!m_isLeavingGame)
            {
                m_isLeavingGame = true;
                LeaveGameSession();
            }
        }
    });

    // Update Game properties
    m_ui->FindControl<ImageButton>(c_sessionDetailsPanelId, 3205)->SetCallback([this](IPanel*, IControl*) { UpdateGameProperties(); });

    // Update Game host
    m_ui->FindControl<ImageButton>(c_sessionDetailsPanelId, 3206)->SetCallback([this](IPanel*, IControl*) { SetGameHost(); });

    // Invite Friends
    m_ui->FindControl<ImageButton>(c_sessionDetailsPanelId, 3004)->SetCallback([this](IPanel*, IControl*) { InviteFriends(); });

    // Update Joinability
    m_ui->FindControl<ImageButton>(c_sessionDetailsPanelId, 3005)->SetCallback([this](IPanel*, IControl*) { UpdateJoinability(); });
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
        m_joinGameBtn->SetImageId(c_joinGameBtnId);
        m_hudPanel->Close();
        m_mainMenuPanel->Close();
        m_sessionDetailsPanel->Show();
        AddLocalUser();
        break;

    case APP_JOIN_LOBBY:
        m_displayEventQueue.clear();
        m_joinGameBtn->SetImageId(c_joinGameBtnId);
        m_hudPanel->Close();
        m_mainMenuPanel->Close();
        m_sessionDetailsPanel->Show();
        break;

    case APP_JOIN_GAME:
        m_displayEventQueue.clear();
        m_joinGameBtn->SetVisible(c_exitGameBtnId);
        m_hudPanel->Close();
        m_mainMenuPanel->Close();
        m_sessionDetailsPanel->Show();
        JoinGame();
        break;

    case APP_SHOW_FRIEND_GAMES:
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

    if (m_isProtocolActivated &&
        m_liveResources->GetUser() != nullptr && m_liveResources->GetUser()->is_signed_in())
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
    auto kb = m_keyboard->GetState();
    m_keyboardButtons.Update(kb);

    if (!m_ui->Update(elapsedTime, *m_mouse, *m_keyboard))
    {
        if (kb.Escape)
        {
            Windows::ApplicationModel::Core::CoreApplication::Exit();
        }

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
            if (m_keyboardButtons.IsKeyPressed(Keyboard::A))
            {
                m_liveResources->SignIn();
            }
        }
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
        m_joinabilityLabel->SetText(JoinabilityStateText[static_cast<int>(m_multiplayerManager->joinability())]);
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

    m_lobbyView->Clear();
    m_gameView->Clear();
    m_eventQueueConsole->Clear();
    m_perfConsole->Clear();

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

void Sample::OnProtocolActivated(Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ protocolArgs)
{
    m_isProtocolActivated = true;
    m_protocolActivatedEventArgs = protocolArgs;

    if (m_multiplayerManager == nullptr || 
        (m_liveResources != nullptr && m_liveResources->GetUser() != nullptr && !m_liveResources->GetUser()->is_signed_in()))
    {
        // Game hasn't been intialized yet. We will process the protocol after initialization.
        return;
    }

    HandleProtocolActivation();
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
    width = 1920;
    height = 1080;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Sample::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    m_eventQueueConsole = std::make_shared<DX::TextConsole>(context, L"Courier_16.spritefont");
    m_errorMsgConsole = std::make_shared<DX::TextConsole>(context, L"Courier_16.spritefont");
    m_perfConsole = std::make_shared<DX::TextConsole>(context, L"Courier_16.spritefont");

    m_joinGameImage = std::make_unique<DX::Texture>(m_deviceResources->GetD3DDevice(), L"Assets//btnJoin.png");
    m_exitGameImage = std::make_unique<DX::Texture>(m_deviceResources->GetD3DDevice(), L"Assets//btnExitON.png");
    m_ui->RegisterImage(c_joinGameBtnId, m_joinGameImage->Get());
    m_ui->RegisterImage(c_exitGameBtnId, m_exitGameImage->Get());

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
    m_errorMsgConsole->SetForegroundColor(DirectX::Colors::Red);

    m_perfConsole->SetWindow({ 1100, 600, 1750, 900 });
    m_perfConsole->SetForegroundColor(ATG::Colors::OffWhite);
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
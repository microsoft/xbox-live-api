// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Game.h"
#include "GamePad.h"
#include "Support\PerformanceCounters.h"
#include "AsyncIntegration.h"

using namespace DirectX;

using Microsoft::WRL::ComPtr;
using namespace xbox::services::social::manager;

Game* g_sampleInstance = nullptr;
std::mutex Game::m_displayEventQueueLock;

#define COLUMN_1_X                      60
#define COLUMN_2_X                      300
#define ACTION_BUTONS_Y                 60
#if PERF_COUNTERS
#define PERF_ROW_OFFSET                 50
#define PERF_X_POS                      900
#define SOCIAL_GROUP_Y                  300
#else
#define ACHIEVEMENTS_Y                  200
#endif

#define ACHIEVEMENT_ID                  "1"
#define ACHIEVEMENT_PROGRESS            10
#define SKIP_ITEMS                      0
#define MAX_ITEMS                       5

Game::Game()
{
    g_sampleInstance = this;
    m_userController.reset(new UserController());
    m_userController->Initialize();
    m_deviceResources = std::make_shared<DX::DeviceResources>();
}

Game::~Game()
{
    CleanupXboxLive();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(IUnknown* window)
{
    m_gamePad = std::make_unique<GamePad>();

    m_deviceResources->SetWindow(window);

    m_deviceResources->CreateDeviceResources();  
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    InitializeXboxLive();
}

void Game::Reset()
{
}

#pragma region Frame Update
// Executes basic render loop.
void Game::Tick()
{
    m_timer.Tick([this]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Update");

    float elapsedTime = float(timer.GetElapsedSeconds());
    UNREFERENCED_PARAMETER(elapsedTime);

    auto pad = m_gamePad->GetState(0);
    if (pad.IsConnected())
    {
        m_gamePadButtons.Update(pad);

        if (pad.IsViewPressed())
        {
            Windows::ApplicationModel::Core::CoreApplication::Exit();
        }
    }
    else
    {
        m_gamePadButtons.Reset();
    }

    UpdateGame();

    PIXEndEvent();
}

void Game::UpdateGame()
{
    // Call one of the DrainAsyncCompletionQueue* helper functions
    // For example: 
    // DrainAsyncCompletionQueue(m_queue, 5);
    // DrainAsyncCompletionQueueUntilEmpty(m_queue);
    double timeoutMilliseconds = 0.5f;
    DrainAsyncCompletionQueueWithTimeout(m_queue, timeoutMilliseconds);

    if (m_gamePadButtons.a == GamePad::ButtonStateTracker::PRESSED)
    {
        GetAchievementsForTitle(SKIP_ITEMS, MAX_ITEMS);
    }

    if (m_gamePadButtons.b == GamePad::ButtonStateTracker::PRESSED)
    {
        AchievementResultsGetNext(m_achievementsResult, MAX_ITEMS);
    }

    if (m_gamePadButtons.x == GamePad::ButtonStateTracker::PRESSED)
    {
        GetAchievement(ACHIEVEMENT_ID);
    }

    if (m_gamePadButtons.y == GamePad::ButtonStateTracker::PRESSED)
    {
        UpdateAchievement(ACHIEVEMENT_ID, ACHIEVEMENT_PROGRESS);
    }
}

void Game::Log(string_t log)
{
    std::lock_guard<std::mutex> guard(m_displayEventQueueLock);
    m_displayEventQueue.push_back(log);
    if (m_displayEventQueue.size() > 15)
    {
        m_displayEventQueue.erase(m_displayEventQueue.begin());
    }
}

#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
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

    m_sprites->Begin();
    RenderUI();
    m_sprites->End();

    PIXEndEvent(context);

    PIXBeginEvent(context, PIX_COLOR_DEFAULT, L"Present");
    m_deviceResources->Present();
    m_graphicsMemory->Commit();
    PIXEndEvent(context);
}

void Game::RenderUI()
{
    XMVECTORF32 TITLE_COLOR = Colors::Yellow;
    XMVECTORF32 TEXT_COLOR = Colors::White;

    FLOAT fTextHeight = 25.0f; 
    FLOAT scale = 0.4f;

    m_font->DrawString(m_sprites.get(), L"Achievements Sample", XMFLOAT2(COLUMN_1_X * 15.0f, 10), TITLE_COLOR, 0.0f, XMFLOAT2(0, 0), 0.5f);

    RenderMenuOptions(scale, TEXT_COLOR);
    RenderAchievements(COLUMN_1_X, ACHIEVEMENTS_Y, fTextHeight, scale, TEXT_COLOR);
    RenderEventLog(COLUMN_1_X, ACHIEVEMENTS_Y, fTextHeight, scale, TEXT_COLOR);

#if PERF_COUNTERS
    RenderPerfCounters(PERF_X_POS, PERF_ROW_OFFSET, fTextHeight, scale, TEXT_COLOR);
#endif
}

std::wstring AchievementProgressToString(XblAchievement achievement)
{
    stringstream_t stream;

    if (achievement.progressState == XblAchievementProgressState_Achieved)
    {
        char buff[20];
        strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&achievement.progression.timeUnlocked));
        stream << "Achieved (" << buff << ")";
    }
    else if (achievement.progressState == XblAchievementProgressState_InProgress)
    {
        stream << "Started";
        stream << "(" << achievement.progression.requirements[0].currentProgressValue << "/" << achievement.progression.requirements[0].targetProgressValue << ")";
    }
    else if (achievement.progressState == XblAchievementProgressState_NotStarted)
    {
        stream << "Not Started";
    }
    else if (achievement.progressState == XblAchievementProgressState_Unknown)
    {
        stream << "Unknown";
    }

    return stream.str();
}

void Game::RenderAchievements(
    FLOAT fGridXColumn1,
    FLOAT fGridY,
    FLOAT fTextHeight,
    FLOAT scale,
    const DirectX::XMVECTORF32& TEXT_COLOR
    )
{
    if (!m_achievementsResultSet) return;

    float verticalBaseOffset = fTextHeight;

    bool hasNext;
    XblAchievementsResultHasNext(m_achievementsResult, &hasNext);

    m_font->DrawString(m_sprites.get(), hasNext ? L"Next page available" : L"No more remaining pages", XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

    verticalBaseOffset += fTextHeight;

    XblAchievement* achievements;
    uint32_t achievementsCount;
    XblAchievementsResultGetAchievements(m_achievementsResult, &achievements, &achievementsCount);

    for (uint32_t i = 0; i < achievementsCount; i++)
    {
        stringstream_t stream;
        stream << (i + 1) << ") " << achievements[i].name << "\n";
        stream << "State: " << AchievementProgressToString(achievements[i]) << "\n\n";

        m_font->DrawString(m_sprites.get(), stream.str().c_str(), XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

        verticalBaseOffset += fTextHeight * 2;
    }
}

void Game::RenderMenuOptions(
    FLOAT scale,
    const FXMVECTOR& TEXT_COLOR
    )
{
    WCHAR text[1024];
    swprintf_s(text, ARRAYSIZE(text),
        L"Press A to get achievements for this title.\n"
        L"Press B to get the next page of achievements.\n"
        L"Press X to get a specific achievement.\n"
        L"Press Y to update a specific achievement.\n"
        );

    m_font->DrawString(m_sprites.get(), text, XMFLOAT2(COLUMN_1_X, ACTION_BUTONS_Y), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
}

void Game::RenderEventLog(
    FLOAT fGridXColumn1,
    FLOAT fGridY,
    FLOAT fTextHeight,
    FLOAT scale,
    const FXMVECTOR& TEXT_COLOR
    )
{
    WCHAR text[1024];
    FXMVECTOR TITLE_COLOR = Colors::White;

    fGridY -= 50;

    std::lock_guard<std::mutex> guard(g_sampleInstance->m_displayEventQueueLock);
    if (g_sampleInstance->m_displayEventQueue.size() > 0)
    {
        swprintf_s(text, 128, L"EVENTS:");
        m_font->DrawString(m_sprites.get(), text, XMFLOAT2(15 * fGridXColumn1, fGridY - fTextHeight), Colors::Yellow, 0.0f, XMFLOAT2(0, 0), scale);
    }

    for (unsigned int i = 0; i < g_sampleInstance->m_displayEventQueue.size(); ++i)
    {
        m_font->DrawString(
            m_sprites.get(), 
            g_sampleInstance->m_displayEventQueue[i].c_str(),
            XMFLOAT2(15 * fGridXColumn1, fGridY + (i * fTextHeight) * 1.0f), 
            TEXT_COLOR,
            0.0f, XMFLOAT2(0, 0), scale
            );
    }
}

void Game::RenderPerfCounters(
    FLOAT fGridXColumn1,
    FLOAT fGridY,
    FLOAT fTextHeight,
    FLOAT scale,
    const XMVECTORF32& TEXT_COLOR
    )
{
    WCHAR text[1024];
    auto perfInstance = performance_counters::get_singleton_instance();
    float verticalBaseOffset = 2 * fTextHeight;

    m_font->DrawString(m_sprites.get(), L"TYPE", XMFLOAT2(fGridXColumn1, fGridY), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
    m_font->DrawString(m_sprites.get(), L"AVG", XMFLOAT2(fGridXColumn1 + 150.0f, fGridY), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
    m_font->DrawString(m_sprites.get(), L"MIN", XMFLOAT2(fGridXColumn1 + 300.0f, fGridY), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
    m_font->DrawString(m_sprites.get(), L"MAX", XMFLOAT2(fGridXColumn1 + 450.0f, fGridY), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
    m_font->DrawString(m_sprites.get(), L"_________________________________________________________", XMFLOAT2(fGridXColumn1, fGridY + 10.0f), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

    auto noUpdateInstance = perfInstance->get_capture_instace(L"no_updates");
    if (noUpdateInstance != nullptr)
    {
        swprintf_s(text, ARRAYSIZE(text), L"No updates:");
        m_font->DrawString(m_sprites.get(), text, XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

        swprintf_s(text, ARRAYSIZE(text), L"%s", noUpdateInstance->average_time().ToString()->Data());
        m_font->DrawString(m_sprites.get(), text, XMFLOAT2(fGridXColumn1 + 150.0f, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

        swprintf_s(text, ARRAYSIZE(text), L"%s", noUpdateInstance->min_time().ToString()->Data());
        m_font->DrawString(m_sprites.get(), text, XMFLOAT2(fGridXColumn1 + 300.0f, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

        swprintf_s(text, ARRAYSIZE(text), L"%s", noUpdateInstance->max_time().ToString()->Data());
        m_font->DrawString(m_sprites.get(), text, XMFLOAT2(fGridXColumn1 + 450.0f, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

        verticalBaseOffset += fTextHeight;
    }

    auto updatedInstance = perfInstance->get_capture_instace(L"updates");
    if (updatedInstance != nullptr)
    {
        swprintf_s(text, ARRAYSIZE(text), L"With Updates:");
        m_font->DrawString(m_sprites.get(), text, XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

        swprintf_s(text, ARRAYSIZE(text), L"%s", updatedInstance->average_time().ToString()->Data());
        m_font->DrawString(m_sprites.get(), text, XMFLOAT2(fGridXColumn1 + 150.0f, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

        swprintf_s(text, ARRAYSIZE(text), L"%s", updatedInstance->min_time().ToString()->Data());
        m_font->DrawString(m_sprites.get(), text, XMFLOAT2(fGridXColumn1 + 300.0f, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

        swprintf_s(text, ARRAYSIZE(text), L"%s", updatedInstance->max_time().ToString()->Data());
        m_font->DrawString(m_sprites.get(), text, XMFLOAT2(fGridXColumn1 + 450.0f, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

        verticalBaseOffset += fTextHeight;
    }
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    PIXBeginEvent(context, PIX_COLOR_DEFAULT, L"Clear");

    // Clear the views
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
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
void Game::OnSuspending()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    context->Suspend(0);
}

void Game::OnResuming()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    context->Resume();
    m_timer.ResetElapsedTime();
    m_gamePadButtons.Reset();
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    m_graphicsMemory = std::make_unique<GraphicsMemory>(device, m_deviceResources->GetBackBufferCount());

    auto context = m_deviceResources->GetD3DDeviceContext();
    m_sprites.reset(new SpriteBatch(context));
    m_font.reset(new SpriteFont(device, L"assets\\italic.spritefont"));
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
}
#pragma endregion

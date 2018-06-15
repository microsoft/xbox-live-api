// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Support\UserController.h"
#include "Support\DeviceResources.h"
#include "Support\StepTimer.h"
#include "CommonStates.h"
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "Model.h"
#include "PrimitiveBatch.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"

// A basic sample implementation that creates a D3D11 device and
// provides a render loop.
class Game
{
public:

    Game();
    ~Game();

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

    void Log(string_t log);

    std::vector< std::wstring > m_displayEventQueue;
    static std::mutex m_displayEventQueueLock;

private:

    void Update(DX::StepTimer const& timer);
    void RenderUI();
    
    void RenderAchievements(
        FLOAT fGridXColumn1,
        FLOAT fGridY,
        FLOAT fTextHeight,
        FLOAT scale,
        const DirectX::XMVECTORF32& TEXT_COLOR
        );

    void RenderMenuOptions(
        FLOAT scale,
        const DirectX::FXMVECTOR& TEXT_COLOR
        );

    void RenderEventLog(
        FLOAT fGridXColumn1,
        FLOAT fGridY,
        FLOAT fTextHeight,
        FLOAT scale,
        const DirectX::FXMVECTOR& TEXT_COLOR
        );

    void RenderPerfCounters(
        FLOAT fGridXColumn1,
        FLOAT fGridY,
        FLOAT fTextHeight,
        FLOAT scale,
        const DirectX::XMVECTORF32& TEXT_COLOR
        );

    void UpdateGame();
    void Reset();
    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    std::shared_ptr<DX::DeviceResources>        m_deviceResources;
    DX::StepTimer                               m_timer;
    std::unique_ptr<DirectX::GamePad>           m_gamePad;
    DirectX::GamePad::ButtonStateTracker        m_gamePadButtons;
    std::unique_ptr<DirectX::GraphicsMemory>    m_graphicsMemory;
    std::unique_ptr<DirectX::SpriteBatch>       m_sprites;
    std::unique_ptr<DirectX::SpriteFont>        m_font;

    std::shared_ptr<UserController> m_userController;
    
private:
    async_queue_handle_t m_queue;
    uint32_t m_asyncQueueCallbackToken;
    HANDLE m_hBackgroundThread;

    xbl_context_handle m_xboxLiveContext;
    uint64_t m_xuid;
    const XblAppConfig* m_config;

    void InitializeXboxLive();
    void CleanupXboxLive();

private:
    xbl_achievements_result_handle m_achievementsResult;
    bool m_achievementsResultSet;
    void SetAchievementsResult(
        _In_ xbl_achievements_result_handle achievementsResult
    );

    void GetAchievementsForTitle(
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems
    );

    void AchievementResultsGetNext(
        _In_ xbl_achievements_result_handle resultHandle,
        _In_ uint32_t maxItems
    );

    void GetAchievement(
        _In_ const char* achievementId
    );

    void UpdateAchievement(
        _In_ const char* achievementId,
        _In_ uint32_t percentComplete
    );
};

extern Game* g_sampleInstance;

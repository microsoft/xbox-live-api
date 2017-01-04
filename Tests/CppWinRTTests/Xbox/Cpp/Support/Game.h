//--------------------------------------------------------------------------------------
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

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
    std::vector<std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>> GetSocialGroups();

    std::vector< std::wstring > m_displayEventQueue;
    static std::mutex m_displayEventQueueLock;

private:

    void Update(DX::StepTimer const& timer);
    void RenderUI();

    void RenderSocialGroupList(
        FLOAT fGridXColumn1,
        FLOAT fGridXColumn2,
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

    std::shared_ptr<xbox::services::social::manager::social_manager> m_socialManager;
    std::vector<string_t> m_xuidsInCustomSocialGroup;
    std::vector<std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>> m_socialGroups;
    static std::mutex m_socialManagerLock;

    bool m_allFriends;
    bool m_onlineFriends;
    bool m_allFavs;
    bool m_onlineInTitle;
    bool m_customList;
    bool m_isInitialized;

private:
    void InitializeSocialManager(Windows::Foundation::Collections::IVectorView<Windows::Xbox::System::User^>^ userList);
public:
    void AddUserToSocialManager(
        _In_ Windows::Xbox::System::User^ user
        );
    void RemoveUserFromSocialManager(
        _In_ Windows::Xbox::System::User^ user
        );
private:
    void CreateSocialGroupFromList(
        _In_ Windows::Xbox::System::User^ user,
        _In_ std::vector<string_t> xuidList
        );
    void DestorySocialGroupFromList(
        _In_ Windows::Xbox::System::User^ user
        );
    void CreateSocialGroupFromFilters(
        _In_ Windows::Xbox::System::User^ user,
        _In_ xbox::services::social::manager::presence_filter presenceDetailLevel,
        _In_ xbox::services::social::manager::relationship_filter filter
        );
    void DestroySocialGroup(
        _In_ Windows::Xbox::System::User^ user,
        _In_ xbox::services::social::manager::presence_filter presenceDetailLevel,
        _In_ xbox::services::social::manager::relationship_filter filter
        );
    void UpdateSocialManager();
    void LogSocialEventList(std::vector<xbox::services::social::manager::social_event> eventList);
    void CreateSocialGroupsBasedOnUI(
        _In_ Windows::Xbox::System::User^ user
        );

    void UpdateSocialGroupForAllUsers(
        _In_ bool toggle,
        _In_ xbox::services::social::manager::presence_filter presenceFilter,
        _In_ xbox::services::social::manager::relationship_filter relationshipFilter
        );
    void UpdateSocialGroup(
        _In_ Windows::Xbox::System::User^ user,
        _In_ bool toggle,
        _In_ xbox::services::social::manager::presence_filter presenceFilter,
        _In_ xbox::services::social::manager::relationship_filter relationshipFilter
        );

    void UpdateSocialGroupOfListForAllUsers(
        _In_ bool toggle
        );
    void UpdateSocialGroupOfList(
        _In_ Windows::Xbox::System::User^ user,
        _In_ bool toggle
        );

};

extern Game* g_sampleInstance;

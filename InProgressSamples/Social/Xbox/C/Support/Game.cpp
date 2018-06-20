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
std::mutex Game::m_socialManagerLock;

#define COLUMN_1_X                      60
#define COLUMN_2_X                      300
#define ACTION_BUTONS_Y                 60
#if PERF_COUNTERS
#define PERF_ROW_OFFSET                 50
#define PERF_X_POS                      900
#define SOCIAL_GROUP_Y                  300
#else
#define SOCIAL_GROUP_Y                  200
#endif

Game::Game() :
    m_allFriends(false),
    m_onlineFriends(false),
    m_allFavs(false),
    m_onlineInTitle(false),
    m_customList(false),
    m_isInitialized(false),
    m_userAdded(false)
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
    m_allFriends = false;
    m_onlineFriends = false;
    m_allFavs = false;
    m_onlineInTitle = false;
    m_customList = false;

    m_socialGroups.clear();
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

    UpdateSocialManager();

    if (m_gamePadButtons.a == GamePad::ButtonStateTracker::PRESSED)
    {
        if (!m_userAdded)
        {
            Log(L"Local user must be added first");
            return;
        }
        m_allFriends = !m_allFriends;
        UpdateSocialGroupForAllUsers(m_allFriends, XblPresenceFilter_All, XblRelationshipFilter_Friends);
    }

    if (m_gamePadButtons.b == GamePad::ButtonStateTracker::PRESSED)
    {
        if (!m_userAdded)
        {
            Log(L"Local user must be added first");
            return;
        }
        m_onlineFriends = !m_onlineFriends;
        UpdateSocialGroupForAllUsers(m_onlineFriends, XblPresenceFilter_AllOnline, XblRelationshipFilter_Friends);
    }

    if (m_gamePadButtons.x == GamePad::ButtonStateTracker::PRESSED)
    {
        if (!m_userAdded)
        {
            Log(L"Local user must be added first");
            return;
        }
        m_allFavs = !m_allFavs;
        UpdateSocialGroupForAllUsers(m_allFavs, XblPresenceFilter_All, XblRelationshipFilter_Favorite);
    }

    if (m_gamePadButtons.y == GamePad::ButtonStateTracker::PRESSED)
    {
        if (!m_userAdded)
        {
            Log(L"Local user must be added first");
            return;
        }
        m_onlineInTitle = !m_onlineInTitle;
        UpdateSocialGroupForAllUsers(m_onlineInTitle, XblPresenceFilter_TitleOnline, XblRelationshipFilter_Friends);
    }

    if (m_gamePadButtons.leftShoulder == GamePad::ButtonStateTracker::PRESSED)
    {
        if (!m_userAdded)
        {
            Log(L"Local user must be added first");
            return;
        }
        m_customList = !m_customList;
        UpdateSocialGroupOfListForAllUsers(m_customList);
    }

    if(m_gamePadButtons.start == GamePad::ButtonStateTracker::PRESSED)
    {
        if (!m_isInitialized)
        {
            InitializeSocialManager(m_userController->GetUserList());
        }
        else
        {
            for (auto user : m_userController->GetUserList())
            {
                g_sampleInstance->RemoveUserFromSocialManager(user);
            }

            Reset();
        }

        m_isInitialized = !m_isInitialized;
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

string_t
ConvertEventTypeToString(XblSocialManagerEventType eventType)
{
    switch (eventType)
    {
    case XblSocialManagerEventType_UsersAddedToSocialGraph: return _T("users_added");
    case XblSocialManagerEventType_UsersRemovedFromSocialGraph: return _T("users_removed");
    case XblSocialManagerEventType_PresenceChanged: return _T("presence_changed");
    case XblSocialManagerEventType_ProfilesChanged: return _T("profiles_changed");
    case XblSocialManagerEventType_SocialRelationshipsChanged: return _T("social_relationships_changed");
    case XblSocialManagerEventType_LocalUserAdded: return _T("local_user_added");
    case XblSocialManagerEventType_LocalUserRemoved: return _T("local user removed");
    case XblSocialManagerEventType_SocialUserGroupLoaded: return _T("social_user_group_loaded");
    case XblSocialManagerEventType_SocialUserGroupUpdated: return _T("social_user_group_updated");
    default: return _T("unknown");
    }
}

void
Game::LogSocialEventList(XblSocialManagerEvent* events, uint32_t eventCount)
{
    for (uint32_t i = 0; i < eventCount; ++i)
    {
        auto socialEvent = events[i];

        stringstream_t source;
        if (socialEvent.err)
        {
            source << _T("Event:");
            source << ConvertEventTypeToString(socialEvent.eventType);
            source << _T(" ErrorCode: ");
            source << socialEvent.err;
        }
        else
        {
            source << _T("Event: ");
            source << ConvertEventTypeToString(socialEvent.eventType);
            if (socialEvent.usersAffectedCount > 0)
            {
                std::vector<uint64_t> affectedUsers(socialEvent.usersAffectedCount);

                XblSocialManagerEventGetUsersAffected(&socialEvent, affectedUsers.data());

                source << _T(" UserAffected: ");
                for (uint32_t j = 0; j < socialEvent.usersAffectedCount; ++j)
                {
                    source << affectedUsers[j] << _T(", ");
                }
            }
        }
        Log(source.str());
    }
}

void
Game::CreateSocialGroupsBasedOnUI(
    _In_ Windows::Xbox::System::User^ user
    )
{
    UpdateSocialGroup(user, m_allFriends, XblPresenceFilter_All, XblRelationshipFilter_Friends);
    UpdateSocialGroup(user, m_onlineFriends, XblPresenceFilter_AllOnline, XblRelationshipFilter_Friends);
    UpdateSocialGroup(user, m_allFavs, XblPresenceFilter_All, XblRelationshipFilter_Favorite);
    UpdateSocialGroup(user, m_onlineInTitle, XblPresenceFilter_AllTitle, XblRelationshipFilter_Friends);
    UpdateSocialGroupOfList(user, m_customList);
}

void Game::UpdateSocialGroupForAllUsers(
    _In_ bool toggle,
    _In_ XblPresenceFilter presenceFilter,
    _In_ XblRelationshipFilter relationshipFilter
    )
{
    for (const auto& user : m_userController->GetUserList())
    {
        UpdateSocialGroup(user, toggle, presenceFilter, relationshipFilter);
    }
}

void Game::UpdateSocialGroup(
    _In_ Windows::Xbox::System::User^ user,
    _In_ bool toggle,
    _In_ XblPresenceFilter presenceFilter,
    _In_ XblRelationshipFilter relationshipFilter
    )
{
    if (toggle)
    {
        CreateSocialGroupFromFilters(user, presenceFilter, relationshipFilter);
    }
    else
    {
        DestroySocialGroup(user, presenceFilter, relationshipFilter);
    }
}

void Game::UpdateSocialGroupOfListForAllUsers(_In_ bool toggle)
{
    for (const auto& user : m_userController->GetUserList())
    {
        UpdateSocialGroupOfList(user, toggle);
    }
}

void Game::UpdateSocialGroupOfList(
    _In_ Windows::Xbox::System::User^ user,
    _In_ bool toggle
    )
{
    if (toggle)
    {
        m_xuidsInCustomSocialGroup.clear();

        // Change these XUIDs to a list of users that your title to track from multiplayer, etc
        m_xuidsInCustomSocialGroup.push_back(2814674724269793);
        m_xuidsInCustomSocialGroup.push_back(2814667276146249);
        m_xuidsInCustomSocialGroup.push_back(2814666456633892);
        m_xuidsInCustomSocialGroup.push_back(2814672389110410);

        CreateSocialGroupFromList(user, m_xuidsInCustomSocialGroup);
    }
    else
    {
        DestorySocialGroup(user);
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

    m_font->DrawString(m_sprites.get(), L"Social Manager Sample", XMFLOAT2(COLUMN_1_X * 15.0f, 10), TITLE_COLOR, 0.0f, XMFLOAT2(0, 0), 0.5f);

    RenderSocialGroupList(COLUMN_1_X, COLUMN_2_X, SOCIAL_GROUP_Y, fTextHeight, scale, TEXT_COLOR);
    RenderMenuOptions(scale, TEXT_COLOR);
    RenderEventLog(COLUMN_1_X, SOCIAL_GROUP_Y, fTextHeight, scale, TEXT_COLOR);

#if PERF_COUNTERS
    RenderPerfCounters(PERF_X_POS, PERF_ROW_OFFSET, fTextHeight, scale, TEXT_COLOR);
#endif
}

std::wstring
ConvertPresenceUserStateToString(
    _In_ XblPresenceUserState presenceState
    )
{
    switch (presenceState)
    {
        case XblPresenceUserState_Away: return _T("away");
        case XblPresenceUserState_Offline: return _T("offline");
        case XblPresenceUserState_Online: return _T("online");
        default:
        case XblPresenceUserState_Unknown: return _T("unknown");
    }
}

std::wstring
ConvertPresenceFilterToString(_In_ XblPresenceFilter presenceFilter)
{
    switch (presenceFilter)
    {
        case XblPresenceFilter_Unknown: return _T("unknown");
        case XblPresenceFilter_TitleOnline: return _T("title_online");
        case XblPresenceFilter_TitleOffline: return _T("title_offline");
        case XblPresenceFilter_AllOnline: return _T("all_online");
        case XblPresenceFilter_AllOffline: return _T("all_offline");
        case XblPresenceFilter_AllTitle: return _T("all_title");
        default:
        case XblPresenceFilter_All: return _T("all");
    }
}

std::wstring
ConvertRelationshipFilterToString(_In_ XblRelationshipFilter relationshipFilter)
{
    switch (relationshipFilter)
    {
        case XblRelationshipFilter_Favorite: return _T("favorite");
        default:
        case XblRelationshipFilter_Friends: return _T("friends");
    }
}

void
Game::RenderSocialGroupList(
    FLOAT fGridXColumn1,
    FLOAT fGridXColumn2,
    FLOAT fGridY,
    FLOAT fTextHeight,
    FLOAT scale,
    const DirectX::XMVECTORF32& TEXT_COLOR
    )
{
    WCHAR text[1024];
    float verticalBaseOffset = 2 * fTextHeight;


    std::lock_guard<std::mutex> guard(m_socialManagerLock);
    for (const auto& group : m_socialGroups)
    {
        m_font->DrawString(m_sprites.get(), L"_________________________________________", XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
        verticalBaseOffset += fTextHeight;
        if(group->socialUserGroupType == XblSocialUserGroupType_FilterType)
        {
            swprintf_s(text, ARRAYSIZE(text), L"Group from filter: %s %s",
                ConvertPresenceFilterToString(group->presenceFilterOfGroup).c_str(),
                ConvertRelationshipFilterToString(group->relationshipFilterOfGroup).c_str()
                );
            m_font->DrawString(m_sprites.get(), text, XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
            verticalBaseOffset += fTextHeight;
        }
        else
        {
            m_font->DrawString(m_sprites.get(), L"Group from custom list", XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
            verticalBaseOffset += fTextHeight;
        }

        if (group->usersCount == 0)
        {
            m_font->DrawString(m_sprites.get(), L"No friends found", XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
            verticalBaseOffset += fTextHeight;
        }
        else
        {
            std::vector<XblSocialManagerUser> userList(group->usersCount);
            XblSocialManagerUserGroupGetUsers(group, group->usersCount, userList.data());

            for (uint32_t i = 0; i < group->usersCount; ++i)
            {
                auto& user = userList[i];
                m_font->DrawString(m_sprites.get(), utility::conversions::to_utf16string(user.gamertag).c_str(), XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
                m_font->DrawString(m_sprites.get(), ConvertPresenceUserStateToString(user.presenceRecord.userState).c_str(), XMFLOAT2(fGridXColumn2, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

                verticalBaseOffset += fTextHeight;
            }
        }
    }
}


void Game::RenderMenuOptions(
    FLOAT scale,
    const FXMVECTOR& TEXT_COLOR
    )
{
    WCHAR text[1024];
    swprintf_s(text, ARRAYSIZE(text),
        L"Press Menu to add local user/remove local user (%s).\n"
        L"Press A to toggle social group for all friends (%s).\n"
        L"Press B to toggle social group for online friends (%s).\n"
        L"Press X to toggle social group for all favorites (%s).\n"
        L"Press Y to toggle social group for online in title (%s).\n"
        L"Press Left Shoulder to toggle social group for hard coded custom list (%s).\n",
        m_isInitialized ? L"Local User Added" : L"Local User Removed",
        m_allFriends ? L"On" : L"Off",
        m_onlineFriends ? L"On" : L"Off",
        m_allFavs ? L"On" : L"Off",
        m_onlineInTitle ? L"On" : L"Off",
        m_customList ? L"On" : L"Off"
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
        swprintf_s(text, 128, L"SOCIAL EVENTS:");
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

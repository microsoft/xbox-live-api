//--------------------------------------------------------------------------------------
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "Game.h"
#include "GamePad.h"
#include "Support\PerformanceCounters.h"

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
    m_isInitialized(false)
{
    g_sampleInstance = this;
    m_userController.reset(new UserController());
    m_userController->Initialize();
    m_deviceResources = std::make_shared<DX::DeviceResources>();
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
}

void Game::Reset()
{
    m_socialManager = nullptr;
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
    if (m_socialManager != nullptr)
    {
        UpdateSocialManager();
    }

    if (m_gamePadButtons.a == GamePad::ButtonStateTracker::PRESSED)
    {
        if (m_socialManager == nullptr)
        {
            Log(L"Local user must be added first");
            return;
        }
        m_allFriends = !m_allFriends;
        UpdateSocialGroupForAllUsers(m_allFriends, presence_filter::all, relationship_filter::friends);
    }

    if (m_gamePadButtons.b == GamePad::ButtonStateTracker::PRESSED)
    {
        if (m_socialManager == nullptr)
        {
            Log(L"Local user must be added first");
            return;
        }
        m_onlineFriends = !m_onlineFriends;
        UpdateSocialGroupForAllUsers(m_onlineFriends, presence_filter::all_online, relationship_filter::friends);
    }

    if (m_gamePadButtons.x == GamePad::ButtonStateTracker::PRESSED)
    {
        if (m_socialManager == nullptr)
        {
            Log(L"Local user must be added first");
            return;
        }
        m_allFavs = !m_allFavs;
        UpdateSocialGroupForAllUsers(m_allFavs, presence_filter::all, relationship_filter::favorite);
    }

    if (m_gamePadButtons.y == GamePad::ButtonStateTracker::PRESSED)
    {
        if (m_socialManager == nullptr)
        {
            Log(L"Local user must be added first");
            return;
        }
        m_onlineInTitle = !m_onlineInTitle;
        UpdateSocialGroupForAllUsers(m_onlineInTitle, presence_filter::title_online, relationship_filter::friends);
    }

    if (m_gamePadButtons.leftShoulder == GamePad::ButtonStateTracker::PRESSED)
    {
        if (m_socialManager == nullptr)
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
ConvertEventTypeToString(xbox::services::social::manager::social_event_type eventType)
{
    switch (eventType)
    {
        case xbox::services::social::manager::social_event_type::users_added_to_social_graph: return _T("users_added");
        case xbox::services::social::manager::social_event_type::users_removed_from_social_graph: return _T("users_removed");
        case xbox::services::social::manager::social_event_type::presence_changed: return _T("presence_changed");
        case xbox::services::social::manager::social_event_type::profiles_changed: return _T("profiles_changed");
        case xbox::services::social::manager::social_event_type::social_relationships_changed: return _T("social_relationships_changed");
        case xbox::services::social::manager::social_event_type::local_user_added: return _T("local_user_added");
        case xbox::services::social::manager::social_event_type::local_user_removed: return _T("local user removed");
        case xbox::services::social::manager::social_event_type::social_user_group_loaded: return _T("social_user_group_loaded");
        case xbox::services::social::manager::social_event_type::social_user_group_updated: return _T("social_user_group_updated");
        default:
        case xbox::services::social::manager::social_event_type::unknown: return _T("unknown");
    }
}

void
Game::LogSocialEventList(std::vector<xbox::services::social::manager::social_event> eventList)
{
    for (const auto& socialEvent : eventList)
    {
        stringstream_t source;
        if (socialEvent.err())
        {
            source << _T("Event:");
            source << ConvertEventTypeToString(socialEvent.event_type());
            source << _T(" ErrorCode: ");
            source << utility::conversions::to_utf16string(socialEvent.err().message());
            source << _T(" ErrorMessage:");
            source << utility::conversions::to_utf16string(socialEvent.err_message());
        }
        else
        {
            source << _T("Event: ");
            source << ConvertEventTypeToString(socialEvent.event_type());
            if (socialEvent.users_affected().size() > 0)
            {
                source << _T(" UserAffected: ");
                for (const auto& u : socialEvent.users_affected())
                {
                    source << u.xbox_user_id();
                    source << _T(", ");
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
    UpdateSocialGroup(user, m_allFriends, presence_filter::all, relationship_filter::friends);
    UpdateSocialGroup(user, m_onlineFriends, presence_filter::all_online, relationship_filter::friends);
    UpdateSocialGroup(user, m_allFavs, presence_filter::all, relationship_filter::favorite);
    UpdateSocialGroup(user, m_onlineInTitle, presence_filter::title_online, relationship_filter::friends);
    UpdateSocialGroupOfList(user, m_customList);
}

void Game::UpdateSocialGroupForAllUsers(
    _In_ bool toggle,
    _In_ presence_filter presenceFilter,
    _In_ relationship_filter relationshipFilter
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
    _In_ presence_filter presenceFilter,
    _In_ relationship_filter relationshipFilter
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
        m_xuidsInCustomSocialGroup.push_back(L"2814674724269793");
        m_xuidsInCustomSocialGroup.push_back(L"2814667276146249");
        m_xuidsInCustomSocialGroup.push_back(L"2814666456633892");
        m_xuidsInCustomSocialGroup.push_back(L"2814672389110410");

        CreateSocialGroupFromList(user, m_xuidsInCustomSocialGroup);
    }
    else
    {
        DestorySocialGroupFromList(user);
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
    _In_ xbox::services::presence::user_presence_state presenceState
    )
{
    switch (presenceState)
    {
        case xbox::services::presence::user_presence_state::away: return _T("away");
        case xbox::services::presence::user_presence_state::offline: return _T("offline");
        case xbox::services::presence::user_presence_state::online: return _T("online");
        default:
        case xbox::services::presence::user_presence_state::unknown: return _T("unknown");
    }
}

std::wstring
ConvertPresenceFilterToString(_In_ xbox::services::social::manager::presence_filter presenceFilter)
{
    switch (presenceFilter)
    {
        case xbox::services::social::manager::presence_filter::unknown: return _T("unknown");
        case xbox::services::social::manager::presence_filter::title_online: return _T("title_online");
        case xbox::services::social::manager::presence_filter::title_offline: return _T("title_offline");
        case xbox::services::social::manager::presence_filter::all_online: return _T("all_online");
        case xbox::services::social::manager::presence_filter::all_offline: return _T("all_offline");
        case xbox::services::social::manager::presence_filter::all_title: return _T("all_title");
        default:
        case xbox::services::social::manager::presence_filter::all: return _T("all");
    }
}

std::wstring
ConvertRelationshipFilterToString(_In_ xbox::services::social::manager::relationship_filter relationshipFilter)
{
    switch (relationshipFilter)
    {
        case xbox::services::social::manager::relationship_filter::favorite: return _T("favorite");
        default:
        case xbox::services::social::manager::relationship_filter::friends: return _T("friends");
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
    for (const std::shared_ptr<xbox::services::social::manager::xbox_social_user_group>& group : m_socialGroups)
    {
        m_font->DrawString(m_sprites.get(), L"_________________________________________", XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
        verticalBaseOffset += fTextHeight;
        if(group->social_user_group_type() == social_user_group_type::filter_type )
        {
            swprintf_s(text, ARRAYSIZE(text), L"Group from filter: %s %s",
                ConvertPresenceFilterToString(group->presence_filter_of_group()).c_str(),
                ConvertRelationshipFilterToString(group->relationship_filter_of_group()).c_str()
                );
            m_font->DrawString(m_sprites.get(), text, XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
            verticalBaseOffset += fTextHeight;
        }
        else
        {
            m_font->DrawString(m_sprites.get(), L"Group from custom list", XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
            verticalBaseOffset += fTextHeight;
        }

        const std::vector<xbox_social_user*>& userList = group->users();
        for (const auto& user : userList)
        {
            m_font->DrawString(m_sprites.get(), user->gamertag(), XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
            m_font->DrawString(m_sprites.get(), ConvertPresenceUserStateToString(user->presence_record().user_state()).c_str(), XMFLOAT2(fGridXColumn2, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);

            verticalBaseOffset += fTextHeight;
        }
        if (userList.size() == 0)
        {
            m_font->DrawString(m_sprites.get(), L"No friends found", XMFLOAT2(fGridXColumn1, fGridY + verticalBaseOffset), TEXT_COLOR, 0.0f, XMFLOAT2(0, 0), scale);
            verticalBaseOffset += fTextHeight;
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

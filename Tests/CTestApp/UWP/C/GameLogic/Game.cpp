// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Game.h"
#include "Common\DirectXHelper.h"
#include "Utils\PerformanceCounters.h"
#include "httpClient\httpClient.h"
#include "AsyncIntegration.h"

using namespace Sample;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

Game* g_sampleInstance = nullptr;
std::mutex Game::m_displayEventQueueLock;
// Loads and initializes application assets when the application is loaded.
Game::Game(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    bInitialized(false)
{
    g_sampleInstance = this;

    // Register to be notified if the Device is lost or recreated
    m_deviceResources->RegisterDeviceNotify(this);
    m_sceneRenderer = std::unique_ptr<Renderer>(new Renderer(m_deviceResources));

    uint32_t sharedAsyncQueueId = 0;
    CreateSharedAsyncQueue(
        sharedAsyncQueueId,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        &m_queue);

    XblInitialize();
    InitializeAsync(m_queue, &m_asyncQueueCallbackToken);

    XblUserCreateHandle(&m_user);
}

void Game::RegisterInputKeys()
{
    m_input->RegisterKey(Windows::System::VirtualKey::S, ButtonPress::SignIn);
    m_input->RegisterKey(Windows::System::VirtualKey::P, ButtonPress::GetUserProfile);
    m_input->RegisterKey(Windows::System::VirtualKey::F, ButtonPress::GetFriends);
    m_input->RegisterKey(Windows::System::VirtualKey::A, ButtonPress::GetAchievementsForTitle);
    m_input->RegisterKey(Windows::System::VirtualKey::Number1, ButtonPress::ToggleSocialGroup1);
    m_input->RegisterKey(Windows::System::VirtualKey::Number2, ButtonPress::ToggleSocialGroup2);
    m_input->RegisterKey(Windows::System::VirtualKey::Number3, ButtonPress::ToggleSocialGroup3);
    m_input->RegisterKey(Windows::System::VirtualKey::Number4, ButtonPress::ToggleSocialGroup4);
    m_input->RegisterKey(Windows::System::VirtualKey::Number5, ButtonPress::ToggleSocialGroup5);
    m_input->RegisterKey(Windows::System::VirtualKey::C, ButtonPress::ImportCustomList);
}

Game::~Game()
{
    // Deregister device notification
    m_deviceResources->RegisterDeviceNotify(nullptr);
    if (m_user != nullptr)
    {
        XblUserCloseHandle(m_user);
    }
    if (m_xboxLiveContext != nullptr)
    {
        XblContextCloseHandle(m_xboxLiveContext);
    }
    CleanupAsync(m_queue, m_asyncQueueCallbackToken);
    XblCleanup();
}

// Updates application state when the window size changes (e.g. device orientation change)
void Game::CreateWindowSizeDependentResources() 
{
    m_sceneRenderer->CreateWindowSizeDependentResources();    
}

// Updates the application state once per frame.
void Game::Update() 
{
    if (!bInitialized)
    {
        return;
    }

    // Update scene objects.
    m_timer.Tick([&]()
    {
        m_input->Update();

        UpdateSocialManager();

        OnGameUpdate();

        m_sceneRenderer->Update(m_timer);

        // Since the KeyUp event could take a few frames to trigger, 
        // clear the keys to avoid running the scenarios multiple times.  
        m_input->ClearKeyHash();
    });
}

void Game::OnProtocolActivation(Windows::ApplicationModel::Activation::IProtocolActivatedEventArgs^ args)
{
    m_protocolActivatedEventArgs = args;

    // the app has not initialized yet, it'll process the protocol after initialization
    if (g_sampleInstance == nullptr || !bInitialized)
    {
        return;
    }
}

void Game::OnMainMenu()
{
}

void Game::OnGameUpdate()
{
    // Call one of the DrainAsyncCompletionQueue* helper functions
    // For example: 
    // DrainAsyncCompletionQueue(m_queue, 5);
    // DrainAsyncCompletionQueueUntilEmpty(m_queue);
    double timeoutMilliseconds = 0.5f;
    DrainAsyncCompletionQueueWithTimeout(m_queue, timeoutMilliseconds);

    if (m_input != nullptr)
    {
        if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup1))
        {
            m_allFriends = !m_allFriends;
            UpdateSocialGroupForAllUsers(m_allFriends, XblPresenceFilter_All, XblRelationshipFilter_Friends);
        }

        if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup2))
        {
            m_onlineFriends = !m_onlineFriends;
            UpdateSocialGroupForAllUsers(m_onlineFriends, XblPresenceFilter_AllOnline, XblRelationshipFilter_Friends);
        }

        if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup3))
        {
            m_allFavs = !m_allFavs;
            UpdateSocialGroupForAllUsers(m_allFavs, XblPresenceFilter_All, XblRelationshipFilter_Favorite);
        }

        if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup4))
        {
            m_onlineInTitle = !m_onlineInTitle;
            UpdateSocialGroupForAllUsers(m_onlineInTitle, XblPresenceFilter_TitleOnline, XblRelationshipFilter_Friends);
        }

        if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup5))
        {
            m_customList = !m_customList;
            UpdateSocialGroupOfListForAllUsers(m_customList);
        }

        if (m_input->IsKeyDown(ButtonPress::SignIn))
        {
            SignIn();
        }

        if (m_input->IsKeyDown(ButtonPress::GetUserProfile))
        {
            GetUserProfile();
        }

        if (m_input->IsKeyDown(ButtonPress::GetFriends))
        {
            GetSocialRelationships();
        }

        if (m_input->IsKeyDown(ButtonPress::GetAchievementsForTitle))
        {
            GetAchievmentsForTitle();
        }
    }
}

std::vector<std::wstring>
string_split(
    _In_ const std::wstring& string,
    _In_ std::wstring::value_type seperator
    )
{
    std::vector<std::wstring> vSubStrings;

    if (!string.empty())
    {
        size_t posStart = 0, posFound = 0;
        while (posFound != std::wstring::npos && posStart < string.length())
        {
            posFound = string.find(seperator, posStart);
            if (posFound != std::wstring::npos)
            {
                if (posFound != posStart)
                {
                    // this substring is not empty
                    vSubStrings.push_back(string.substr(posStart, posFound - posStart));
                }
                posStart = posFound + 1;
            }
            else
            {
                vSubStrings.push_back(string.substr(posStart));
            }
        }
    }

    return vSubStrings;
}

void replace_all(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void Game::ReadLastCsv()
{
    if (Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->Entries->Size > 0)
    {
        Windows::Storage::AccessCache::AccessListEntry entry = Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->Entries->GetAt(0);
        std::weak_ptr<Game> thisWeakPtr = shared_from_this();

        create_task(Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->GetFileAsync(entry.Token))
        .then([thisWeakPtr](pplx::task<Windows::Storage::StorageFile^> t)
        {
            try
            {
                std::shared_ptr<Game> pThis(thisWeakPtr.lock());
                if (pThis == nullptr)
                {
                    return;
                }

                Windows::Storage::StorageFile^ file = t.get();
                pThis->ReadCsvFile(file);
            }
            catch (Platform::Exception^)
            {
            }
        });
    }
}

void Game::UpdateCustomList(_In_ const std::vector<uint64_t>& xuidList)
{
    m_xuidList = xuidList;

    // Refresh custom list if its active
    if (m_customList)
    {
        UpdateSocialGroupOfListForAllUsers(false);
    }
}


void Game::ReadCsvFile(Windows::Storage::StorageFile^ file)
{
    WCHAR text[1024];
    swprintf_s(text, ARRAYSIZE(text), L"Reading %s", file->Path->Data());
    Log(text);
    std::weak_ptr<Game> thisWeakPtr = shared_from_this();

    //var lines = await 
    auto asyncOp = Windows::Storage::FileIO::ReadLinesAsync(file);

    create_task(asyncOp)
    .then([thisWeakPtr](pplx::task<Windows::Foundation::Collections::IVector<Platform::String^>^> t)
    {
        try
        {
            std::shared_ptr<Game> pThis(thisWeakPtr.lock());
            if (pThis == nullptr)
            {
                return;
            }

            Windows::Foundation::Collections::IVector<Platform::String^>^ lines = t.get();
            std::vector<uint64_t> xuidList;
            int count = 0;
            for (Platform::String^ line : lines)
            {
                count++;
                if (count == 1)
                {
                    continue; // skip header
                }

                std::vector<string_t> items = string_split(line->Data(), L',');
                if (items.size() > 4)
                {
                    std::string xuid = utility::conversions::to_utf8string(items[3]);
                    replace_all(xuid, "\"", "");
                    replace_all(xuid, "=", "");
                    xuidList.push_back(std::strtoul(xuid.data(), nullptr, 0));

                    CHAR text[1024];
                    sprintf_s(text, ARRAYSIZE(text), "Read from CSV: %s", xuid.c_str());
                    pThis->Log(utility::conversions::utf8_to_utf16(text));
                }
            }

            pThis->UpdateCustomList(xuidList);
        }
        catch (Platform::Exception^)
        {
        }
    });
}

void Game::Init(Windows::UI::Core::CoreWindow^ window)
{
    bInitialized = true;
    m_input = ref new Input();
    m_gameData.reset(new GameData());
    m_displayEventQueue = std::vector< std::wstring >();
    m_previousDisplayQueueSize = 0;

    m_gameData->SetViewDebugOutputOffsetX(1000);
    m_gameData->SetViewCurrentCountersOffsetY(0);
    m_gameData->SetViewGameDataOffsetX(0);
    m_gameData->SetViewSelectedIndex(0);
    m_gameData->SetViewDebug(false);
    m_gameData->SetDisplayString(0, L"");

    RegisterInputKeys();

    window->KeyDown += ref new TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::KeyEventArgs^>(m_input, &Input::OnKeyDown);
    window->KeyUp += ref new TypedEventHandler<Windows::UI::Core::CoreWindow^, Windows::UI::Core::KeyEventArgs^>(m_input, &Input::OnKeyUp);

    std::weak_ptr<Game> thisWeakPtr = shared_from_this();
    XblUserAddSignOutCompletedHandler(Game::HandleSignout);

    ReadLastCsv();
    SignInSilently();
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool Game::Render() 
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return false;
    }

    auto context = m_deviceResources->GetD3DDeviceContext();

    // Reset the viewport to target the whole screen.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    // Reset render targets to the screen.
    ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
    context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

    // Clear the back buffer and depth stencil view.
    context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
    context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Render the scene objects.
    m_sceneRenderer->Render();

    return true;
}

// Notifies renderers that device resources need to be released.
void Game::OnDeviceLost()
{
    m_sceneRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void Game::OnDeviceRestored()
{
    m_sceneRenderer->CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void Game::Log(std::wstring log)
{
    std::lock_guard<std::mutex> guard(m_displayEventQueueLock);
    m_displayEventQueue.push_back(log);
    if (m_displayEventQueue.size() > 15)
    {
        m_displayEventQueue.erase(m_displayEventQueue.begin());
    }
}

void Game::Log(std::string log)
{
    Log(utility::conversions::to_utf16string(log));
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
    case XblSocialManagerEventType_SocialUserGroupLoaded : return _T("social_user_group_loaded");
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
            source << utility::conversions::to_utf16string(ConvertEventTypeToString(socialEvent.eventType));
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

void Game::UpdateSocialGroupForAllUsers(
    _In_ bool toggle,
    _In_ XblPresenceFilter presenceFilter,
    _In_ XblRelationshipFilter relationshipFilter
)
{
    if (m_xboxLiveContext != nullptr)
    {
        UpdateSocialGroup(m_user, toggle, presenceFilter, relationshipFilter);
    }
}

void Game::UpdateSocialGroup(
    _In_ xbl_user_handle user,
    _In_ bool toggle,
    _In_ XblPresenceFilter presenceFilter,
    _In_ XblRelationshipFilter relationshipFilter
)
{
    if (m_xboxLiveContext != nullptr)
    {
        if (toggle)
        {
            CreateSocialUserGroupFromFilters(user, presenceFilter, relationshipFilter);
        }
        else
        {
            DestroySocialGroup(user, presenceFilter, relationshipFilter);
        }
    }
}

void Game::UpdateSocialGroupOfListForAllUsers(_In_ bool toggle)
{
    if (m_xboxLiveContext != nullptr)
    {
        return UpdateSocialGroupOfList(m_user, toggle);
    }
}

void Game::UpdateSocialGroupOfList(
    _In_ xbl_user_handle user,
    _In_ bool toggle
)
{
    if (m_xboxLiveContext != nullptr)
    {
        if (toggle)
        {
            CreateOrUpdateSocialGroupFromList(m_user, m_xuidList);
        }
        else
        {
            DestroySocialGroup(m_user);
        }
    }
}

void
Game::CreateSocialGroupsBasedOnUI(
    xbl_user_handle user
)
{
    UpdateSocialGroup(user, m_allFriends, XblPresenceFilter_All, XblRelationshipFilter_Friends);
    UpdateSocialGroup(user, m_onlineFriends, XblPresenceFilter_AllOnline, XblRelationshipFilter_Friends);
    UpdateSocialGroup(user, m_allFavs, XblPresenceFilter_All, XblRelationshipFilter_Favorite);
    UpdateSocialGroup(user, m_onlineInTitle, XblPresenceFilter_AllTitle, XblRelationshipFilter_Friends);
    UpdateSocialGroupOfList(user, m_customList);
}


void Game::HandleSignInResult(XblSignInResult signInResult)
{
    switch (signInResult.status)
    {
    case xbox::services::system::sign_in_status::success:
        XblUserGetXboxUserId(m_user, &m_xuid);
        XblContextCreateHandle(m_user, &m_xboxLiveContext);
        AddUserToSocialManager(m_user);
        Log(L"Sign in succeeded");
        break;

    case xbox::services::system::sign_in_status::user_cancel:
        Log(L"User cancel");
        break;

    case xbox::services::system::sign_in_status::user_interaction_required:
        Log(L"User interaction required");
        break;

    default:
        Log(L"Unknown error");
        break;
    }
}

void Game::SignIn()
{
    bool isSignedIn;
    XblUserIsSignedIn(m_user, &isSignedIn);
    if (isSignedIn)
    {
        Log(L"Already signed in.");
        return;
    }
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        XblSignInResult signInResult;
        auto result = XblUserGetSignInResult(asyncBlock, &signInResult);

        if (SUCCEEDED(result))
        {
            pThis->HandleSignInResult(signInResult);
        }
        else
        {
            pThis->Log(L"Failed signing in.");
            return;
        }
        delete asyncBlock;
    };

    XblUserSignInWithCoreDispatcher(
        asyncBlock,
        m_user,
        Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->CoreWindow->Dispatcher
        );
}

void Game::GetUserProfile()
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        XblUserProfile profile;
        XblProfileGetUserProfileResult(asyncBlock, &profile);

        pThis->Log(L"Successfully got profile!");
        WCHAR text[1024];
        swprintf_s(text, ARRAYSIZE(text), L"Gamertag: %S", profile.gamertag);
        pThis->Log(text);
        swprintf_s(text, ARRAYSIZE(text), L"XboxUserId: %I64u", profile.xboxUserId);
        pThis->Log(text);
        swprintf_s(text, ARRAYSIZE(text), L"Gamerscore: %S", profile.gamerscore);
        pThis->Log(text);
        swprintf_s(text, ARRAYSIZE(text), L"GameDisplayPic: %S", profile.gameDisplayPictureResizeUri);
        pThis->Log(text);

        delete asyncBlock;
    };
    XblProfileGetUserProfileAsync(asyncBlock, m_xboxLiveContext, m_xuid);
}


void Game::SignInSilently()
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        XblSignInResult signInResult;
        auto result = XblUserGetSignInResult(asyncBlock, &signInResult);

        if (SUCCEEDED(result))
        {
            pThis->HandleSignInResult(signInResult);
        }
        else
        {
            pThis->Log(L"Failed signing in.");
            return;
        }
        delete asyncBlock;
    };

    XblUserSignInSilently(asyncBlock, m_user);
}

void Game::GetSocialRelationships()
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        xbl_social_relationship_result_handle resultHandle;
        auto hr = XblSocialGetSocialRelationshipsResult(asyncBlock, &resultHandle);

        if (SUCCEEDED(hr))
        {
            XblSocialRelationship* relationships;
            uint32_t relationshipCount;
            XblSocialRelationshipResultGetRelationships(resultHandle, &relationships, &relationshipCount);

            std::stringstream ss;
            ss << "Got social relationships. User has " << relationshipCount << " relationships:";
            pThis->Log(ss.str());

            for (unsigned i = 0; i < relationshipCount; ++i)
            {
                std::stringstream ss;
                ss << "Xuid: " << relationships[i].xboxUserId << " isFollowing: " << relationships[i].isFollowingCaller;
                ss << " isFavorite: " << relationships[i].isFavorite;
                pThis->Log(ss.str());
            }
            XblSocialRelationshipResultCloseHandle(resultHandle);
        }
        else
        {
            pThis->Log(L"Failed getting social relationships.");
        }
    };

    XblSocialGetSocialRelationshipsAsync(asyncBlock, m_xboxLiveContext, m_xuid, XblSocialRelationshipFilter_All);
}

void Game::GetAchievmentsForTitle()
{
    XblGetXboxLiveAppConfig(&m_config);
    
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        xbl_achievements_result_handle resultHandle;
        auto hr = XblAchievementsGetAchievementsForTitleIdResult(asyncBlock, &resultHandle);

        if (SUCCEEDED(hr))
        {
            pThis->Log(L"Successfully got achievements for this title!");

            XblAchievement* achievements;
            uint32_t achievementsCount;
            XblAchievementsResultGetAchievements(resultHandle, &achievements, &achievementsCount);

            // Just get/update the first achievement as an example
            pThis->GetAchievement(achievements[0].serviceConfigurationId, achievements[0].id);

            bool hasNext = false;
            XblAchievementsResultHasNext(resultHandle, &hasNext);
            if (hasNext)
            {
                pThis->AchievementResultsGetNext(resultHandle);
            }
            else
            {
                XblAchievementsResultCloseHandle(resultHandle);
            }
        }
        else
        {
            pThis->Log(L"Failed getting achievements for this title.");
            return;
        }

        delete asyncBlock;
    };

    XblAchievementsGetAchievementsForTitleIdAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        m_config->titleId,
        XblAchievementType_All, 
        false, 
        XblAchievementOrderBy_DefaultOrder, 
        0, 
        1);
}

void Game::AchievementResultsGetNext(xbl_achievements_result_handle resultHandle)
{
    struct context_t
    {
        Game* pThis;
        xbl_achievements_result_handle resultHandle;
    };

    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = new context_t{ this, resultHandle };
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        auto context = reinterpret_cast<context_t*>(asyncBlock->context);
        
        xbl_achievements_result_handle nextResultHandle;
        auto hr = XblAchievementsResultGetNextResult(asyncBlock, &nextResultHandle);

        if (SUCCEEDED(hr))
        {
            context->pThis->Log(L"Successfully got next page of achievements!");
            context->pThis->AchievementResultsGetNext(nextResultHandle);
            XblAchievementsResultCloseHandle(context->resultHandle);
        }
        else
        {
            context->pThis->Log(L"Failed getting next page of achievements.");
        }

        delete asyncBlock->context;
        delete asyncBlock;
    };

    XblAchievementsResultGetNextAsync(
        asyncBlock,
        m_xboxLiveContext,
        resultHandle,
        1);
}

void Game::GetAchievement(std::string scid, std::string achievementId)
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        xbl_achievements_result_handle resultHandle;
        auto hr = XblAchievementsGetAchievementResult(asyncBlock, &resultHandle);

        if (SUCCEEDED(hr))
        {
            pThis->Log(L"Successfully got achievement!");

            XblAchievement* achievements;
            uint32_t achievementsCount;
            XblAchievementsResultGetAchievements(resultHandle, &achievements, &achievementsCount);
            assert(achievementsCount == 1);
            pThis->UpdateAchievement(achievements[0].serviceConfigurationId, achievements[0].id);

            XblAchievementsResultCloseHandle(resultHandle);
        }
        else
        {
            pThis->Log(L"Failed getting achievement.");
            return;
        }

        delete asyncBlock;
    };

    XblAchievementsGetAchievementAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        scid.data(),
        achievementId.data()
        );
}

void Game::UpdateAchievement(std::string scid, std::string achievementId)
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);
        
        auto result = GetAsyncStatus(asyncBlock, false);
        if (SUCCEEDED(result))
        {
            pThis->Log(L"Successfully updated achievement!");
        }
        else if (result == HTTP_E_STATUS_NOT_MODIFIED)
        {
            pThis->Log(L"Achievement not modified!");
        }
        else
        {
            pThis->Log(L"Failed updating achievement.");
        }

        delete asyncBlock;
    };

    auto tid = m_config->titleId;
    XblAchievementsUpdateAchievementAsync(
        asyncBlock,
        m_xboxLiveContext,
        m_xuid,
        &tid,
        scid.data(),
        achievementId.data(),
        10);
}

void Game::HandleSignout(xbl_user_handle user)
{
    WCHAR text[1024];
    char gamertag[GamertagMaxBytes];
    XblUserGetGamertag(user, GamertagMaxBytes, gamertag, nullptr);

    swprintf_s(text, ARRAYSIZE(text), L"User %s signed out", utility::conversions::utf8_to_utf16(gamertag).data());
    g_sampleInstance->Log(text);
}

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Game.h"
#include "Common\DirectXHelper.h"
#include "Utils\PerformanceCounters.h"
#include "httpClient\httpClient.h"

using namespace Sample;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

Game* g_sampleInstance = nullptr;
std::mutex Game::m_displayEventQueueLock;

class win32_handle
{
public:
    win32_handle() : m_handle(nullptr)
    {
    }

    ~win32_handle()
    {
        if (m_handle != nullptr) CloseHandle(m_handle);
        m_handle = nullptr;
    }

    void set(HANDLE handle)
    {
        m_handle = handle;
    }

    HANDLE get() { return m_handle; }

private:
    HANDLE m_handle;
};

win32_handle g_stopRequestedHandle;
win32_handle g_workReadyHandle;
win32_handle g_completionReadyHandle;

void CALLBACK HandleAsyncQueueCallback(
    _In_ void* context,
    _In_ async_queue_handle_t queue,
    _In_ AsyncQueueCallbackType type
)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(queue);

    switch (type)
    {
    case AsyncQueueCallbackType::AsyncQueueCallbackType_Work:
        SetEvent(g_workReadyHandle.get());
        break;

    case AsyncQueueCallbackType::AsyncQueueCallbackType_Completion:
        SetEvent(g_completionReadyHandle.get());
        break;
    }
}


DWORD WINAPI background_thread_proc(LPVOID lpParam)
{
    HANDLE hEvents[3] =
    {
        g_workReadyHandle.get(),
        g_completionReadyHandle.get(),
        g_stopRequestedHandle.get()
    };

    async_queue_handle_t queue;
    uint32_t sharedAsyncQueueId = 0;
    CreateSharedAsyncQueue(
        sharedAsyncQueueId,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        &queue);

    bool stop = false;
    uint64_t taskGroupId = 0;
    while (!stop)
    {
        DWORD dwResult = WaitForMultipleObjectsEx(3, hEvents, false, INFINITE, false);
        switch (dwResult)
        {
        case WAIT_OBJECT_0: // work ready
            DispatchAsyncQueue(queue, AsyncQueueCallbackType_Work, 0);

            if (!IsAsyncQueueEmpty(queue, AsyncQueueCallbackType_Work))
            {
                // If there's more pending work, then set the event to process them
                SetEvent(g_workReadyHandle.get());
            }
            break;

        case WAIT_OBJECT_0 + 1: // completed 
                                // Typically completions should be dispatched on the game thread, but
                                // for this simple XAML app we're doing it here
            DispatchAsyncQueue(queue, AsyncQueueCallbackType_Completion, 0);

            if (!IsAsyncQueueEmpty(queue, AsyncQueueCallbackType_Completion))
            {
                // If there's more pending completions, then set the event to process them
                SetEvent(g_completionReadyHandle.get());
            }
            break;

        default:
            stop = true;
            break;
        }
    }

    CloseAsyncQueue(queue);
    return 0;
}

// Loads and initializes application assets when the application is loaded.
Game::Game(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    bInitialized(false)
{
    g_sampleInstance = this;

    // Register to be notified if the Device is lost or recreated
    m_deviceResources->RegisterDeviceNotify(this);
    m_sceneRenderer = std::unique_ptr<Renderer>(new Renderer(m_deviceResources));

    g_stopRequestedHandle.set(CreateEvent(nullptr, true, false, nullptr));
    g_workReadyHandle.set(CreateEvent(nullptr, false, false, nullptr));
    g_completionReadyHandle.set(CreateEvent(nullptr, false, false, nullptr));

    XblGlobalInitialize();
    uint32_t sharedAsyncQueueId = 0;
    CreateSharedAsyncQueue(
        sharedAsyncQueueId,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        &m_queue);
    AddAsyncQueueCallbackSubmitted(m_queue, nullptr, HandleAsyncQueueCallback, &m_callbackToken);

    m_hBackgroundThread = CreateThread(nullptr, 0, background_thread_proc, nullptr, 0, nullptr);

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
    RemoveAsyncQueueCallbackSubmitted(m_queue, m_callbackToken);
    CloseAsyncQueue(m_queue);

    XblGlobalCleanup();
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

        switch (m_gameData->GetAppState())
        {
        case APP_IN_GAME:
            OnGameUpdate();
            break;

        case APP_CRITICAL_ERROR:
            break;
        }

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
    switch (m_gameData->GetGameState())
    {
    case GAME_PLAY:
    {
        if (m_input != nullptr)
        {
            if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup1))
            {
                m_allFriends = !m_allFriends;
                UpdateSocialGroupForAllUsers(m_allFriends, XBL_PRESENCE_FILTER_ALL, XBL_RELATIONSHIP_FILTER_FRIENDS);
            }

            if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup2))
            {
                m_onlineFriends = !m_onlineFriends;
                UpdateSocialGroupForAllUsers(m_onlineFriends, XBL_PRESENCE_FILTER_ALL_ONLINE, XBL_RELATIONSHIP_FILTER_FRIENDS);
            }

            if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup3))
            {
                m_allFavs = !m_allFavs;
                UpdateSocialGroupForAllUsers(m_allFavs, XBL_PRESENCE_FILTER_ALL, XBL_RELATIONSHIP_FILTER_FAVORITE);
            }

            if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup4))
            {
                m_onlineInTitle = !m_onlineInTitle;
                UpdateSocialGroupForAllUsers(m_onlineInTitle, XBL_PRESENCE_FILTER_TITLE_ONLINE, XBL_RELATIONSHIP_FILTER_FRIENDS);
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
    break;

    default:
        break;
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

void Game::UpdateCustomList(_In_ const std::vector<std::string>& xuidList)
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
            std::vector<std::string> xuidList;
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
                    xuidList.push_back(xuid);

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
    XblAddSignOutCompletedHandler(Game::HandleSignout);

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
ConvertEventTypeToString(XBL_SOCIAL_EVENT_TYPE eventType)
{
    switch (eventType)
    {
    case XBL_SOCIAL_EVENT_TYPE_USERS_ADDED_TO_SOCIAL_GRAPH: return _T("users_added");
    case XBL_SOCIAL_EVENT_TYPE_USERS_REMOVED_FROM_SOCIAL_GRAPH: return _T("users_removed");
    case XBL_SOCIAL_EVENT_TYPE_PRESENCE_CHANGED: return _T("presence_changed");
    case XBL_SOCIAL_EVENT_TYPE_PROFILES_CHANGED: return _T("profiles_changed");
    case XBL_SOCIAL_EVENT_TYPE_SOCIAL_RELATIONSHIPS_CHANGED: return _T("social_relationships_changed");
    case XBL_SOCIAL_EVENT_TYPE_LOCAL_USER_ADDED: return _T("local_user_added");
    case XBL_SOCIAL_EVENT_TYPE_LOCAL_USER_REMOVED: return _T("local user removed");
    case XBL_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_LOADED: return _T("social_user_group_loaded");
    case XBL_SOCIAL_EVENT_TYPE_SOCIAL_USER_GROUP_UPDATED: return _T("social_user_group_updated");
    default: return _T("unknown");
    }
}

void
Game::LogSocialEventList(XBL_SOCIAL_EVENT* events, uint32_t eventCount)
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
                XBL_XBOX_USER_ID_CONTAINER *affectedUsers;
                affectedUsers = new XBL_XBOX_USER_ID_CONTAINER[socialEvent.usersAffectedCount];

                XblSocialEventGetUsersAffected(&socialEvent, affectedUsers);

                source << _T(" UserAffected: ");
                for (uint32_t j = 0; j < socialEvent.usersAffectedCount; ++j)
                {
                    source << affectedUsers[j].xboxUserId;
                    source << _T(", ");
                }
            }
        }
        Log(source.str());
    }
}

void Game::UpdateSocialGroupForAllUsers(
    _In_ bool toggle,
    _In_ XBL_PRESENCE_FILTER presenceFilter,
    _In_ XBL_RELATIONSHIP_FILTER relationshipFilter
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
    _In_ XBL_PRESENCE_FILTER presenceFilter,
    _In_ XBL_RELATIONSHIP_FILTER relationshipFilter
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
    UpdateSocialGroup(user, m_allFriends, XBL_PRESENCE_FILTER_ALL, XBL_RELATIONSHIP_FILTER_FRIENDS);
    UpdateSocialGroup(user, m_onlineFriends, XBL_PRESENCE_FILTER_ALL_ONLINE, XBL_RELATIONSHIP_FILTER_FRIENDS);
    UpdateSocialGroup(user, m_allFavs, XBL_PRESENCE_FILTER_ALL, XBL_RELATIONSHIP_FILTER_FAVORITE);
    UpdateSocialGroup(user, m_onlineInTitle, XBL_PRESENCE_FILTER_ALL_TITLE, XBL_RELATIONSHIP_FILTER_FRIENDS);
    UpdateSocialGroupOfList(user, m_customList);
}


void Game::HandleSignInResult(XblSignInResult signInResult)
{
    switch (signInResult.status)
    {
    case xbox::services::system::sign_in_status::success:
        XblUserGetXboxUserId(m_user, &m_xuid);
        XblContextCreateHandle(m_user, &m_xboxLiveContext);
        //pThis->AddUserToSocialManager(pThis->m_user);
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
    AsyncBlock* asyncBlock = new AsyncBlock;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        XblSignInResult signInResult;
        auto result = XblGetSignInResult(asyncBlock, &signInResult);

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
        m_user, 
        Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->CoreWindow->Dispatcher,
        asyncBlock
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
        XblGetProfileResult(asyncBlock, 1, &profile, nullptr);

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
    XblGetUserProfile(m_xboxLiveContext, m_xuid, asyncBlock);
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
        auto result = XblGetSignInResult(asyncBlock, &signInResult);

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

    XblUserSignInSilently(m_user, asyncBlock);
}

void Game::GetSocialRelationships()
{
    AsyncBlock* asyncBlock = new AsyncBlock{};
    asyncBlock->queue = m_queue;
    asyncBlock->context = this;
    asyncBlock->callback = [](AsyncBlock* asyncBlock)
    {
        Game *pThis = reinterpret_cast<Game*>(asyncBlock->context);

        size_t size;
        auto hr = XblGetSocialRelationshipResultSize(asyncBlock, &size);

        if (SUCCEEDED(hr))
        {
            auto result = (XblSocialRelationshipResult*) malloc(size);

            XblGetSocialRelationshipResult(asyncBlock, size, result, nullptr);

            std::stringstream ss;
            ss << "Got social relationships. User has " << result->itemsCount << " relationships:";
            pThis->Log(ss.str());

            for (unsigned i = 0; i < result->itemsCount; ++i)
            {
                std::stringstream ss;
                ss << "Xuid: " << result->items[i].xboxUserId << " isFollowing: " << result->items[i].isFollowingCaller;
                ss << " isFavorite: " << result->items[i].isFavorite;
                pThis->Log(ss.str());
            }
            free(result);
        }
        else
        {
            pThis->Log(L"Failed getting social relationships.");
        }
    };

    XblGetSocialRelationships(m_xboxLiveContext, asyncBlock);
}

void Game::GetAchievmentsForTitle()
{
    XblGetXboxLiveAppConfig(&m_config);
    
    XblAchievementServiceGetAchievementsForTitleId(
        m_xboxLiveContext, 
        nullptr, // TODO
        m_config->titleId,
        XBL_ACHIEVEMENT_TYPE_ALL, 
        false, 
        XBL_ACIEVEMENT_ORDER_BY_DEFAULT_ORDER, 
        0, 
        1,
        nullptr,
        this,
        [](XBL_RESULT result,
           XBL_ACHIEVEMENTS_RESULT* achievementsResult,
           void* context) 
        {
            Game *pThis = reinterpret_cast<Game*>(context);

            if (result.errorCondition == XBL_ERROR_CONDITION_NO_ERROR)
            {
                pThis->Log(L"Successfully got achievements for this title!");

                pThis->AchievementResultsGetNext(achievementsResult);
            }
            else
            {
                pThis->Log(L"Failed getting achievements for this title.");
            }
        });
}

void Game::AchievementResultsGetNext(XBL_ACHIEVEMENTS_RESULT* result)
{
    if (XblAchievementsResultHasNext(result))
    {
        XblAchievementsResultGetNext(
            result, 
            1,
            nullptr,
            this,
            [](XBL_RESULT result,
                XBL_ACHIEVEMENTS_RESULT* achievementsResult,
                void* context) 
            {
                Game *pThis = reinterpret_cast<Game*>(context);

                if (result.errorCondition == XBL_ERROR_CONDITION_NO_ERROR)
                {
                    pThis->Log(L"Successfully got next page of achievements!");

                    pThis->AchievementResultsGetNext(achievementsResult);

                    pThis->TestAchievementsResultCopy(achievementsResult);

                    XblAchievementServiceReleaseAchievementsResult(achievementsResult);
                }
                else
                {
                    pThis->Log(L"Failed getting next page of achievements.");
                }
            });
    }
    else if (result->itemsCount > 0)
    {
        GetAchievement(result->items[0]->serviceConfigurationId, result->items[0]->id);
    }
}

void Game::GetAchievement(PCSTR scid, PCSTR achievementId)
{
    XblAchievementServiceGetAchievement(
        m_xboxLiveContext,
        nullptr, // TODO
        scid,
        achievementId,
        nullptr,
        this,
        [](XBL_RESULT result,
           XBL_ACHIEVEMENT* achievement,
           void* context)
        {
            Game *pThis = reinterpret_cast<Game*>(context);

            if (result.errorCondition == XBL_ERROR_CONDITION_NO_ERROR)
            {
                pThis->Log(L"Successfully got achievement!");

                pThis->TestAchievementCopy(achievement);

                pThis->UpdateAchievement(achievement->serviceConfigurationId, achievement->id);
                XblAchievementServiceReleaseAchievement(achievement);
            }
            else
            {
                pThis->Log(L"Failed getting achievement.");
            }
        });
}

void Game::UpdateAchievement(PCSTR scid, PCSTR achievementId)
{
    auto tid = m_config->titleId;
    XblAchievementServiceUpdateAchievement(
        m_xboxLiveContext,
        nullptr, // TODO
        &tid,
        scid,
        achievementId,
        10,
        nullptr,
        this,
        [](XBL_RESULT result,
            void* context)
    {
        Game *pThis = reinterpret_cast<Game*>(context);

        if (result.errorCondition == XBL_ERROR_CONDITION_NO_ERROR)
        {
            pThis->Log(L"Successfully updated achievement!");
        }
        else if (result.errorCode == XBL_ERROR_CODE_HTTP_STATUS_304_NOT_MODIFIED)
        {
            pThis->Log(L"Achievement not modified!");
        }
        else
        {
            pThis->Log(L"Failed updating achievement.");
        }
    });
}

void Game::TestAchievementCopy(XBL_ACHIEVEMENT* source)
{
    uint64_t size = 0;
    XblCopyAchievement(source, nullptr, &size);

    auto buffer = Alloc(size);
    auto copy = XblCopyAchievement(source, buffer, &size);

    AssertAchievement(copy, source);
}

void Game::TestAchievementsResultCopy(XBL_ACHIEVEMENTS_RESULT* source)
{
    uint64_t size = 0;
    XblCopyAchievementsResult(source, nullptr, &size);

    auto buffer = Alloc(size);
    auto copy = XblCopyAchievementsResult(source, buffer, &size);

    assert((copy->items != source->items) || (copy->items == nullptr && source->items == nullptr));
    assert(copy->itemsCount == source->itemsCount);
    for (size_t i = 0; i < copy->itemsCount; i++)
    {
        assert(copy->items[i] != source->items[i]);
        AssertAchievement(copy->items[i], source->items[i]);
    }

}

void Game::AssertAchievement(XBL_ACHIEVEMENT* copy, XBL_ACHIEVEMENT* source)
{
    assert(copy->id != source->id);
    assert(copy->serviceConfigurationId != source->serviceConfigurationId);
    assert(copy->name != source->name);
    assert((copy->titleAssociations != source->titleAssociations) || (copy->titleAssociations == nullptr && source->titleAssociations == nullptr));
    assert(copy->titleAssociationsCount == source->titleAssociationsCount);
    for (size_t i = 0; i < copy->titleAssociationsCount; i++)
    {
        assert(copy->titleAssociations[i]->name != source->titleAssociations[i]->name);
        assert(copy->titleAssociations[i]->titleId == source->titleAssociations[i]->titleId);
    }
    assert(copy->progressState == source->progressState);
    assert(copy->progression != source->progression);
        assert((copy->progression->requirements != source->progression->requirements) || (copy->progression->requirements == nullptr && source->progression->requirements == nullptr));
        assert(copy->progression->requirementsCount == source->progression->requirementsCount);
        for (size_t i = 0; i < copy->progression->requirementsCount; i++)
        {
            assert(copy->progression->requirements[i] != source->progression->requirements[i]);
            assert(copy->progression->requirements[i]->id != source->progression->requirements[i]->id);
            assert(copy->progression->requirements[i]->currentProgressValue != source->progression->requirements[i]->currentProgressValue);
            assert(copy->progression->requirements[i]->targetProgressValue != source->progression->requirements[i]->targetProgressValue);
        }
        assert(copy->progression->timeUnlocked == source->progression->timeUnlocked);
    assert((copy->mediaAssets != source->mediaAssets) || (copy->mediaAssets == nullptr && source->mediaAssets == nullptr));
    assert(copy->mediaAssetsCount == source->mediaAssetsCount);
    for (size_t i = 0; i < copy->mediaAssetsCount; i++)
    {
        assert(copy->mediaAssets[i]->name != source->mediaAssets[i]->name);
        assert(copy->mediaAssets[i]->url != source->mediaAssets[i]->url);
        assert(copy->mediaAssets[i]->mediaAssetType == source->mediaAssets[i]->mediaAssetType);
    }
    assert((copy->platformsAvailableOn != source->platformsAvailableOn) || (copy->platformsAvailableOn == nullptr && source->platformsAvailableOn == nullptr));
    assert(copy->platformsAvailableOnCount == source->platformsAvailableOnCount);
    for (size_t i = 0; i < copy->platformsAvailableOnCount; i++)
    {
        assert(copy->platformsAvailableOn[i] != source->platformsAvailableOn[i]);
    }
    assert(copy->isSecret == source->isSecret);
    assert(copy->unlockedDescription != source->unlockedDescription);
    assert(copy->lockedDescription != source->lockedDescription);
    assert(copy->productId != source->productId);
    assert(copy->type == source->type);
    assert(copy->participationType == source->participationType);
    assert(copy->available != source->available);
        assert(copy->available->startDate == source->available->startDate);
        assert(copy->available->endDate == source->available->endDate);
    assert((copy->rewards != source->rewards) || (copy->rewards == nullptr && source->rewards == nullptr));
    assert(copy->rewardsCount == source->rewardsCount);
    for (size_t i = 0; i < copy->rewardsCount; i++)
    {
        assert(copy->rewards[i]->description != source->rewards[i]->description);
        assert((copy->rewards[i]->mediaAsset != source->rewards[i]->mediaAsset) || (copy->rewards[i]->mediaAsset == nullptr && source->rewards[i]->mediaAsset == nullptr));
        if (copy->rewards[i]->mediaAsset != nullptr && source->rewards[i]->mediaAsset != nullptr)
        {
            assert(copy->rewards[i]->mediaAsset->name != source->rewards[i]->mediaAsset->name);
            assert(copy->rewards[i]->mediaAsset->url != source->rewards[i]->mediaAsset->url);
            assert(copy->rewards[i]->mediaAsset->mediaAssetType == source->rewards[i]->mediaAsset->mediaAssetType);
        }
        assert(copy->rewards[i]->name != source->rewards[i]->name);
        assert(copy->rewards[i]->rewardType == source->rewards[i]->rewardType);
        assert(copy->rewards[i]->value != source->rewards[i]->value);
        assert(copy->rewards[i]->valueType != source->rewards[i]->valueType);
    }
    assert(copy->estimatedUnlockTime == source->estimatedUnlockTime);
    assert(copy->deepLink != source->deepLink);
    assert(copy->isRevoked == source->isRevoked);
}

void Game::HandleSignout(xbl_user_handle user)
{
    WCHAR text[1024];
    char gamertag[GamertagMaxBytes];
    XblUserGetGamertag(user, GamertagMaxBytes, gamertag, nullptr);

    swprintf_s(text, ARRAYSIZE(text), L"User %s signed out", utility::conversions::utf8_to_utf16(gamertag).data());
    g_sampleInstance->Log(text);
}

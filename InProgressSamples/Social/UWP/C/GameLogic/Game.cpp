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

HANDLE g_stopRequestedHandle;
HANDLE g_pendingReadyHandle;
HANDLE g_completeReadyHandle;
XBL_ASYNC_QUEUE g_asyncQueue;

void xbl_event_handler(
    _In_opt_ void* context,
    _In_ XBL_ASYNC_EVENT_TYPE eventType,
    _In_ XBL_ASYNC_QUEUE queue
)
{
    UNREFERENCED_PARAMETER(context);
    UNREFERENCED_PARAMETER(queue);

    switch (eventType)
    {
    case XBL_ASYNC_EVENT_TYPE::XBL_ASYNC_EVENT_WORK_PENDING:
        SetEvent(g_pendingReadyHandle);
        break;

    case XBL_ASYNC_EVENT_TYPE::XBL_ASYNC_EVENT_WORK_STARTED:
        break;

    case XBL_ASYNC_EVENT_TYPE::XBL_ASYNC_EVENT_WORK_COMPLETED:
        SetEvent(g_completeReadyHandle);
        break;
    }
}

DWORD WINAPI background_thread_proc(LPVOID lpParam)
{
    HANDLE hEvents[3] =
    {
        g_pendingReadyHandle,
        g_completeReadyHandle,
        g_stopRequestedHandle
    };

    bool stop = false;
    while (!stop)
    {
        DWORD dwResult = WaitForMultipleObjectsEx(3, hEvents, false, INFINITE, false);
        switch (dwResult)
        {
        case WAIT_OBJECT_0: // pending 
            XblDispatchAsyncQueue(g_asyncQueue, XBL_ASYNC_QUEUE_CALLBACK_TYPE_WORK);
            if (!XblIsAsyncQueueEmpty(g_asyncQueue))
            {
                SetEvent(g_pendingReadyHandle);
            }
            break;
        case WAIT_OBJECT_0 + 1: // completed 
            XblDispatchAsyncQueue(g_asyncQueue, XBL_ASYNC_QUEUE_CALLBACK_TYPE_COMPLETION);
            if (!XblIsAsyncQueueEmpty(g_asyncQueue))
            {
                SetEvent(g_completeReadyHandle);
            }
            break;
        default:
            stop = true;
            break;
        }
    }
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

    g_stopRequestedHandle = CreateEvent(nullptr, true, false, nullptr);
    g_pendingReadyHandle = CreateEvent(nullptr, false, false, nullptr);
    g_completeReadyHandle = CreateEvent(nullptr, false, false, nullptr);

    XblGlobalInitialize();
    XblCreateAsyncQueue(&g_asyncQueue);

    XblAddTaskEventHandler(
        nullptr,
        xbl_event_handler,
        nullptr);

    m_hBackgroundThread = CreateThread(nullptr, 0, background_thread_proc, nullptr, 0, nullptr);

    XboxLiveUserCreate(&m_user);
}

void Game::RegisterInputKeys()
{
    m_input->RegisterKey(Windows::System::VirtualKey::S, ButtonPress::SignIn);
    m_input->RegisterKey(Windows::System::VirtualKey::P, ButtonPress::GetUserProfile);
    m_input->RegisterKey(Windows::System::VirtualKey::F, ButtonPress::GetFriends);
}

Game::~Game()
{
    // Deregister device notification
    m_deviceResources->RegisterDeviceNotify(nullptr);
    if (m_user != nullptr)
    {
        XboxLiveUserDelete(m_user);
    }
    if (m_xboxLiveContext != nullptr)
    {
        XblXboxLiveContextCloseHandle(m_xboxLiveContext);
    }
    if (g_asyncQueue != nullptr)
    {
        XblCloseAsyncQueue(g_asyncQueue);
    }

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

void replace_all(std::wstring& str, const std::wstring& from, const std::wstring& to) 
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
            std::vector<string_t> xuidList;
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
                    std::wstring xuid = items[3];
                    replace_all(xuid, L"\"", L"");
                    replace_all(xuid, L"=", L"");
                    xuidList.push_back(xuid);

                    WCHAR text[1024];
                    swprintf_s(text, ARRAYSIZE(text), L"Read from CSV: %s", xuid.c_str());
                    pThis->Log(text);
                }
            }

            //pThis->UpdateCustomList(xuidList);
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
    AddSignOutCompletedHandler([](XBL_XBOX_LIVE_USER *user)
    {
        g_sampleInstance->HandleSignout(user);
    });

    //ReadLastCsv();
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

void Game::HandleSignInResult(
    _In_ XBL_RESULT result,
    _In_ XSAPI_SIGN_IN_RESULT payload,
    _In_opt_ void* context)
{
    Game *pThis = reinterpret_cast<Game*>(context);

    if (!result.errorCondition == XBL_ERROR_CONDITION_NO_ERROR)
    {
        pThis->Log(L"Failed signing in.");
        return;
    }

    switch (payload.status)
    {
        case xbox::services::system::sign_in_status::success:
            XblXboxLiveContextCreateHandle(pThis->m_user, &(pThis->m_xboxLiveContext));
            pThis->Log(L"Sign in succeeded");
            break;

        case xbox::services::system::sign_in_status::user_cancel:
            pThis->Log(L"User cancel");
            break;

        case xbox::services::system::sign_in_status::user_interaction_required:
            pThis->Log(L"User interaction required");
            break;

        default:
            pThis->Log(L"Unknown error");
            break;
    }
}

void Game::SignIn()
{
    if (m_user->isSignedIn)
    {
        Log(L"Already signed in.");
        return;
    }
    XboxLiveUserSignInWithCoreDispatcher(
        m_user, 
        Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->CoreWindow->Dispatcher, 
        HandleSignInResult, 
        this,
        g_asyncQueue);
}

void Game::SignInSilently()
{
    XboxLiveUserSignInSilently(m_user, HandleSignInResult, this, g_asyncQueue);
}

void Game::GetUserProfile()
{
    if (m_xboxLiveContext == nullptr || m_user == nullptr || !m_user->isSignedIn)
    {
        Log(L"Must be signed in first to get profile!");
        return;
    }

    XblGetUserProfile(m_xboxLiveContext, m_user->xboxUserId, g_asyncQueue, this,
    [](XBL_RESULT result, const XBL_XBOX_USER_PROFILE *profile, void* context)
    {
        Game *pThis = reinterpret_cast<Game*>(context);
        if (result.errorCondition == XBL_ERROR_CONDITION_NO_ERROR)
        {
            pThis->Log(L"Successfully got profile!");
            WCHAR text[1024];
            swprintf_s(text, ARRAYSIZE(text), L"Gamertag: %S", profile->gamertag);
            pThis->Log(text);
            swprintf_s(text, ARRAYSIZE(text), L"XboxUserId: %S", profile->xboxUserId);
            pThis->Log(text);
            swprintf_s(text, ARRAYSIZE(text), L"Gamerscore: %S", profile->gamerscore);
            pThis->Log(text);
            swprintf_s(text, ARRAYSIZE(text), L"GameDisplayPic: %S", profile->gameDisplayPictureResizeUri);
            pThis->Log(text);
        }
        else
        {
            pThis->Log(L"Failed getting profile.");
        }
    });
}

void Game::CopySocialRelationshipResult()
{
    XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT r;
    r.filter = XBL_XBOX_SOCIAL_RELATIONSHIP_FILTER_ALL;
    r.hasNext = false;
    r.itemsCount = 3;
    r.totalCount = 3;
    r.items = new XBL_XBOX_SOCIAL_RELATIONSHIP[3];
    r.items[0].xboxUserId = "1";
    r.items[1].xboxUserId = "2";
    r.items[2].xboxUserId = "3";
    r.items[0].isFavorite = true;
    r.items[1].isFavorite = true;
    r.items[2].isFavorite = false;
    r.items[0].socialNetworks = (PCSTR*)new char*[2];
    r.items[0].socialNetworksCount = 2;
    r.items[0].socialNetworks[0] = "foo";
    r.items[0].socialNetworks[1] = "bar";
    r.items[1].socialNetworks = nullptr;
    r.items[1].socialNetworksCount = 0;
    r.items[2].socialNetworks = nullptr;
    r.items[2].socialNetworksCount = 0;

    uint64_t size = 0;
    auto copy = XblCopySocialRelationshipResult(&r, nullptr, &size);

    auto buffer = new char[size];
    copy = XblCopySocialRelationshipResult(&r, buffer, &size);

    Log(L"Copied result");
}

void Game::GetSocialRelationships()
{
    if (!m_user->isSignedIn)
    {
        Log(L"Must be signed in first to get profile!");
        return;
    }

    XblGetSocialRelationships(m_xboxLiveContext, g_asyncQueue, this,
        [](XBL_RESULT result, CONST XBL_XBOX_SOCIAL_RELATIONSHIP_RESULT *socialResult, void* context)
    {
        Game *pThis = reinterpret_cast<Game*>(context);

        if (result.errorCondition == XBL_ERROR_CONDITION_NO_ERROR)
        {
            pThis->CopySocialRelationshipResult();
            pThis->Log(L"Successfully got social relationships!");
        }
        else
        {
            pThis->Log(L"Failed getting social relationships.");
        }
    });
}

void Game::HandleSignout(XBL_XBOX_LIVE_USER *user)
{
    WCHAR text[1024];
    swprintf_s(text, ARRAYSIZE(text), L"User %s signed out", utility::conversions::utf8_to_utf16(user->gamertag).data());
    g_sampleInstance->Log(text);
}

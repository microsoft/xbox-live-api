// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Game.h"
#include "Common\DirectXHelper.h"
#include "Utils\PerformanceCounters.h"

using namespace Sample;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace xbox::services::system;
using namespace xbox::services;

Game* g_sampleInstance = nullptr;
std::mutex Game::m_displayEventQueueLock;

// Loads and initializes application assets when the application is loaded.
Game::Game(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    bInitialized(false),
    m_allFriends(false),
    m_onlineFriends(false),
    m_allFavs(false),
    m_onlineInTitle(false),
    m_customList(false),
    m_isMultiUserApplication(false)
{
    g_sampleInstance = this;

    // Register to be notified if the Device is lost or recreated
    m_deviceResources->RegisterDeviceNotify(this);
    m_sceneRenderer = std::unique_ptr<Renderer>(new Renderer(m_deviceResources));

    bool APIExist = Windows::Foundation::Metadata::ApiInformation::IsMethodPresent("Windows.System.UserPicker", "IsSupported");
    m_isMultiUserApplication = APIExist && Windows::System::UserPicker::IsSupported();
}

void Game::RegisterInputKeys()
{
    m_input->RegisterKey(Windows::System::VirtualKey::S, ButtonPress::SignIn);
    m_input->RegisterKey(Windows::System::VirtualKey::GamepadA, ButtonPress::SignIn);
}

Game::~Game()
{
    // Deregister device notification
    m_deviceResources->RegisterDeviceNotify(nullptr);
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
        }
    }
    break;

    default:
        break;
    }
}


std::vector<string_t>
string_split(
    _In_ const string_t& string,
    _In_ string_t::value_type seperator
    )
{
    std::vector<string_t> vSubStrings;

    if (!string.empty())
    {
        size_t posStart = 0, posFound = 0;
        while (posFound != string_t::npos && posStart < string.length())
        {
            posFound = string.find(seperator, posStart);
            if (posFound != string_t::npos)
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
    m_signOutContext = xbox::services::system::xbox_live_user::add_sign_out_completed_handler(
    [thisWeakPtr](const xbox::services::system::sign_out_completed_event_args& args)
    {
        UNREFERENCED_PARAMETER(args);
        std::shared_ptr<Game> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->HandleSignout(args.user());
        }
    });

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

void Game::Log(string_t log)
{
    std::lock_guard<std::mutex> guard(m_displayEventQueueLock);
    m_displayEventQueue.push_back(log);
    if (m_displayEventQueue.size() > 15)
    {
        m_displayEventQueue.erase(m_displayEventQueue.begin());
    }
}

string_t Game::GetAllUserNames()
{
    string_t result;
    for (auto pair : m_xboxLiveContexts)
    {
        result += pair.second->user()->gamertag();
        result += L", ";
    }

    return result;
}


void Game::HandleSignInResult(
    _In_ const xbox::services::system::sign_in_result& result,
    const std::shared_ptr<xbox::services::system::xbox_live_user>& xboxliveuser
    )
{
    string_t key = xboxliveuser->windows_system_user()->NonRoamableId->Data();
    switch (result.status())
    {
        case xbox::services::system::sign_in_status::success:
            m_xboxLiveContexts.emplace(
                string_t(xboxliveuser->windows_system_user()->NonRoamableId->Data()), 
                std::make_shared< xbox::services::xbox_live_context >(xboxliveuser));
            Log(key + L" Sign in succeeded");
            break;

        case xbox::services::system::sign_in_status::user_cancel:
            Log(key + L"User cancel");
            break;

        case xbox::services::system::sign_in_status::user_interaction_required:
            Log(key + L"User interaction required");
            break;

        default:
            Log(key + L"Unknown error");
            break;
    }
}

void Game::SignIn()
{
    std::weak_ptr<Game> thisWeakPtr = shared_from_this();

    auto task = create_task(Windows::System::User::FindAllAsync())
    .then([thisWeakPtr](IVectorView<Windows::System::User^>^ users)
    {
        std::shared_ptr<Game> pThis(thisWeakPtr.lock());

        for (auto systemuser : users)
        {
            if (systemuser->Type != Windows::System::UserType::LocalGuest || systemuser->Type != Windows::System::UserType::RemoteGuest)
            {
                auto liveuser = std::make_shared<xbox_live_user>(systemuser);
                pThis->Log(string_t(systemuser->NonRoamableId->Data()) + L"Signing in.");


                liveuser->signin(Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->Dispatcher)
                .then([thisWeakPtr, liveuser](xbox::services::xbox_live_result<xbox::services::system::sign_in_result> t)
                {
                    std::shared_ptr<Game> pThis(thisWeakPtr.lock());
                    if (pThis == nullptr)
                    {
                        return;
                    }

                    if (!t.err())
                    {
                        auto result = t.payload();
                        pThis->HandleSignInResult(result, liveuser);
                    }
                    else
                    {
                        pThis->Log(L"Failed signing in.");
                    }

                }, task_continuation_context::use_current());
            }
        }
    });
}

void Game::SignInSilently()
{
    std::weak_ptr<Game> thisWeakPtr = shared_from_this();

    auto task = create_task(Windows::System::User::FindAllAsync())
    .then([thisWeakPtr](IVectorView<Windows::System::User^>^ users)
    {
        std::shared_ptr<Game> pThis(thisWeakPtr.lock());

        for (auto systemuser : users)
        {
            if (systemuser->Type != Windows::System::UserType::LocalGuest || systemuser->Type != Windows::System::UserType::RemoteGuest)
            {
                auto liveuser = std::make_shared<xbox_live_user>(systemuser);
                pThis->Log(string_t(systemuser->NonRoamableId->Data()) +  L"Signing in.");

                create_task(liveuser->signin_silently(Windows::ApplicationModel::Core::CoreApplication::GetCurrentView()->Dispatcher))
                    .then([thisWeakPtr, liveuser](xbox::services::xbox_live_result<xbox::services::system::sign_in_result> t)
                {
                    std::shared_ptr<Game> pThis(thisWeakPtr.lock());
                    if (pThis == nullptr)
                    {
                        return;
                    }

                    if (!t.err())
                    {
                        auto result = t.payload();
                        pThis->HandleSignInResult(result, liveuser);
                    }
                    else
                    {
                        pThis->Log(L"Failed signing in.");
                    }
                }, task_continuation_context::use_current());
            }
        }
    });
}


void Game::HandleSignout(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user)
{
    string_t key = user->windows_system_user()->NonRoamableId->Data();
    auto iter = m_xboxLiveContexts.find(key);
    if (iter != m_xboxLiveContexts.end())
    {
        m_xboxLiveContexts.erase(key);
    }

    WCHAR text[1024];
    swprintf_s(text, ARRAYSIZE(text), L"User %s signed out", user->gamertag().c_str());
    Log(text);
}

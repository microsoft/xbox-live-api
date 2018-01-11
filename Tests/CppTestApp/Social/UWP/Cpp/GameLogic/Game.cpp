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
using namespace xbox::services::social::manager;

Game* g_sampleInstance = nullptr;
std::mutex Game::m_displayEventQueueLock;
std::mutex Game::m_socialManagerLock;

// Loads and initializes application assets when the application is loaded.
Game::Game(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    bInitialized(false),
    m_allFriends(false),
    m_onlineFriends(false),
    m_allFavs(false),
    m_onlineInTitle(false),
    m_customList(false)
{
    g_sampleInstance = this;

    // Register to be notified if the Device is lost or recreated
    m_deviceResources->RegisterDeviceNotify(this);
    m_sceneRenderer = std::unique_ptr<Renderer>(new Renderer(m_deviceResources));
    m_user = std::make_shared< xbox::services::system::xbox_live_user >();

    xbox::services::system::xbox_live_services_settings::get_singleton_instance()->add_wns_handler([this](xbox::services::system::xbox_live_wns_event_args args)
    {
        Log(L"WNS notification received.");
        Log(L"  type:" + args.notification_type());
        Log(L"  xuid:" + args.xbox_user_id());
        Log(L"  content:" + args.notification_content());
    });
}

void Game::RegisterInputKeys()
{
    m_input->RegisterKey(Windows::System::VirtualKey::S, ButtonPress::SignIn);
    m_input->RegisterKey(Windows::System::VirtualKey::GamepadA, ButtonPress::SignIn);
    m_input->RegisterKey(Windows::System::VirtualKey::Number1, ButtonPress::ToggleSocialGroup1);
    m_input->RegisterKey(Windows::System::VirtualKey::Number2, ButtonPress::ToggleSocialGroup2);
    m_input->RegisterKey(Windows::System::VirtualKey::Number3, ButtonPress::ToggleSocialGroup3);
    m_input->RegisterKey(Windows::System::VirtualKey::Number4, ButtonPress::ToggleSocialGroup4);
    m_input->RegisterKey(Windows::System::VirtualKey::Number5, ButtonPress::ToggleSocialGroup5);
    m_input->RegisterKey(Windows::System::VirtualKey::P, ButtonPress::CallGetProfile);
    m_input->RegisterKey(Windows::System::VirtualKey::C, ButtonPress::ImportCustomList);
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

        if (m_socialManager != nullptr)
        {
            UpdateSocialManager();
        }

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
                UpdateSocialGroupForAllUsers(m_allFriends, presence_filter::all, relationship_filter::friends);
            }

            if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup2))
            {
                m_onlineFriends = !m_onlineFriends;
                UpdateSocialGroupForAllUsers(m_onlineFriends, presence_filter::all_online, relationship_filter::friends);
            }

            if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup3))
            {
                m_allFavs = !m_allFavs;
                UpdateSocialGroupForAllUsers(m_allFavs, presence_filter::all , relationship_filter::favorite);
            }

            if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup4))
            {
                m_onlineInTitle = !m_onlineInTitle;
                UpdateSocialGroupForAllUsers(m_onlineInTitle, presence_filter::title_online, relationship_filter::friends);
            }

            if (m_input->IsKeyDown(ButtonPress::ToggleSocialGroup5))
            {
                m_customList = !m_customList;
                UpdateSocialGroupOfListForAllUsers(m_customList);
            }

            if (m_input->IsKeyDown(ButtonPress::CallGetProfile))
            {
                Log(L"Calling get_user_profile");
                m_xboxLiveContext->profile_service().get_user_profile(m_xboxLiveContext->user()->xbox_user_id())
                .then([this](xbox::services::xbox_live_result<xbox::services::social::xbox_user_profile> result)
                {
                    if (result.err())
                        return;

                    auto &payload = result.payload();

                    Log(L"get_user_profile returned: " + payload.gamertag() + L" Gamerscore: " + payload.gamerscore());
                });
            }

            if (m_input->IsKeyDown(ButtonPress::ImportCustomList))
            {
                Log(L"Dev accounts CSV can be exported from XDP");

                Windows::Storage::Pickers::FileOpenPicker^ openPicker = ref new Windows::Storage::Pickers::FileOpenPicker();
                openPicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::List;
                openPicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::Desktop;
                openPicker->FileTypeFilter->Append(ref new Platform::String(L".csv"));

                std::weak_ptr<Game> thisWeakPtr = shared_from_this();

                create_task(openPicker->PickSingleFileAsync())
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
                        if (file != nullptr)
                        {
                            Windows::Storage::AccessCache::StorageApplicationPermissions::FutureAccessList->AddOrReplace(ref new Platform::String(L"LastUsedFile"), file);
                            pThis->ReadCsvFile(file);
                        }
                    }
                    catch (Platform::Exception^)
                    {
                    }
                });
            }

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

void Game::UpdateCustomList(_In_ const std::vector<string_t>& xuidList)
{
    m_xuidList = xuidList;

    // Refresh custom list if its active
    if (m_customList)
    {
        UpdateSocialGroupOfListForAllUsers(false);
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

    InitializeSocialManager();
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

void
Game::CreateSocialGroupsBasedOnUI(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user
    )
{
    UpdateSocialGroup(user, m_allFriends, presence_filter::all, relationship_filter::friends);
    UpdateSocialGroup(user, m_onlineFriends, presence_filter::all_online, relationship_filter::friends);
    UpdateSocialGroup(user, m_allFavs, presence_filter::all, relationship_filter::favorite);
    UpdateSocialGroup(user, m_onlineInTitle, presence_filter::title_online, relationship_filter::friends);
    UpdateSocialGroupOfList(user, m_customList);
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
        default: return _T("unknown");
    }
}

void
Game::LogSocialEventList(std::vector<social_event> eventList)
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

void Game::UpdateSocialGroupForAllUsers(
    _In_ bool toggle,
    _In_ presence_filter presenceFilter,
    _In_ relationship_filter relationshipFilter
    )
{
    if (m_xboxLiveContext != nullptr)
    {
        UpdateSocialGroup(m_xboxLiveContext->user(), toggle, presenceFilter, relationshipFilter);
    }
}

void Game::UpdateSocialGroup(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
    _In_ bool toggle,
    _In_ presence_filter presenceFilter,
    _In_ relationship_filter relationshipFilter
    )
{
    std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext = m_xboxLiveContext;

    if (xboxLiveContext != nullptr)
    {
        if (toggle)
        {
            CreateSocialGroupFromFilters(xboxLiveContext->user(), presenceFilter, relationshipFilter);
        }
        else
        {
            DestroySocialGroup(xboxLiveContext->user(), presenceFilter, relationshipFilter);
        }
    }
}

void Game::UpdateSocialGroupOfListForAllUsers(_In_ bool toggle)
{
    if (m_xboxLiveContext != nullptr)
    {
        return UpdateSocialGroupOfList(m_xboxLiveContext->user(), toggle);
    }
}

void Game::UpdateSocialGroupOfList(
    _In_ std::shared_ptr<xbox::services::system::xbox_live_user> user,
    _In_ bool toggle
    )
{
    std::shared_ptr<xbox::services::xbox_live_context> xboxLiveContext = m_xboxLiveContext;

    if (xboxLiveContext != nullptr)
    {
        if (toggle)
        {
            CreateOrUpdateSocialGroupFromList(xboxLiveContext->user(), m_xuidList);
        }
        else
        {
            DestorySocialGroupFromList(xboxLiveContext->user());
        }
    }
}

Platform::String^ StringFormat(LPCWSTR strMsg, ...)
{
    WCHAR strBuffer[2048];

    va_list args;
    va_start(args, strMsg);
    _vsnwprintf_s(strBuffer, 2048, _TRUNCATE, strMsg, args);
    strBuffer[2047] = L'\0';

    va_end(args);

    Platform::String^ str = ref new Platform::String(strBuffer);
    return str;
}

void Game::HandleSignInResult(
    _In_ const xbox::services::system::sign_in_result& result
    )
{
    switch (result.status())
    {
        case xbox::services::system::sign_in_status::success:
            Log(L"xuid: "+ m_user->xbox_user_id());
            m_xboxLiveContext = std::make_shared< xbox::services::xbox_live_context >(m_user);

            m_xboxLiveContext->settings()->add_service_call_routed_handler([this](xbox::services::xbox_service_call_routed_event_args args)
            {
                stringstream_t ss;
                Log(L"service_call_routed_handler:");
                ss << "[URL]: " << args.http_method().c_str() << " " << args.uri().c_str();
                Log(ss.str());
                ss << "[Response]: " << args.http_status() << " " << args.response_body().c_str();
                Log(ss.str());
            });
            m_xboxLiveContext->settings()->set_enable_service_call_routed_events(true);
            xbox::services::service_call_logging_config::get_singleton_instance()->enable();

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
    if (m_user->is_signed_in())
    {
        Log(L"Already signed in.");
        return;
    }

    std::weak_ptr<Game> thisWeakPtr = shared_from_this();
    auto asyncOp = m_user->signin(nullptr);
    create_task(asyncOp)
    .then([thisWeakPtr](xbox::services::xbox_live_result<xbox::services::system::sign_in_result> t)
    {
        std::shared_ptr<Game> pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            return;
        }

        if (!t.err())
        {
            auto result = t.payload();
            pThis->HandleSignInResult(result);
        }
        else
        {
            std::stringstream ss;
            ss << "Failed signing in" << t.err().value() << " ,msg: " << t.err_message();
            pThis->Log(utility::conversions::to_utf16string(ss.str()));
        }

    }, task_continuation_context::use_current());
}

void Game::SignInSilently()
{
    std::weak_ptr<Game> thisWeakPtr = shared_from_this();

    auto asyncOp = m_user->signin_silently(nullptr);
    create_task(asyncOp)
    .then([thisWeakPtr](xbox::services::xbox_live_result<xbox::services::system::sign_in_result> t)
    {
        std::shared_ptr<Game> pThis(thisWeakPtr.lock());
        if (pThis == nullptr)
        {
            return;
        }

        if (!t.err())
        {
            auto result = t.payload();
            pThis->HandleSignInResult(result);
        }
        else
        {
            pThis->Log(L"Failed signing in.");
        }
    }, task_continuation_context::use_current());
}


void Game::HandleSignout(_In_ std::shared_ptr<xbox::services::system::xbox_live_user> user)
{
    std::shared_ptr<xbox::services::xbox_live_context> xblContext = GetXboxLiveContext();
    m_xboxLiveContext = nullptr;

    WCHAR text[1024];
    swprintf_s(text, ARRAYSIZE(text), L"User %s signed out", user->gamertag().c_str());
    Log(text);

    RemoveUserFromSocialManager(user);
}

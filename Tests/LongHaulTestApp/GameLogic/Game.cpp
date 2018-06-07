// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Game.h"
#include "Common\DirectXHelper.h"
#include "Utils\PerformanceCounters.h"
#include "httpClient\httpClient.h"
#include "AsyncIntegration.h"
#include "Tests.h"

using namespace LongHaulTestApp;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

using namespace xbox::services;
using namespace xbox::services::system;

Game* g_sampleInstance = nullptr;
std::mutex Game::m_displayEventQueueLock;

#define TEST_DELAY_SLOW 15
#define TEST_DELAY_FAST 2

// Loads and initializes application assets when the application is loaded.
Game::Game(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
    m_deviceResources(deviceResources),
    bInitialized(false)
{
    g_sampleInstance = this;
    m_lastDeltaMem = 0; 
    m_curDeltaMem = 0;
    
    // Register to be notified if the Device is lost or recreated
    m_deviceResources->RegisterDeviceNotify(this);
    m_sceneRenderer = std::unique_ptr<Renderer>(new Renderer(m_deviceResources)); 

    CreateSharedAsyncQueue(
        0,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        AsyncQueueDispatchMode::AsyncQueueDispatchMode_Manual,
        &m_queue);
    
    XblInitialize();
    InitializeAsync(m_queue, &m_asyncQueueCallbackToken);

    XblDisableAssertsForXboxLiveThrottlingInDevSandboxes(
        XblConfigSetting_ThisCodeNeedsToBeChanged
        );

    XblUserCreateHandle(&m_user);
}

void Game::RegisterInputKeys()
{
    m_input->RegisterKey(Windows::System::VirtualKey::S, ButtonPress::SignIn);
    m_input->RegisterKey(Windows::System::VirtualKey::T, ButtonPress::StartTests);
    m_input->RegisterKey(Windows::System::VirtualKey::R, ButtonPress::StartTestsFast);
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
        if (m_input->IsKeyDown(ButtonPress::SignIn))
        {
            SignIn();
        }

        if (m_input->IsKeyDown(ButtonPress::StartTests))
        {
            Tests::TestDelay = TEST_DELAY_SLOW;
            m_testsManager.StartTests(m_xboxLiveContext);
        }
            
        if (m_input->IsKeyDown(ButtonPress::StartTestsFast))
        {
            Tests::TestDelay = TEST_DELAY_FAST;
            m_testsManager.StartTests(m_xboxLiveContext);
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

    SignInSilently();


    Platform::String^ localfolder = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
    std::wstring localFolderW(localfolder->Begin());
    std::string localFolderA(localFolderW.begin(), localFolderW.end());
    m_logFileName = localFolderA + "\\long_haul_log.txt";

    Windows::System::Launcher::LaunchFolderAsync(Windows::Storage::ApplicationData::Current->LocalFolder);

    Game* pThis = this;
    auto settings = xbox_live_services_settings::get_singleton_instance(true);
    settings->set_diagnostics_trace_level(xbox_services_diagnostics_trace_level::off);
    settings->add_logging_handler([pThis](
        xbox_services_diagnostics_trace_level traceLevel,
        const std::string& category,
        const std::string& message
        )
    {
        pThis->Log("[" + pThis->TaceLevelToString(traceLevel) + "][" + category + "] " + message);
    });

    PrintMemoryUsage();
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

void Game::Log(std::wstring log, bool showOnUI)
{
    std::lock_guard<std::mutex> guard(m_displayEventQueueLock);
    if (showOnUI)
    {
        m_displayEventQueue.push_back(log);
        if (m_displayEventQueue.size() > 30)
        {
            m_displayEventQueue.erase(m_displayEventQueue.begin());
        }
    }

    if (!m_logFileName.empty())
    {
        m_logFile.open(m_logFileName, std::ofstream::out | std::ofstream::app);
        m_logFile << utility::conversions::to_utf8string(log) << "\n";
        m_logFile.flush();
        m_logFile.close();
    }
}

void Game::Log(std::string log, bool showOnUI)
{
    Log(utility::conversions::to_utf16string(log), showOnUI);
}

void Game::PrintMemoryUsage()
{
    auto process = Windows::System::Diagnostics::ProcessDiagnosticInfo::GetForCurrentProcess();
    auto report = process->MemoryUsage->GetReport();
    if (!m_gotInitMemReport)
    {
        m_initMemReport = report;
        m_curMemReport = report;
        m_gotInitMemReport = true;
    }
    else
    {
        m_curMemReport = report;
        g_sampleInstance->m_lastDeltaMem = g_sampleInstance->m_curDeltaMem;
        g_sampleInstance->m_curDeltaMem = m_curMemReport->PeakVirtualMemorySizeInBytes - m_initMemReport->PeakVirtualMemorySizeInBytes;
    }

    stringstream_t stream;

    stream
        << "\n\n\n"
        << "=========================\n"
        << "===   Memory Ussage   ===\n"
        << "=========================\n\n\n"
        << "Tests Run: " << Tests::TestsRun << "\n"
        << "NonPagedPoolSizeInBytes: " << report->NonPagedPoolSizeInBytes << "\n"
        << "PagedPoolSizeInBytes: " << report->PagedPoolSizeInBytes << "\n"
        << "PageFaultCount: " << report->PageFaultCount << "\n"
        << "PageFileSizeInBytes: " << report->PageFileSizeInBytes << "\n"
        << "PeakNonPagedPoolSizeInBytes: " << report->PeakNonPagedPoolSizeInBytes << "\n"
        << "PeakPagedPoolSizeInBytes: " << report->PeakPagedPoolSizeInBytes << "\n"
        << "PeakPageFileSizeInBytes: " << report->PeakPageFileSizeInBytes << "\n"
        << "PeakVirtualMemorySizeInBytes: " << report->PeakVirtualMemorySizeInBytes << "\n"
        << "PeakWorkingSetSizeInBytes: " << report->PeakWorkingSetSizeInBytes << "\n"
        << "PrivatePageCount: " << report->PrivatePageCount << "\n"
        << "VirtualMemorySizeInBytes: " << report->VirtualMemorySizeInBytes << "\n"
        << "WorkingSetSizeInBytes: " << report->WorkingSetSizeInBytes
        << "\n\n\n";

    Log(stream.str(), false);
}

string Game::TaceLevelToString(xbox_services_diagnostics_trace_level traceLevel)
{
    string level = "Unknown";

    switch (traceLevel)
    {
    case xbox_services_diagnostics_trace_level::error: level = "Error"; break;
    case xbox_services_diagnostics_trace_level::info: level = "Info"; break;
    case xbox_services_diagnostics_trace_level::verbose: level = "Verbose"; break;
    case xbox_services_diagnostics_trace_level::warning: level = "Warning"; break;
    }

    return level;
}
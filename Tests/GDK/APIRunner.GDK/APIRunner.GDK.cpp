// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include "pch.h"
#include "APIRunner.GDK.h"

#include "ATGColors.h"
#include "FindMedia.h"
#include "Kits\ATGTK\StringUtil.h"

#include <XGameErr.h>

extern void ExitSample();

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Sample* g_Sample = nullptr;

namespace
{
    const int c_sampleUIPanel = 2000;
}

Sample::Sample() noexcept(false) :
    m_frame(0),
    m_asyncQueue(nullptr)
{
    g_Sample = this;

    DX::ThrowIfFailed(
        XTaskQueueCreate(XTaskQueueDispatchMode::ThreadPool, XTaskQueueDispatchMode::Manual, &m_asyncQueue)
    );

    // Renders only 2D, so no need for a depth buffer.
    m_deviceResources = std::make_unique<DX::DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_UNKNOWN);
    m_liveResources = std::make_shared<ATG::LiveResources>(m_asyncQueue);
    m_liveInfoHUD = std::make_unique<ATG::LiveInfoHUD>("Xbox Live API Runner GDK");

    ATG::UIConfig uiconfig;
    m_ui = std::make_shared<ATG::UIManager>(uiconfig);
    m_log = std::make_unique<DX::TextConsoleImage>();
    ApiRunnerSetupApiExplorer();
}

Sample::~Sample()
{
    if (m_deviceResources)
    {
        m_deviceResources->WaitForGpu();
    }

    if (m_asyncQueue)
    {
        XTaskQueueCloseHandle(m_asyncQueue);
        m_asyncQueue = nullptr;
    }
}

// Initialize the Direct3D resources required to run.
void Sample::Initialize(HWND window, int width, int height)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();

    m_mouse = std::make_unique<Mouse>();
#ifdef _GAMING_DESKTOP
    m_mouse->SetWindow(window);
#endif


#if !defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
    // NOTE: When running the app from the Start Menu (required for
    //    Store API's to work) the Current Working Directory will be
    //    returned as C:\Windows\system32 unless you overwrite it.
    //    The sample relies on the font and image files in the .exe's
    //    directory and so we do the following to set the working
    //    directory to what we want.
    char dir[1024];
    GetModuleFileNameA(NULL, dir, 1024);
    std::string exe = dir;
    exe = exe.substr(0, exe.find_last_of("\\"));
    SetCurrentDirectoryA(exe.c_str());
#endif

    m_ui->LoadLayout(L".\\Assets\\SampleUI.csv", L".\\Assets");

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();      
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    uint32_t titleId = 0;
    HRESULT hr = XGameGetXboxTitleId(&titleId);
    if (FAILED(hr))
    {
        m_log->WriteLine(L"Can not get title ID.  Ensure you are running inside a app package");
    }

    m_liveResources->SetUserChangedCallback([this](XUserHandle user)
        {
            m_liveInfoHUD->SetUser(user, m_asyncQueue);
            m_ui->FindPanel<ATG::IPanel>(c_sampleUIPanel)->Show();
        });

    m_liveResources->SetUserSignOutCompletedCallback([this](XUserHandle /*user*/)
        {
            m_liveInfoHUD->SetUser(nullptr, m_asyncQueue);
            m_ui->FindPanel<ATG::IPanel>(c_sampleUIPanel)->Close();
        });

    m_liveResources->SetErrorHandler([this](HRESULT error)
    {
        if (error == E_XAL_UIREQUIRED)
        {
            m_liveResources->SignInWithUI();
        }
        else // Handle other error cases
        {

        }
    });

    m_liveResources->Initialize();
    m_liveInfoHUD->Initialize();

    SetupUI();

    // Before we can make an Xbox Live call we need to ensure that the Game OS has initialized the network stack
    // For sample purposes we block user interaction with the sample.  A game should wait for the network to be
    // initialized before the main menu appears.  For samples, we will wait at the end of initialization.
    while (!m_liveResources->IsNetworkAvailable())
    {
    }
}

DWORD WINAPI ApiRunnerDoWork(LPVOID )
{
    if (g_Sample->m_runBVTs)
    {
        ApiRunnerRunTests(TestSet::SingleDeviceBVTs);
    }
    else
    {
        std::string jsonFileContents = ApiRunnerReadFile("cmds.json");
        ApiRunnerProcessJsonCmds(jsonFileContents);
    }
    g_Sample->m_bRunningTests = false;
    g_Sample->m_bTestsFinished = true;

    return 0;
}


void Sample::StartRunTests()
{
    if (!m_bRunningTests && m_log)
    {
        m_bRunningTests = true;
        m_log->Clear();
        m_log->WriteLine(L"Running tests");
        CreateThread(nullptr, 0, ApiRunnerDoWork, nullptr, 0, nullptr);
    }
}


#pragma region UI Methods
void Sample::SetupUI()
{
    using namespace ATG;

}

#pragma endregion

#pragma region Frame Update
// Executes basic render loop.
void Sample::Tick()
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Frame %I64u", m_frame);

    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();

    PIXEndEvent();
    m_frame++;
}

// Updates the world.
void Sample::Update(DX::StepTimer const& timer)
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Update");
    float elapsedTime = float(timer.GetElapsedSeconds());

    // Auto start tests after a small delay
    m_startTimer -= elapsedTime;
    if (m_startTimer < 0.0f && !m_bAutoStarted)
    {
        if (m_cmdLine.find(L"/test:") != std::wstring::npos)
        {
            std::wstring num = m_cmdLine.substr(m_cmdLine.find(L"/test:") + 6);
            if (num.find(L" ") != std::wstring::npos)
            {
                num = num.substr(0, num.find(L" "));
            }
            int testNum = _wtoi(num.c_str());
            ApiRunnerSetRunTestsParams(testNum, 0, 0);
        }

        if (m_cmdLine.find(L"/range:") != std::wstring::npos)
        {
            std::wstring num = m_cmdLine.substr(m_cmdLine.find(L"/range:") + 7);
            if (num.find(L" ") != std::wstring::npos)
            {
                num = num.substr(0, num.find(L" "));
            }
            std::wstring num1 = num.substr(0, num.find(L":"));
            std::wstring num2 = num.substr(num.find(L":")+1);
            int minNum = _wtoi(num1.c_str());
            int maxNum = _wtoi(num2.c_str());
            ApiRunnerSetRunTestsParams(0, minNum, maxNum);
        }

        m_bAutoStarted = true;
        m_runBVTs = false;
        StartRunTests();
    }

    if (m_bTestsFinished)
    {
        if (m_cmdLine.find(L"/exit") != std::wstring::npos)
        {
            ExitSample();
        }
    }

    auto pad = m_gamePad->GetState(0);
    if (pad.IsConnected())
    {
        m_gamePadButtons.Update(pad);

        if (pad.IsViewPressed())
        {
            ExitSample();
        }

        if (m_gamePadButtons.menu == GamePad::ButtonStateTracker::PRESSED)
        {
            if (!m_liveResources->IsUserSignedIn())
            {
                m_log->WriteLine(L"Sign in silently");
                m_liveResources->SignInSilently();
            }
            else
            {
                m_log->WriteLine(L"Sign in with UI");
                m_liveResources->SignInWithUI();
            }
        }

        m_ui->Update(elapsedTime, pad);
    }
    else
    {
        m_gamePadButtons.Reset();
    }

    auto kb = m_keyboard->GetState();
    m_keyboardButtons.Update(kb);

    if (kb.Enter || kb.Space || kb.B)
    {
        m_runBVTs = true;
        StartRunTests();        
    }
    if (kb.A)
    {
        m_runBVTs = false;
        StartRunTests();
    }
    if (kb.Escape)
    {
        ExitSample();
    }

    // Process any completed tasks
    while (XTaskQueueDispatch(m_asyncQueue, XTaskQueuePort::Completion, 0))
    {
    }

    m_liveInfoHUD->Update(m_deviceResources->GetCommandQueue());

    PIXEndEvent();
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Sample::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    // Prepare the command list to render a new frame.
    m_deviceResources->Prepare();
    Clear();

    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Render");

    ID3D12DescriptorHeap* heap = m_resourceDescriptors->Heap();
    commandList->SetDescriptorHeaps(1, &heap);

    m_liveInfoHUD->Render(commandList);

    if (m_log) m_log->Render(commandList);
    if (m_ui) m_ui->Render(commandList);

    PIXEndEvent(commandList);

    // Show the new frame.
    PIXBeginEvent(m_deviceResources->GetCommandQueue(), PIX_COLOR_DEFAULT, L"Present");
    m_deviceResources->Present();
    m_graphicsMemory->Commit(m_deviceResources->GetCommandQueue());
    PIXEndEvent(m_deviceResources->GetCommandQueue());
}

// Helper method to clear the back buffers.
void Sample::Clear()
{
    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Clear");

    // Clear the views.
    auto rtvDescriptor = m_deviceResources->GetRenderTargetView();

    commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, nullptr);
    commandList->ClearRenderTargetView(rtvDescriptor, ATG::Colors::Background, 0, nullptr);

    // Set the viewport and scissor rect.
    auto viewport = m_deviceResources->GetScreenViewport();
    auto scissorRect = m_deviceResources->GetScissorRect();
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    PIXEndEvent(commandList);
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Sample::OnActivated()
{
}

void Sample::OnDeactivated()
{
}

void Sample::OnSuspending()
{
    m_deviceResources->Suspend();
}

void Sample::OnResuming()
{
    m_deviceResources->Resume();
    m_timer.ResetElapsedTime();
    m_gamePadButtons.Reset();
    m_keyboardButtons.Reset();
    m_liveResources->Refresh();
    m_ui->Reset();
    m_log.reset();
}

void Sample::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Sample::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

// Properties
void Sample::GetDefaultSize(int& width, int& height) const
{
    width = 1920;
    height = 1080;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Sample::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    m_graphicsMemory = std::make_unique<GraphicsMemory>(device);

    RenderTargetState rtState(m_deviceResources->GetBackBufferFormat(), m_deviceResources->GetDepthBufferFormat());

    ResourceUploadBatch resourceUpload(device);
    resourceUpload.Begin();

    m_resourceDescriptors = std::make_unique<DirectX::DescriptorPile>(device,
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        Descriptors::Count,
        Descriptors::Reserve
        );

    m_liveInfoHUD->RestoreDevice(device, rtState, resourceUpload, *m_resourceDescriptors);

    m_log->RestoreDevice(
        device,
        resourceUpload,
        rtState,
        L"Media\\Fonts\\SegoeUI_18.spritefont",
        L"Assets\\ATGSampleBackground.DDS",
        m_resourceDescriptors->GetCpuHandle(Descriptors::Font),
        m_resourceDescriptors->GetGpuHandle(Descriptors::Font),
        m_resourceDescriptors->GetCpuHandle(Descriptors::Background),
        m_resourceDescriptors->GetGpuHandle(Descriptors::Background)
    );

    m_ui->RestoreDevice(device, rtState, resourceUpload, *m_resourceDescriptors);

    auto uploadResourcesFinished = resourceUpload.End(m_deviceResources->GetCommandQueue());
    uploadResourcesFinished.wait();
}

// Allocate all memory resources that change on a window SizeChanged event.
void Sample::CreateWindowSizeDependentResources()
{
    m_liveInfoHUD->SetViewport(m_deviceResources->GetScreenViewport());

    auto viewport = m_deviceResources->GetScreenViewport();
    static const RECT screenDisplay = { 50, 150, static_cast<LONG>(viewport.Width-50), static_cast<LONG>(viewport.Height-150) };
    m_log->SetWindow(screenDisplay, false);
    m_log->SetViewport(viewport);

    RECT size = m_deviceResources->GetOutputSize();
    m_ui->SetWindow(size);
}

void Sample::OnDeviceLost()
{
    m_graphicsMemory.reset();
    m_liveInfoHUD->ReleaseDevice();
    m_resourceDescriptors.reset();
}

void Sample::OnDeviceRestored()
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}
#pragma endregion

void LogToScreen(_Printf_format_string_ char const* format, ...)
{
    char message[8000] = {};

    va_list varArgs{};
    va_start(varArgs, format);
    pal::vsprintf(message, 4096, format, varArgs);
    va_end(varArgs);

    if (g_Sample && g_Sample->m_log != nullptr)
    {
        std::wstring wstr = DX::Utf8ToWide(message);
        g_Sample->m_log->WriteLine(wstr.c_str());
    }
    else
    {
        std::cout << message;
        std::cout << "\n";
    }

    LogToFile(message);
}

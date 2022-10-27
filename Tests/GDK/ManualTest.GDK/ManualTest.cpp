//--------------------------------------------------------------------------------------
// ManualTest.cpp
//
// Advanced Technology Group (ATG)
// Copyright (C) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include "pch.h"
#include "ManualTest.h"

#include "ATGColors.h"
#include "FindMedia.h"
#include "StringUtil.h"
#include <XGameRuntimeFeature.h>
#include <XNetworking.h>
#include <XGameRuntimeInit.h>

inline std::string DebugFormat(_In_z_ _Printf_format_string_ const char* format, ...)
{
#ifdef _DEBUG
    va_list args;
    va_start(args, format);

    char buff[1024*10] = {};
    vsprintf_s(buff, format, args);
    std::string str = buff;
    va_end(args);
    return buff;
#else
    UNREFERENCED_PARAMETER(format);
#endif
}


extern void ExitSample() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Sample* g_sample = nullptr;

namespace
{
    const char *ProgressStateText[] = {
        u8"Unknown",
        u8"Achieved",
        u8"Not Started",
        u8"In Progress"
    };

    const char *AchievementTypeText[] = {
        u8"Unknown",
        u8"All",
        u8"Persistent",
        u8"Challenge"
    };

    const int c_sampleUIPanel = 2000;
    const int c_getAchievementsBtn = 2101;
    const int c_getSingleAchievementBtn = 2102;
    const int c_setSingleAchievementBtn = 2103;
    const int c_getSingleAchievementBtn2 = 2104;
    const int c_setSingleAchievement2Btn25 = 2105;
    const int c_setSingleAchievement2Btn50 = 2106;
    const int c_setSingleAchievement2Btn100 = 2107;
}


void CALLBACK MyXUserChangeEventCallback(
    _In_opt_ void* context,
    _In_ XUserLocalId userLocalId,
    _In_ XUserChangeEvent event)
{
    UNREFERENCED_PARAMETER(context);
    std::string eventType = "";
    switch (event)
    {
        case XUserChangeEvent::SignedInAgain: eventType = "SignedInAgain"; break;
        case XUserChangeEvent::SigningOut: eventType = "SigningOut"; break;
        case XUserChangeEvent::SignedOut: eventType = "SignedOut"; break;
        case XUserChangeEvent::Gamertag: eventType = "Gamertag"; break;
        case XUserChangeEvent::GamerPicture: eventType = "GamerPicture"; break;
        case XUserChangeEvent::Privileges:eventType = "Privileges"; break;
        default: break;
    }

    XUserHandle handle;
    uint64_t xuid = {};
    HRESULT hr = XUserFindUserByLocalId(
        userLocalId,
        &handle);
    if (SUCCEEDED(hr))
    {
        XUserGetId(handle, &xuid);
        XUserCloseHandle(handle);
    }

    g_sample->AddLog(DebugFormat("XUserChangeEventCallback: userLocalId: %ull = %s xuid: 0x%0.8x", userLocalId.value, eventType.c_str(), xuid));
}


void __cdecl MyHCCallRoutedHandler(
    _In_ HCCallHandle call,
    _In_opt_ void* context
    )
{
    UNREFERENCED_PARAMETER(context);
    const char* url = nullptr;
    uint32_t status = 0;
    HCHttpCallGetRequestUrl(call, &url);
    HCHttpCallResponseGetStatusCode(call, &status);
    HRESULT hrNet = S_OK;
    uint32_t hrPlat = S_OK;
    HCHttpCallResponseGetNetworkErrorCode(call, &hrNet, &hrPlat);
    g_sample->AddLog(DebugFormat("HCCallRoutedHandler: %d [0x%0.8x] %s", status, hrNet, url));
}

void CALLBACK MyHCTraceCallback(
    _In_z_ const char* areaName,
    _In_ HCTraceLevel level,
    _In_ uint64_t threadId,
    _In_ uint64_t timestamp,
    _In_z_ const char* message
)
{
    UNREFERENCED_PARAMETER(areaName);
    UNREFERENCED_PARAMETER(level);
    UNREFERENCED_PARAMETER(threadId);
    UNREFERENCED_PARAMETER(timestamp);
    g_sample->AddLog(DebugFormat("HC: %s", message));
}

void MyNetworkConnectivityChangedCallback(void* context, const XNetworkingConnectivityHint* /*hint*/)
{
    UNREFERENCED_PARAMETER(context);
    // Always requery the latest network connectivity hint rather than relying on the passed parameter in case this is a stale notification
    XNetworkingConnectivityHint hint{};
    HRESULT hr = XNetworkingGetConnectivityHint(&hint);
    if (SUCCEEDED(hr))
    {
        g_sample->AddLog(DebugFormat("NetworkConnectivityChangedCallback: networkInitialized: %d connectivityLevel: %d", hint.networkInitialized, hint.connectivityLevel));
    }
    else
    {
        g_sample->AddLog(DebugFormat("NetworkConnectivityChangedCallback: 0x%0.8x", hr));
    }
}


Sample::Sample() noexcept(false) :
    m_frame(0)
{
    g_sample = this;
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
    m_liveInfoHUD = std::make_unique<ATG::SampleLiveInfoHUD>("Title-managed ManualTest Sample");

    DX::ThrowIfFailed(
        XTaskQueueCreate(XTaskQueueDispatchMode::ThreadPool, XTaskQueueDispatchMode::Manual, &m_mainAsyncQueue)
    );

    ATG::UIConfig uiconfig;
    m_ui = std::make_unique<ATG::UIManager>(uiconfig);
    m_log = std::make_unique<DX::TextConsoleImage>();
    m_display = std::make_unique<DX::TextConsoleImage>();  

    XTaskQueueRegistrationToken token = {};
    XUserRegisterForChangeEvent(nullptr, nullptr, MyXUserChangeEventCallback, &token);

    HCTraceSetClientCallback(MyHCTraceCallback);
    m_liveInfoHUD->AddLog("**************** GAME START ****************");

    XTaskQueueRegistrationToken tokenHint;
    HRESULT hr = XNetworkingRegisterConnectivityHintChanged(nullptr, nullptr, MyNetworkConnectivityChangedCallback, &tokenHint);
    m_liveInfoHUD->AddLog(DebugFormat("XNetworkingRegisterConnectivityHintChanged 0x%0.8x", hr));
}

Sample::~Sample()
{
    if (m_deviceResources)
    {
        m_deviceResources->WaitForGpu();
    }

    if (m_mainAsyncQueue)
    {
        XTaskQueueCloseHandle(m_mainAsyncQueue);
        m_mainAsyncQueue = nullptr;
    }

    XGameRuntimeUninitialize();
}

// Initialize the Direct3D resources required to run.
void Sample::Initialize(HWND window, int width, int height)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    m_deviceResources->SetWindow(window, width, height);

    wchar_t result[MAX_PATH];
    DX::FindMediaFile(result, MAX_PATH, L".\\Assets\\SampleUI.csv");
    m_ui->LoadLayout(result, L".\\Assets\\");

    HRESULT hr = XGameRuntimeInitialize();
    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    m_liveInfoHUD->Initialize();

    SetupUI();
}

void Sample::SetupUI()
{
}

#pragma region Frame Update
// Executes basic render loop.
void Sample::Tick()
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Frame %llu", m_frame);

    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();

    PIXEndEvent();
    m_frame++;
}

static std::string NewGuid()
{
    GUID id = {};
    char buf[64] = {};

    CoCreateGuid(&id);

    sprintf_s(buf, "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
        id.Data1, id.Data2, id.Data3,
        id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3],
        id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);

    return std::string(buf);
}


void Sample::CreateSession()
{
    if (m_xblContext == nullptr)
    {
        g_sample->m_liveInfoHUD->AddLog(DebugFormat("CreateSession no m_xblContext"));
        return;
    }

    if (m_userHandle1 == nullptr)
    {
        g_sample->m_liveInfoHUD->AddLog(DebugFormat("CreateSession Add user first"));
        return;
    }

    if (m_currentSessionHandle != nullptr)
    {
        g_sample->m_liveInfoHUD->AddLog(DebugFormat("CreateSession Session exists"));
        return;
    }

    XblMultiplayerSessionInitArgs initArgs;
    initArgs.MaxMembersInSession = 8; //This matches our session template
    initArgs.Visibility = XblMultiplayerSessionVisibility::Open; //The session is open and can be joined by anyone.
    initArgs.InitiatorXuids = nullptr;
    initArgs.InitiatorXuidsCount = 0;
    initArgs.CustomJson = nullptr;

    const char* scid = nullptr;
    XblGetScid(&scid);
    std::string currentSessionName = NewGuid();
    XblMultiplayerSessionReference createdSessionRef = XblMultiplayerSessionReferenceCreate(scid, "GameSession", currentSessionName.c_str());

    uint64_t userId = 0;
    XUserGetId(m_userHandle1, &userId);
    m_currentSessionHandle = XblMultiplayerSessionCreateHandle(userId, &createdSessionRef, &initArgs);
    m_liveInfoHUD->AddLog(DebugFormat("m_currentSessionHandle 0x%0.8x", m_currentSessionHandle));

    HRESULT hr = XblMultiplayerSessionJoin(m_currentSessionHandle, nullptr, true, true);
    m_liveInfoHUD->AddLog(DebugFormat("XblMultiplayerSessionJoin 0x%0.8x", hr));
}

void Sample::WriteSession()
{
    if (m_xblContext == nullptr)
    {
        g_sample->m_liveInfoHUD->AddLog(DebugFormat("no m_xblContext"));
        return;
    }
    if( m_currentSessionHandle == nullptr)
    {
        g_sample->m_liveInfoHUD->AddLog(DebugFormat("no m_currentSessionHandle"));
        return;
    }

    auto asyncBlock = std::make_unique<XAsyncBlock>();
    asyncBlock->queue = nullptr;
    asyncBlock->context = nullptr;
    asyncBlock->callback = [](XAsyncBlock* asyncBlock)
    {
        g_sample->m_liveInfoHUD->AddLog("***** XblMultiplayerWriteSessionAsync callback");

        std::unique_ptr<XAsyncBlock> asyncBlockPtr{ asyncBlock }; //Take over ownership of the XAsyncBlock*

        XblMultiplayerSessionHandle createdHandle;
        HRESULT hr = XblMultiplayerWriteSessionResult(asyncBlock, &createdHandle);
        g_sample->m_liveInfoHUD->AddLog(DebugFormat("XblMultiplayerWriteSessionResult 0x%0.8x", hr));
        if (hr == S_OK)
        {
            XblWriteSessionStatus status = XblMultiplayerSessionWriteStatus(createdHandle);
            g_sample->m_liveInfoHUD->AddLog(DebugFormat("XblMultiplayerSessionWriteStatus %d", status));
        }

        XblMultiplayerSessionCloseHandle(createdHandle);
    };

    HRESULT hr = XblMultiplayerWriteSessionAsync(m_xblContext, m_currentSessionHandle, XblMultiplayerSessionWriteMode::UpdateOrCreateNew, asyncBlock.get());
    m_liveInfoHUD->AddLog(DebugFormat("XblMultiplayerWriteSessionAsync 0x%0.8x", hr));
    if (SUCCEEDED(hr))
    {
        // The call succeeded, so release the std::unique_ptr ownership of XAsyncBlock* since the callback will take over ownership.
        // If the call fails, the std::unique_ptr will keep ownership and delete the XAsyncBlock*
        asyncBlock.release();
    }
    else
    {
        XblMultiplayerSessionCloseHandle(m_currentSessionHandle);
    }    
}

// Updates the world.
void Sample::Update(DX::StepTimer const& timer)
{
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Update");

    float elapsedTime = float(timer.GetElapsedSeconds());

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

        }

        m_ui->Update(elapsedTime, pad);
    }
    else
    {
        m_gamePadButtons.Reset();
    }

    auto kb = m_keyboard->GetState();
    m_keyboardButtons.Update(kb);

    if (kb.Escape)
    {
        ExitSample();
    }

    bool isControlPressed = kb.LeftControl || kb.RightControl;

    if (!isControlPressed)
    {
        if ((m_keyboardButtons.IsKeyPressed(Keyboard::Keys::D1) && !m_keyboardButtonsLast.IsKeyPressed(Keyboard::Keys::D1)))
        {
            if (!m_xblInit)
            {
                m_xblInit = true;
                uint32_t titleId = 0;
                auto hr = XGameGetXboxTitleId(&titleId);
                char scidBuffer[64] = {};
                sprintf_s(scidBuffer, "00000000-0000-0000-0000-0000%08X", titleId);
                XblInitArgs xblInit = { nullptr, scidBuffer };
                hr = XblInitialize(&xblInit);
                m_liveInfoHUD->AddLog(DebugFormat("XblInitialize 0x%0.8x", hr));
                hr = HCAddCallRoutedHandler(MyHCCallRoutedHandler, nullptr);
                m_liveInfoHUD->AddLog(DebugFormat("HCAddCallRoutedHandler 0x%0.8x", hr));
                g_sample->AddLog(DebugFormat("XblInitialize 0x%0.8x done", hr));
            }
        }

        if ((m_keyboardButtons.IsKeyPressed(Keyboard::Keys::D2) && !m_keyboardButtonsLast.IsKeyPressed(Keyboard::Keys::D2)))
        {
            if (m_userHandle1 == nullptr)
            {
                AddUser(XUserAddOptions::AddDefaultUserSilently);
                g_sample->AddLog(DebugFormat("AddUser done"));
            }
            else
            {
                g_sample->AddLog(DebugFormat("no user"));
            }
        }

        if ((m_keyboardButtons.IsKeyPressed(Keyboard::Keys::D3) && !m_keyboardButtonsLast.IsKeyPressed(Keyboard::Keys::D3)))
        {
            if (m_xblInit && !m_xblRTA && m_xblContext)
            {
                HRESULT hr = XblMultiplayerSetSubscriptionsEnabled(m_xblContext, true);
                g_sample->AddLog(DebugFormat("XblMultiplayerSetSubscriptionsEnabled 0x%0.8x", hr));

                m_xblRTA = true;
                hr = XblRealTimeActivityActivate(m_xblContext);
                g_sample->AddLog(DebugFormat("XblRealTimeActivityActivate 0x%0.8x", hr));
                g_sample->AddLog(DebugFormat("RTA done"));
            }
            else
            {
                if(!m_xblInit)
                    g_sample->AddLog(DebugFormat("rta - not init"));
                if (!m_xblContext)
                    g_sample->AddLog(DebugFormat("rta - not m_xblContext"));
            }
        }

        if ((m_keyboardButtons.IsKeyPressed(Keyboard::Keys::D4) && !m_keyboardButtonsLast.IsKeyPressed(Keyboard::Keys::D4)))
        {
            CreateSession();
        }

        if ((m_keyboardButtons.IsKeyPressed(Keyboard::Keys::D5) && !m_keyboardButtonsLast.IsKeyPressed(Keyboard::Keys::D5)))
        {
            WriteSession();
        }
    }
    else
    {
        if ((m_keyboardButtons.IsKeyPressed(Keyboard::Keys::D1) && !m_keyboardButtonsLast.IsKeyPressed(Keyboard::Keys::D1)))
        {
            if (m_xblInit)
            {
                m_xblInit = false;
                auto async = new XAsyncBlock{};
                HRESULT hr = XblCleanupAsync(async);
                g_sample->AddLog(DebugFormat("XblCleanupAsync 0x%0.8x", hr));
                XAsyncGetStatus(async, true);
                g_sample->AddLog(DebugFormat("XblCleanupAsync 0x%0.8x done", hr));
            }
            else
            {
                g_sample->AddLog(DebugFormat("cleanup - not init"));
            }
        }

        if ((m_keyboardButtons.IsKeyPressed(Keyboard::Keys::D2) && !m_keyboardButtonsLast.IsKeyPressed(Keyboard::Keys::D2)))
        {
            if (m_userHandle1 != nullptr)
            {
                uint64_t xuid = {};
                XUserGetId(m_userHandle1, &xuid);
                XUserLocalId userLocalId = {};
                XUserGetLocalId(m_userHandle1, &userLocalId);

                g_sample->AddLog(DebugFormat("Closing XUserHandle 0x%0.8x. userLocalId: %ull, xuid: 0x%0.8x", m_userHandle1, userLocalId, xuid));
                SetUserHandle(nullptr);
            }
            else
            {
                g_sample->AddLog(DebugFormat("cleanup - no user"));
            }
        }

        if ((m_keyboardButtons.IsKeyPressed(Keyboard::Keys::D3) && !m_keyboardButtonsLast.IsKeyPressed(Keyboard::Keys::D3)))
        {
            if (m_xblInit && m_xblRTA)
            {
                m_xblRTA = false;
                HRESULT hr = XblRealTimeActivityDeactivate(m_xblContext);
                g_sample->AddLog(DebugFormat("XblRealTimeActivityDeactivate 0x%0.8x", hr));
            }
            else
            {
                g_sample->AddLog(DebugFormat("cleanup - no session")); 
            }
        }

        if ((m_keyboardButtons.IsKeyPressed(Keyboard::Keys::D4) && !m_keyboardButtonsLast.IsKeyPressed(Keyboard::Keys::D4)))
        {
            if (m_currentSessionHandle != nullptr)
            {
                XblMultiplayerSessionCloseHandle(m_currentSessionHandle);
                g_sample->AddLog(DebugFormat("XblMultiplayerSessionCloseHandle"));
                m_currentSessionHandle = nullptr;
            }
            else
            {
                g_sample->AddLog(DebugFormat("cleanup - no session"));
            }
        }
    }

    m_keyboardButtonsLast = m_keyboardButtons;

    m_ui->Update(elapsedTime, *m_mouse, *m_keyboard);

    m_liveInfoHUD->Update(m_deviceResources->GetCommandQueue());

    // Process any completed tasks
    while (XTaskQueueDispatch(m_mainAsyncQueue, XTaskQueuePort::Completion, 0))
    {
        SwitchToThread();
    }

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
    //m_log->Render(commandList);
    //m_display->Render(commandList);
    m_ui->Render(commandList);

    PIXEndEvent(commandList);

    // Show the new frame.
    PIXBeginEvent(PIX_COLOR_DEFAULT, L"Present");
    m_deviceResources->Present();
    m_graphicsMemory->Commit(m_deviceResources->GetCommandQueue());
    PIXEndEvent();
}

// Helper method to clear the back buffers.
void Sample::Clear()
{
    auto commandList = m_deviceResources->GetCommandList();
    PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Clear");

    // Clear the views.
    auto rtvDescriptor = m_deviceResources->GetRenderTargetView();
    auto dsvDescriptor = m_deviceResources->GetDepthStencilView();

    commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
    commandList->ClearRenderTargetView(rtvDescriptor, ATG::Colors::Background, 0, nullptr);
    commandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

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


void Sample::AddLog(const std::string& str)
{
    m_liveInfoHUD->AddLog(str);
}

void CALLBACK HandleXblRealTimeActivityConnectionStateChange(
    _In_opt_ void* context,
    _In_ XblRealTimeActivityConnectionState connectionState
    )
{
    UNREFERENCED_PARAMETER(context);
    if (connectionState == XblRealTimeActivityConnectionState::Connected)
    {
        g_sample->AddLog(DebugFormat("RTA connected"));
    }
    if (connectionState == XblRealTimeActivityConnectionState::Connecting)
    {
        g_sample->AddLog(DebugFormat("RTA connecting"));
    }
    if (connectionState == XblRealTimeActivityConnectionState::Disconnected)
    {
        g_sample->AddLog(DebugFormat("RTA disconnected"));
    }
}

void Sample::SetUserHandle(XUserHandle user)
{
    if (m_userHandle1 != nullptr)
    {
        XblContextCloseHandle(m_xblContext);
        XUserCloseHandle(m_userHandle1);
    }
    m_userHandle1 = user;
    if (m_userHandle1 != nullptr)
    {
        HRESULT hr = XblContextCreateHandle(m_userHandle1, &m_xblContext);
        if (FAILED(hr))
        {
            g_sample->AddLog(DebugFormat("XblContextCreateHandle 0x%0.8x", hr));
        }
        else
        {
            hr = XblRealTimeActivityAddConnectionStateChangeHandler(m_xblContext, HandleXblRealTimeActivityConnectionStateChange, nullptr);
            g_sample->AddLog(DebugFormat("XblRealTimeActivityAddConnectionStateChangeHandler 0x%0.8x", hr));
        }
    }
}

HRESULT Sample::AddUser(XUserAddOptions options)
{
    // Attempt to get the default user, i.e. the user who launched the game
    auto async = new XAsyncBlock{};

    //async->context = (void*)options;
    async->callback = [](XAsyncBlock* async)
    {
        //int x = static_cast<int>(reinterpret_cast<std::uintptr_t>(async->context));
        //XUserAddOptions options = (XUserAddOptions)x;
        XUserHandle user = nullptr;
        HRESULT result = XUserAddResult(async, &user);
        if (SUCCEEDED(result))
        {
            // This failure doesn't come up until you try to actually do something with the user
            uint64_t xuid = {};
            if (FAILED(result = XUserGetId(user, &xuid)))
            {
                g_sample->AddLog(DebugFormat("User ResolveUserIssueWithUI 0x%0.8x", result));
                //ResolveUserIssueWithUI(user);
            }
            else
            {
                XUserLocalId userLocalId = {};
                XUserGetLocalId(user, &userLocalId);

                g_sample->AddLog(DebugFormat("Got XUserHandle 0x%0.8x. userLocalId: %ull, xuid: 0x%0.8x", user, userLocalId.value, xuid));
                g_sample->SetUserHandle(user);
                //OnSignInCompleted(ATG::XboxHandle<XUserHandle>{user});
            }
        }
        else if (result == E_GAMEUSER_RESOLVE_USER_ISSUE_REQUIRED
            || result == E_GAMEUSER_NO_DEFAULT_USER
            || result == static_cast<HRESULT>(0x8015DC12))
        {
            g_sample->AddLog(DebugFormat("User SwitchUser 0x%0.8x", result));
            g_sample->AddUser(XUserAddOptions::AllowGuests);
        }
        else
        {
            g_sample->AddLog(DebugFormat("User HandleError 0x%0.8x", result));
            //HandleError(result);
        }

        delete async;
    };

    HRESULT hr = XUserAddAsync(
        options,
        async
    );
    g_sample->AddLog(DebugFormat("XUserAddAsync 0x%0.8x", hr));

    if (FAILED(hr))
    {
        g_sample->AddLog("Unable to add user!");

        delete async;
        return hr;
    }

    return S_OK;
}

void Sample::OnSuspending()
{
    m_liveInfoHUD->AddLog("PLM resuming");
    m_deviceResources->Suspend();
}

void Sample::OnResuming()
{
    m_liveInfoHUD->AddLog("PLM resuming");
    //AddUser(XUserAddOptions::AddDefaultUserSilently);

    m_deviceResources->Resume();
    m_timer.ResetElapsedTime();
    m_gamePadButtons.Reset();
    m_keyboardButtons.Reset();
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
void Sample::GetDefaultSize(int& width, int& height) const noexcept
{
    width = 1840;
    height = 1035;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Sample::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

#ifdef _GAMING_DESKTOP
    D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_0 };
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel)))
        || (shaderModel.HighestShaderModel < D3D_SHADER_MODEL_6_0))
    {
#ifdef _DEBUG
        OutputDebugStringA("ERROR: Shader Model 6.0 is not supported!\n");
#endif
        throw std::runtime_error("Shader Model 6.0 is not supported!");
    }
#endif

    m_graphicsMemory = std::make_unique<GraphicsMemory>(device);

    RenderTargetState rtState(m_deviceResources->GetBackBufferFormat(), m_deviceResources->GetDepthBufferFormat());

    m_resourceDescriptors = std::make_unique<DirectX::DescriptorPile>(device,
        Descriptors::Count,
        Descriptors::Reserve
        );

    ResourceUploadBatch resourceUpload(device);
    resourceUpload.Begin();

    m_liveInfoHUD->RestoreDevice(device, rtState, resourceUpload, *m_resourceDescriptors);

    wchar_t font[260];
    wchar_t background[260];

    DX::FindMediaFile(font, 260, L"courier_16.spritefont");
    DX::FindMediaFile(background, 260, L"ATGSampleBackground.DDS");

    m_log->RestoreDevice(
        device,
        resourceUpload,
        rtState,
        font,
        background,
        m_resourceDescriptors->GetCpuHandle(Descriptors::Font),
        m_resourceDescriptors->GetGpuHandle(Descriptors::Font),
        m_resourceDescriptors->GetCpuHandle(Descriptors::Background),
        m_resourceDescriptors->GetGpuHandle(Descriptors::Background)
    );

    m_display->RestoreDevice(
        device,
        resourceUpload,
        rtState,
        font,
        background,
        m_resourceDescriptors->GetCpuHandle(Descriptors::ConsoleFont),
        m_resourceDescriptors->GetGpuHandle(Descriptors::ConsoleFont),
        m_resourceDescriptors->GetCpuHandle(Descriptors::ConsoleBackground),
        m_resourceDescriptors->GetGpuHandle(Descriptors::ConsoleBackground)
    );

    m_ui->RestoreDevice(device, rtState, resourceUpload, *m_resourceDescriptors);

    auto uploadResourcesFinished = resourceUpload.End(m_deviceResources->GetCommandQueue());
    uploadResourcesFinished.wait();
}

void ScaleRect(const RECT &originalDisplayRect, const RECT &displayRect, const RECT &originalSubRect, RECT &scaledSubRect)
{
    const float widthScale = ((float)displayRect.right - (float)displayRect.left) / ((float)originalDisplayRect.right - (float)originalDisplayRect.left);
    const float heightScale = ((float)displayRect.bottom - (float)displayRect.top) / ((float)originalDisplayRect.bottom - (float)originalDisplayRect.top);

    scaledSubRect.top = (LONG)((float)originalSubRect.top * heightScale);
    scaledSubRect.left = (LONG)((float)originalSubRect.left * widthScale);
    scaledSubRect.bottom = (LONG)((float)originalSubRect.bottom * heightScale);
    scaledSubRect.right = (LONG)((float)originalSubRect.right * widthScale);
}

// Allocate all memory resources that change on a window SizeChanged event.
void Sample::CreateWindowSizeDependentResources()
{
    RECT fullscreen = m_deviceResources->GetOutputSize();
    auto viewport = m_deviceResources->GetScreenViewport();

    m_liveInfoHUD->SetViewport(viewport);

    // Scaled for 1920x1080
    static const RECT originalScale = { 0, 0, 1920, 1080 };
    static const RECT screenDisplay = { 960, 200, 1780, 450 };
    RECT scaledDisplay;
    ScaleRect(originalScale, fullscreen, screenDisplay, scaledDisplay);

    m_log->SetWindow(scaledDisplay, false);
    m_log->SetViewport(viewport);

    // Scaled for 1920x1080
    static const RECT screenDisplay2 = { 960, 500, 1780, 950 };
    ScaleRect(originalScale, fullscreen, screenDisplay2, scaledDisplay);

    m_display->SetWindow(scaledDisplay, false);
    m_display->SetViewport(viewport);

    m_ui->SetWindow(fullscreen);
}

void Sample::OnDeviceLost()
{
    m_ui->ReleaseDevice();
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

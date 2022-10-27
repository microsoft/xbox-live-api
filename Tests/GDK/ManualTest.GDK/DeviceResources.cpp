//
// DeviceResources.cpp - A wrapper for the Direct3D 12/12.X device and swapchain
//

#include "pch.h"
#include "DeviceResources.h"

using namespace DirectX;
using namespace DX;

using Microsoft::WRL::ComPtr;

#ifdef _GAMING_DESKTOP
#ifdef __clang__
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#pragma warning(disable : 4061)

namespace
{
    inline DXGI_FORMAT NoSRGB(DXGI_FORMAT fmt) noexcept
    {
        switch (fmt)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8X8_UNORM;
        default:                                return fmt;
        }
    }
}
#endif

// Constructor for DeviceResources.
DeviceResources::DeviceResources(
    DXGI_FORMAT backBufferFormat,
    DXGI_FORMAT depthBufferFormat,
    UINT backBufferCount) noexcept(false) :
        m_backBufferIndex(0),
        m_fenceValues{},
#ifdef _GAMING_XBOX
        m_framePipelineToken{},
#endif
        m_rtvDescriptorSize(0),
        m_screenViewport{},
        m_scissorRect{},
        m_backBufferFormat(backBufferFormat),
        m_depthBufferFormat(depthBufferFormat),
        m_backBufferCount(backBufferCount),
        m_window(nullptr),
        m_d3dFeatureLevel(D3D_FEATURE_LEVEL_11_0),
        m_outputSize{ 0, 0, 1, 1 },
        m_deviceNotify(nullptr)
{
    if (backBufferCount < 2 || backBufferCount > MAX_BACK_BUFFER_COUNT)
    {
        throw std::out_of_range("invalid backBufferCount");
    }
}

// Destructor for DeviceResources.
DeviceResources::~DeviceResources()
{
    // Ensure that the GPU is no longer referencing resources that are about to be destroyed.
    WaitForGpu();

#ifdef _GAMING_XBOX
    // Ensure we present a blank screen before cleaning up resources.
    if (m_commandQueue)
    {
        (void)m_commandQueue->PresentX(0, nullptr, nullptr);
    }
#endif
}

// Configures the Direct3D device, and stores handles to it and the device context.
void DeviceResources::CreateDeviceResources()
{
#ifdef _GAMING_XBOX

    // Create the DX12 API device object.
    D3D12XBOX_CREATE_DEVICE_PARAMETERS params = {};
    params.Version = D3D12_SDK_VERSION;

#if defined(_DEBUG)
    // Enable the debug layer.
    params.ProcessDebugFlags = D3D12_PROCESS_DEBUG_FLAG_DEBUG_LAYER_ENABLED;
#elif defined(PROFILE)
    // Enable the instrumented driver.
    params.ProcessDebugFlags = D3D12XBOX_PROCESS_DEBUG_FLAG_INSTRUMENTED;
#endif

    params.GraphicsCommandQueueRingSizeBytes = static_cast<UINT>(D3D12XBOX_DEFAULT_SIZE_BYTES);
    params.GraphicsScratchMemorySizeBytes = static_cast<UINT>(D3D12XBOX_DEFAULT_SIZE_BYTES);
    params.ComputeScratchMemorySizeBytes = static_cast<UINT>(D3D12XBOX_DEFAULT_SIZE_BYTES);

    HRESULT hr = D3D12XboxCreateDevice(
        nullptr,
        &params,
        IID_GRAPHICS_PPV_ARGS(m_d3dDevice.ReleaseAndGetAddressOf()));
#ifdef _DEBUG
    if (hr == D3D12_ERROR_DRIVER_VERSION_MISMATCH)
    {
#ifdef _GAMING_XBOX_SCARLETT
        OutputDebugStringA("ERROR: Running a d3d12_xs.lib (Xbox Series X|S) linked binary on an Xbox One is not supported\n");
#else
        OutputDebugStringA("ERROR: Running a d3d12_x.lib (Xbox One) linked binary on a Xbox Series X|S in 'Scarlett' mode is not supported\n");
#endif
    }
#endif
    ThrowIfFailed(hr);

    m_d3dDevice->SetName(L"DeviceResources");

    m_d3dFeatureLevel = D3D_FEATURE_LEVEL_12_0;

#else // _GAMING_DESKTOP

    DWORD dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    //
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
        {
            debugController->EnableDebugLayer();
        }
        else
        {
            OutputDebugStringA("WARNING: Direct3D Debug Device is not available\n");
        }

        ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
        {
            dxgiFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

            dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
            dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

            DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
            {
                80 /* IDXGISwapChain::GetContainingOutput: The swapchain's adapter does not control the output on which the swapchain's window resides. */,
            };
            DXGI_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
            filter.DenyList.pIDList = hide;
            dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
        }
    }
#endif

    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));

    ComPtr<IDXGIAdapter1> adapter;
    GetAdapter(adapter.GetAddressOf());

    // Create the DX12 API device object.
    HRESULT hr = D3D12CreateDevice(
        adapter.Get(),
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(m_d3dDevice.ReleaseAndGetAddressOf())
        );
    ThrowIfFailed(hr);

    m_d3dDevice->SetName(L"DeviceResources");

#ifndef NDEBUG
    // Configure debug device (if active).
    ComPtr<ID3D12InfoQueue> d3dInfoQueue;
    if (SUCCEEDED(m_d3dDevice.As(&d3dInfoQueue)))
    {
#ifdef _DEBUG
        d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        d3dInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
#endif
        D3D12_MESSAGE_ID hide[] =
        {
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
            // Workarounds for debug layer issues on hybrid-graphics systems
            D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE,
        };
        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
        filter.DenyList.pIDList = hide;
        d3dInfoQueue->AddStorageFilterEntries(&filter);
    }
#endif

    // Determine maximum supported feature level for this device
    static const D3D_FEATURE_LEVEL s_featureLevels[] =
    {
#if defined(NTDDI_WIN10_FE) && (NTDDI_VERSION >= NTDDI_WIN10_FE)
        D3D_FEATURE_LEVEL_12_2,
#endif
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D12_FEATURE_DATA_FEATURE_LEVELS featLevels =
    {
        static_cast<UINT>(std::size(s_featureLevels)), s_featureLevels, D3D_FEATURE_LEVEL_11_0
    };

    hr = m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featLevels, sizeof(featLevels));
    if (SUCCEEDED(hr))
    {
        m_d3dFeatureLevel = featLevels.MaxSupportedFeatureLevel;
    }
    else
    {
        m_d3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;
    }

#endif

    // Create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_GRAPHICS_PPV_ARGS(m_commandQueue.ReleaseAndGetAddressOf())));

    m_commandQueue->SetName(L"DeviceResources");

    // Create descriptor heaps for render target views and depth stencil views.
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
    rtvDescriptorHeapDesc.NumDescriptors = m_backBufferCount;
    rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_GRAPHICS_PPV_ARGS(m_rtvDescriptorHeap.ReleaseAndGetAddressOf())));

    m_rtvDescriptorHeap->SetName(L"DeviceResources");

    m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
        dsvDescriptorHeapDesc.NumDescriptors = 1;
        dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

        ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_GRAPHICS_PPV_ARGS(m_dsvDescriptorHeap.ReleaseAndGetAddressOf())));

        m_dsvDescriptorHeap->SetName(L"DeviceResources");
    }

    // Create a command allocator for each back buffer that will be rendered to.
    for (UINT n = 0; n < m_backBufferCount; n++)
    {
        ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_GRAPHICS_PPV_ARGS(m_commandAllocators[n].ReleaseAndGetAddressOf())));

        wchar_t name[25] = {};
        swprintf_s(name, L"Render target %u", n);
        m_commandAllocators[n]->SetName(name);
    }

    // Create a command list for recording graphics commands.
    ThrowIfFailed(m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[0].Get(), nullptr, IID_GRAPHICS_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())));
    ThrowIfFailed(m_commandList->Close());

    m_commandList->SetName(L"DeviceResources");

    // Create a fence for tracking GPU execution progress.
    ThrowIfFailed(m_d3dDevice->CreateFence(m_fenceValues[m_backBufferIndex], D3D12_FENCE_FLAG_NONE, IID_GRAPHICS_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())));
    m_fenceValues[m_backBufferIndex]++;

    m_fence->SetName(L"DeviceResources");

    m_fenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
    if (!m_fenceEvent.IsValid())
    {
        throw std::system_error(std::error_code(static_cast<int>(GetLastError()), std::system_category()), "CreateEventEx");
    }

#ifdef _GAMING_XBOX
    RegisterFrameEvents();
#endif
}

// These resources need to be recreated every time the window size is changed.
void DeviceResources::CreateWindowSizeDependentResources()
{
    if (!m_window)
    {
        throw std::logic_error("Call SetWindow with a valid Win32 window handle");
    }

    // Wait until all previous GPU work is complete.
    WaitForGpu();

#ifdef _GAMING_XBOX
    // Ensure we present a blank screen before cleaning up resources.
    ThrowIfFailed(m_commandQueue->PresentX(0, nullptr, nullptr));
#endif

    // Release resources that are tied to the swap chain and update fence values.
    for (UINT n = 0; n < m_backBufferCount; n++)
    {
        m_renderTargets[n].Reset();
        m_fenceValues[n] = m_fenceValues[m_backBufferIndex];
    }

    // Determine the render target size in pixels.
    const UINT backBufferWidth = std::max<UINT>(static_cast<UINT>(m_outputSize.right - m_outputSize.left), 1u);
    const UINT backBufferHeight = std::max<UINT>(static_cast<UINT>(m_outputSize.bottom - m_outputSize.top), 1u);

#ifdef _GAMING_XBOX

    // Obtain the back buffers for this window which will be the final render targets
    // and create render target views for each of them.
    CD3DX12_HEAP_PROPERTIES swapChainHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC swapChainBufferDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        m_backBufferFormat,
        backBufferWidth,
        backBufferHeight,
        1, // This resource has only one texture.
        1  // Use a single mipmap level.
    );
    swapChainBufferDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE swapChainOptimizedClearValue = {};
    swapChainOptimizedClearValue.Format = m_backBufferFormat;

    for (UINT n = 0; n < m_backBufferCount; n++)
    {
        ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
            &swapChainHeapProperties,
            D3D12_HEAP_FLAG_ALLOW_DISPLAY,
            &swapChainBufferDesc,
            D3D12_RESOURCE_STATE_PRESENT,
            &swapChainOptimizedClearValue,
            IID_GRAPHICS_PPV_ARGS(m_renderTargets[n].GetAddressOf())));

        wchar_t name[25] = {};
        swprintf_s(name, L"Render target %u", n);
        m_renderTargets[n]->SetName(name);

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = m_backBufferFormat;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(
            m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            static_cast<INT>(n), m_rtvDescriptorSize);
        m_d3dDevice->CreateRenderTargetView(m_renderTargets[n].Get(), &rtvDesc, rtvDescriptor);
    }

    // Reset the index to the current back buffer.
    m_backBufferIndex = 0;

#else // _GAMING_DESKTOP

    DXGI_FORMAT backBufferFormat = NoSRGB(m_backBufferFormat);

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        // If the swap chain already exists, resize it.
        HRESULT hr = m_swapChain->ResizeBuffers(
            m_backBufferCount,
            backBufferWidth,
            backBufferHeight,
            backBufferFormat,
            0
        );

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
#ifdef _DEBUG
            char buff[64] = {};
            sprintf_s(buff, "Device Lost on ResizeBuffers: Reason code 0x%08X\n",
                static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr));
            OutputDebugStringA(buff);
#endif
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            HandleDeviceLost();

            // Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method
            // and correctly set up the new device.
            return;
        }
        else
        {
            ThrowIfFailed(hr);
        }
    }
    else
    {
        // Create a descriptor for the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = backBufferWidth;
        swapChainDesc.Height = backBufferHeight;
        swapChainDesc.Format = backBufferFormat;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = m_backBufferCount;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
        fsSwapChainDesc.Windowed = TRUE;

        // Create a swap chain for the window.
        ComPtr<IDXGISwapChain1> swapChain;
        ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),
            m_window,
            &swapChainDesc,
            &fsSwapChainDesc,
            nullptr,
            swapChain.GetAddressOf()
        ));

        ThrowIfFailed(swapChain.As(&m_swapChain));

        // This class does not support exclusive full-screen mode and prevents DXGI from responding to the ALT+ENTER shortcut
        ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the back buffers for this window which will be the final render targets
    // and create render target views for each of them.
    for (UINT n = 0; n < m_backBufferCount; n++)
    {
        ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(m_renderTargets[n].GetAddressOf())));

        wchar_t name[25] = {};
        swprintf_s(name, L"Render target %u", n);
        m_renderTargets[n]->SetName(name);

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = m_backBufferFormat;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(
            m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            static_cast<INT>(n), m_rtvDescriptorSize);
        m_d3dDevice->CreateRenderTargetView(m_renderTargets[n].Get(), &rtvDesc, rtvDescriptor);
    }

    // Reset the index to the current back buffer.
    m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

#endif

    if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
    {
        // Allocate a 2-D surface as the depth/stencil buffer and create a depth/stencil view
        // on this surface.
        CD3DX12_HEAP_PROPERTIES depthHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            m_depthBufferFormat,
            backBufferWidth,
            backBufferHeight,
            1, // This depth stencil view has only one texture.
            1  // Use a single mipmap level.
            );
        depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = m_depthBufferFormat;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
            &depthHeapProperties,
            D3D12_HEAP_FLAG_NONE,
            &depthStencilDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_GRAPHICS_PPV_ARGS(m_depthStencil.ReleaseAndGetAddressOf())
            ));

        m_depthStencil->SetName(L"Depth stencil");

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = m_depthBufferFormat;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

        m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &dsvDesc, m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    }

    // Set the 3D rendering viewport and scissor rectangle to target the entire window.
    m_screenViewport.TopLeftX = m_screenViewport.TopLeftY = 0.f;
    m_screenViewport.Width = static_cast<float>(backBufferWidth);
    m_screenViewport.Height = static_cast<float>(backBufferHeight);
    m_screenViewport.MinDepth = D3D12_MIN_DEPTH;
    m_screenViewport.MaxDepth = D3D12_MAX_DEPTH;

    m_scissorRect.left = m_scissorRect.top = 0;
    m_scissorRect.right = static_cast<LONG>(backBufferWidth);
    m_scissorRect.bottom = static_cast<LONG>(backBufferHeight);
}

// This method is called when the Win32 window is created (or re-created).
void DeviceResources::SetWindow(HWND window, int width, int height) noexcept
{
    m_window = window;

    m_outputSize.left = m_outputSize.top = 0;
    m_outputSize.right = width;
    m_outputSize.bottom = height;
}

// This method is called when the Win32 window changes size.
bool DeviceResources::WindowSizeChanged(int width, int height)
{
    RECT newRc;
    newRc.left = newRc.top = 0;
    newRc.right = width;
    newRc.bottom = height;
    if (newRc.left == m_outputSize.left
        && newRc.top == m_outputSize.top
        && newRc.right == m_outputSize.right
        && newRc.bottom == m_outputSize.bottom)
    {
        return false;
    }

    m_outputSize = newRc;
    CreateWindowSizeDependentResources();
    return true;
}

// Recreate all device resources and set them back to the current state.
void DeviceResources::HandleDeviceLost()
{
#ifdef _GAMING_DESKTOP
    if (m_deviceNotify)
    {
        m_deviceNotify->OnDeviceLost();
    }

    for (UINT n = 0; n < m_backBufferCount; n++)
    {
        m_commandAllocators[n].Reset();
        m_renderTargets[n].Reset();
    }

    m_depthStencil.Reset();
    m_commandQueue.Reset();
    m_commandList.Reset();
    m_fence.Reset();
    m_rtvDescriptorHeap.Reset();
    m_dsvDescriptorHeap.Reset();
    m_swapChain.Reset();
    m_d3dDevice.Reset();
    m_dxgiFactory.Reset();

#ifdef _DEBUG
    {
        ComPtr<IDXGIDebug1> dxgiDebug;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
        {
            dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
        }
    }
#endif

    CreateDeviceResources();
    CreateWindowSizeDependentResources();

    if (m_deviceNotify)
    {
        m_deviceNotify->OnDeviceRestored();
    }
#endif
}

// Prepare the command list and render target for rendering.
void DeviceResources::Prepare(D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
#ifdef _GAMING_XBOX
    // Wait until frame start is signaled
    m_framePipelineToken = D3D12XBOX_FRAME_PIPELINE_TOKEN_NULL;
    ThrowIfFailed(m_d3dDevice->WaitFrameEventX(D3D12XBOX_FRAME_EVENT_ORIGIN, INFINITE, nullptr, D3D12XBOX_WAIT_FRAME_EVENT_FLAG_NONE, &m_framePipelineToken));
#endif

    // Reset command list and allocator.
    ThrowIfFailed(m_commandAllocators[m_backBufferIndex]->Reset());
    ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_backBufferIndex].Get(), nullptr));

    if (beforeState != afterState)
    {
        // Transition the render target into the correct state to allow for drawing into it.
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_backBufferIndex].Get(),
            beforeState, afterState);
        m_commandList->ResourceBarrier(1, &barrier);
    }
}

// Present the contents of the swap chain to the screen.
void DeviceResources::Present(D3D12_RESOURCE_STATES beforeState)
{
    if (beforeState != D3D12_RESOURCE_STATE_PRESENT)
    {
        // Transition the render target to the state that allows it to be presented to the display.
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_backBufferIndex].Get(), beforeState, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &barrier);
    }

    // Send the command list off to the GPU for processing.
    ThrowIfFailed(m_commandList->Close());
    m_commandQueue->ExecuteCommandLists(1, CommandListCast(m_commandList.GetAddressOf()));

#ifdef _GAMING_XBOX

    // Present the backbuffer using the PresentX API.
    D3D12XBOX_PRESENT_PLANE_PARAMETERS planeParameters = {};
    planeParameters.Token = m_framePipelineToken;
    planeParameters.ResourceCount = 1;
    planeParameters.ppResources = m_renderTargets[m_backBufferIndex].GetAddressOf();

    ThrowIfFailed(
        m_commandQueue->PresentX(1, &planeParameters, nullptr)
    );

    // Xbox One apps do not need to handle DXGI_ERROR_DEVICE_REMOVED or DXGI_ERROR_DEVICE_RESET.

#else

    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
#ifdef _DEBUG
        char buff[64] = {};
        sprintf_s(buff, "Device Lost on Present: Reason code 0x%08X\n",
            static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? m_d3dDevice->GetDeviceRemovedReason() : hr));
        OutputDebugStringA(buff);
#endif
        HandleDeviceLost();
    }
    else
    {
        ThrowIfFailed(hr);
    }

#endif

    MoveToNextFrame();
}

// Handle GPU suspend/resume
void DeviceResources::Suspend()
{
#ifdef _GAMING_XBOX
    m_commandQueue->SuspendX(0);
#endif
}

void DeviceResources::Resume()
{
#ifdef _GAMING_XBOX
    m_commandQueue->ResumeX();

    RegisterFrameEvents();
#endif
}

// Wait for pending GPU work to complete.
void DeviceResources::WaitForGpu() noexcept
{
    if (m_commandQueue && m_fence && m_fenceEvent.IsValid())
    {
        // Schedule a Signal command in the GPU queue.
        UINT64 fenceValue = m_fenceValues[m_backBufferIndex];
        if (SUCCEEDED(m_commandQueue->Signal(m_fence.Get(), fenceValue)))
        {
            // Wait until the Signal has been processed.
            if (SUCCEEDED(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent.Get())))
            {
                WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);

                // Increment the fence value for the current frame.
                m_fenceValues[m_backBufferIndex]++;
            }
        }
    }
}

// Prepare to render the next frame.
void DeviceResources::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = m_fenceValues[m_backBufferIndex];
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

    // Update the back buffer index.
#ifdef _GAMING_XBOX
    m_backBufferIndex = (m_backBufferIndex + 1) % m_backBufferCount;
#else
    m_backBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
#endif

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (m_fence->GetCompletedValue() < m_fenceValues[m_backBufferIndex])
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_backBufferIndex], m_fenceEvent.Get()));
        WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    m_fenceValues[m_backBufferIndex] = currentFenceValue + 1;
}

#ifdef _GAMING_XBOX
// Set frame interval and register for frame events
void DeviceResources::RegisterFrameEvents()
{
    // First, retrieve the underlying DXGI device from the D3D device.
    ComPtr<IDXGIDevice1> dxgiDevice;
    ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

    // Identify the physical adapter (GPU or card) this device is running on.
    ComPtr<IDXGIAdapter> dxgiAdapter;
    ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

    // Retrieve the outputs for the adapter.
    ComPtr<IDXGIOutput> dxgiOutput;
    ThrowIfFailed(dxgiAdapter->EnumOutputs(0, dxgiOutput.GetAddressOf()));

    // Set frame interval and register for frame events
    ThrowIfFailed(m_d3dDevice->SetFrameIntervalX(
        dxgiOutput.Get(),
        D3D12XBOX_FRAME_INTERVAL_60_HZ,
        m_backBufferCount - 1u /* Allow n-1 frames of latency */,
        D3D12XBOX_FRAME_INTERVAL_FLAG_NONE));

    ThrowIfFailed(m_d3dDevice->ScheduleFrameEventX(
        D3D12XBOX_FRAME_EVENT_ORIGIN,
        0U,
        nullptr,
        D3D12XBOX_SCHEDULE_FRAME_EVENT_FLAG_NONE));
}
#else
// This method acquires the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, try WARP. Otherwise throw an exception.
void DeviceResources::GetAdapter(IDXGIAdapter1** ppAdapter)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;
    for (UINT adapterIndex = 0;
        SUCCEEDED(m_dxgiFactory->EnumAdapterByGpuPreference(
            adapterIndex,
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf())));
        adapterIndex++)
    {
        DXGI_ADAPTER_DESC1 desc;
        ThrowIfFailed(adapter->GetDesc1(&desc));

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            // Don't select the Basic Render Driver adapter.
            continue;
        }

        // Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
        {
#ifdef _DEBUG
            wchar_t buff[256] = {};
            swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
            OutputDebugStringW(buff);
#endif
            break;
        }
    }

#if !defined(NDEBUG)
    if (!adapter)
    {
        // Try WARP12 instead
        if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf()))))
        {
            throw std::runtime_error("WARP12 not available. Enable the 'Graphics Tools' optional feature");
        }

        OutputDebugStringA("Direct3D Adapter - WARP12\n");
    }
#endif

    if (!adapter)
    {
        throw std::runtime_error("No Direct3D 12 device found");
    }

    *ppAdapter = adapter.Detach();
}
#endif

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// Helper for managing MSAA render targets
//

#pragma once

#include "pch.h"
#include "MSAAHelper.h"

#include "DirectXHelpers.h"

#include <algorithm>
#include <stdio.h>
#include <stdexcept>

#include <wrl/client.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

#if defined(__d3d12_h__) || defined(__d3d12_x_h__)
//======================================================================================
// Direct3D 12
//======================================================================================

DX::MSAAHelper::MSAAHelper(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat, unsigned int sampleCount) :
    m_backBufferFormat(backBufferFormat),
    m_depthBufferFormat(depthBufferFormat),
    m_targetSampleCount(sampleCount),
    m_outputSize{}
{
    if (sampleCount < 2 || sampleCount > D3D12_MAX_MULTISAMPLE_SAMPLE_COUNT)
    {
        throw std::out_of_range("MSAA sample count invalid.");
    }
}


void DX::MSAAHelper::SetDevice(_In_ ID3D12Device* device)
{
    if (device == m_device.Get())
        return;

    if (m_device)
    {
        ReleaseDevice();
    }

    D3D12_FEATURE_DATA_FORMAT_SUPPORT formatSupport = { m_backBufferFormat };
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &formatSupport, sizeof(formatSupport))))
    {
        throw std::exception("CheckFeatureSupport");
    }

    if ((formatSupport.Support1 & (D3D12_FORMAT_SUPPORT1_MULTISAMPLE_RESOLVE | D3D12_FORMAT_SUPPORT1_MULTISAMPLE_RENDERTARGET))
        != (D3D12_FORMAT_SUPPORT1_MULTISAMPLE_RESOLVE | D3D12_FORMAT_SUPPORT1_MULTISAMPLE_RENDERTARGET))
    {
#ifdef _DEBUG
        char buff[128] = {};
        sprintf_s(buff, "MSAAHelper: Device does not support MSAA for requested format (%u)!\n", m_backBufferFormat);
        OutputDebugStringA(buff);
#endif
        throw std::exception("MSAA not supported for requested format");
    }

    for (m_sampleCount = m_targetSampleCount; m_sampleCount > 1; m_sampleCount--)
    {
        D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS levels = { m_backBufferFormat, m_sampleCount };
        if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &levels, sizeof(levels))))
            continue;

        if (levels.NumQualityLevels > 0)
            break;
    }

    if (m_sampleCount < 2)
    {
#ifdef _DEBUG
        OutputDebugStringA("MSAAHelper: Device does not support MSAA!\n");
#endif
        throw std::exception("MSAA not supported");
    }

    // Create descriptor heaps for render target views and depth stencil views.
    D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc = {};
    rtvDescriptorHeapDesc.NumDescriptors = 1;
    rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    DX::ThrowIfFailed(device->CreateDescriptorHeap(&rtvDescriptorHeapDesc,
        IID_GRAPHICS_PPV_ARGS(m_rtvDescriptorHeap.ReleaseAndGetAddressOf())));

    if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc = {};
        dsvDescriptorHeapDesc.NumDescriptors = 1;
        dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

        DX::ThrowIfFailed(device->CreateDescriptorHeap(&dsvDescriptorHeapDesc,
            IID_GRAPHICS_PPV_ARGS(m_dsvDescriptorHeap.ReleaseAndGetAddressOf())));
    }

    m_device = device;
}


void DX::MSAAHelper::SetWindow(const RECT& output, const float clearColor[4])
{
    if (output.left == m_outputSize.left
        && output.right == m_outputSize.right
        && output.top == m_outputSize.top
        && output.bottom == m_outputSize.bottom)
        return;

    if (!m_device)
        return;

    // Determine the render target size in pixels.
    UINT backBufferWidth = std::max<UINT>(output.right - output.left, 1);
    UINT backBufferHeight = std::max<UINT>(output.bottom - output.top, 1);

    CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    DXGI_FORMAT msaaFormat = m_backBufferFormat;

#if !defined(_XBOX_ONE) || !defined(_TITLE)

    switch (m_backBufferFormat)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        msaaFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
        m_resolveFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;

    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        msaaFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
        m_resolveFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
        break;

    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        msaaFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
        m_resolveFormat = DXGI_FORMAT_B8G8R8X8_UNORM;
        break;

    default:
        m_resolveFormat = m_backBufferFormat;
        break;
    }

#endif

    // Create an MSAA render target
    D3D12_RESOURCE_DESC msaaRTDesc = CD3DX12_RESOURCE_DESC::Tex2D(
        msaaFormat,
        backBufferWidth,
        backBufferHeight,
        1, // This render target view has only one texture.
        1, // Use a single mipmap level
        m_sampleCount
    );
    msaaRTDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_CLEAR_VALUE msaaOptimizedClearValue = {};
    msaaOptimizedClearValue.Format = m_backBufferFormat;
    memcpy(msaaOptimizedClearValue.Color, clearColor, sizeof(float) * 4);

    DX::ThrowIfFailed(m_device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &msaaRTDesc,
        D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
        &msaaOptimizedClearValue,
        IID_GRAPHICS_PPV_ARGS(m_msaaRenderTarget.ReleaseAndGetAddressOf())
    ));

    m_msaaRenderTarget->SetName( L"MSAA Render Target" );

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = m_backBufferFormat;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;

    m_device->CreateRenderTargetView(
        m_msaaRenderTarget.Get(), &rtvDesc,
        m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

    if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
    {
        // Create an MSAA depth stencil view
        D3D12_RESOURCE_DESC depthStencilDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            m_depthBufferFormat,
            backBufferWidth,
            backBufferHeight,
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            m_sampleCount
        );
        depthStencilDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = m_depthBufferFormat;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        DX::ThrowIfFailed(m_device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &depthStencilDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_GRAPHICS_PPV_ARGS(m_msaaDepthStencil.ReleaseAndGetAddressOf())
        ));

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = m_depthBufferFormat;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;

        m_device->CreateDepthStencilView(
            m_msaaDepthStencil.Get(), &dsvDesc,
            m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    }

    m_outputSize = output;
}


void DX::MSAAHelper::ReleaseDevice()
{
    m_rtvDescriptorHeap.Reset();
    m_dsvDescriptorHeap.Reset();

    m_msaaRenderTarget.Reset();
    m_msaaDepthStencil.Reset();

    m_outputSize = {};

    m_device.Reset();
}


void DX::MSAAHelper::Prepare(_In_ ID3D12GraphicsCommandList* commandList)
{
    D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_msaaRenderTarget.Get(),
        D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &barrier);
}


void DX::MSAAHelper::Resolve(_In_ ID3D12GraphicsCommandList* commandList, _In_ ID3D12Resource* backBuffer,
                             D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
    if (beforeState != D3D12_RESOURCE_STATE_RESOLVE_DEST)
    {
        D3D12_RESOURCE_BARRIER barriers[2] =
        {
            CD3DX12_RESOURCE_BARRIER::Transition(m_msaaRenderTarget.Get(),
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_RESOLVE_SOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(backBuffer,
                beforeState,
                D3D12_RESOURCE_STATE_RESOLVE_DEST)
        };

        commandList->ResourceBarrier(2, barriers);
    }
    else
    {
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_msaaRenderTarget.Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_RESOLVE_SOURCE);

        commandList->ResourceBarrier(1, &barrier);
    }

#if defined(_XBOX_ONE) && defined(_TITLE)
    commandList->ResolveSubresource(backBuffer, 0, m_msaaRenderTarget.Get(), 0, m_backBufferFormat);
#else
    commandList->ResolveSubresource(backBuffer, 0, m_msaaRenderTarget.Get(), 0, m_resolveFormat);
#endif

    if (afterState != D3D12_RESOURCE_STATE_RESOLVE_DEST)
    {
        D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            backBuffer,
            D3D12_RESOURCE_STATE_RESOLVE_DEST,
            afterState);
        commandList->ResourceBarrier(1, &barrier);
    }
}



#else
//======================================================================================
// Direct3D 11
//======================================================================================

DX::MSAAHelper::MSAAHelper(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat, unsigned int sampleCount) :
    m_backBufferFormat(backBufferFormat),
    m_depthBufferFormat(depthBufferFormat),
    m_targetSampleCount(sampleCount),
    m_outputSize{}
{
    if (sampleCount < 2 || sampleCount > D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT)
    {
        throw std::out_of_range("MSAA sample count invalid.");
    }
}


void DX::MSAAHelper::SetDevice(_In_ ID3D11Device* device)
{
    if (device == m_device.Get())
        return;

    if (m_device)
    {
        ReleaseDevice();
    }

    UINT formatSupport = 0;
    if (FAILED(device->CheckFormatSupport(m_backBufferFormat, &formatSupport)))
    {
        throw std::exception("CheckFormatSupport");
    }

    if ( (formatSupport & (D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE | D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET))
         != (D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE | D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET))
    {
#ifdef _DEBUG
        char buff[128] = {};
        sprintf_s(buff, "MSAAHelper: Device does not support MSAA for requested format (%u)!\n", m_backBufferFormat);
        OutputDebugStringA(buff);
#endif
        throw std::exception("MSAA not supported for requested format");
    }

    for (m_sampleCount = m_targetSampleCount; m_sampleCount > 1; m_sampleCount--)
    {
        UINT levels = 0;
        if (FAILED(device->CheckMultisampleQualityLevels(m_backBufferFormat, m_sampleCount, &levels)))
            continue;

        if ( levels > 0)
            break;
    }

    if (m_sampleCount < 2)
    {
#ifdef _DEBUG
        OutputDebugStringA("MSAAHelper: Device does not support MSAA!\n");
#endif
        throw std::exception("MSAA not supported");
    }

    m_device = device;

#if defined(_XBOX_ONE) && defined(_TITLE)
    m_fastSemantics = (device->GetCreationFlags() & D3D11_CREATE_DEVICE_IMMEDIATE_CONTEXT_FAST_SEMANTICS) != 0;
#endif
}


void DX::MSAAHelper::SetWindow(const RECT& output)
{
    if (output == m_outputSize)
        return;

    if (!m_device)
        return;

    // Determine the render target size in pixels.
    UINT backBufferWidth = std::max<UINT>(output.right - output.left, 1);
    UINT backBufferHeight = std::max<UINT>(output.bottom - output.top, 1);

    DXGI_FORMAT msaaFormat = m_backBufferFormat;

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)

    switch (m_backBufferFormat)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        msaaFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
        m_resolveFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;

    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        msaaFormat = DXGI_FORMAT_B8G8R8A8_TYPELESS;
        m_resolveFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
        break;

    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        msaaFormat = DXGI_FORMAT_B8G8R8X8_TYPELESS;
        m_resolveFormat = DXGI_FORMAT_B8G8R8X8_UNORM;
        break;

    default:
        m_resolveFormat = m_backBufferFormat;
        break;
    }

#endif

    // Create an MSAA render target
    CD3D11_TEXTURE2D_DESC renderTargetDesc(
        msaaFormat,
        backBufferWidth,
        backBufferHeight,
        1, // The render target view has only one texture.
        1, // Use a single mipmap level.
        D3D11_BIND_RENDER_TARGET,
        D3D11_USAGE_DEFAULT,
        0,
        m_sampleCount
    );

    DX::ThrowIfFailed(m_device->CreateTexture2D(
        &renderTargetDesc,
        nullptr,
        m_msaaRenderTarget.ReleaseAndGetAddressOf()
    ));

    SetDebugObjectName(m_msaaRenderTarget.Get(), "MSAA Render Target");


    CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2DMS, m_backBufferFormat);

    DX::ThrowIfFailed(m_device->CreateRenderTargetView(
        m_msaaRenderTarget.Get(),
        &renderTargetViewDesc,
        m_renderTargetView.ReleaseAndGetAddressOf()
    ));

    SetDebugObjectName(m_renderTargetView.Get(), "MSAA Render Target");

    if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
    {
        // Create an MSAA depth stencil view
        CD3D11_TEXTURE2D_DESC depthStencilDesc(
            m_depthBufferFormat,
            backBufferWidth,
            backBufferHeight,
            1, // This depth stencil view has only one texture.
            1, // Use a single mipmap level.
            D3D11_BIND_DEPTH_STENCIL,
            D3D11_USAGE_DEFAULT,
            0,
            m_sampleCount
        );

        ComPtr<ID3D11Texture2D> depthStencil;
        DX::ThrowIfFailed(m_device->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            depthStencil.GetAddressOf()
        ));

        SetDebugObjectName(depthStencil.Get(), "MSAA Depth/Stencil");

        DX::ThrowIfFailed(m_device->CreateDepthStencilView(
            depthStencil.Get(),
            nullptr,
            m_depthStencilView.ReleaseAndGetAddressOf()
        ));

        SetDebugObjectName(m_depthStencilView.Get(), "MSAA Depth/Stencil");
    }

    m_outputSize = output;
}


void DX::MSAAHelper::ReleaseDevice()
{
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();

    m_msaaRenderTarget.Reset();

    m_outputSize = {};

    m_device.Reset();
}


#if defined(_XBOX_ONE) && defined(_TITLE)
void DX::MSAAHelper::Resolve(_In_ ID3D11DeviceContextX* context, _In_ ID3D11Texture2D* backBuffer)
#else
void DX::MSAAHelper::Resolve(_In_ ID3D11DeviceContext* context, _In_ ID3D11Texture2D* backBuffer)
#endif
{
#if defined(_XBOX_ONE) && defined(_TITLE)
    if (m_fastSemantics)
    {
        context->DecompressResource(
            m_msaaRenderTarget.Get(), 0, nullptr,
            m_msaaRenderTarget.Get(), 0, nullptr,
            m_backBufferFormat, D3D11X_DECOMPRESS_ALL);
    }
#endif

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
    context->ResolveSubresource(backBuffer, 0, m_msaaRenderTarget.Get(), 0, m_resolveFormat);
#else
    context->ResolveSubresource(backBuffer, 0, m_msaaRenderTarget.Get(), 0, m_backBufferFormat);
#endif
}

#endif
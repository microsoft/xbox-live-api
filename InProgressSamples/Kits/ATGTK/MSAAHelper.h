// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// Helper for managing MSAA render targets
//

#pragma once

namespace DX
{
    class MSAAHelper
    {
    public:
        MSAAHelper(DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
                   DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT,
                   unsigned int sampleCount = 4);

#if defined(__d3d12_h__) || defined(__d3d12_x_h__)

        void SetDevice(_In_ ID3D12Device* device);

        void SetWindow(const RECT& output, const float clearColor[4]);

        void ReleaseDevice();

        void Prepare(_In_ ID3D12GraphicsCommandList* commandList);

        void Resolve(_In_ ID3D12GraphicsCommandList* commandList, _In_ ID3D12Resource* backBuffer,
                     D3D12_RESOURCE_STATES beforeState = D3D12_RESOURCE_STATE_RENDER_TARGET,
                     D3D12_RESOURCE_STATES afterState = D3D12_RESOURCE_STATE_PRESENT);

        CD3DX12_CPU_DESCRIPTOR_HANDLE GetMSAARenderTargetView() const
        {
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        }
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetMSAADepthStencilView() const
        {
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12Device>                m_device;
        Microsoft::WRL::ComPtr<ID3D12Resource>              m_msaaRenderTarget;
        Microsoft::WRL::ComPtr<ID3D12Resource>              m_msaaDepthStencil;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        m_rtvDescriptorHeap;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>        m_dsvDescriptorHeap;

#if !defined(_XBOX_ONE) || !defined(_TITLE)
        DXGI_FORMAT                                         m_resolveFormat;
#endif

#elif defined(__d3d11_h__) || defined(__d3d11_x_h__)

        void SetDevice(_In_ ID3D11Device* device);

        void SetWindow(const RECT& output);

        void ReleaseDevice();

#if defined(_XBOX_ONE) && defined(_TITLE)
        void Resolve(_In_ ID3D11DeviceContextX* context, _In_ ID3D11Texture2D* backBuffer);
#else
        void Resolve(_In_ ID3D11DeviceContext* context, _In_ ID3D11Texture2D* backBuffer);
#endif

        ID3D11RenderTargetView*	GetMSAARenderTargetView() const { return m_renderTargetView.Get(); }
        ID3D11DepthStencilView* GetMSAADepthStencilView() const { return m_depthStencilView.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D11Device>                m_device;

        Microsoft::WRL::ComPtr<ID3D11Texture2D>             m_msaaRenderTarget;

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView>      m_renderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView>      m_depthStencilView;

#if defined(_XBOX_ONE) && defined(_TITLE)
        bool                                                m_fastSemantics;
#endif

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
        DXGI_FORMAT                                         m_resolveFormat;
#endif

#else
#   error Please #include <d3d11.h> or <d3d12.h>
#endif

    public:
        DXGI_FORMAT GetBackBufferFormat() const { return m_backBufferFormat; }
        DXGI_FORMAT GetDepthBufferFormat() const { return m_depthBufferFormat; }
        unsigned int GetSampleCount() const { return m_sampleCount; }

    private:
        DXGI_FORMAT                                         m_backBufferFormat;
        DXGI_FORMAT                                         m_depthBufferFormat;
        unsigned int                                        m_sampleCount;
        unsigned int                                        m_targetSampleCount;

        RECT                                                m_outputSize;
    };
}
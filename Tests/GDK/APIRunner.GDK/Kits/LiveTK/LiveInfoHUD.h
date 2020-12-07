//--------------------------------------------------------------------------------------
// File: LiveInfoHUD.h
//
// A Heads Up Display (HUD) for Xbox Live samples
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-------------------------------------------------------------------------------------
#pragma once

#include <atomic>
#include <memory>
#include <string>

#include <XUser.h>

#include "SpriteBatch.h"
#include "SpriteFont.h"

namespace ATG
{
    class LiveInfoHUD
    {
    public:
        explicit LiveInfoHUD(_In_ char const* sampleTitle) noexcept(false);

        LiveInfoHUD(LiveInfoHUD&&) = delete;
        LiveInfoHUD& operator= (LiveInfoHUD&&) = delete;

        LiveInfoHUD(LiveInfoHUD const&) = delete;
        LiveInfoHUD& operator= (LiveInfoHUD const&) = delete;

        void Initialize(int windowWidth = 0, int windowHeight = 0);

        void Update(_In_ ID3D12CommandQueue* commandQueue);

        void ReleaseDevice();
        void RestoreDevice(_In_ ID3D12Device* context,
                           const DirectX::RenderTargetState& renderTarget,
                           DirectX::ResourceUploadBatch& resourceUpload,
                           DirectX::DescriptorPile& pile);

        void SetUser(_In_opt_ XUserHandle user, _In_ XTaskQueueHandle queue);

        void Render(_In_ ID3D12GraphicsCommandList *commandList);

        void SetViewport(const D3D12_VIEWPORT &viewport);

        void SetWindowSize(float width, float height) { m_scaleWidth = width / LAYOUT_PIXEL_WIDTH, m_scaleHeight = height / LAYOUT_PIXEL_HEIGHT; }

    private:
        const float                            LAYOUT_PIXEL_WIDTH  = 1920.f;
        const float                            LAYOUT_PIXEL_HEIGHT = 1080.f;

        std::string                            m_sampleTitle;
        std::string                            m_serviceConfigId;
        std::string                            m_titleId;
        std::string                            m_sandboxId;
        std::string                            m_gamerTag;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_gamerPic;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_gamerDefaultPic;

        Microsoft::WRL::ComPtr<ID3D12Device>   m_device;

        // Direct3D resources
        std::unique_ptr<DirectX::SpriteBatch>  m_batch;
        std::unique_ptr<DirectX::SpriteFont>   m_smallFont;
        std::unique_ptr<DirectX::SpriteFont>   m_boldFont;
        std::unique_ptr<DirectX::SpriteFont>   m_titleFont;
        
        float                                  m_scaleWidth;
        float                                  m_scaleHeight;

        D3D12_CPU_DESCRIPTOR_HANDLE            m_gamerPicCPU;
        D3D12_GPU_DESCRIPTOR_HANDLE            m_gamerPicGPU;
        std::unique_ptr<uint8_t>               m_gamerPicData;
        size_t                                 m_gamerPicDataSize;
        std::atomic<bool>                      m_gamerPicReady;
    };
}
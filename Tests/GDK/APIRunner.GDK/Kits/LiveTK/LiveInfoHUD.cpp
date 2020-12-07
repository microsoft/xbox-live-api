//--------------------------------------------------------------------------------------
// File: LiveInfoHUD.cpp
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
#include "pch.h"
#include "LiveInfoHUD.h"

#include <XGame.h>
#include <XSystem.h>

#include "DescriptorHeap.h"
#include "DirectXHelpers.h"
#include "ResourceUploadBatch.h"
#include "WICTextureLoader.h"

#include "ATGColors.h"
#include "FindMedia.h"

using namespace ATG;
using namespace DirectX;

namespace
{
    const size_t c_GamerPicBuffer = 1024 * 16;
}

_Use_decl_annotations_
LiveInfoHUD::LiveInfoHUD(char const* sampleTitle) noexcept(false) :
    m_sampleTitle(sampleTitle),
    m_gamerTag("No User Signed in"),
    m_scaleWidth(1.f),
    m_scaleHeight(1.f),
    m_gamerPicCPU{},
    m_gamerPicGPU{},
    m_gamerPicDataSize(0),
    m_gamerPicReady(false)
{

}

void LiveInfoHUD::Initialize(int windowWidth, int windowHeight)
{
    UNREFERENCED_PARAMETER(windowWidth);
    UNREFERENCED_PARAMETER(windowHeight);

    uint32_t titleId = {};
    HRESULT hr = XGameGetXboxTitleId(&titleId);

    if (SUCCEEDED(hr))
    {
        char hexTitleId[16] = {};
        sprintf_s(hexTitleId, "0x%08X", titleId);
        m_titleId.assign(hexTitleId);

        char scidBuffer[64] = {};
        sprintf_s(scidBuffer, "00000000-0000-0000-0000-0000%08x", titleId);
        m_serviceConfigId = scidBuffer;
    }
    else
    {
        m_titleId = "Not Set";
        m_serviceConfigId = "Not Set";
    }

    char sandboxId[XSystemXboxLiveSandboxIdMaxBytes] = {};
    XSystemGetXboxLiveSandboxId(XSystemXboxLiveSandboxIdMaxBytes, sandboxId, nullptr);
    m_sandboxId = sandboxId;
}

void LiveInfoHUD::Update(_In_ ID3D12CommandQueue* commandQueue)
{
    if (!m_gamerPicReady || !m_device)
        return;

    CreateShaderResourceView(m_device.Get(), m_gamerDefaultPic.Get(), m_gamerPicCPU);

    if (m_gamerPicDataSize && m_gamerPicData)
    {
        ResourceUploadBatch upload(m_device.Get());

        upload.Begin();

        if (SUCCEEDED(CreateWICTextureFromMemory(m_device.Get(), upload, m_gamerPicData.get(), m_gamerPicDataSize, m_gamerPic.ReleaseAndGetAddressOf())))
        {
            CreateShaderResourceView(m_device.Get(), m_gamerPic.Get(), m_gamerPicCPU);
        }

        auto result = upload.End(commandQueue);
        result.wait();
    }

    m_gamerPicReady = false;
}

void LiveInfoHUD::ReleaseDevice()
{
    m_gamerPic.Reset();
    m_gamerDefaultPic.Reset();

    m_device.Reset();

    m_batch.reset();
    m_smallFont.reset();
    m_boldFont.reset();
    m_titleFont.reset();

    m_gamerPicCPU = {};
    m_gamerPicGPU = {};
}

_Use_decl_annotations_
void LiveInfoHUD::RestoreDevice(
    ID3D12Device* device,
    const RenderTargetState& renderTarget,
    ResourceUploadBatch& resourceUpload,
    DescriptorPile& pile)
{
    m_device = device;

    SpriteBatchPipelineStateDescription pd(renderTarget);
    m_batch = std::make_unique<SpriteBatch>(device, resourceUpload, pd);

    wchar_t buff[MAX_PATH] = {};

    DX::FindMediaFile(buff, MAX_PATH, L"SegoeUI_18.spritefont");
    size_t index = pile.Allocate();
    m_smallFont = std::make_unique<SpriteFont>(device, resourceUpload, buff, pile.GetCpuHandle(index), pile.GetGpuHandle(index));

    DX::FindMediaFile(buff, MAX_PATH, L"SegoeUI_18_Bold.spritefont");
    index = pile.Allocate();
    m_boldFont = std::make_unique<SpriteFont>(device, resourceUpload, buff, pile.GetCpuHandle(index), pile.GetGpuHandle(index));

    DX::FindMediaFile(buff, MAX_PATH, L"SegoeUI_36.spritefont");
    index = pile.Allocate();
    m_titleFont = std::make_unique<SpriteFont>(device, resourceUpload, buff, pile.GetCpuHandle(index), pile.GetGpuHandle(index));

    DX::FindMediaFile(buff, MAX_PATH, L"GamerPic.png");
    DX::ThrowIfFailed(
        CreateWICTextureFromFile(device, resourceUpload, buff, m_gamerDefaultPic.ReleaseAndGetAddressOf())
    );

    index = pile.Allocate();
    m_gamerPicCPU = pile.GetCpuHandle(index);
    m_gamerPicGPU = pile.GetGpuHandle(index);

    if (m_gamerPicDataSize > 0 && m_gamerPicData)
    {
        m_gamerPicReady = true;
    }

    CreateShaderResourceView(device, m_gamerDefaultPic.Get(), m_gamerPicCPU);
}

_Use_decl_annotations_
void LiveInfoHUD::SetUser(XUserHandle user, XTaskQueueHandle queue)
{
    if (!user)
    {
        m_gamerTag = "No User Signed in";

        m_gamerPicDataSize = 0;
        m_gamerPicData.reset();

        CreateShaderResourceView(m_device.Get(), m_gamerDefaultPic.Get(), m_gamerPicCPU);
    }
    else
    {
        m_gamerTag.resize(XUserGamertagComponentClassicMaxBytes);
        if (FAILED(XUserGetGamertag(user, XUserGamertagComponent::Classic, XUserGamertagComponentClassicMaxBytes, &m_gamerTag[0], nullptr)))
        {
            m_gamerTag = "***ERROR***";
        }

        auto async = new XAsyncBlock{};
        async->context = this;
        async->queue = queue;
        async->callback = [](XAsyncBlock *async)
        {
            auto pThis = reinterpret_cast<LiveInfoHUD*>(async->context);

            pThis->m_gamerPicData.reset(new uint8_t[c_GamerPicBuffer]);
            size_t bufferFilled = 0;

            HRESULT hr = XUserGetGamerPictureResult(async, c_GamerPicBuffer, pThis->m_gamerPicData.get(), &bufferFilled);
            if (SUCCEEDED(hr))
            {
                pThis->m_gamerPicDataSize = bufferFilled;
                pThis->m_gamerPicReady = true;
            }
            else
            {
                pThis->m_gamerPicDataSize = 0;
                pThis->m_gamerPicData.reset();
            }

            delete async;
        };

        DX::ThrowIfFailed(XUserGetGamerPictureAsync(user, XUserGamerPictureSize::Small, async));
    }
}

_Use_decl_annotations_
void LiveInfoHUD::Render(ID3D12GraphicsCommandList *commandList)
{
    m_batch->Begin(
        commandList,
        SpriteSortMode_Deferred,
        DirectX::XMMatrixAffineTransformation2D(XMVectorSet(m_scaleWidth, m_scaleHeight, 0, 0), XMVectorZero(), 0.f, XMVectorZero())
    );

    m_titleFont->DrawString(m_batch.get(), m_sampleTitle.c_str(), XMFLOAT2(88.f, 44.f), ATG::Colors::OffWhite, 0.0f);

    m_boldFont->DrawString(m_batch.get(), "Sandbox Id:", XMFLOAT2(270.f, 999.f), ATG::Colors::OffWhite, 0.0f);
    m_smallFont->DrawString(m_batch.get(), m_sandboxId.c_str(), XMFLOAT2(410.f, 999.f), ATG::Colors::OffWhite, 0.0f);

    m_boldFont->DrawString(m_batch.get(), "Title Id:", XMFLOAT2(590.f, 999.f), ATG::Colors::OffWhite, 0.0f);
    m_smallFont->DrawString(m_batch.get(), m_titleId.c_str(), XMFLOAT2(680.f, 999.f), ATG::Colors::OffWhite, 0.0f);

    m_boldFont->DrawString(m_batch.get(), "Service Config Id:", XMFLOAT2(950.f, 999.f), ATG::Colors::OffWhite, 0.0f);
    m_smallFont->DrawString(m_batch.get(), m_serviceConfigId.c_str(), XMFLOAT2(1155.f, 999.f), ATG::Colors::OffWhite, 0.0f);

    auto pos = XMFLOAT2(1770.f, 70.f);
    pos.x -= XMVectorGetX(m_smallFont->MeasureString(m_gamerTag.c_str()));

    m_smallFont->DrawString(m_batch.get(), m_gamerTag.c_str(), pos, ATG::Colors::OffWhite, 0.0f);

    static const RECT gamerPicRect = { 1780, 55, 1780 + 64, 55 + 64 };

    m_batch->Draw(m_gamerPicGPU, XMUINT2(64, 64), gamerPicRect);

    m_batch->End();
}

void LiveInfoHUD::SetViewport(const D3D12_VIEWPORT &viewport)
{
    if (m_batch)
    {
        m_batch->SetViewport(viewport);
        SetWindowSize(viewport.Width, viewport.Height);
    }
}

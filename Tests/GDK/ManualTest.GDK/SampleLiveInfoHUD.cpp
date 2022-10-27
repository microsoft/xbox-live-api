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
#include "SampleLiveInfoHUD.h"

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
    constexpr float c_StatusBarCoordinate = 1010.0f;
    constexpr float c_HeaderBarCoordinate = 10.0f;
    constexpr long c_GamerPicCoordinate = long(c_HeaderBarCoordinate) + 11;
}

_Use_decl_annotations_
SampleLiveInfoHUD::SampleLiveInfoHUD(char const* sampleTitle) noexcept(false) :
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

void SampleLiveInfoHUD::Initialize(int windowWidth, int windowHeight)
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

void SampleLiveInfoHUD::Update(_In_ ID3D12CommandQueue* commandQueue)
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

void SampleLiveInfoHUD::ReleaseDevice()
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
void SampleLiveInfoHUD::RestoreDevice(
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
void SampleLiveInfoHUD::SetUser(XUserHandle user, XTaskQueueHandle queue)
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
            auto pThis = reinterpret_cast<SampleLiveInfoHUD*>(async->context);

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

void SampleLiveInfoHUD::ClearLog()
{
    std::lock_guard<std::mutex> lock(m_logLinesMutex);
    m_logLines.clear();
}

void WriteLogToFile(const std::string& strIn)
{
    HANDLE hFile;
    std::string str = strIn;
    str += "\r\n";
    DWORD dwBytesToWrite = (DWORD) str.length();
    DWORD dwBytesWritten = 0;
    BOOL bErrorFlag = FALSE;

    hFile = CreateFile(L"D:\\EventsLog.txt", FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return;
    }

    bErrorFlag = WriteFile(
        hFile,           // open file handle
        str.data(),      // start of data to write
        dwBytesToWrite,  // number of bytes to write
        &dwBytesWritten, // number of bytes that were written
        NULL);            // no overlapped structure

    if (FALSE == bErrorFlag)
    {
    }
    else
    {
        if (dwBytesWritten != dwBytesToWrite)
        {
            //printf("Error: dwBytesWritten != dwBytesToWrite\n");
        }
        else
        {
            //printf("Wrote %d bytes to EventsLog.txt successfully.\n", dwBytesWritten);
        }
    }

    CloseHandle(hFile);
}

void SampleLiveInfoHUD::AddLog(const std::string& strIn)
{
    std::lock_guard<std::mutex> lock(m_logLinesMutex);

    SYSTEMTIME st;
    GetLocalTime(&st);
    char sz[255];
    sprintf_s(sz, 255, "[%0.2d/%0.2d %0.2d:%0.2d:%0.2d:%0.4d] ",
        st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    std::string strTime = sz;
    std::string str = strTime + strIn;

    for (size_t i = 0; i < str.length(); i++)
    {
        if (static_cast<unsigned char>(str[i]) > 0x80) // ignore invalid chars since font can't render
        {
            str[i] = ' ';
        }
    }

    OutputDebugStringA(str.c_str());
    OutputDebugStringA("\n");

    if (m_logLines.size() > 27)
    {
        m_logLines.erase(m_logLines.begin());
    }
    m_logLines.push_back(str);
    WriteLogToFile(str);

}

_Use_decl_annotations_
void SampleLiveInfoHUD::Render(ID3D12GraphicsCommandList* commandList)
{
    m_batch->Begin(
        commandList,
        SpriteSortMode_Deferred,
        DirectX::XMMatrixAffineTransformation2D(XMVectorSet(m_scaleWidth, m_scaleHeight, 0, 0), XMVectorZero(), 0.f, XMVectorZero())
    );

    if(m_logLines.size() > 0)
    {
        std::lock_guard<std::mutex> lock(m_logLinesMutex);
        float y = 100.0f;
        for (auto& s : m_logLines)
        {
            m_smallFont->DrawString(m_batch.get(), s.c_str(), XMFLOAT2(60.f, y), ATG::Colors::White, 0.0f);
            y += 30.0f;
        }
    }

    float y = 800.0f;
    float x = 1210.0f;
    m_smallFont->DrawString(m_batch.get(), "Press 1 for XblInit. Ctrl+1 for XblCleanup", XMFLOAT2(x, y += 30.0f), ATG::Colors::OffWhite, 0.0f);
    m_smallFont->DrawString(m_batch.get(), "Press 2 for XUserAdd. Ctrl+2 for UserClose", XMFLOAT2(x, y += 30.0f), ATG::Colors::OffWhite, 0.0f);
    m_smallFont->DrawString(m_batch.get(), "Press 3 for RTA on. Ctrl+3 for RTA off", XMFLOAT2(x, y += 30.0f), ATG::Colors::OffWhite, 0.0f);
    m_smallFont->DrawString(m_batch.get(), "Press 4 for CreateSession. Ctrl+4 for session close", XMFLOAT2(x, y += 30.0f), ATG::Colors::OffWhite, 0.0f);
    m_smallFont->DrawString(m_batch.get(), "Press 5 for WriteSession", XMFLOAT2(x, y += 30.0f), ATG::Colors::OffWhite, 0.0f);

    m_boldFont->DrawString(m_batch.get(), "Sandbox Id:", XMFLOAT2(270.f, c_StatusBarCoordinate), ATG::Colors::OffWhite, 0.0f);
    m_smallFont->DrawString(m_batch.get(), m_sandboxId.c_str(), XMFLOAT2(410.f, c_StatusBarCoordinate), ATG::Colors::OffWhite, 0.0f);

    m_boldFont->DrawString(m_batch.get(), "Title Id:", XMFLOAT2(590.f, c_StatusBarCoordinate), ATG::Colors::OffWhite, 0.0f);
    m_smallFont->DrawString(m_batch.get(), m_titleId.c_str(), XMFLOAT2(680.f, c_StatusBarCoordinate), ATG::Colors::OffWhite, 0.0f);

    m_boldFont->DrawString(m_batch.get(), "Service Config Id:", XMFLOAT2(950.f, c_StatusBarCoordinate), ATG::Colors::OffWhite, 0.0f);
    m_smallFont->DrawString(m_batch.get(), m_serviceConfigId.c_str(), XMFLOAT2(1155.f, c_StatusBarCoordinate), ATG::Colors::OffWhite, 0.0f);

    m_batch->End();
}

void SampleLiveInfoHUD::SetViewport(const D3D12_VIEWPORT &viewport)
{
    if (m_batch)
    {
        m_batch->SetViewport(viewport);
        SetWindowSize(viewport.Width, viewport.Height);
    }
}

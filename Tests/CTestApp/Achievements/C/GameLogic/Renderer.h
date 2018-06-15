// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "Common\DeviceResources.h"
#include "Common\StepTimer.h"
#include "CommonStates.h"
#include "DirectXTK\Inc\Effects.h"
#include "GeometricPrimitive.h"
#include "Model.h"
#include "PrimitiveBatch.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "GameData.h"

#define TEXT_BUFFER_SIZE 1024

namespace Sample
{
    // This class renders a scene using DirectXTK
    class Renderer
    {
    public:
        Renderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void Update(DX::StepTimer const& timer);
        void Render();
        void AddTextObject();

        void RenderMenuOptions(FLOAT scale, const DirectX::FXMVECTOR& TEXT_COLOR);
        void RenderEventLog(FLOAT fGridXColumn1, FLOAT fGridY, FLOAT fTextHeight, FLOAT scale, const DirectX::FXMVECTOR& TEXT_COLOR);
        void RenderPerfCounters(FLOAT fGridXColumn1, FLOAT fGridY, FLOAT fTextHeight, FLOAT scale, const DirectX::XMVECTORF32& TEXT_COLOR);

        void Renderer::RenderAchievements(
            FLOAT fGridXColumn1,
            FLOAT fGridY,
            FLOAT fTextHeight,
            FLOAT scale,
            const DirectX::XMVECTORF32& TEXT_COLOR
        );

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        std::unique_ptr<DirectX::CommonStates>                                  m_states;
        std::unique_ptr<DirectX::IEffectFactory>                                m_fxFactory;
        std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
        std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
        std::unique_ptr<DirectX::SpriteFont>                                    m_font;
    };
}


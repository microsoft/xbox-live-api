// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#else
#include <d3d11_1.h>
#endif

#include "DDSTextureLoader.h"


namespace DX
{
	class Texture
	{
	public:
        Texture(_In_ ID3D11Device* device, _In_z_ const wchar_t* fileName, bool forceSRGB = false);
		explicit Texture(_In_ ID3D11ShaderResourceView* resourceView);

        Texture(Texture const&) = delete;
        Texture& operator= (Texture const&) = delete;

        int Width() const { return m_width; }
        int Height() const { return m_height; }
        int Depth() const { return m_depth; }
        int LevelCount() const { return m_mips; }
        int ArrayCount() const { m_array; }
		DXGI_FORMAT Format() const { return m_format; }
        DirectX::DDS_ALPHA_MODE AlphaMode() const { return m_alphaMode; }

        void GetResource(_Outptr_ ID3D11Resource** resource) const
        {
            assert(resource != 0);
            m_resourceView->GetResource(resource);
        }

        void GetResourceView(_Outptr_ ID3D11ShaderResourceView** resourceView) const
        {
            assert(resourceView != 0);
            *resourceView = m_resourceView.Get();
            (*resourceView)->AddRef();
        }

        ID3D11ShaderResourceView* Get() const { return m_resourceView.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_resourceView;

        int                     m_width;
        int                     m_height;
        int                     m_depth;
        int                     m_mips;
        int                     m_array;
        DXGI_FORMAT             m_format;
        DirectX::DDS_ALPHA_MODE m_alphaMode;

        void GetDesc(_In_ ID3D11Resource* resource);
	};
}
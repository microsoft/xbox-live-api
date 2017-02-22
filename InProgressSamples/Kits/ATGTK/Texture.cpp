// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Texture.h"

#include <exception>

#include "DirectXHelpers.h"
#include "WICTextureLoader.h"

using namespace DirectX;
using namespace DX;

using Microsoft::WRL::ComPtr;

_Use_decl_annotations_
Texture::Texture(ID3D11Device* device, const wchar_t* fileName, bool forceSRGB) :
    m_width(0),
    m_height(0),
    m_depth(0),
    m_mips(0),
    m_array(0),
    m_format(DXGI_FORMAT_UNKNOWN),
    m_alphaMode(DDS_ALPHA_MODE_UNKNOWN)
{
    assert(device != 0 && fileName != 0);

    WCHAR ext[_MAX_EXT];
    _wsplitpath_s(fileName, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

    ComPtr<ID3D11Resource> resource;
    ComPtr<ID3D11ShaderResourceView> resourceView;
    if (_wcsicmp(ext, L".dds") == 0)
    {
        ThrowIfFailed(
            CreateDDSTextureFromFileEx( device, fileName, 0,
                D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
                forceSRGB, 
                resource.GetAddressOf(), resourceView.GetAddressOf(), &m_alphaMode));
    }
    else
    {
        ThrowIfFailed(
            CreateWICTextureFromFileEx(device, fileName, 0, 
                D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
                forceSRGB, 
                resource.GetAddressOf(), resourceView.GetAddressOf()));
    }

    GetDesc(resource.Get());

    // Take ownership of resource
    m_resourceView.Swap(resourceView);
}

Texture::Texture(_In_ ID3D11ShaderResourceView* resourceView) :
    m_width(0),
    m_height(0),
    m_depth(0),
    m_mips(0),
    m_array(0),
    m_format(DXGI_FORMAT_UNKNOWN),
    m_alphaMode(DDS_ALPHA_MODE_UNKNOWN)
{
    assert(resourceView != 0);

    ComPtr<ID3D11Resource> resource;
    m_resourceView->GetResource(resource.GetAddressOf());

    GetDesc(resource.Get());

    // Take ownership of resource
    m_resourceView = resourceView;
}

void Texture::GetDesc(_In_ ID3D11Resource* resource)
{
    D3D11_RESOURCE_DIMENSION dim;
    resource->GetType(&dim);

    switch (dim)
    {
    case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
        {
            ComPtr<ID3D11Texture1D> tex;
            if (SUCCEEDED(resource->QueryInterface(IID_GRAPHICS_PPV_ARGS(tex.GetAddressOf()))))
            {
                D3D11_TEXTURE1D_DESC desc;
                tex->GetDesc(&desc);

                m_width = static_cast<int>(desc.Width);
                m_height = 1;
                m_depth = 1;
                m_mips = static_cast<int>(desc.MipLevels);
                m_array = static_cast<int>(desc.ArraySize);
                m_format = desc.Format;
            }
        }
        break;

    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
        {
            ComPtr<ID3D11Texture2D> tex;
            if (SUCCEEDED(resource->QueryInterface(IID_GRAPHICS_PPV_ARGS(tex.GetAddressOf()))))
            {
                D3D11_TEXTURE2D_DESC desc;
                tex->GetDesc(&desc);

                m_width = static_cast<int>(desc.Width);
                m_height = static_cast<int>(desc.Height);
                m_depth = 1;
                m_mips = static_cast<int>(desc.MipLevels);
                m_array = static_cast<int>(desc.ArraySize);
                m_format = desc.Format;
            }
        }
        break;

    case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
        {
            ComPtr<ID3D11Texture3D> tex;
            if (SUCCEEDED(resource->QueryInterface(IID_GRAPHICS_PPV_ARGS(tex.GetAddressOf()))))
            {
                D3D11_TEXTURE3D_DESC desc;
                tex->GetDesc(&desc);

                m_width = static_cast<int>(desc.Width);
                m_height = static_cast<int>(desc.Height);
                m_depth = static_cast<int>(desc.Depth);
                m_mips = static_cast<int>(desc.MipLevels);
                m_array = 1;
                m_format = desc.Format;
            }
        }
        break;

    default:
        throw std::exception("Unknown resource dimension");
    }
}

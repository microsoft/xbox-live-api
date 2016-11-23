//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "CatalogItemImage_WinRT.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

CatalogItemImage::CatalogItemImage(
    _In_ xbox::services::marketplace::catalog_item_image cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_purposes = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.purposes())->GetView();
}

Windows::Foundation::Collections::IVectorView<Platform::String^>^ 
CatalogItemImage::Purposes::get()
{
    return m_purposes;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END

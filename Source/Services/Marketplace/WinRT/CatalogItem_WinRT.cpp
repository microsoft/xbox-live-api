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
#include "CatalogItem_WinRT.h"
#include "CatalogItemImage_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::marketplace;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

CatalogItem::CatalogItem(
    _In_ xbox::services::marketplace::catalog_item cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_images = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<CatalogItemImage, catalog_item_image>(m_cppObj.images())->GetView();
    m_availabilityContentIds = UtilsWinRT::CreatePlatformVectorFromStdVectorString(m_cppObj.availability_content_ids())->GetView();
}

Windows::Foundation::Collections::IVectorView<CatalogItemImage^>^ 
CatalogItem::Images::get()
{
    return m_images;
}

Windows::Foundation::Collections::IVectorView<Platform::String^>^ 
CatalogItem::AvailabilityContentIds::get()
{
    return m_availabilityContentIds;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
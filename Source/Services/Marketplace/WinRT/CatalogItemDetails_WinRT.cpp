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
#include "CatalogItemDetails_WinRT.h"
#include "CatalogItemImage_WinRT.h"

using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::marketplace;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

CatalogItemDetails::CatalogItemDetails(
    _In_ xbox::services::marketplace::catalog_item_details cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_images = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<CatalogItemImage, catalog_item_image>(m_cppObj.images())->GetView();
    m_availabilities = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<CatalogItemAvailability, catalog_item_availability>(m_cppObj.availabilities())->GetView();
}

Windows::Foundation::Collections::IVectorView<CatalogItemImage^>^ 
CatalogItemDetails::Images::get()
{
    return m_images;
}

Windows::Foundation::Collections::IVectorView<CatalogItemAvailability^>^ 
CatalogItemDetails::Availabilities::get()
{
    return m_availabilities;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
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
#include "CatalogService_WinRT.h"

using namespace Concurrency;
using namespace Platform::Collections;
using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::Media;
using namespace Windows::Data::Json;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::marketplace;
using namespace xbox::services;

#if TV_API
using namespace Windows::Media::ContentRestrictions;
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

CatalogService::CatalogService( 
    _In_ xbox::services::marketplace::catalog_service cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

Windows::Foundation::IAsyncOperation<BrowseCatalogResult^>^ 
CatalogService::BrowseCatalogAsync(
    _In_ Platform::String^ parentId,
    _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType parentMediaType,
    _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType childMediaType,
    _In_ CatalogSortOrder orderBy,
    _In_ uint32 skipItems,
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.browse_catalog(
        STRING_T_FROM_PLATFORM_STRING(parentId),
        static_cast<media_item_type>(parentMediaType),
        static_cast<media_item_type>(childMediaType),
        static_cast<catalog_sort_order>(orderBy),
        skipItems,
        maxItems
        )
    .then([](xbox_live_result<browse_catalog_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new BrowseCatalogResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<BrowseCatalogResult^>^ 
CatalogService::BrowseCatalogBundlesAsync(
    _In_ Platform::String^ parentId,
    _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType parentMediaType,
    _In_ Platform::String^ productId,
    _In_ BundleRelationshipType relationship, 
    _In_ uint32 skipItems,
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.browse_catalog_bundles(
        STRING_T_FROM_PLATFORM_STRING(parentId),
        static_cast<media_item_type>(parentMediaType),
        STRING_T_FROM_PLATFORM_STRING(productId),
        static_cast<bundle_relationship_type>(relationship),
        skipItems,
        maxItems
        )
    .then([](xbox_live_result<browse_catalog_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new BrowseCatalogResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<CatalogItemDetails^>^>^ 
CatalogService::GetCatalogItemDetailsAsync(
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ productIds
    )
{
    auto task = m_cppObj.get_catalog_item_details(
        UtilsWinRT::CreateStdVectorStringFromPlatformVectorObj(productIds)
        )
    .then([](xbox_live_result<std::vector<catalog_item_details>> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return UtilsWinRT::CreatePlatformVectorFromStdVectorObj<CatalogItemDetails, catalog_item_details>(cppResult.payload())->GetView();
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/marketplace.h"
#include "CatalogItemDetails_WinRT.h"
#include "CatalogSortOrder_WinRT.h"
#include "BrowseCatalogResult_WinRT.h"
#include "MediaItemType_WinRT.h"
#include "BundleRelationshipType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

public ref class CatalogService sealed
{
public:

    /// <summary>
    /// Browse for catalog items from within a single media group.
    /// </summary>
    /// <param name="parentId">The product ID of the parent product</param>
    /// <param name="parentMediaType">The media type of the parent</param>
    /// <param name="childMediaType">The media type of the child</param>
    /// <param name="orderby">Controls how the list is ordered</param>
    /// <param name="skipItems">The number of items to skip</param>
    /// <param name="maxItems">The maximum number of items the result can contain. Pass 0 to attempt retrieve as many items as possible.</param>
    /// <returns>BrowseCatalogResult object containing the items</returns>
    /// <remarks>Calls V3.2 GET /media/{marketplaceId}/browse</remarks>
    Windows::Foundation::IAsyncOperation<BrowseCatalogResult^>^ 
    BrowseCatalogAsync(
        _In_ Platform::String^ parentId,
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType parentMediaType,
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType childMediaType,
        _In_ CatalogSortOrder orderBy,
        _In_ uint32 skipItems,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Browse the catalog related to bundles an item is a member of or included items of a bundle
    /// </summary>
    /// <param name="parentId">The product ID of the parent product</param>
    /// <param name="parentMediaType">The media type of the parent</param>
    /// <param name="productId">The ID of the product we are browsing the relationship for</param>
    /// <param name="relationship">The relationship to be used in the browse call either included products in a bundle, or bundles the product is a member of</param>
    /// <param name="skipItems">The number of items to skip</param>
    /// <param name="maxItems">The maximum number of items the result can contain. Pass 0 to attempt retrieve as many items as possible.</param>
    /// <returns>BrowseCatalogResult object containing the items</returns>
    /// <remarks>Calls V3.2 GET /media/{marketplaceId}/browse</remarks>
    Windows::Foundation::IAsyncOperation<BrowseCatalogResult^>^ 
    BrowseCatalogBundlesAsync(
        _In_ Platform::String^ parentId,
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType parentMediaType,
        _In_ Platform::String^ productId,
        _In_ BundleRelationshipType relationship, 
        _In_ uint32 skipItems,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Get the product details for a set of ProductIDs
    /// </summary>
    /// <param name="productIds">A collection of product IDs to get item details for. A maximum of ten strings may be passed per call to this method.</param>
    /// <returns>A collection of CatalogItems</returns>
    /// <remarks>Calls V3.2 GET /media/{marketplaceId}/details</remarks>
    Windows::Foundation::IAsyncOperation<Windows::Foundation::Collections::IVectorView<CatalogItemDetails^>^>^ 
    GetCatalogItemDetailsAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ productIds
        );

internal:
    CatalogService::CatalogService( 
        _In_ xbox::services::marketplace::catalog_service cppObj
        );

private:
    xbox::services::marketplace::catalog_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END

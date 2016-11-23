//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#pragma once
#include "xsapi/marketplace.h"
#include "CatalogItem_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

public ref class BrowseCatalogResult sealed
{
public:
    //    "Items":
    //    [
    //      {  
    //        // ...  
    //      }
    //    ],
    //
    //    "Totals":
    //    [
    //      {
    //         "Name":"GameType",
    //         "Count":126,
    //         "MediaItemTypes":
    //         [
    //            {
    //               "Name":"XboxGameConsumable",
    //               "Count":126
    //            }
    //         ]
    //      }
    //    ],

    /// <summary>
    /// Collection of XboxCatalogItem objects returned by a request
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<CatalogItem^>^ Items 
    { 
        Windows::Foundation::Collections::IVectorView<CatalogItem^>^ get(); 
    }

    /// <summary>
    // The total count of the items
    /// </summary>
    DEFINE_PROP_GET_OBJ(TotalCount, total_count, uint32);

    /// <summary>
    /// Returns an BrowseCatalogResult object containing the next page of BrowseCatalogResult
    /// </summary>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt retrieving all items.</param>
    /// <returns>A BrowseCatalogResult object.</returns>
    /// <remarks>Calls V3.2 GET /media/{marketplaceId}/browse</remarks>
    Windows::Foundation::IAsyncOperation<BrowseCatalogResult^>^ GetNextAsync(
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Indicates if there is additional data to retrieve from a GetNextAsync call
    /// </summary>
    DEFINE_PROP_GET_OBJ(HasNext, has_next, bool);

internal:
    BrowseCatalogResult(
        _In_ xbox::services::marketplace::browse_catalog_result cppObj
        );

private:
    xbox::services::marketplace::browse_catalog_result m_cppObj;
    Windows::Foundation::Collections::IVectorView<CatalogItem^>^ m_items;
};


NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END

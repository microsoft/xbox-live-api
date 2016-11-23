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
#include "InventoryItem_WinRT.h"
#include "InventoryItemAvailability_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

public ref class InventoryItemsResult sealed
{
public:
    /// <summary>
    /// Collection of InventoryItem objects returned by a request
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<InventoryItem^>^ Items 
    { 
        Windows::Foundation::Collections::IVectorView<InventoryItem^>^ get(); 
    }

    /// <summary>
    /// The total number of inventory items that matched the request.
    /// </summary>
    DEFINE_PROP_GET_OBJ(TotalItems, total_items, uint32);

    /// <summary>
    /// Returns an InventoryResult object containing the next page of InventoryItems
    /// </summary>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt
    /// retrieving all items.</param>
    /// <returns>InventoryResult object containing the next page of InventoryItem objects.</returns>
    /// <remarks>Calls V2 GET /users/me/inventory</remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetNextAsync(
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Indicates if there is additional data to retrieve from a GetNextAsync call
    /// </summary>
    DEFINE_PROP_GET_OBJ(HasNext, has_next, bool);

internal:
    InventoryItemsResult(
        _In_ xbox::services::marketplace::inventory_items_result cppObj
        );

private:
    xbox::services::marketplace::inventory_items_result m_cppObj;
    Windows::Foundation::Collections::IVectorView<InventoryItem^>^ m_items;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END

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
#include "ConsumeInventoryItemResult_WinRT.h"
#include "InventoryItem_WinRT.h"
#include "InventoryItemAvailability_WinRT.h"
#include "InventoryItemsResult_WinRT.h"
#include "InventoryItemState_WinRT.h"
#include "MediaItemType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

public ref class InventoryService sealed
{
public:

    /// <summary>
    /// Enumerate user's inventory for all available items of the specified type that are in the enabled state.
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate</param>
    /// <returns>InventoryItemsResult object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetInventoryItemsAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType mediaItemType
        );

    /// <summary>
    /// Get the inventory results for a set of ProductIDs for the current user
    /// </summary>
    /// <param name="productIds">A collection of IDs to query the inventory for</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <returns>InventoryItemsResult object containing the inventoryItems</returns>
    /// <remarks>
    /// It is recommended that you limit your ProductIds requested per list to a maximum of 100.
    ///
    /// Calls V4 GET /users/me/inventory
    /// </remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetInventoryItemsAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ productIds,
        _In_ bool expandSatisfyingEntitlements
        );

    /// <summary>
    /// Enumerate user's inventory with filter options to include satisfying entitlements and pre-order or unavailable items.
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <param name="includeAllItemStatesAndAvailabilities">Include results of pre-orders and now-unavailable products the user owns</param>
    /// <returns>InventoryItemsResult object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetInventoryItemsAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType mediaItemType,
        _In_ bool expandSatisfyingEntitlements,
        _In_ bool includeAllItemStatesAndAvailabilities
        );

    /// <summary>
    /// Enumerate a user's inventory
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate</param>
    /// <param name="inventoryItemState">The state of the inventory items to include in the result</param>
    /// <param name="inventoryItemAvailability">The availability of the inventory items to include in the result</param>
    /// <param name="inventoryItemContainerId">The container Id of the inventory items to include in the result (Optional)</param>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt
    /// retrieving all items.</param>
    /// <returns>InventoryItemsResult object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetInventoryItemsAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType mediaItemType,
        _In_ Microsoft::Xbox::Services::Marketplace::InventoryItemState inventoryItemState,
        _In_ Microsoft::Xbox::Services::Marketplace::InventoryItemAvailability inventoryItemAvailability,
        _In_opt_ Platform::String^ inventoryItemContainerId,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Enumerate a user's inventory with filter options to include satisfying entitlements
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate</param>
    /// <param name="inventoryItemState">The state of the inventory items to include in the result</param>
    /// <param name="inventoryItemAvailability">The availability of the inventory items to include in the result</param>
    /// <param name="inventoryItemContainerId">The container Id of the inventory items to include in the result (Optional)</param>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt
    /// retrieving all items.</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <returns>InventoryItemsResult object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetInventoryItemsAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType mediaItemType,
        _In_ Microsoft::Xbox::Services::Marketplace::InventoryItemState inventoryItemState,
        _In_ Microsoft::Xbox::Services::Marketplace::InventoryItemAvailability inventoryItemAvailability,
        _In_opt_ Platform::String^ inventoryItemContainerId,
        _In_ uint32 maxItems,
        _In_ bool expandSatisfyingEntitlements
        );

    /// <summary>
    /// Enumerate inventory for all signed-in users for all available items of the specified type that are in the enabled state.
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate</param>
    /// <returns>InventoryItemsResult object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetInventoryItemsForAllUsersAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType mediaItemType
        );

    /// <summary>
    /// Enumerate inventory for all signed-in users with filter options to include satisfying entitlements and pre-order or unavailable items.
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <param name="includeAllItemStatesAndAvailabilities">Include results of pre-orders and now-unavailable products the user owns</param>
    /// <returns>InventoryItemsResult object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetInventoryItemsForAllUsersAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType mediaItemType,
        _In_ bool expandSatisfyingEntitlements,
        _In_ bool includeAllItemStatesAndAvailabilities
        );

    /// <summary>
    /// Enumerate inventory for all signed-in users
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate</param>
    /// <param name="inventoryItemState">The state of the inventory items to include in the result</param>
    /// <param name="inventoryItemAvailability">The availability of the inventory items to include in the result</param>
    /// <param name="inventoryItemContainerId">The container Id of the inventory items to include in the result (Optional)</param>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt
    /// retrieving all items.</param>
    /// <returns>InventoryItemsResult object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetInventoryItemsForAllUsersAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType mediaItemType,
        _In_ Microsoft::Xbox::Services::Marketplace::InventoryItemState inventoryItemState,
        _In_ Microsoft::Xbox::Services::Marketplace::InventoryItemAvailability inventoryItemAvailability,
        _In_opt_ Platform::String^ inventoryItemContainerId,
        _In_ uint32 maxItems
        );

    /// <summary>
    /// Enumerate inventory for all signed-in users
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate</param>
    /// <param name="inventoryItemState">The state of the inventory items to include in the result</param>
    /// <param name="inventoryItemAvailability">The availability of the inventory items to include in the result</param>
    /// <param name="inventoryItemContainerId">The container Id of the inventory items to include in the result (Optional)</param>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt
    /// retrieving all items.</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <returns>InventoryItemsResult object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetInventoryItemsForAllUsersAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType mediaItemType,
        _In_ Microsoft::Xbox::Services::Marketplace::InventoryItemState inventoryItemState,
        _In_ Microsoft::Xbox::Services::Marketplace::InventoryItemAvailability inventoryItemAvailability,
        _In_opt_ Platform::String^ inventoryItemContainerId,
        _In_ uint32 maxItems,
        _In_ bool expandSatisfyingEntitlements
        );

    /// <summary>
    /// Get the inventory results for a set of ProductIDs for the current user
    /// </summary>
    /// <param name="productIds">A collection of IDs to query the inventory for</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <returns>InventoryItemsResult object containing the inventoryItems</returns>
    /// <remarks>
    /// It is recommended that you limit your ProductIds requested per list to a maximum of 100.
    ///
    /// Calls V4 GET /users/me/inventory
    /// </remarks>
    Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^ GetInventoryItemsForAllUsersAsync(
        _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ productIds,
        _In_ bool expandSatisfyingEntitlements
        );

    /// <summary>
    /// Gets a single inventory item for the specified Id
    /// </summary>
    /// <param name="inventoryItem">The InventoryItem to get an update for.</param>
    /// <returns>The requested InventoryItem object</returns>
    /// <remarks>Calls V4 GET /users/me/inventory/{inventoryItemId}</remarks>
    Windows::Foundation::IAsyncOperation<InventoryItem^>^ GetInventoryItemAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::InventoryItem^ inventoryItem
        );

    /// <summary>
    /// Consumes the specified quantity of a consumable inventory item.
    /// </summary>
    /// <param name="inventoryItem">The InventoryItem to consume quantity from.</param>
    /// <param name="quantityToConsume">The quantity to consume of the specified InventoryItem.</param>
    /// <param name="transactionId">A client generated unique Id for the transaction.
    /// Transactions with the same Id will only be processed once.</param>
    /// <returns>A ConsumeInventoryItemResult object.</returns>
    /// <remarks>Calls V4 POST /users/me/consumables/{consumableId}</remarks>
    Windows::Foundation::IAsyncOperation<ConsumeInventoryItemResult^>^ ConsumeInventoryItemAsync(
        _In_ Microsoft::Xbox::Services::Marketplace::InventoryItem^ inventoryItem,
        _In_ uint32 quantityToConsume,
        _In_ Platform::String^ transactionId
        );

internal:
    InventoryService( 
        _In_ xbox::services::marketplace::inventory_service cppObj
        );

private:
    xbox::services::marketplace::inventory_service m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
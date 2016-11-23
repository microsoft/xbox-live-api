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

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

/// <summary>Enumeration values that indicate the current availability inventory items</summary>
public enum class InventoryItemAvailability
{
    /// <summary>
    /// All items should be returned regardless of availability.
    /// </summary>
    All = xbox::services::marketplace::inventory_item_availability::all,

    /// <summary>
    /// Item is available.  The current date falls between the start and end date of the item.
    /// </summary>
    Available = xbox::services::marketplace::inventory_item_availability::available,

    /// <summary>
    /// Item is unavailable.  The current date falls outside the start and end date of the item.
    /// </summary>
    Unavailable = xbox::services::marketplace::inventory_item_availability::unavailable
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
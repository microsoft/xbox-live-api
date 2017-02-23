// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
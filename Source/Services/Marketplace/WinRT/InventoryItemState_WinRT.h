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

/// <summary>Enumeration values that indicate the inventory item state</summary>
public enum class InventoryItemState
{
    /// <summary>Unknown item state.</summary>
    Unknown = xbox::services::marketplace::inventory_item_state::unknown,

    /// <summary>All is used to request inventory items for all states.</summary>
    All = xbox::services::marketplace::inventory_item_state::all,

    /// <summary>Enabled item state.</summary>
    Enabled = xbox::services::marketplace::inventory_item_state::enabled,

    /// <summary>Suspended item state.</summary>
    Suspended = xbox::services::marketplace::inventory_item_state::suspended,

    /// <summary>Expired item state.</summary>
    Expired = xbox::services::marketplace::inventory_item_state::expired,

    /// <summary>Canceled item state.  This state pertains to subscriptions that have been canceled.</summary>
    Canceled = xbox::services::marketplace::inventory_item_state::canceled
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END

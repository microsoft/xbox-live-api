// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/marketplace.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

public ref class ConsumeInventoryItemResult sealed
{
public:
    /// <summary>
    /// The consumable item Url of the item that was consumed
    /// </summary>
    DEFINE_PROP_GET_URI_OBJ(ConsumableUrl, consumable_url);

    /// <summary>
    /// The new balance for the consumable item.
    /// </summary>
    DEFINE_PROP_GET_OBJ(ConsumableBalance, consumable_balance, uint32);

    /// <summary>
    /// The transaction Id that serves to acknowledge receipt of a specific request
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(TransactionId, transaction_id);

internal:
    ConsumeInventoryItemResult(
        _In_ xbox::services::marketplace::consume_inventory_item_result cppObj
        );

private:
    xbox::services::marketplace::consume_inventory_item_result m_cppObj;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
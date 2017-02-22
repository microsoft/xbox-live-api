// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "ConsumeInventoryItemResult_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

ConsumeInventoryItemResult::ConsumeInventoryItemResult(
    _In_ xbox::services::marketplace::consume_inventory_item_result cppObj
    ):
    m_cppObj(std::move(cppObj))
{
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
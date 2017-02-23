// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "InventoryItem_WinRT.h"
#include "InventoryItemsResult_WinRT.h"

using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace Microsoft::Xbox::Services::System;
using namespace xbox::services::marketplace;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

InventoryItemsResult::InventoryItemsResult(
    _In_ xbox::services::marketplace::inventory_items_result cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
    m_items = UtilsWinRT::CreatePlatformVectorFromStdVectorObj<InventoryItem, inventory_item>(m_cppObj.items())->GetView();
}

IVectorView<InventoryItem^>^
InventoryItemsResult::Items::get()
{
    return m_items;
}

IAsyncOperation<InventoryItemsResult^>^
InventoryItemsResult::GetNextAsync(
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.get_next(maxItems)
    .then([](xbox::services::xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
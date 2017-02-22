// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "InventoryService_WinRT.h"
#include "utils_winrt.h"

using namespace Concurrency;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Marketplace;
using namespace Microsoft::Xbox::Services::System;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
using namespace xbox::services::marketplace;
using namespace xbox::services;

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

const wchar_t c_inventoryContractVersionHeaderValue[] = L"4";

InventoryService::InventoryService( 
    _In_ xbox::services::marketplace::inventory_service cppObj
    ) :
    m_cppObj(std::move(cppObj))
{
}

IAsyncOperation<InventoryItemsResult^>^
InventoryService::GetInventoryItemsAsync(
    _In_ MediaItemType mediaItemType
    )
{
    auto task = m_cppObj.get_inventory_items(
        static_cast<media_item_type>(mediaItemType)
        )
    .then([](xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^
InventoryService::GetInventoryItemsAsync(
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ productIds,
    _In_ bool expandSatisfyingEntitlements
    )
{
    auto task = m_cppObj.get_inventory_items(
        UtilsWinRT::CovertVectorViewToStdVectorString(productIds),
        expandSatisfyingEntitlements
        )
    .then([](xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^
InventoryService::GetInventoryItemsAsync(
    _In_ MediaItemType mediaItemType,
    _In_ bool expandSatisfyingEntitlements,
    _In_ bool includeAllItemStatesAndAvailabilities
    )
{
    auto task = m_cppObj.get_inventory_items(
        static_cast<media_item_type>(mediaItemType),
        expandSatisfyingEntitlements,
        includeAllItemStatesAndAvailabilities
        )
    .then([](xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<InventoryItemsResult^>^
InventoryService::GetInventoryItemsAsync(
    _In_ MediaItemType mediaItemType,
    _In_ InventoryItemState inventoryItemState,
    _In_ InventoryItemAvailability inventoryItemAvailability,
    _In_opt_ Platform::String^ inventoryItemContainerId,
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.get_inventory_items(
        static_cast<media_item_type>(mediaItemType),
        static_cast<inventory_item_state>(inventoryItemState),
        static_cast<inventory_item_availability>(inventoryItemAvailability),
        STRING_T_FROM_PLATFORM_STRING(inventoryItemContainerId),
        maxItems
        )
    .then([](xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^
InventoryService::GetInventoryItemsAsync(
    _In_ MediaItemType mediaItemType,
    _In_ InventoryItemState inventoryItemState,
    _In_ InventoryItemAvailability inventoryItemAvailability,
    _In_opt_ Platform::String^ inventoryItemContainerId,
    _In_ uint32 maxItems,
    _In_ bool expandSatisfyingEntitlements
    )
{
    auto task = m_cppObj.get_inventory_items(
        static_cast<media_item_type>(mediaItemType),
        static_cast<inventory_item_state>(inventoryItemState),
        static_cast<inventory_item_availability>(inventoryItemAvailability),
        STRING_T_FROM_PLATFORM_STRING(inventoryItemContainerId),
        maxItems,
        expandSatisfyingEntitlements
        )
    .then([](xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<InventoryItemsResult^>^
InventoryService::GetInventoryItemsForAllUsersAsync(
    _In_ MediaItemType mediaItemType
    )
{
    auto task = m_cppObj.get_inventory_items_for_all_users(
        static_cast<media_item_type>(mediaItemType)
        )
    .then([](xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^
InventoryService::GetInventoryItemsForAllUsersAsync(
    _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType mediaItemType,
    _In_ bool expandSatisfyingEntitlements,
    _In_ bool includeAllItemStatesAndAvailabilities
    )
{
    auto task = m_cppObj.get_inventory_items_for_all_users(
        static_cast<media_item_type>(mediaItemType),
        expandSatisfyingEntitlements,
        includeAllItemStatesAndAvailabilities
        )
    .then([](xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^
InventoryService::GetInventoryItemsForAllUsersAsync(
    _In_ Windows::Foundation::Collections::IVectorView<Platform::String^>^ productIds,
    _In_ bool expandSatisfyingEntitlements
    )
{
    auto task = m_cppObj.get_inventory_items_for_all_users(
        UtilsWinRT::CovertVectorViewToStdVectorString(productIds),
        expandSatisfyingEntitlements
        )
    .then([](xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<InventoryItemsResult^>^
InventoryService::GetInventoryItemsForAllUsersAsync(
    _In_ MediaItemType mediaItemType,
    _In_ InventoryItemState inventoryItemState,
    _In_ InventoryItemAvailability inventoryItemAvailability,
    _In_opt_ Platform::String^ inventoryItemContainerId,
    _In_ uint32 maxItems
    )
{
    auto task = m_cppObj.get_inventory_items_for_all_users(
        static_cast<media_item_type>(mediaItemType),
        static_cast<inventory_item_state>(inventoryItemState),
        static_cast<inventory_item_availability>(inventoryItemAvailability),
        STRING_T_FROM_PLATFORM_STRING(inventoryItemContainerId),
        maxItems
        )
    .then([](xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

Windows::Foundation::IAsyncOperation<InventoryItemsResult^>^
InventoryService::GetInventoryItemsForAllUsersAsync(
    _In_ Microsoft::Xbox::Services::Marketplace::MediaItemType mediaItemType,
    _In_ Microsoft::Xbox::Services::Marketplace::InventoryItemState inventoryItemState,
    _In_ Microsoft::Xbox::Services::Marketplace::InventoryItemAvailability inventoryItemAvailability,
    _In_opt_ Platform::String^ inventoryItemContainerId,
    _In_ uint32 maxItems,
    _In_ bool expandSatisfyingEntitlements
    )
{
    auto task = m_cppObj.get_inventory_items_for_all_users(
        static_cast<media_item_type>(mediaItemType),
        static_cast<inventory_item_state>(inventoryItemState),
        static_cast<inventory_item_availability>(inventoryItemAvailability),
        STRING_T_FROM_PLATFORM_STRING(inventoryItemContainerId),
        maxItems,
        expandSatisfyingEntitlements
        )
    .then([](xbox_live_result<inventory_items_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItemsResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<InventoryItem^>^
InventoryService::GetInventoryItemAsync(
    _In_ InventoryItem^ inventoryItem
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(inventoryItem);

    auto task = m_cppObj.get_inventory_item(
        inventoryItem->GetCppObj()
        )
    .then([](xbox_live_result<inventory_item> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new InventoryItem(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

IAsyncOperation<ConsumeInventoryItemResult^>^
InventoryService::ConsumeInventoryItemAsync(
    _In_ InventoryItem^ inventoryItem,
    _In_ uint32 quantityToConsume,
    _In_ Platform::String^ transactionId
    )
{
    THROW_INVALIDARGUMENT_IF_NULL(inventoryItem);

    auto task = m_cppObj.consume_inventory_item(
        inventoryItem->GetCppObj(),
        quantityToConsume,
        STRING_T_FROM_PLATFORM_STRING(transactionId)
        )
    .then([](xbox_live_result<consume_inventory_item_result> cppResult)
    {
        THROW_IF_ERR(cppResult);
        return ref new ConsumeInventoryItemResult(cppResult.payload());
    });

    return ASYNC_FROM_TASK(task);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
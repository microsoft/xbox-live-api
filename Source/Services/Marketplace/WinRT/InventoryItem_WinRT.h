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
#include "InventoryItemState_WinRT.h"
#include "MediaItemType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

public ref class InventoryItem sealed
{
public:
    /// <summary>
    /// The inventory item Url
    /// </summary>
    DEFINE_PROP_GET_URI_OBJ(Url, url);

    /// <summary>
    /// The state of the inventory item.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(InventoryItemState, item_state, Microsoft::Xbox::Services::Marketplace::InventoryItemState);

    /// <summary>
    /// The media item type of the inventory item.
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(MediaItemType, item_type, Microsoft::Xbox::Services::Marketplace::MediaItemType);

    /// <summary>
    /// The unique identifier of the product that this item represents.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ProductId, product_id);

    /// <summary>
    /// The title Id that a durable item is associated with or null for consumable and subscription items.
    /// </summary>
    DEFINE_PROP_GET_OBJ(TitleId, title_id, uint32);

    /// <summary>
    /// The list of container Ids that contain this item
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ ContainerIds 
    { 
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); 
    }

    /// <summary>
    /// The date when the rights to the item were obtained.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(RightsObtainedDate, rights_obtained_date);

    /// <summary>
    /// The start date when the item became available for use.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(StartDate, start_date);

    /// <summary>
    /// The end date when the item became or will become unusable.
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(EndDate, end_date);

    /// <summary>
    /// The consumable item Url or null for non-consumable inventory items.
    /// </summary>
    DEFINE_PROP_GET_URI_OBJ(ConsumableUrl, consumable_url);

    /// <summary>
    /// The balance of the consumable inventory item at the time the request was received. For non-consumable items, the quantity will be 0.
    /// </summary>
    DEFINE_PROP_GET_OBJ(ConsumableBalance, consumable_balance, uint32);

    /// <summary>
    /// True if this entitlement is a trial; otherwise, false. If you buy the trial version of an entitlement and then buy the full version, you will receive both
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsTrialEntitlement, is_trial_entitlement, bool);

    /// <summary>
    /// How much time is remaining on the trial.
    /// </summary>
    DEFINE_PROP_GET_TIMESPAN_OBJ(TrialTimeRemaining, trial_time_remaining);

internal:
    InventoryItem(
        _In_ xbox::services::marketplace::inventory_item cppObj
        );
    xbox::services::marketplace::inventory_item GetCppObj() const;

private:
    xbox::services::marketplace::inventory_item m_cppObj;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_containerIds;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END

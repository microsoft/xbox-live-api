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
#include "CatalogItemImage_WinRT.h"
#include "MediaItemType_WinRT.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

public ref class CatalogItem sealed
{
    //{
    //   "desiredMediaItemTypes":"Subscription.DGame.DGameDemo.DDurable.DConsumable.DApp",
    //   "MediaItemType":"DConsumable",
    //   "ID":"44c99601-7db8-4ad6-9610-218bbff99daf",
    //   "Name":"Gold Coins",
    //   "ReleaseDate":"2013-05-15T00:00:00Z",
    //   "TitleId":"521617E4",
    //   "VuiDisplayName":"Gold Coins V",
    // 
    //   "Genres":
    //   [
    //      {
    //         "Name":"Educational"
    //      }
    //   ],
    // 
    //   "Images":
    //   [
    //      {
    //         "ID":"b3e560ae-dd9c-4ac5-b41d-5ddee19a7944",
    //         "ResizeUrl":"http://images.xboxlive.com/image?url=etc",
    //         "Purposes":
    //         [
    //            "Box_Art"
    //         ],
    //         "Purpose":"Box_Art",
    //         "Height":1080,
    //         "Width":1080
    //      }
    //   ],
    //
    //   "SortName":"Gold s",
    //   "KValue":"4",
    //   "KValueNamespace":"bingbox",
    //   "LegacyIds":
    //   [
    //      {
    //         "IdType":"ProductId",
    //         "Value":"b3e560ae-dd9c-4ac5-b41d-5ddee19a7944"
    //      }
    //   ],
    //
    //   "Availabilities":
    //   [
    //      {
    //         "ContentId":"562acf0f-2021-42d8-b158-f3493214116d",
    //      }
    //   ],
    //   "SandboxId":"DECO.0",
    //   "IsBundle": true,
    //   "IsPartOfAnyBundle": true
    //}

public:
    /// <summary>
    /// The media type of the catalog item
    /// </summary>
    DEFINE_PROP_GET_ENUM_OBJ(MediaItemType, item_type, Microsoft::Xbox::Services::Marketplace::MediaItemType);

    /// <summary>
    /// The catalog ID of the catalog item
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Id, id);

    /// <summary>
    /// The name of the catalog item
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(Name, name);

    /// <summary>
    /// The name of the catalog item
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ReducedName, reduced_name);

    /// <summary>
    /// The title ID of the catalog item
    /// </summary>
    DEFINE_PROP_GET_OBJ(TitleId, title_id, uint32);

    /// <summary>
    /// The release date of the catalog item
    /// </summary>
    DEFINE_PROP_GET_DATETIME_OBJ(ReleaseDate, release_date);

    /// <summary>
    /// The product ID of the catalog item
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ProductId, product_id);

    /// <summary>
    /// The sandbox ID of the catalog item
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SandboxId, sandbox_id);

    /// <summary>
    /// Is this product a Bundle and includes entitlements to other products
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsBundle, is_bundle, bool);

    /// <summary>
    /// Is this product a member of a bundle in the Marketplace
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsPartOfAnyBundle, is_part_of_any_bundle, bool);

    /// <summary>
    /// A collection of images for this catalog item
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<CatalogItemImage^>^ Images 
    { 
        Windows::Foundation::Collections::IVectorView<CatalogItemImage^>^ get(); 
    }

    /// <summary>
    /// A collection of availability content IDs for this catalog item
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ AvailabilityContentIds
    { 
        Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); 
    }

internal:
    CatalogItem(
        _In_ xbox::services::marketplace::catalog_item cppObj
        );

private:
    xbox::services::marketplace::catalog_item m_cppObj;
    Windows::Foundation::Collections::IVectorView<CatalogItemImage^>^ m_images;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_availabilityContentIds;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
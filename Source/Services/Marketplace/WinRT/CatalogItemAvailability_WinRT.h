// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/marketplace.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_BEGIN

public ref class CatalogItemAvailability sealed
{
    //      {
    //         "ContentId":"562acf0f-2021-42d8-b158-f3493214116d",
    //         "OfferDisplayData":
    //         {
    //            "acceptablePaymentInstrumentTypes":
    //            [
    //               "CreditCard",
    //               "Tokens"
    //            ],
    //            "availabilityDescription":"AvailabilityDescription for 3cba007c-50db-4332-89c1-3ebc116fa27b",
    //            "currencyCode":"USD",
    //            "displayPrice":"$0.01",
    //            "displayListPrice":"$5.00",
    //            "distributionType":"Full",
    //            "isPurchasable":true,
    //            "listPrice":5.0,
    //            "price":0.01,
    //            "promotionalText":"Need Gold subscription. Click to buy"
    //         },
    //         "SignedOffer":"etc"
    //      }

public:
    /// <summary>
    /// The content ID 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(ContentId, content_id);

    /// <summary>
    /// A collection of acceptable payment instrument types
    /// </summary>
    property Windows::Foundation::Collections::IVectorView<Platform::String^>^ AcceptablePaymentInstrumentTypes { Windows::Foundation::Collections::IVectorView<Platform::String^>^ get(); }

    /// <summary>
    /// The title of the availability
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(AvailabilityTitle, availability_title);

    /// <summary>
    /// The description of the availability
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(AvailabilityDescription, availability_description);

    /// <summary>
    /// The currency code 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(CurrencyCode, currency_code);

    /// <summary>
    /// The display price 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DisplayPrice, display_price);

    /// <summary>
    /// The display list price 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DisplayListPrice, display_list_price);

    /// <summary>
    /// The distribution type 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(DistributionType, distribution_type);

    /// <summary>
    /// Indicates if the availability is purchasable 
    /// </summary>
    DEFINE_PROP_GET_OBJ(IsPurchasable, is_purchasable, bool);

    /// <summary>
    /// The list price in numeric form
    /// </summary>
    DEFINE_PROP_GET_OBJ(ListPrice, list_price, double);

    /// <summary>
    /// The price in numeric form
    /// </summary>
    DEFINE_PROP_GET_OBJ(Price, price, double);

    /// <summary>
    /// The quantity of the catalog item
    /// </summary>
    DEFINE_PROP_GET_OBJ(ConsumableQuantity, consumable_quantity, uint32);

    /// <summary>
    /// The distribution type 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(PromotionalText, promotional_text);

    /// <summary>
    /// The signed offer 
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(SignedOffer, signed_offer);

    /// <summary>
    /// The offer ID
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(OfferId, offer_id);

    /// <summary>
    /// Json string that contains the offer display data.
    /// </summary>
    DEFINE_PROP_GET_STR_OBJ(OfferDisplayDataJson, offer_display_data_json);

internal:
    CatalogItemAvailability(
        _In_ xbox::services::marketplace::catalog_item_availability cppObj
        );

private:
    xbox::services::marketplace::catalog_item_availability m_cppObj;
    Windows::Foundation::Collections::IVectorView<Platform::String^>^ m_acceptablePaymentInstrumentTypes;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_MARKETPLACE_END
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#if TV_API || UNIT_TEST_SERVICES

namespace xbox {
namespace services {
/// <summary>
/// Contains classes and enumerations used to represent products and itemized product catalogs available for purchase.
/// </summary>
namespace marketplace {

/// <summary>Enumeration values that indicate the media item type.</summary>
enum class media_item_type
{
    /// <summary>Unknown media type.</summary>
    unknown,

    /// <summary>Game media type.</summary>
    game,

    /// <summary>GameV2 media type.</summary>
    game_v2,

    /// <summary>Application media type.</summary>
    application,

    /// <summary>Game content media type.</summary>
    game_content,

    /// <summary>Game consumable media type.</summary>
    game_consumable,

    /// <summary>Subscription media type.</summary>
    subscription,

    /// <summary>Movie media type.</summary>
    movie,

    /// <summary>Television Show media type.</summary>
    television_show,

    /// <summary>Television Series media type.</summary>
    television_series,

    /// <summary>Television Season media type.</summary>
    television_season,

    /// <summary>Service detects if media type is an application or a game.</summary>
    application_or_game_unknown,

    /// <summary>Game demo media type.</summary>
    game_demo,

    /// <summary>WebVideo media type.</summary>
    web_video,

    /// <summary>Request All media types for inventory calls.</summary>
    all
};

/// <summary> Defines values used to indicate the order of sorted catalog results. </summary>
enum class catalog_sort_order
{
    /// <summary>
    /// Sort by count of free and paid purchases, for the most recent day.
    /// </summary>
    free_and_paid_count_daily,

    /// <summary>
    /// Sort by count of only paid purchases, for all time.
    /// </summary>
    paid_count_all_time,

    /// <summary>
    /// Sort by count of only paid purchases, for the most recent day.
    /// </summary>
    paid_count_daily,

    /// <summary>
    /// Sort by date available for download.
    /// </summary>
    digital_release_date,

    /// <summary>
    /// Sort by date available in stores, falling back to digital release date (if available).
    /// </summary>
    release_date,

    /// <summary>
    /// Sort by average user ratings.
    /// </summary>
    user_ratings
};

/// <summary>Enumeration values that indicate the media item type.</summary>
enum class bundle_relationship_type
{
    /// <summary>Unknown media type.</summary>
    unknown,

    /// <summary>Find which bundles include this product.</summary>
    bundles_with_product,

    /// <summary>Find which products are included in a specified bundle.</summary>
    products_in_bundle
};

/// <summary>Enumeration values that indicate the inventory item state.</summary>
enum class inventory_item_state
{
    /// <summary>Unknown item state.</summary>
    unknown,

    /// <summary>All is used to request inventory items for all states.</summary>
    all,

    /// <summary>Enabled item state.</summary>
    enabled,

    /// <summary>Suspended item state.</summary>
    suspended,

    /// <summary>Expired item state.</summary>
    expired,

    /// <summary>Canceled item state.  This state pertains to subscriptions that have been canceled.</summary>
    canceled
};

/// <summary>Enumeration values that indicate the current availability inventory items.</summary>
enum class inventory_item_availability
{
    /// <summary>
    /// All items should be returned regardless of availability.
    /// </summary>
    all,

    /// <summary>
    /// Item is available.  The current date falls between the start and end date of the item.
    /// </summary>
    available,

    /// <summary>
    /// Item is unavailable.  The current date falls outside the start and end date of the item.
    /// </summary>
    unavailable
};

/// <summary> Represents an image associated with a catalog item.</summary>
class catalog_item_image
{
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

public:
    /// <summary>
    /// Internal function
    /// </summary>
    catalog_item_image();

    /// <summary>
    /// Internal function
    /// </summary>
    catalog_item_image(
        _In_ string_t id,
        _In_ web::uri resizeUrl,
        _In_ std::vector<string_t> purposes,
        _In_ string_t purpose,
        _In_ uint32_t height,
        _In_ uint32_t width
        );

    /// <summary>
    /// Returns the URI of the catalog item image.
    /// </summary>
    _XSAPIIMP const string_t& id() const;

    /// <summary>
    /// Returns the URI of the catalog item image.
    /// </summary>
    _XSAPIIMP const web::uri& resize_url() const;

    /// <summary>
    /// A collection of purposes. 
    /// </summary>
    _XSAPIIMP const std::vector<string_t>& purposes() const;

    /// <summary>
    // The default purpose of the catalog item image.
    /// </summary>
    _XSAPIIMP const string_t& purpose() const;

    /// <summary>
    // The height of the image.
    /// </summary>
    _XSAPIIMP uint32_t height() const;

    /// <summary>
    // The width of the image.
    /// </summary>
    _XSAPIIMP uint32_t width() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<catalog_item_image> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_id;
    web::uri m_resizeUrl;
    std::vector<string_t> m_purposes;
    string_t m_purpose;
    uint32_t m_height;
    uint32_t m_width;
};

/// <summary> Represents a catalog item.</summary>
class catalog_item
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
    catalog_item();

    catalog_item(
        _In_ string_t id,
        _In_ string_t name,
        _In_ string_t reducedName,
        _In_ uint32_t titleId,
        _In_ utility::datetime releaseDate,
        _In_ string_t productId,
        _In_ string_t sandboxId,
        _In_ bool isBundle,
        _In_ bool isPartOfAnyBundle,
        _In_ std::vector<catalog_item_image> images,
        _In_ std::vector<string_t> availabilityContentIds,
        _In_ media_item_type mediaItemType
        );

    /// <summary>
    /// The media type of the catalog item.
    /// </summary>
    _XSAPIIMP media_item_type item_type() const;

    /// <summary>
    /// The catalog ID of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& id() const;

    /// <summary>
    /// The name of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& name() const;

    /// <summary>
    /// The name of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& reduced_name() const;

    /// <summary>
    /// The title ID of the catalog item.
    /// </summary>
    _XSAPIIMP uint32_t title_id() const;

    /// <summary>
    /// The release date of the catalog item.
    /// </summary>
    _XSAPIIMP const utility::datetime& release_date() const;

    /// <summary>
    /// The product ID of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& product_id() const;

    /// <summary>
    /// The sandbox ID of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& sandbox_id() const;

    /// <summary>
    /// Is this product a Bundle and includes entitlements to other products.
    /// </summary>
    _XSAPIIMP bool is_bundle() const;

    /// <summary>
    /// Is this product a member of a bundle in the Marketplace.
    /// </summary>
    _XSAPIIMP bool is_part_of_any_bundle() const;

    /// <summary>
    /// A collection of images for this catalog item.
    /// </summary>
    _XSAPIIMP const std::vector<catalog_item_image>& images() const;

    /// <summary>
    /// A collection of availability content IDs for this catalog item.
    /// </summary>
    _XSAPIIMP const std::vector<string_t>& availability_content_ids() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<catalog_item> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_id;
    string_t m_name;
    string_t m_reducedName;
    uint32_t m_titleId;
    utility::datetime m_releaseDate;
    string_t m_productId;
    string_t m_sandboxId;
    bool m_isBundle;
    bool m_isPartOfAnyBundle;
    std::vector<catalog_item_image> m_images;
    std::vector<string_t> m_availabilityContentIds;
    media_item_type m_mediaItemType;
};

/// <summary>Contains the results of a browse catalog request.</summary>
class browse_catalog_result
{
public:
    //    "Items":
    //    [
    //      {  
    //        // ...  
    //      }
    //    ],
    //
    //    "Totals":
    //    [
    //      {
    //         "Name":"GameType",
    //         "Count":126,
    //         "MediaItemTypes":
    //         [
    //            {
    //               "Name":"XboxGameConsumable",
    //               "Count":126
    //            }
    //         ]
    //      }
    //    ],

    /// <summary>
    /// Internal function
    /// </summary>
    browse_catalog_result();

    /// <summary>
    /// Internal function
    /// </summary>
    browse_catalog_result(
        _In_ string_t parentId,
        _In_ media_item_type parentMediaItemType,
        _In_ media_item_type childMediaItemType,
        _In_ catalog_sort_order orderBy,
        _In_ uint32_t skipItems,
        _In_ string_t productId,
        _In_ bundle_relationship_type bundleRelationship,
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
        _In_ std::vector<catalog_item> items,
        _In_ uint32_t totalCount,
        _In_ string_t continuationToken,
        _In_ bool m_isBundleRelated
        );

    /// <summary>
    /// Collection of XboxCatalogItem objects returned by a request.
    /// </summary>
    _XSAPIIMP const std::vector<catalog_item>& items() const;

    /// <summary>
    // The total count of the items.
    /// </summary>
    _XSAPIIMP uint32_t total_count() const;

    /// <summary>
    /// Returns an BrowseCatalogResult object containing the next page of BrowseCatalogResult.
    /// </summary>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt retrieving all items.</param>
    /// <returns>A BrowseCatalogResult object.</returns>
    /// <remarks>Calls V3.2 GET /media/{marketplaceId}/browse</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<browse_catalog_result>> get_next(
        _In_ uint32_t maxItems
        );

    /// <summary>
    /// Indicates if there is additional data to retrieve from a get_next call
    /// </summary>
    _XSAPIIMP bool has_next() const;

    /// <summary>
    /// Internal function
    /// </summary>
    void _Initialize(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ string_t parentId,
        _In_ media_item_type parentMediaType,
        _In_ media_item_type childItemType,
        _In_ catalog_sort_order orderBy,
        _In_ uint32_t skipItems
        );

    /// <summary>
    /// Internal function
    /// </summary>
    void _Set_browse_bundles(
        _In_ string_t productId,
        _In_ bundle_relationship_type relationship
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<browse_catalog_result> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_parentId;
    media_item_type m_parentMediaItemType;
    media_item_type m_childMediaItemType;
    catalog_sort_order m_orderBy;
    uint32_t m_skipItems;
    string_t m_productId;
    bundle_relationship_type m_bundleRelationship;
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
    std::vector<catalog_item> m_items;
    uint32_t m_totalCount;
    string_t m_continuationToken;
    bool m_isBundleRelated;
};

/// <summary> Indicates availability of content by date, payment methods, currency, and distribution options. </summary>
class catalog_item_availability
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
    /// Internal function
    /// </summary>
    catalog_item_availability();

    /// <summary>
    /// Internal function
    /// </summary>
    catalog_item_availability(
        _In_ string_t contentId,
        _In_ string_t signedOffer
        );

    /// <summary>
    /// Internal function
    /// </summary>
    catalog_item_availability(
        _In_ string_t contentId,
        _In_ std::vector<string_t> acceptablePaymentInstrumentTypes,
        _In_ string_t availabilityTitle,
        _In_ string_t availabilityDescription,
        _In_ string_t currencyCode,
        _In_ string_t displayPrice,
        _In_ string_t displayListPrice,
        _In_ string_t distributionType,
        _In_ bool isPurchasable,
        _In_ double listPrice,
        _In_ double price,
        _In_ uint32_t consumableQuantity,
        _In_ string_t promotionalText,
        _In_ string_t signedOffer,
        _In_ string_t offerId,
        _In_ string_t offerDisplayDataJson
        );

    /// <summary>
    /// The content ID. 
    /// </summary>
    _XSAPIIMP const string_t& content_id() const;

    /// <summary>
    /// A collection of acceptable payment instrument types.
    /// </summary>
    _XSAPIIMP const std::vector<string_t>& acceptable_payment_Instrument_types() const;

    /// <summary>
    /// The title of the availability.
    /// </summary>
    _XSAPIIMP const string_t& availability_title() const;

    /// <summary>
    /// The description of the availability.
    /// </summary>
    _XSAPIIMP const string_t& availability_description() const;

    /// <summary>
    /// The currency code. 
    /// </summary>
    _XSAPIIMP const string_t& currency_code() const;

    /// <summary>
    /// The display price 
    /// </summary>
    _XSAPIIMP const string_t& display_price() const;

    /// <summary>
    /// The display list price.
    /// </summary>
    _XSAPIIMP const string_t& display_list_price() const;

    /// <summary>
    /// The distribution type. 
    /// </summary>
    _XSAPIIMP const string_t& distribution_type() const;

    /// <summary>
    /// Indicates if the availability is purchasable. 
    /// </summary>
    _XSAPIIMP bool is_purchasable() const;

    /// <summary>
    /// The list price in numeric form.
    /// </summary>
    _XSAPIIMP double list_price() const;

    /// <summary>
    /// The price in numeric form.
    /// </summary>
    _XSAPIIMP double price() const;

    /// <summary>
    /// The quantity of the catalog item.
    /// </summary>
    _XSAPIIMP uint32_t consumable_quantity() const;

    /// <summary>
    /// The distribution type. 
    /// </summary>
    _XSAPIIMP const string_t& promotional_text() const;

    /// <summary>
    /// The signed offer.
    /// </summary>
    _XSAPIIMP const string_t& signed_offer() const;

    /// <summary>
    /// The offer ID.
    /// </summary>
    _XSAPIIMP const string_t& offer_id() const;

    /// <summary>
    /// Json string that contains the offer display data.
    /// </summary>
    _XSAPIIMP const string_t& offer_display_data_json() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<catalog_item_availability> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_contentId;
    std::vector<string_t> m_acceptablePaymentInstrumentTypes;
    string_t m_availabilityTitle;
    string_t m_availabilityDescription;
    string_t m_currencyCode;
    string_t m_displayPrice;
    string_t m_displayListPrice;
    string_t m_distributionType;
    bool m_isPurchasable;
    double m_listPrice;
    double m_price;
    uint32_t m_consumableQuantity;
    string_t m_promotionalText;
    string_t m_signedOffer;
    string_t m_offerId;
    string_t m_offerDisplayDataJson;
};

/// <summary> Contains information about a catalog item. </summary>
class catalog_item_details
{
    //{
    //   "desiredMediaItemTypes":"Subscription.DGame.DGameDemo.DDurable.DConsumable.DApp",
    //   "MediaItemType":"DConsumable",
    //   "ID":"44c99601-7db8-4ad6-9610-218bbff99daf",
    //   "Name":"Gold Coins",
    //   "Description":"Consumable in-game currency",
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
    //         "SignedOffer":"Q"
    //      }
    //   ],
    //   "SandboxId":"DECO.0"
    //   "IsBundle": true,
    //   "IsPartOfAnyBundle": true
    //}

public:
    /// <summary>
    /// Internal function
    /// </summary>
    catalog_item_details();

    /// <summary>
    /// Internal function
    /// </summary>
    catalog_item_details(
        _In_ string_t id,
        _In_ string_t m_name,
        _In_ string_t reducedName,
        _In_ string_t description,
        _In_ uint32_t titleId,
        _In_ utility::datetime releaseDate,
        _In_ string_t productId,
        _In_ string_t sandboxId,
        _In_ bool isBundle,
        _In_ bool isPartOfAnyBundle,
        _In_ std::vector<catalog_item_image> images,
        _In_ std::vector<catalog_item_availability> availabilities,
        _In_ media_item_type mediaItemType
        );

    /// <summary>
    /// The media item type of the catalog item.
    /// </summary>
    _XSAPIIMP const media_item_type item_type() const;

    /// <summary>
    /// The catalog ID of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& id() const;

    /// <summary>
    /// The name of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& name() const;

    /// <summary>
    /// The reduced name of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& reduced_name() const;

    /// <summary>
    /// The description of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& description() const;

    /// <summary>
    /// The title ID of the catalog item.
    /// </summary>
    _XSAPIIMP uint32_t title_id() const;

    /// <summary>
    /// The release date of the catalog item.
    /// </summary>
    _XSAPIIMP const utility::datetime& release_date() const;

    /// <summary>
    /// The product ID of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& product_id() const;

    /// <summary>
    /// The sandbox ID of the catalog item.
    /// </summary>
    _XSAPIIMP const string_t& sandbox_id() const;

    /// <summary>
    /// Is this product a Bundle and includes entitlements to other products.
    /// </summary>
    _XSAPIIMP bool is_bundle() const;

    /// <summary>
    /// Is this product a member of a bundle in the Marketplace.
    /// </summary>
    _XSAPIIMP bool is_part_of_any_bundle() const;

    /// <summary>
    /// A collection of images for this catalog item.
    /// </summary>
    _XSAPIIMP const std::vector<catalog_item_image>& images() const;

    /// <summary>
    /// A collection of images for this catalog item.
    /// </summary>
    _XSAPIIMP const std::vector<catalog_item_availability>& availabilities() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<catalog_item_details> _Deserialize(_In_ const web::json::value& json);

private:
    string_t m_id;
    string_t m_name;
    string_t m_reducedName;
    string_t m_description;
    uint32_t m_titleId;
    utility::datetime m_releaseDate;
    string_t m_productId;
    string_t m_sandboxId;
    bool m_isBundle;
    bool m_isPartOfAnyBundle;
    std::vector<catalog_item_image> m_images;
    std::vector<catalog_item_availability> m_availabilities;
    media_item_type m_mediaItemType;
};

/// <summary> Indicates the total number of items within a catalog </summary>
class catalog_total
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    catalog_total();

    /// <summary>
    /// Internal function
    /// </summary>
    catalog_total(
        _In_ uint32_t total
        );

    /// <summary>
    /// Total number of items
    /// </summary>
    _XSAPIIMP uint32_t total() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<catalog_total> _Deserialize(_In_ const web::json::value& json);

private:
    uint32_t m_total;
};

/// <summary> Represents a service for managing the catalog. </summary>
class catalog_service
{
public:
    /// <summary>
    /// Browse for catalog items from within a single media group.
    /// </summary>
    /// <param name="parentId">The product ID of the parent product.</param>
    /// <param name="parentMediaType">The media type of the parent.</param>
    /// <param name="childMediaType">The media type of the child.</param>
    /// <param name="orderby">Controls how the list is ordered.</param>
    /// <param name="skipItems">The number of items to skip.</param>
    /// <param name="maxItems">The maximum number of items the result can contain. Pass 0 to attempt retrieve as many items as possible.</param>
    /// <returns>BrowseCatalogResult object containing the items.</returns>
    /// <remarks>Calls V3.2 GET /media/{marketplaceId}/browse</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<browse_catalog_result>> browse_catalog(
        _In_ const string_t& parentId,
        _In_ media_item_type parentMediaType,
        _In_ media_item_type childMediaType,
        _In_ catalog_sort_order orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems
        );

    /// <summary>
    /// Browse the catalog related to bundles an item is a member of or included items of a bundle.
    /// </summary>
    /// <param name="parentId">The product ID of the parent product.</param>
    /// <param name="parentMediaType">The media type of the parent.</param>
    /// <param name="productId">The ID of the product we are browsing the relationship for.</param>
    /// <param name="relationship">The relationship to be used in the browse call either included products in a bundle, or bundles the product is a member of.</param>
    /// <param name="skipItems">The number of items to skip.</param>
    /// <param name="maxItems">The maximum number of items the result can contain. Pass 0 to attempt retrieve as many items as possible.</param>
    /// <returns>BrowseCatalogResult object containing the items</returns>
    /// <remarks>Calls V3.2 GET /media/{marketplaceId}/browse</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<browse_catalog_result>> browse_catalog_bundles(
        _In_ const string_t& parentId,
        _In_ media_item_type parentMediaType,
        _In_ const string_t& productId,
        _In_ bundle_relationship_type relationship,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems
        );

    /// <summary>
    /// Get the product details for a set of ProductIDs.
    /// </summary>
    /// <param name="productIds">A collection of product IDs to get item details for. A maximum of ten strings may be passed per call to this method.</param>
    /// <returns>A collection of CatalogItems.</returns>
    /// <remarks>Calls V3.2 GET /media/{marketplaceId}/details</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<std::vector<catalog_item_details>>> get_catalog_item_details(
        _In_ const std::vector<string_t>& productIds
        );

    static media_item_type _Convert_string_to_media_item_type(
        _In_ const string_t& itemType
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static string_t _Convert_media_item_type_to_string(
        _In_ media_item_type itemType
        );
private:
    catalog_service();

    catalog_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    static string_t convert_bundle_relationship_type_to_string(
        _In_ bundle_relationship_type type
        );

    static xbox_live_result<string_t> convert_sort_order_to_string(
        _In_ catalog_sort_order type
        );

    static string_t marketplace_browse_catalog_subpath(
        _In_ const string_t& parentId,
        _In_ const string_t& titleType,
        _In_ const string_t& desiredMediaItemTypes,
        _In_ const string_t& relationship,
        _In_ const string_t& orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems
        );

    static pplx::task<xbox_live_result<browse_catalog_result>> browse_catalog_helper(
        _In_ const string_t& parentId,
        _In_ media_item_type parentMediaType,
        _In_ media_item_type childMediaType,
        _In_ catalog_sort_order orderBy,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ string_t customHeader,
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    static pplx::task<xbox_live_result<browse_catalog_result>> browse_catalog_bundles_helper(
        _In_ const string_t& parentId,
        _In_ media_item_type parentMediaType,
        _In_ const string_t& productId,
        _In_ bundle_relationship_type relationship,
        _In_ uint32_t skipItems,
        _In_ uint32_t maxItems,
        _In_ string_t customHeader,
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    static string_t marketplace_catalog_details_subpath(
        _In_ const std::vector<string_t>& productIds
        );

    static const string_t BROWSE_CATALOG_CONTRACT_HEADER_VALUE;
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    friend class xbox_live_context_impl;
    friend class browse_catalog_result;
};

// Helper class for serialization
class consume_inventory_item_request
{
public:
    consume_inventory_item_request();

    consume_inventory_item_request(
        _In_ string_t transactionId,
        _In_ uint32_t removeQuantity
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<consume_inventory_item_request> _Deserialize(_In_ const web::json::value& json);

    web::json::value _Serialize();
private:
    string_t m_transactionId;
    uint32_t m_removeQuantity;
};

/// <summary>The results of a consume inventory item request.</summary>
class consume_inventory_item_result
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    consume_inventory_item_result();

    /// <summary>
    /// Internal function
    /// </summary>
    consume_inventory_item_result(
        _In_ web::uri consumableUrl,
        _In_ uint32_t consumableBalance,
        _In_ string_t transactionId
        );

    /// <summary>
    /// The consumable item Url of the item that was consumed.
    /// </summary>
    _XSAPIIMP const web::uri& consumable_url() const;

    /// <summary>
    /// The new balance for the consumable item.
    /// </summary>
    _XSAPIIMP uint32_t consumable_balance() const;

    /// <summary>
    /// The transaction Id that serves to acknowledge receipt of a specific request.
    /// </summary>
    _XSAPIIMP const string_t& transaction_id() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<consume_inventory_item_result> _Deserialize(_In_ const web::json::value& json);

private:
    web::uri m_consumableUrl;
    uint32_t m_consumableBalance;
    string_t m_transactionId;
};

/// <summary>Represents an inventory item.</summary>
class inventory_item
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    inventory_item();

    /// <summary>
    /// Internal function
    /// </summary>
    inventory_item(
        _In_ web::uri url,
        _In_ inventory_item_state inventoryItemState,
        _In_ media_item_type mediaItemType,
        _In_ string_t productId,
        _In_ uint32_t titleId,
        _In_ std::vector<string_t> containerIds,
        _In_ utility::datetime rightsObtainedDate,
        _In_ utility::datetime startDate,
        _In_ utility::datetime endDate,
        _In_ web::uri consumableUrl,
        _In_ uint32_t consumableBalance,
        _In_ bool isTrialEntitlement,
        _In_ std::chrono::seconds trialTimeRemaining
        );

    /// <summary>
    /// The inventory item Url.
    /// </summary>
    _XSAPIIMP const web::uri& url() const;

    /// <summary>
    /// The state of the inventory item.
    /// </summary>
    _XSAPIIMP inventory_item_state item_state() const;

    /// <summary>
    /// The media item type of the inventory item.
    /// </summary>
    _XSAPIIMP media_item_type item_type() const;

    /// <summary>
    /// The unique identifier of the product that this item represents.
    /// </summary>
    _XSAPIIMP const string_t& product_id() const;

    /// <summary>
    /// The title Id that a durable item is associated with or null for consumable and subscription items.
    /// </summary>
    _XSAPIIMP uint32_t title_id() const;

    /// <summary>
    /// The list of container Ids that contain this item
    /// </summary>
    _XSAPIIMP const std::vector<string_t>& container_ids() const;

    /// <summary>
    /// The date when the rights to the item were obtained.
    /// </summary>
    _XSAPIIMP const utility::datetime& rights_obtained_date() const;

    /// <summary>
    /// The start date when the item became available for use.
    /// </summary>
    _XSAPIIMP const utility::datetime& start_date() const;

    /// <summary>
    /// The end date when the item became or will become unusable.
    /// </summary>
    _XSAPIIMP const utility::datetime& end_date() const;

    /// <summary>
    /// The consumable item Url or null for non-consumable inventory items.
    /// </summary>
    _XSAPIIMP const web::uri& consumable_url() const;

    /// <summary>
    /// The balance of the consumable inventory item at the time the request was received. For non-consumable items, the quantity will be 0.
    /// </summary>
    _XSAPIIMP uint32_t consumable_balance() const;

    /// <summary>
    /// True if this entitlement is a trial; otherwise, false. If you buy the trial version of an entitlement and then buy the full version, you will receive both
    /// </summary>
    _XSAPIIMP bool is_trial_entitlement() const;

    /// <summary>
    /// How much time is remaining on the trial.
    /// </summary>
    _XSAPIIMP const std::chrono::seconds& trial_time_remaining() const;

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<inventory_item> _Deserialize(_In_ const web::json::value& json);

private:

    static inventory_item_state convert_string_to_inventory_item_state(
        _In_ const string_t& value
        );

    static media_item_type convert_string_to_media_item_type(
        _In_ const string_t& value
        );

    web::uri m_url;
    inventory_item_state m_inventoryItemState;
    media_item_type m_itemType;
    string_t m_productId;
    uint32_t m_titleId;
    std::vector<string_t> m_containerIds;
    utility::datetime m_rightsObtainedDate;
    utility::datetime m_startDate;
    utility::datetime m_endDate;
    web::uri m_consumableUrl;
    uint32_t m_consumableBalance;
    bool m_isTrialEntitlement;
    std::chrono::seconds m_trialTimeRemaining;
};

/// <summary>The results of an inventory item request.</summary>
class inventory_items_result
{
public:
    /// <summary>
    /// Internal function
    /// </summary>
    inventory_items_result();

    /// <summary>
    /// Internal function
    /// </summary>
    inventory_items_result(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
        _In_ media_item_type mediaItemType,
        _In_ inventory_item_state inventoryItemState,
        _In_ inventory_item_availability inventoryItemAvailability,
        _In_ string_t inventoryItemContainerId,
        _In_ bool allUsersAuthRequired,
        _In_ std::vector<inventory_item> items,
        _In_ uint32_t totalItems,
        _In_ string_t continuationToken,
        _In_ bool expandSatisfyingEntitlements
        );

    /// <summary>
    /// Collection of InventoryItem objects returned by a request.
    /// </summary>
    _XSAPIIMP const std::vector<inventory_item>& items() const;

    /// <summary>
    /// The total number of inventory items that matched the request.
    /// </summary>
    _XSAPIIMP uint32_t total_items() const;

    /// <summary>
    /// For paged enumeration requests, the ContinuationToken is used when requesting an additional page of data.
    /// </summary>
    _XSAPIIMP const string_t& continuation_token() const;

    /// <summary>
    /// Returns the next page of inventory items.
    /// </summary>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt
    /// retrieving all items.</param>
    /// <returns>InventoryResult object containing the next page of InventoryItem objects.</returns>
    /// <remarks>Calls V2 GET /users/me/inventory</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<inventory_items_result>> get_next(
        _In_ uint32_t maxItems
        );

    /// <summary>
    /// Indicates if there is additional data to retrieve from a get_next call
    /// </summary>
    _XSAPIIMP bool has_next() const;

    /// <summary>
    /// Internal function
    /// </summary>
    void _Initialize(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig,
        _In_ media_item_type mediaItemType,
        _In_ inventory_item_state inventoryItemState,
        _In_ inventory_item_availability inventoryItemAvailability,
        _In_ string_t inventoryItemContainerId,
        _In_ bool allUsersAuthRequired,
        _In_ bool expandSatisfyingEntitlements
        );

    /// <summary>
    /// Internal function
    /// </summary>
    static xbox_live_result<inventory_items_result> _Deserialize(_In_ const web::json::value& json);

private:
    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
    media_item_type m_mediaItemType;
    inventory_item_state m_inventoryItemState;
    inventory_item_availability m_inventoryItemAvailability;
    string_t m_inventoryItemContainerId;
    bool m_allUsersAuthRequired;
    std::vector<inventory_item> m_items;
    uint32_t m_totalItems;
    string_t m_continuationToken;
    bool m_expandSatisfyingEntitlements;
};

/// <summary>Defines methods and enumerations used to manage the item inventory a signed-in user.</summary>
class inventory_service
{
public:
    /// <summary>
    /// Enumerate user's inventory
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate.</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <param name="includeAllItemStatesAndAvailabilities">Include results of pre-orders and now-unavailable products the user owns</param>
    /// <returns>inventory_items_result object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<inventory_items_result>> get_inventory_items(
        _In_ media_item_type mediaItemType,
        _In_ bool expandSatisfyingEntitlements = false,
        _In_ bool includeAllItemStatesAndAvailabilities = false
        );

    /// <summary>
    /// Get the inventory results for a set of product ids for the current user
    /// </summary>
    /// <param name="productIds">A collection of IDs to query the inventory for</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <returns>inventory_items_result object containing the inventoryItems</returns>
    /// <remarks>
    /// It is recommended that you limit your ProductIds requested per list to a maximum of 100.
    ///
    /// Calls V4 GET /users/me/inventory
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<inventory_items_result>> get_inventory_items(
        _In_ const std::vector<string_t>& productIds,
        _In_ bool expandSatisfyingEntitlements = false
        );

    /// <summary>
    /// Enumerate a user's inventory.
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate.</param>
    /// <param name="inventoryItemState">The state of the inventory items to include in the result.</param>
    /// <param name="inventoryItemAvailability">The availability of the inventory items to include in the result.</param>
    /// <param name="inventoryItemContainerId">The container Id of the inventory items to include in the result. (Optional)</param>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt
    /// retrieving all items.</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <returns>inventory_items_result object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<inventory_items_result>> get_inventory_items(
        _In_ media_item_type mediaItemType,
        _In_ inventory_item_state inventoryItemState,
        _In_ inventory_item_availability inventoryItemAvailability,
        _In_ const string_t& inventoryItemContainerId,
        _In_ uint32_t maxItems,
        _In_ bool expandSatisfyingEntitlements = false
        );

    /// <summary>
    /// Enumerate inventory for all signed-in users for all available items of the specified type that are in the enabled state.
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <param name="includeAllItemStatesAndAvailabilities">Include results of pre-orders and now-unavailable products the user owns</param>
    /// <returns>inventory_items_result object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<inventory_items_result>> get_inventory_items_for_all_users(
        _In_ media_item_type mediaItemType,
        _In_ bool expandSatisfyingEntitlements = false,
        _In_ bool includeAllItemStatesAndAvailabilities = false
        );

    /// <summary>
    /// Get the inventory results for a set of product ids for the current user
    /// </summary>
    /// <param name="productIds">A collection of IDs to query the inventory for</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <returns>inventory_items_result object containing the inventoryItems</returns>
    /// <remarks>
    /// It is recommended that you limit your ProductIds requested per list to a maximum of 100.
    ///
    /// Calls V4 GET /users/me/inventory
    /// </remarks>
    _XSAPIIMP pplx::task<xbox_live_result<inventory_items_result>> get_inventory_items_for_all_users(
        _In_ const std::vector<string_t>& productIds,
        _In_ bool expandSatisfyingEntitlements = false
        );

    /// <summary>
    /// Enumerate inventory for all signed-in users.
    /// </summary>
    /// <param name="mediaItemType">The media item type to enumerate</param>
    /// <param name="inventoryItemState">The state of the inventory items to include in the result</param>
    /// <param name="inventoryItemAvailability">The availability of the inventory items to include in the result</param>
    /// <param name="inventoryItemContainerId">The container Id of the inventory items to include in the result (Optional)</param>
    /// <param name="maxItems">The maximum number of items the result can contain.  Pass 0 to attempt
    /// retrieving all items.</param>
    /// <param name="expandSatisfyingEntitlements">Include all satisfying entitlements from bundles, Xbox 360 entitlements, etc. in the results</param>
    /// <returns>inventory_items_result object containing the inventoryItems</returns>
    /// <remarks>Calls V4 GET /users/me/inventory</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<inventory_items_result>> get_inventory_items_for_all_users(
        _In_ media_item_type mediaItemType,
        _In_ inventory_item_state inventoryItemState,
        _In_ inventory_item_availability inventoryItemAvailability,
        _In_ const string_t& inventoryItemContainerId,
        _In_ uint32_t maxItems,
        _In_ bool expandSatisfyingEntitlements = false
        );

    /// <summary>
    /// Gets a single inventory item for the specified Id.
    /// </summary>
    /// <param name="inventoryItem">The InventoryItem to get an update for.</param>
    /// <returns>The requested InventoryItem object</returns>
    /// <remarks>Calls V4 GET /users/me/inventory/{inventoryItemId}</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<inventory_item>> get_inventory_item(
        _In_ inventory_item inventoryItem
        );

    /// <summary>
    /// Consumes the specified quantity of a consumable inventory item.
    /// </summary>
    /// <param name="inventoryItem">The InventoryItem to consume quantity from.</param>
    /// <param name="quantityToConsume">The quantity to consume of the specified InventoryItem.</param>
    /// <param name="transactionId">A client generated unique Id for the transaction.
    /// Transactions with the same Id will only be processed once.</param>
    /// <returns>A ConsumeInventoryItemResult object.</returns>
    /// <remarks>Calls V4 POST /users/me/consumables/{consumableId}</remarks>
    _XSAPIIMP pplx::task<xbox_live_result<consume_inventory_item_result>> consume_inventory_item(
        _In_ inventory_item inventoryItem,
        _In_ uint32_t quantityToConsume,
        _In_ const string_t& transactionId
        );
private:
    inventory_service();

    inventory_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    pplx::task<xbox_live_result<inventory_items_result>> get_inventory_items(
        _In_ media_item_type mediaItemType,
        _In_ inventory_item_state inventoryItemState,
        _In_ inventory_item_availability inventoryItemAvailability,
        _In_ const string_t& inventoryItemContainerId,
        _In_ bool allUsersAuthRequired,
        _In_ uint32_t maxItems,
        _In_ const string_t& continuationToken,
        _In_ bool expandSatisfyingEntitlements,
        _In_ const std::vector<string_t>& productIds
        );

    static const xbox_live_result<string_t> convert_media_item_type_to_string(
        _In_ media_item_type mediaItemType
        );

    static const xbox_live_result<string_t> convert_inventory_item_state_to_string(
        _In_ inventory_item_state inventorytItemState
        );

    static const xbox_live_result<string_t> convert_inventory_item_availability_to_string(
        _In_ inventory_item_availability inventorytItemAvailability
        );

    static const string_t inventory_end_point();

    static const string_t get_inventory_items_sub_path(
        _In_ media_item_type mediaItemType,
        _In_ const string_t& mediaItemTypeString,
        _In_ const string_t& inventoryItemState,
        _In_ const string_t& inventoryItemAvailability,
        _In_ const string_t& inventoryItemContainerId,
        _In_ uint32_t maxItems,
        _In_ const string_t& continuationToken,
        _In_ bool expandSatisfyingEntitlements,
        _In_ const std::vector<string_t>& productIds
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;

    friend class xbox_live_context_impl;
    friend class inventory_items_result;
};

} } }

#endif
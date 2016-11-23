//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#define TEST_CLASS_OWNER L"adityat"
#define TEST_CLASS_AREA L"Marketplace"
#include "UnitTestIncludes.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Marketplace;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

/*GET /media/en-US/browse?fields=all&id=TitleParentId&mediaItemType=DGame&orderBy=DigitalReleaseDate&desiredMediaItemTypes=DConsumable*/
const std::wstring defaultBrowseCatalogResponse =
LR"(
{
   "Items": 
    [
      {
         "MediaGroup": "GameType",
         "MediaItemType": "DConsumable",
         "ID": "4552d9af-5f52-4573-b9ef-2c170fc7e75b",
         "Name": "Sample Consumable Gold Coins",
         "ReducedDescription": "Consumable sample product for in-game currency",
         "ReducedName": "Gold Coins",
         "ReleaseDate": "2014-10-10T00:00:00Z",
         "VuiDisplayName": "Gold Coins voice",
         "Genres": 
         [
            {
               "Name": "Educational"
            }
         ],
         "Images": 
         [
            {
               "ID": "4552d9af-5f52-4573-b9ef-2c170fc7e75b",
               "ResizeUrl": "http://images-eds.xboxlive.com/image?url=etc",
               "Purposes": 
               [
                  "Image"
               ],
               "Purpose": "Image",
               "Height": 1080,
               "Width": 1920
            },
            {
               "ID": "4552d9af-5f52-4573-b9ef-2c170fc7e75b",
               "ResizeUrl": "http://images-eds.xboxlive.com/image?url=etc",
               "Purposes": [
                  "Spotlight"
               ],
               "Purpose": "Spotlight",
               "Height": 424,
               "Width": 856
            },
            {
               "ID": "4552d9af-5f52-4573-b9ef-2c170fc7e75b",
               "ResizeUrl": "http://images-eds.xboxlive.com/image?url=etc",
               "Purposes": [
                  "SuperHeroArt"
               ],
               "Purpose": "SuperHeroArt",
               "Height": 1080,
               "Width": 1920
            }
         ],
         "PublisherName": "",
         "Updated": "2014-10-10T15:59:54.487Z",
         "ParentalRating": "Unrated",
         "SortName": "Gold Coins sort",
         "KValue": "4",
         "KValueNamespace": "bingbox",
         "RelatedMedia": 
         [
            {
               "ID": "eabe6ccb-48f2-4cd2-832c-1d9753bd707b",
               "RelationType": "Parent",
               "IsDefaultCompanion": false
            },
            {
               "ID": "9ee14af1-43b1-436f-a9cd-3108c28af88e",
               "RelationType": "Parent",
               "IsDefaultCompanion": false
            },
            {
               "ID": "613925c8-e7f9-4ed4-bfc4-9187695284dd",
               "RelationType": "Parent",
               "IsDefaultCompanion": false
            }
         ],
         "LegacyIds": [
            {
               "IdType": "ProductId",
               "Value": "4552d9af-5f52-4573-b9ef-2c170fc7e75b"
            }
         ],
         "Availabilities": [
            {
               "AvailabilityID": "a2685a6d-a422-4262-8a65-71071e47cabe",
               "ContentId": "00000000-0000-0000-0000-000000000000",
               "Devices": [
                  {
                     "Name": "XboxOne"
                  }
               ]
            },
            {
               "AvailabilityID": "9ec43bfc-ae03-4d70-9810-5cf3349ed990",
               "ContentId": "00000000-0000-0000-0000-000000000000",
               "Devices": [
                  {
                     "Name": "XboxOne"
                  }
               ]
            },
            {
               "AvailabilityID": "742bbdcb-5235-4290-a449-347286c4c10d",
               "ContentId": "00000000-0000-0000-0000-000000000000",
               "Devices": [
                  {
                     "Name": "XboxOne"
                  }
               ]
            }
         ],
         "SandboxId": "XDKS.1",
         "IsRetail": false,
         "ParentalRatings": [
            {
               "RatingId": "",
               "Rating": "Unrated",
               "LocalizedDetails": {
                  "ShortName": "Unrated",
                  "LongName": "Unrated",
                  "RatingImages": [
                     {
                        "Url": "http://compass.xbox.com/assets/0a/04/0a04ecb2-b63c-4d30-9e42-c19d1c3ecee9.png?n=ersb_rb_content.png"
                     }
                  ]
               }
            }
         ]
      },
      {
         "MediaGroup": "GameType",
         "MediaItemType": "DConsumable",
         "ID": "a3184da7-bd2c-435d-a476-d8a2176d8b96",
         "Name": "Sample Consumable XP Boost",
         "ReducedDescription": "Consumable sample product for an in-game item",
         "ReducedName": "XP Boost",
         "ReleaseDate": "2013-10-12T00:00:00Z",
         "VuiDisplayName": "XP Boost voice",
         "Genres": [
            {
               "Name": "Educational"
            }
         ],
         "Images": [
            {
               "ID": "a3184da7-bd2c-435d-a476-d8a2176d8b96",
               "ResizeUrl": "http://images-eds.xboxlive.com/image?url=etc",
               "Purposes": [
                  "Image"
               ],
               "Purpose": "Image",
               "Height": 1080,
               "Width": 1920
            },
            {
               "ID": "a3184da7-bd2c-435d-a476-d8a2176d8b96",
               "ResizeUrl": "http://images-eds.xboxlive.com/image?url=etc",
               "Purposes": [
                  "SuperHeroArt"
               ],
               "Purpose": "SuperHeroArt",
               "Height": 1080,
               "Width": 1920
            }
         ],
         "PublisherName": "",
         "Updated": "2013-10-12T06:40:53.887Z",
         "ParentalRating": "Unrated",
         "SortName": "XP Boost sort",
         "KValue": "5",
         "KValueNamespace": "bingbox",
         "RelatedMedia": [
            {
               "ID": "eabe6ccb-48f2-4cd2-832c-1d9753bd707b",
               "RelationType": "Parent",
               "IsDefaultCompanion": false
            }
         ],
         "LegacyIds": [
            {
               "IdType": "ProductId",
               "Value": "a3184da7-bd2c-435d-a476-d8a2176d8b96"
            }
         ],
         "Availabilities": [
            {
               "AvailabilityID": "7e2261ef-e31b-4120-96a1-32a4867712a4",
               "ContentId": "d92d86da-34fd-4a8a-b3d6-d16ba8761579",
               "Devices": [
                  {
                     "Name": "XboxOne"
                  }
               ]
            },
            {
               "AvailabilityID": "34b9077f-fb1b-4dd1-8831-cf65f9cc287d",
               "ContentId": "d92d86da-34fd-4a8a-b3d6-d16ba8761579",
               "Devices": [
                  {
                     "Name": "XboxOne"
                  }
               ]
            }
         ],
         "SandboxId": "XDKS.1",
         "IsRetail": false,
         "ParentalRatings": [
            {
               "RatingId": "",
               "Rating": "Unrated",
               "LocalizedDetails": {
                  "ShortName": "Unrated",
                  "LongName": "Unrated",
                  "RatingImages": [
                     {
                        "Url": "http://compass.xbox.com/assets/0a/04/0a04ecb2-b63c-4d30-9e42-c19d1c3ecee9.png?n=ersb_rb_content.png"
                     }
                  ]
               }
            }
         ]
      }
   ],
   "Totals": [
      {
         "Name": "GameType",
         "Count": 2
      }
   ],
   "ImpressionGuid": "c18658ad-43f0-4207-89ea-086ea01019c5"
}
)";

/* GET /media/en-US/details?fields=all&mediaGroup=GameType&ids=4552d9af-5f52-4573-b9ef-2c170fc7e75b.a3184da7-bd2c-435d-a476-d8a2176d8b96 HTTP/1.1*/
const std::wstring defaultCatalogItemDetailsResponse =
LR"(
{
   "Items": [
      {
         "MediaGroup": "GameType",
         "MediaItemType": "DConsumable",
         "ID": "4552d9af-5f52-4573-b9ef-2c170fc7e75b",
         "Name": "Sample Consumable Gold Coins",
         "Description": "Consumable sample product for in-game currency with multiple availabilities for quantity purchase",
         "ReducedDescription": "Consumable sample product for in-game currency",
         "ReducedName": "Gold Coins",
         "ReleaseDate": "2014-10-10T00:00:00Z",
         "VuiDisplayName": "Gold Coins voice",
         "Genres": [
            {
               "Name": "Educational"
            }
         ],
         "Images": [
            {
               "ID": "4552d9af-5f52-4573-b9ef-2c170fc7e75b",
               "ResizeUrl": "http://images-eds.xboxlive.com/image?url=etc",
               "Purposes": [
                  "Poster"
               ],
               "Purpose": "Poster",
               "Height": 1080,
               "Width": 742
            },
            {
               "ID": "4552d9af-5f52-4573-b9ef-2c170fc7e75b",
               "ResizeUrl": "http://images-eds.xboxlive.com/image?url=etc",
               "Purposes": [
                  "SuperHeroArt"
               ],
               "Purpose": "SuperHeroArt",
               "Height": 1080,
               "Width": 1920
            }
         ],
         "PublisherName": "",
         "Updated": "2014-10-10T15:59:54.487Z",
         "ParentalRating": "Unrated",
         "SortName": "Gold Coins sort",
         "KValue": "4",
         "KValueNamespace": "bingbox",
         "RelatedMedia": [
            {
               "ID": "eabe6ccb-48f2-4cd2-832c-1d9753bd707b",
               "RelationType": "Parent",
               "IsDefaultCompanion": false
            },
            {
               "ID": "9ee14af1-43b1-436f-a9cd-3108c28af88e",
               "RelationType": "Parent",
               "IsDefaultCompanion": false
            },
            {
               "ID": "613925c8-e7f9-4ed4-bfc4-9187695284dd",
               "RelationType": "Parent",
               "IsDefaultCompanion": false
            }
         ],
         "LegacyIds": [
            {
               "IdType": "ProductId",
               "Value": "4552d9af-5f52-4573-b9ef-2c170fc7e75b"
            }
         ],
         "Availabilities": [
            {
               "AvailabilityID": "a2685a6d-a422-4262-8a65-71071e47cabe",
               "ContentId": "00000000-0000-0000-0000-000000000000",
               "Devices": [
                  {
                     "Name": "XboxOne"
                  }
               ],
               "OfferDisplayData": "{\"acceptablePaymentInstrumentTypes\":[\"CreditCard\",\"CurrencyStoredValue\",\"CurrencyStoredValueWithCreditCardTopOff\",\"PayPal\",\"Tokens\",\"None\"],\"availabilityTitle\":\"Sample Consumable Gold Coins 10\",\"currencyCode\":\"USD\",\"displayListPrice\":\"$0.00\",\"displayPositionTag\":\"0000010002\",\"displayPrice\":\"$0.00\",\"distributionType\":\"Full\",\"isPurchasable\":true,\"listPrice\":0.00,\"offerId\":\"1013c320-867f-4469-8bd3-b531b7d9fd30\",\"prerequisiteProductId\":\"00000000-0000-0000-0000-000000000000\",\"prerequisiteProductType\":\"DGame\",\"quantity\":10,\"reducedTitle\":\"Gold Coins\",\"taxTypeCode\":\"TaxesNotIncluded\"}",
               "SignedOffer": "etc"
            },
            {
               "AvailabilityID": "9ec43bfc-ae03-4d70-9810-5cf3349ed990",
               "ContentId": "00000000-0000-0000-0000-000000000000",
               "Devices": [
                  {
                     "Name": "XboxOne"
                  }
               ],
               "OfferDisplayData": "{\"acceptablePaymentInstrumentTypes\":[\"CreditCard\",\"CurrencyStoredValue\",\"CurrencyStoredValueWithCreditCardTopOff\",\"PayPal\",\"Tokens\",\"None\"],\"availabilityTitle\":\"Sample Consumable Gold Coins 10\",\"currencyCode\":\"USD\",\"displayListPrice\":\"$0.00\",\"displayPositionTag\":\"0000010002\",\"displayPrice\":\"$0.00\",\"distributionType\":\"Full\",\"isPurchasable\":true,\"listPrice\":0.00,\"offerId\":\"1013c320-867f-4469-8bd3-b531b7d9fd30\",\"prerequisiteProductId\":\"00000000-0000-0000-0000-000000000000\",\"prerequisiteProductType\":\"DGame\",\"quantity\":10,\"reducedTitle\":\"Gold Coins\",\"taxTypeCode\":\"TaxesNotIncluded\"}",
               "SignedOffer": "etc"
            }
         ],
         "SandboxId": "XDKS.1",
         "IsRetail": false,
         "ParentalRatings": [
            {
               "RatingId": "",
               "Rating": "Unrated",
               "LocalizedDetails": {
                  "ShortName": "Unrated",
                  "LongName": "Unrated",
                  "RatingImages": [
                     {
                        "Url": "http://compass.xbox.com/assets/0a/04/0a04ecb2-b63c-4d30-9e42-c19d1c3ecee9.png?n=ersb_rb_content.png"
                     }
                  ]
               }
            }
         ]
      }
   ],
   "ImpressionGuid": "84ed8bb1-7a6d-4fe3-8220-729e84fb362e"
}
)";


/*GET /media/en-US/browse?fields=all&id=9ee14af1-43b1-436f-a9cd-3108c28af88e&mediaItemType=DGame&relationship=bundledProducts&desiredMediaItemTypes=DGame.DDurable.DConsumable.DApp&maxItems=25 */
const std::wstring defaultBrowseCatalogBundlesResponse =
LR"(
{
   "Items": [
      {
         "MediaGroup": "GameType",
         "MediaItemType": "DDurable",
         "ID": "cb79a5f6-f597-4412-b7cc-777709eab8ef",
         "Name": "Sample DLC Expansion 3",
         "ReducedDescription": "Sample Durable that is also available within a bundle offer",
         "ReducedName": "Sample DLC Expansion 3",
         "ReleaseDate": "2014-10-10T00:00:00Z",
         "VuiDisplayName": "Sample DLC Expansion 3",
         "Genres": [
            {
               "Name": "Educational"
            }
         ],
         "Images": [
            {
               "ID": "cb79a5f6-f597-4412-b7cc-777709eab8ef",
               "ResizeUrl": "http://images-eds.xboxlive.com/image?url=etc",
               "Purposes": [
                  "BoxArt"
               ],
               "Purpose": "BoxArt",
               "Height": 1080,
               "Width": 1080
            },
            {
               "ID": "cb79a5f6-f597-4412-b7cc-777709eab8ef",
               "ResizeUrl": "http://images-eds.xboxlive.com/image?url=etc",
               "Purposes": [
                  "BrandedKeyArt"
               ],
               "Purpose": "BrandedKeyArt",
               "Height": 800,
               "Width": 584
            }
         ],
         "PublisherName": "",
         "Updated": "2014-10-10T16:00:03.813Z",
         "ParentalRating": "Unrated",
         "SortName": "Sample DLC Expansion 3",
         "KValue": "4",
         "KValueNamespace": "bingbox",
         "RelatedMedia": [
            {
               "ID": "eabe6ccb-48f2-4cd2-832c-1d9753bd707b",
               "RelationType": "Parent",
               "IsDefaultCompanion": false
            },
            {
               "ID": "9ee14af1-43b1-436f-a9cd-3108c28af88e",
               "RelationType": "Parent",
               "IsDefaultCompanion": false
            }
         ],
         "LegacyIds": [
            {
               "IdType": "ProductId",
               "Value": "cb79a5f6-f597-4412-b7cc-777709eab8ef"
            }
         ],
         "Availabilities": [
            {
               "AvailabilityID": "00e1f4c5-2951-4be2-99a1-0038c181f96f",
               "ContentId": "1a812d42-4c36-4ae8-854f-1639b0a1fc20",
               "Devices": [
                  {
                     "Name": "XboxOne"
                  }
               ]
            }
         ],
         "Packages": [
            {
               "CdnFileLocation": [
                  {
                     "SortOrder": null,
                     "Uri": "http://dlassets.xboxlive.com/public/content/etc"
                  }
               ],
               "ContentId": "1a812d42-4c36-4ae8-854f-1639b0a1fc20",
               "PackageType": "XVC",
               "InstallSize": "10670080",
               "PackageSize": "10670080"
            }
         ],
         "SandboxId": "XDKS.1",
         "IsRetail": false,
         "ParentalRatings": [
            {
               "RatingId": "",
               "Rating": "Unrated",
               "LocalizedDetails": {
                  "ShortName": "Unrated",
                  "LongName": "Unrated",
                  "RatingImages": [
                     {
                        "Url": "http://compass.xbox.com/assets/0a/04/etc.png"
                     }
                  ]
               }
            }
         ],
         "IsPartOfAnyBundle": true
      },
      {
         "MediaGroup": "GameType",
         "MediaItemType": "DDurable",
         "ID": "8ef17cd8-4550-46f0-b886-ec4e74a57674",
         "Name": "Season Pass (Bundle) ",
         "ReducedDescription": "Example of setting up a Season Pass with a Bundle product type.",
         "ReducedName" : "Season Pass (Bundle) ",
         "ReleaseDate" : "2014-10-16T00:00:00Z",
         "VuiDisplayName" : "Season Pass Bundle",
         "Genres" : [
             {
                "Name": "Educational"
             }
         ],
         "Images" : [
             {
                 "ID": "8ef17cd8-4550-46f0-b886-ec4e74a57674",
                     "ResizeUrl" : "http://images-eds.xboxlive.com/image?url=etc",
                     "Purposes" : [
                         "BoxArt"
                     ],
                     "Purpose" : "BoxArt",
                     "Height" : 1080,
                     "Width" : 1080
             },
             {
                 "ID": "8ef17cd8-4550-46f0-b886-ec4e74a57674",
                 "ResizeUrl" : "http://images-eds.xboxlive.com/image?url=etc",
                 "Purposes" : [
                     "BrandedKeyArt"
                 ],
                 "Purpose" : "BrandedKeyArt",
                 "Height" : 800,
                 "Width" : 584
             }
         ],
        "PublisherName": "",
        "Updated" : "2014-10-16T20:41:05.547Z",
        "ParentalRating" : "Unrated",
        "SortName" : "Season Pass (Bundle) ",
        "KValue" : "6",
        "KValueNamespace" : "bingbox",
        "RelatedMedia" : [
            {
                "ID": "eabe6ccb-48f2-4cd2-832c-1d9753bd707b",
                    "RelationType" : "Parent",
                    "IsDefaultCompanion" : false
            },
            {
                "ID": "9ee14af1-43b1-436f-a9cd-3108c28af88e",
                "RelationType" : "Parent",
                "IsDefaultCompanion" : false
            },
            {
                "ID": "bc415805-7955-4449-ba20-3e50667c1b61",
                "RelationType" : "Parent",
                "IsDefaultCompanion" : false
            },
            {
                "ID": "cb79a5f6-f597-4412-b7cc-777709eab8ef",
                "RelationType" : "Parent",
                "IsDefaultCompanion" : false
            },
            {
                "ID": "6795a7ca-1517-41fc-8620-a11da8685c8e",
                "RelationType" : "Parent",
                "IsDefaultCompanion" : false
            },
            {
                "ID": "bf885f1d-7b9f-4df0-908c-a59968fc2185",
                "RelationType" : "Parent",
                "IsDefaultCompanion" : false
            },
            {
                "ID": "910360b1-a971-400d-a310-e0146b7bbda9",
                "RelationType" : "Parent",
                "IsDefaultCompanion" : false
            }
        ],
        "LegacyIds": [
            {
                "IdType": "ProductId",
                    "Value" : "8ef17cd8-4550-46f0-b886-ec4e74a57674"
            }
        ],
        "Availabilities": [
            {
                "AvailabilityID": "c929a96a-a179-4891-baa2-7fdc9deaa13c",
                    "ContentId" : "9048605d-145c-459c-90e7-a6780f794b45",
                    "Devices" : [
                {
                    "Name": "XboxOne"
                }
                    ]
            }
         ],
         "Packages": [
            {
                "CdnFileLocation": [
                {
                    "SortOrder": null,
                        "Uri" : "http://dlassets.xboxlive.com/public/content/8ef17cd8-4550-46f0-b886-ec4e74a57674/9048605d-145c-459c-90e7-a6780f794b45/1.0.0.0.58f41568-1049-4c46-be56-2c6766763609/Sample.DLC.SPBundle_1.0.0.0_neutral__8wekyb3d8bbwe"
                }
                ],
                    "ContentId": "9048605d-145c-459c-90e7-a6780f794b45",
                    "PackageType" : "XVC",
                    "InstallSize" : "10670080",
                    "PackageSize" : "10670080"
            }
         ],
        "SandboxId": "XDKS.1",
        "IsRetail" : false,
        "ParentalRatings" : [
            {
                "RatingId": "",
                    "Rating" : "Unrated",
                    "LocalizedDetails" : {
                    "ShortName": "Unrated",
                        "LongName" : "Unrated",
                        "RatingImages" : [
                    {
                        "Url": "http://compass.xbox.com/assets/0a/04/0a04ecb2-b63c-4d30-9e42-c19d1c3ecee9.png?n=ersb_rb_content.png"
                    }
                        ]
                }
            }
        ],
        "IsBundle": true,
        "IsPartOfAnyBundle" : true
      }
   ],
   "ImpressionGuid": "8cb2d160-90d7-47c4-9e67-832b9d0197bf"
}
)";

const std::wstring defaultInventoryItemsResponse =
LR"(
{
   "pagingInfo": {
      "totalItems": 1
   },
   "items": [
      {
         "url": "https://inventory.xboxlive.com/users/me/inventory/xuid;a3184da7-bd2c-435d-a476-d8a2176d8b96;a3184da7-bd2c-435d-a476-d8a2176d8b96",
         "itemType": "GameConsumable",
         "offerInstanceId": "offerInstanceId",
         "productId": "productId",
         "titleId": 129071,
         "obtained": "2015-01-05T20:20:26.663+00:00",
         "beginDate": "2015-01-05T20:20:26.663+00:00",
         "endDate": "2115-01-05T20:20:26.663+00:00",
         "deviceName": null,
         "distributionType": "Full",
         "sandbox": null,
         "state": "Enabled",
         "consumable": {
            "url": "https://inventory.xboxlive.com/users/me/consumables/xuid;a3184da7-bd2c-435d-a476-d8a2176d8b96;a3184da7-bd2c-435d-a476-d8a2176d8b96",
            "quantity": 1
         }
      }
   ]
}
)";

const std::wstring expectedConsumeInventoryItemResponse =
LR"(
{ 
    "newQuantity": 0, 
    "url": "https://inventory.xboxlive.com/users/me/consumables/xuid;a3184da7-bd2c-435d-a476-d8a2176d8b96;a3184da7-bd2c-435d-a476-d8a2176d8b96", 
    "transactionId": "5DEE2461-648F-4E63-8236-DFC3760B5359"
}
)";

DEFINE_TEST_CLASS(MarketplaceTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(MarketplaceTests)

    Platform::String^ ConvertCatalogMediaItemTypeToString(
        _In_ Marketplace::MediaItemType itemType
        )
    {
        switch (itemType)
        {
        case MediaItemType::GameContent:
            return L"DDurable";

        case MediaItemType::GameConsumable:
            return  L"DConsumable";

        case MediaItemType::Subscription:
            return  L"Subscription";

        case MediaItemType::Application:
            return  L"DApp";

        case MediaItemType::Game:
            return  L"DGame";

        case MediaItemType::Movie:
            return  L"Movie";

        case MediaItemType::TelevisionShow:
            return  L"TVShow";

        case MediaItemType::TelevisionSeries:
            return  L"TVSeries";

        case MediaItemType::TelevisionSeason:
            return  L"TVSeason";

        case MediaItemType::ApplicationOrGameUnknown:
            return  L"AppOrGameUnknown";

        case MediaItemType::GameDemo:
            return  L"DGameDemo";

        case MediaItemType::WebVideo:
            return  L"WebVideo";

        default:
            return L"Unsupported MediaItemType type";
        }
    }

    Platform::String^ ConvertInventoryMediaItemTypeToString(
        _In_ Marketplace::MediaItemType itemType
        )
    {
        switch (itemType)
        {
        case MediaItemType::GameV2:
            return L"GameV2";

        case MediaItemType::GameContent:
            return L"GameContent";

        case MediaItemType::GameConsumable:
            return L"GameConsumable";

        case MediaItemType::Subscription:
            return L"Subscription";

        default:
            return L"Unsupported MediaItemType type";
        }
    }

    Platform::String^ ConvertInventoryItemStateToString(
        _In_ Marketplace::InventoryItemState inventorytItemState
        )
    {
        switch (inventorytItemState)
        {
        case InventoryItemState::All:
            return nullptr;

        case InventoryItemState::Enabled:
            return L"Enabled";

        case InventoryItemState::Suspended:
            return L"Suspended";

        case InventoryItemState::Expired:
            return L"Expired";

        case InventoryItemState::Canceled:
            return L"Canceled";

        default:
            return L"Unsupported InventoryItemState type";
        }
    }


    void VerifyCatalogItemImageResult(
        _In_ Marketplace::CatalogItemImage^ result,
        _In_ web::json::value catalogItemImageResultToVerify
        )
    {
        VERIFY_ARE_EQUAL(result->Id->Data(), catalogItemImageResultToVerify[L"ID"].as_string());
        VERIFY_ARE_EQUAL(result->ResizeUrl->ToString()->Data(), catalogItemImageResultToVerify[L"ResizeUrl"].as_string());
        VERIFY_ARE_EQUAL_INT(result->Purposes->Size, catalogItemImageResultToVerify[L"Purposes"].as_array().size());
        VERIFY_ARE_EQUAL(result->Purpose->Data(), catalogItemImageResultToVerify[L"Purpose"].as_string());
        VERIFY_ARE_EQUAL_INT(result->Height, catalogItemImageResultToVerify[L"Height"].as_integer());
        VERIFY_ARE_EQUAL_INT(result->Width, catalogItemImageResultToVerify[L"Width"].as_integer());
    }

    void VerifyCatalogItemAvailabilityResult(
        _In_ Marketplace::CatalogItemAvailability^ result,
        _In_ web::json::value catalogItemAvailabilityResultToVerify
        )
    {
        VERIFY_ARE_EQUAL(result->ContentId->Data(), catalogItemAvailabilityResultToVerify[L"ContentId"].as_string());
        VERIFY_ARE_EQUAL(result->SignedOffer->Data(), catalogItemAvailabilityResultToVerify[L"SignedOffer"].as_string());

        auto offerDisplayData = catalogItemAvailabilityResultToVerify[L"OfferDisplayData"];
        web::json::value offerDisplayDataJson = utils::json_get_value_from_string(offerDisplayData.as_string());
        VERIFY_ARE_EQUAL_INT(result->AcceptablePaymentInstrumentTypes->Size, offerDisplayDataJson[L"acceptablePaymentInstrumentTypes"].as_array().size());
        VERIFY_ARE_EQUAL(result->AvailabilityTitle->Data(), offerDisplayDataJson[L"availabilityTitle"].as_string());

        if (!offerDisplayDataJson[L"availabilityDescription"].is_null())
        {
            VERIFY_ARE_EQUAL(result->AvailabilityDescription->Data(), offerDisplayDataJson[L"availabilityDescription"].as_string());
        }
        
        VERIFY_ARE_EQUAL(result->CurrencyCode->Data(), offerDisplayDataJson[L"currencyCode"].as_string());
        VERIFY_ARE_EQUAL(result->DisplayPrice->Data(), offerDisplayDataJson[L"displayPrice"].as_string());
        VERIFY_ARE_EQUAL(result->DisplayListPrice->Data(), offerDisplayDataJson[L"displayListPrice"].as_string());
        VERIFY_ARE_EQUAL(result->DistributionType->Data(), offerDisplayDataJson[L"distributionType"].as_string());

        if (!offerDisplayDataJson[L"isPurchasable"].is_null())
        {
            VERIFY_ARE_EQUAL(result->IsPurchasable, offerDisplayDataJson[L"isPurchasable"].as_bool());
        }
        
        if (!offerDisplayDataJson[L"listPrice"].is_null())
        {
            VERIFY_ARE_EQUAL(result->ListPrice, offerDisplayDataJson[L"listPrice"].as_double());
        }

        if (!offerDisplayDataJson[L"price"].is_null())
        {
            VERIFY_ARE_EQUAL(result->Price, offerDisplayDataJson[L"price"].as_double());
        }
        
        if (!offerDisplayDataJson[L"quantity"].is_null())
        {
            VERIFY_ARE_EQUAL_INT(result->ConsumableQuantity, offerDisplayDataJson[L"quantity"].as_integer());
        }

        if (!offerDisplayDataJson[L"promotionalText"].is_null())
        {
            VERIFY_ARE_EQUAL(result->PromotionalText->Data(), offerDisplayDataJson[L"promotionalText"].as_string());
        }
        
        VERIFY_ARE_EQUAL(result->OfferId->Data(), offerDisplayDataJson[L"offerId"].as_string());
    }

    void VerifyCatalogItemResult(
        _In_ Marketplace::CatalogItem^ result,
        _In_ web::json::value catalogItemResultToVerify
        )
    {
        VERIFY_ARE_EQUAL(ConvertCatalogMediaItemTypeToString(result->MediaItemType)->Data(), catalogItemResultToVerify[L"MediaItemType"].as_string());
        VERIFY_ARE_EQUAL(result->Id->Data(), catalogItemResultToVerify[L"ID"].as_string());
        VERIFY_ARE_EQUAL(result->Name->Data(), catalogItemResultToVerify[L"Name"].as_string());
        VERIFY_ARE_EQUAL(result->ReducedName->Data(), catalogItemResultToVerify[L"ReducedName"].as_string());
        if (result->TitleId != 0)
        {
            VERIFY_ARE_EQUAL_INT(result->TitleId, catalogItemResultToVerify[L"TitleId"].as_integer());
        }

        VERIFY_ARE_EQUAL(DateTimeToString(result->ReleaseDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            catalogItemResultToVerify[L"ReleaseDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        VERIFY_ARE_EQUAL(result->ProductId->Data(), catalogItemResultToVerify[L"ID"].as_string());
        VERIFY_ARE_EQUAL(result->SandboxId->Data(), catalogItemResultToVerify[L"SandboxId"].as_string());

        if (!catalogItemResultToVerify[L"IsBundle"].is_null())
        {
            VERIFY_ARE_EQUAL(result->IsBundle, catalogItemResultToVerify[L"IsBundle"].as_bool());
        }

        if (!catalogItemResultToVerify[L"IsPartOfAnyBundle"].is_null())
        {
            VERIFY_ARE_EQUAL(result->IsPartOfAnyBundle, catalogItemResultToVerify[L"IsPartOfAnyBundle"].as_bool());
        }

        int index = 0;
        VERIFY_ARE_EQUAL_INT(result->Images->Size, catalogItemResultToVerify[L"Images"].as_array().size());
        for (Marketplace::CatalogItemImage^ imageResult : result->Images)
        {
            auto imageJson = catalogItemResultToVerify.as_object()[L"Images"].as_array()[index];
            VerifyCatalogItemImageResult(imageResult, imageJson);
            index++;
        }

        index = 0;
        VERIFY_ARE_EQUAL_INT(result->AvailabilityContentIds->Size, catalogItemResultToVerify[L"Availabilities"].as_array().size());
        for (Platform::String^ availabilityResult : result->AvailabilityContentIds)
        {
            auto availabilityJson = catalogItemResultToVerify[L"Availabilities"].as_array()[index];
            VERIFY_ARE_EQUAL(availabilityResult->ToString()->Data(), availabilityJson[L"ContentId"].as_string());
            index++;
        }
    }

    void VerifyCatalogItemDetailsResult(
        _In_ Marketplace::CatalogItemDetails^ result,
        _In_ web::json::value catalogItemDetailsResultToVerify
        )
    {
        VERIFY_ARE_EQUAL(ConvertCatalogMediaItemTypeToString(result->MediaItemType)->Data(), catalogItemDetailsResultToVerify[L"MediaItemType"].as_string());
        VERIFY_ARE_EQUAL(result->Id->Data(), catalogItemDetailsResultToVerify[L"ID"].as_string());
        VERIFY_ARE_EQUAL(result->Name->Data(), catalogItemDetailsResultToVerify[L"Name"].as_string());
        VERIFY_ARE_EQUAL(result->ReducedName->Data(), catalogItemDetailsResultToVerify[L"ReducedName"].as_string());
        VERIFY_ARE_EQUAL(result->Description->Data(), catalogItemDetailsResultToVerify[L"Description"].as_string());
        if (!catalogItemDetailsResultToVerify[L"TitleId"].is_null())
        {
            VERIFY_ARE_EQUAL_INT(result->TitleId, catalogItemDetailsResultToVerify[L"TitleId"].as_integer());
        }

        VERIFY_ARE_EQUAL(DateTimeToString(result->ReleaseDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            catalogItemDetailsResultToVerify[L"ReleaseDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        VERIFY_ARE_EQUAL(result->ProductId->Data(), catalogItemDetailsResultToVerify[L"ID"].as_string());
        VERIFY_ARE_EQUAL(result->SandboxId->Data(), catalogItemDetailsResultToVerify[L"SandboxId"].as_string());

        if (!catalogItemDetailsResultToVerify[L"IsBundle"].is_null())
        {
            VERIFY_ARE_EQUAL(result->IsBundle, catalogItemDetailsResultToVerify[L"IsBundle"].as_bool());
        }

        if (!catalogItemDetailsResultToVerify[L"IsPartOfAnyBundle"].is_null())
        {
            VERIFY_ARE_EQUAL(result->IsPartOfAnyBundle, catalogItemDetailsResultToVerify[L"IsPartOfAnyBundle"].as_bool());
        }

        int index = 0;
        VERIFY_ARE_EQUAL_INT(result->Images->Size, catalogItemDetailsResultToVerify[L"Images"].as_array().size());
        for (Marketplace::CatalogItemImage^ imageResult : result->Images)
        {
            auto imageJson = catalogItemDetailsResultToVerify.as_object()[L"Images"].as_array()[index];
            VerifyCatalogItemImageResult(imageResult, imageJson);
            index++;
        }

        index = 0;
        VERIFY_ARE_EQUAL_INT(result->Availabilities->Size, catalogItemDetailsResultToVerify[L"Availabilities"].as_array().size());
        for (Marketplace::CatalogItemAvailability^ availabilityResult : result->Availabilities)
        {
            auto availabilityJson = catalogItemDetailsResultToVerify.as_object()[L"Availabilities"].as_array()[index];
            VerifyCatalogItemAvailabilityResult(availabilityResult, availabilityJson);
            index++;
        }
    }

    void VerifyInventoryItemResult(
        _In_ Marketplace::InventoryItem^ result,
        _In_ web::json::value inventoryItemResultToVerify
        )
    {
        VERIFY_ARE_EQUAL(result->Url->ToString()->Data(), inventoryItemResultToVerify[L"url"].as_string());
        VERIFY_ARE_EQUAL(ConvertInventoryItemStateToString(result->InventoryItemState)->Data(), inventoryItemResultToVerify[L"state"].as_string());
        VERIFY_ARE_EQUAL(ConvertInventoryMediaItemTypeToString(result->MediaItemType)->Data(), inventoryItemResultToVerify[L"itemType"].as_string());
        VERIFY_ARE_EQUAL(result->ProductId->Data(), inventoryItemResultToVerify[L"productId"].as_string());
        VERIFY_ARE_EQUAL_INT(result->TitleId, inventoryItemResultToVerify[L"titleId"].as_integer());
     
        //ContainerIds
        if (!inventoryItemResultToVerify[L"containers"].is_null())
        {
            VERIFY_ARE_EQUAL_INT(result->ContainerIds->Size, inventoryItemResultToVerify[L"containers"].as_array().size());
        }

        VERIFY_ARE_EQUAL(DateTimeToString(result->RightsObtainedDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            inventoryItemResultToVerify[L"obtained"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL(DateTimeToString(result->StartDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            inventoryItemResultToVerify[L"beginDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));
        VERIFY_ARE_EQUAL(DateTimeToString(result->EndDate).substr(0, DATETIME_STRING_LENGTH_TO_SECOND),
            inventoryItemResultToVerify[L"endDate"].as_string().substr(0, DATETIME_STRING_LENGTH_TO_SECOND));

        if (!inventoryItemResultToVerify[L"trial"].is_null())
        {
            VERIFY_ARE_EQUAL(result->IsTrialEntitlement, inventoryItemResultToVerify[L"trial"].as_bool());
        }

        if (!inventoryItemResultToVerify[L"trialTimeRemaining"].is_null())
        {
            TimeSpan trialTimeRemainingToVerify;
            trialTimeRemainingToVerify.Duration = 60 * TICKS_PER_SECOND; // 60 seconds
            VERIFY_ARE_EQUAL_INT(result->TrialTimeRemaining.Duration, trialTimeRemainingToVerify.Duration);
        }

        auto consumableJson = inventoryItemResultToVerify[L"consumable"];
        VERIFY_ARE_EQUAL(result->ConsumableUrl->ToString()->Data(), consumableJson[L"url"].as_string());
        VERIFY_ARE_EQUAL_INT(result->ConsumableBalance, consumableJson[L"quantity"].as_integer());
    }

    void VerifyConsumeInventoryItemResult(
        _In_ Marketplace::ConsumeInventoryItemResult^ result,
        _In_ web::json::value consumeInventoryItemResultToVerify
        )
    {
        VERIFY_ARE_EQUAL(result->ConsumableUrl->ToString()->Data(), consumeInventoryItemResultToVerify[L"url"].as_string());
        VERIFY_ARE_EQUAL_INT(result->ConsumableBalance, consumeInventoryItemResultToVerify[L"newQuantity"].as_integer());
        VERIFY_ARE_EQUAL(result->TransactionId->Data(), consumeInventoryItemResultToVerify[L"transactionId"].as_string());
    }

    void TestConsumeInventoryItem(
        _In_ Marketplace::InventoryItem^ inventoryItem
        )
    {
        auto expectedResponse = web::json::value::parse(expectedConsumeInventoryItemResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(expectedResponse);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        Marketplace::ConsumeInventoryItemResult^ consumeItemResult =
            create_task(xboxLiveContext->InventoryService->ConsumeInventoryItemAsync(
                inventoryItem,
                1,
                L"5DEE2461-648F-4E63-8236-DFC3760B5359"
            )).get();

        VERIFY_IS_NOT_NULL(consumeItemResult);
        VERIFY_ARE_EQUAL_STR(L"POST", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://inventory.xboxlive.com/users/me/consumables/xuid;a3184da7-bd2c-435d-a476-d8a2176d8b96;a3184da7-bd2c-435d-a476-d8a2176d8b96", httpCall->ServerName);

        VerifyConsumeInventoryItemResult(consumeItemResult, expectedResponse);

        // Testing invalid args here as you cannot create a fake InventoryItem obj.
        TEST_LOG(L"TestConsumeInventoryItemInvalidArgs: Zero quantity to consume.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->InventoryService->ConsumeInventoryItemAsync(
                inventoryItem,
                0,
                L"5DEE2461-648F-4E63-8236-DFC3760B5359"
                )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestConsumeInventoryItemInvalidArgs: Empty TransactionId.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->InventoryService->ConsumeInventoryItemAsync(
                inventoryItem,
                1,
                L""
                )).get(),
            E_INVALIDARG);
    }

    DEFINE_TEST_CASE(TestBrowseCatalog)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestBrowseCatalog);
        auto responseJson = web::json::value::parse(defaultBrowseCatalogResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        Marketplace::BrowseCatalogResult^ browseCatalogResult =
            create_task(xboxLiveContext->CatalogService->BrowseCatalogAsync(
                L"TitleParentId",
                Marketplace::MediaItemType::Game,                   // parent MediaType
                Marketplace::MediaItemType::GameConsumable,         // desired MediaType for results
                Marketplace::CatalogSortOrder::DigitalReleaseDate,  // desired sort order
                0,                                                  // skip to item
                0                                                   // max items (0 for all)
            )).get();

        VERIFY_IS_NOT_NULL(browseCatalogResult);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://eds.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/media/en-US,en/browse?fields=all&id=TitleParentId&mediaItemType=DGame&orderBy=DigitalReleaseDate&desiredMediaItemTypes=DConsumable", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_INT(browseCatalogResult->Items->Size, responseJson.as_object()[L"Items"].as_array().size());

        auto totalJson = responseJson[L"Totals"].as_array()[0];
        VERIFY_ARE_EQUAL_INT(browseCatalogResult->TotalCount, totalJson[L"Count"].as_integer());

        int index = 0;
        for (Marketplace::CatalogItem^ catalogItem : browseCatalogResult->Items)
        {
            auto catalogItemJson = responseJson.as_object()[L"Items"].as_array()[index];
            VerifyCatalogItemResult(catalogItem, catalogItemJson);
            index++;
        }
    }

    DEFINE_TEST_CASE(TestBrowseCatalogBundles)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestBrowseCatalogBundles);
        auto responseJson = web::json::value::parse(defaultBrowseCatalogBundlesResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        Marketplace::BrowseCatalogResult^ browseCatalogResult =
            create_task(xboxLiveContext->CatalogService->BrowseCatalogBundlesAsync(
                L"TitleParentId",
                Marketplace::MediaItemType::Game,
                L"ProductId",
                Marketplace::BundleRelationshipType::ProductsInBundle,  // Search for all included items in the bundle
                0,
                25
            )).get();

        VERIFY_IS_NOT_NULL(browseCatalogResult);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://eds.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/media/en-US,en/browse?fields=all&id=ProductId&mediaItemType=DGame&relationship=bundledProducts&desiredMediaItemTypes=DGame.DDurable.DConsumable.DApp&maxItems=25", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_INT(browseCatalogResult->Items->Size, responseJson.as_object()[L"Items"].as_array().size());

        int index = 0;
        for (Marketplace::CatalogItem^ catalogItem : browseCatalogResult->Items)
        {
            auto catalogItemJson = responseJson.as_object()[L"Items"].as_array()[index];
            VerifyCatalogItemResult(catalogItem, catalogItemJson);
            index++;
        }
    }

    DEFINE_TEST_CASE(TestGetCatalogItemDetails)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetCatalogItemDetails);
        auto responseJson = web::json::value::parse(defaultCatalogItemDetailsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        IVector< Platform::String^ >^ vProductIds = ref new Platform::Collections::Vector< Platform::String^ >();
        vProductIds->Append(L"ProductId1");
        vProductIds->Append(L"ProductId2");

        IVectorView<Marketplace::CatalogItemDetails^>^ results =
            create_task(xboxLiveContext->CatalogService->GetCatalogItemDetailsAsync(
                vProductIds->GetView()
            )).get();

        /* /media/en-US/details?fields=all&mediaGroup=GameType&ids=ProductId1.ProductId2 HTTP/1.1*/
        VERIFY_IS_NOT_NULL(results);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://eds.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/media/en-US,en/details?fields=all&desiredMediaItemTypes=Subscription.DGame.DGameDemo.DDurable.DConsumable.DApp&ids=ProductId1.ProductId2", httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_INT(results->Size, responseJson.as_object()[L"Items"].as_array().size());

        int index = 0;
        for (Marketplace::CatalogItemDetails^ catalogItemDetail : results)
        {
            auto catalogItemDetailJson = responseJson.as_object()[L"Items"].as_array()[index];
            VerifyCatalogItemDetailsResult(catalogItemDetail, catalogItemDetailJson);
            index++;
        }
    }

    DEFINE_TEST_CASE(TestBrowseCatalogInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestBrowseCatalogInvalidArgs);
        auto responseJson = web::json::value::parse(defaultBrowseCatalogResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        TEST_LOG(L"TestBrowseCatalogInvalidArgs: Empty TitleParentId param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->CatalogService->BrowseCatalogAsync(
                L"",            // Invalid
                Marketplace::MediaItemType::Game,
                Marketplace::MediaItemType::GameConsumable,
                Marketplace::CatalogSortOrder::DigitalReleaseDate,
                0,
                0
            )).get(),
            E_INVALIDARG);
    }

    DEFINE_TEST_CASE(TestBrowseCatalogBundlesInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestBrowseCatalogBundlesInvalidArgs);
        auto responseJson = web::json::value::parse(defaultBrowseCatalogResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        TEST_LOG(L"TestBrowseCatalogBundlesInvalidArgs: Empty TitleParentId param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->CatalogService->BrowseCatalogBundlesAsync(
                L"",       // Invalid
                Marketplace::MediaItemType::Game,
                L"ProductId",
                Marketplace::BundleRelationshipType::ProductsInBundle,
                0,
                25
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"TestBrowseCatalogBundlesInvalidArgs: Empty ProductiId param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->CatalogService->BrowseCatalogBundlesAsync(
                L"TitleParentId",       // Invalid
                Marketplace::MediaItemType::Game,
                L"",           // Invalid
                Marketplace::BundleRelationshipType::ProductsInBundle,
                0,
                25
            )).get(),
            E_INVALIDARG);
    }

    DEFINE_TEST_CASE(TestGetCatalogItemDetailsInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetCatalogItemDetailsInvalidArgs);
        auto responseJson = web::json::value::parse(defaultCatalogItemDetailsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        IVector< Platform::String^ >^ vProductIds = ref new Platform::Collections::Vector< Platform::String^ >();
        TEST_LOG(L"TestCatalogItemDetailsInvalidArgs: Empty ProductiIds param.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->CatalogService->GetCatalogItemDetailsAsync(
                vProductIds->GetView()
            )).get(),
            E_INVALIDARG);
    }

    void GetInventoryItemHelper(InventoryItemsResult^ inventoryItemsResult, const string_t& uri, const std::shared_ptr<MockHttpCall>& httpCall, web::json::value& responseJson)
    {
        VERIFY_IS_NOT_NULL(inventoryItemsResult);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://inventory.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL(uri, httpCall->PathQueryFragment.to_string());
        VERIFY_ARE_EQUAL_INT(inventoryItemsResult->Items->Size, responseJson.as_object()[L"items"].as_array().size());

        auto pagingJson = responseJson[L"pagingInfo"];
        VERIFY_ARE_EQUAL_INT(inventoryItemsResult->TotalItems, pagingJson[L"totalItems"].as_integer());

        int index = 0;
        for (Marketplace::InventoryItem^ inventoryItem : inventoryItemsResult->Items)
        {
            auto inventoryItemJson = responseJson.as_object()[L"items"].as_array()[index];
            VerifyInventoryItemResult(inventoryItem, inventoryItemJson);
            TestConsumeInventoryItem(inventoryItem);
            index++;
        }
    }

    DEFINE_TEST_CASE(TestGetInventoryItems)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetInventoryItems);
        auto responseJson = web::json::value::parse(defaultInventoryItemsResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        
        Marketplace::InventoryItemsResult^ inventoryItemsResult =
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsAsync(
                Marketplace::MediaItemType::GameConsumable
            )).get();

        GetInventoryItemHelper(inventoryItemsResult, L"/users/me/inventory?itemType=GameConsumable&state=Enabled&availability=Available", httpCall, responseJson);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        inventoryItemsResult =
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsAsync(
            Marketplace::MediaItemType::GameConsumable,
            true,
            true
            )).get();

        GetInventoryItemHelper(inventoryItemsResult, L"/users/me/inventory?itemType=GameConsumable&availability=All&expandSatisfyingEntitlements=true", httpCall, responseJson);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        Vector<Platform::String^>^ products = ref new Vector<Platform::String^>();
        products->Append("4552d9af-5f52-4573-b9ef-2c170fc7e75b");
        products->Append("9ee14af1-43b1-436f-a9cd-3108c28af88e");
        inventoryItemsResult =
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsAsync(
            products->GetView(),
            true
            )).get();

        GetInventoryItemHelper(inventoryItemsResult, L"/users/me/inventory?availability=All&expandSatisfyingEntitlements=true&productIds=4552d9af-5f52-4573-b9ef-2c170fc7e75b,9ee14af1-43b1-436f-a9cd-3108c28af88e", httpCall, responseJson);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        inventoryItemsResult =
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsAsync(
            Marketplace::MediaItemType::GameConsumable,
            InventoryItemState::Expired,
            InventoryItemAvailability::Unavailable,
            L"5",
            10
            )).get();

        GetInventoryItemHelper(inventoryItemsResult, L"/users/me/inventory?itemType=GameConsumable&state=Expired&container=5&availability=Unavailable&maxItems=10", httpCall, responseJson);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        inventoryItemsResult =
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsForAllUsersAsync(
            Marketplace::MediaItemType::GameConsumable,
            InventoryItemState::Expired,
            InventoryItemAvailability::Unavailable,
            L"5",
            10,
            true
            )).get();

        GetInventoryItemHelper(inventoryItemsResult, L"/users/me/inventory?itemType=GameConsumable&state=Expired&container=5&availability=Unavailable&expandSatisfyingEntitlements=true&maxItems=10", httpCall, responseJson);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        inventoryItemsResult =
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsForAllUsersAsync(
            Marketplace::MediaItemType::GameConsumable
            )).get();

        GetInventoryItemHelper(inventoryItemsResult, L"/users/me/inventory?itemType=GameConsumable&state=Enabled&availability=Available", httpCall, responseJson);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        inventoryItemsResult =
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsForAllUsersAsync(
            Marketplace::MediaItemType::GameConsumable,
            true,
            true
            )).get();

        GetInventoryItemHelper(inventoryItemsResult, L"/users/me/inventory?itemType=GameConsumable&availability=All&expandSatisfyingEntitlements=true", httpCall, responseJson);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        inventoryItemsResult =
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsForAllUsersAsync(
            products->GetView(),
            true
            )).get();

        GetInventoryItemHelper(inventoryItemsResult, L"/users/me/inventory?availability=All&expandSatisfyingEntitlements=true&productIds=4552d9af-5f52-4573-b9ef-2c170fc7e75b,9ee14af1-43b1-436f-a9cd-3108c28af88e", httpCall, responseJson);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        inventoryItemsResult =
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsForAllUsersAsync(
            Marketplace::MediaItemType::GameConsumable,
            InventoryItemState::Expired,
            InventoryItemAvailability::Unavailable,
            L"5",
            10
            )).get();

        GetInventoryItemHelper(inventoryItemsResult, L"/users/me/inventory?itemType=GameConsumable&state=Expired&container=5&availability=Unavailable&maxItems=10", httpCall, responseJson);

        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        inventoryItemsResult =
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsForAllUsersAsync(
            Marketplace::MediaItemType::GameConsumable,
            InventoryItemState::Expired,
            InventoryItemAvailability::Unavailable,
            L"5",
            10,
            true
            )).get();

        GetInventoryItemHelper(inventoryItemsResult, L"/users/me/inventory?itemType=GameConsumable&state=Expired&container=5&availability=Unavailable&expandSatisfyingEntitlements=true&maxItems=10", httpCall, responseJson);

    }

    DEFINE_TEST_CASE(TestInventoryItemInvalidArgs)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestInventoryItemInvalidArgs);
        auto responseJson = web::json::value::parse(expectedConsumeInventoryItemResponse);

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);
        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        TEST_LOG(L"ConsumeInventoryItemAsync: Null InventoryItem param.");
#pragma warning(suppress: 6387)
        VERIFY_THROWS_HR_CX(
            xboxLiveContext->InventoryService->ConsumeInventoryItemAsync(
                nullptr,        // Invalid
                1,
                L"5DEE2461-648F-4E63-8236-DFC3760B5359"
            ),
            E_INVALIDARG);

        TEST_LOG(L"GetInventoryItemsAsync: Invalid passing expandSatisfyingEntitlements == true with MediaType::All.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsAsync(
                MediaItemType::All,
                true,       // Invalid passing expandSatisfyingEntitlements == true with MediaType::All
                true
            )).get(),
            E_INVALIDARG);

        TEST_LOG(L"GetInventoryItemsForAllUsersAsync: Invalid passing expandSatisfyingEntitlements == true with MediaType::All.");
        VERIFY_THROWS_HR_CX(
            create_task(xboxLiveContext->InventoryService->GetInventoryItemsForAllUsersAsync(
                MediaItemType::All,
                true,       // Invalid passing expandSatisfyingEntitlements == true with MediaType::All
                true
            )).get(),
            E_INVALIDARG);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

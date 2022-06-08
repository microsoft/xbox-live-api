// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "UnitTestIncludes.h"
#include "xsapi-c/achievements_manager_c.h"
#include "Achievements/Manager/achievements_manager_internal.h"

using namespace xbox::services::achievements::manager;
using namespace xbox::services::real_time_activity;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const char* defaultAchievementsUri = "https://achievements.xboxlive.com";
const char* defaultNotificationUri = "https://notify.xboxlive.com";

const char* achievementId2013MultipleRequirements = "2";
const char* achievementId2013MultipleRequirementsAchieved = "3";
const char* achievementId2013SingleRequirement = "4";
const char* achievementId2017NotStarted = "5";
const char* achievementId2017InProgress = "6";
const char* achievementId2017Achieved = "7";
const char* achievementIdDoesNotExist = "8";
const char* defaultAchievementId = achievementId2013MultipleRequirements;
const uint8_t achievementInProgressStartProgress = 20;
const char* achievementUnlockTime = "2013-01-17T03:19:00.3087016Z";

const char defaultAchievementResponse[] =
R"(
{
    "achievements":
    [{
        "id":"3",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"Achieved",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789111",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":null,
        "totalRecords" : 1
    }
}
)";

const char multipageAchievementPage1Response[] =
R"(
{
    "achievements":
    [{
        "id":"3",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"Achieved",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789111",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":"abc123",
        "totalRecords" : 1
    }
}
)";

const char multipageAchievementPage2Response[] =
R"(
{
    "achievements":
    [{
        "id":"5",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"NotStarted",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789333",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            }],
            "timeUnlocked": "0001-01-01T00:00:00Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":null,
        "totalRecords" : 1
    }
}
)";

const char firstUserGetAchievementsResponse[] =
R"(
{
    "achievements":
    [{
        "id":"3",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"Achieved",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789111",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"5",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"NotStarted",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789333",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            }],
            "timeUnlocked": "0001-01-01T00:00:00Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":null,
        "totalRecords" : 1
    }
}
)";

const char secondUserGetAchievementsResponse[] =
R"(
{
    "achievements":
    [{
        "id":"3",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"Achieved",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789111",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"5",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"InProgress",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789333",
                "current" : "20",
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            }],
            "timeUnlocked": "0001-01-01T00:00:00Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":null,
        "totalRecords" : 1
    }
}
)";

const char firstUserResyncNotStartedResponse[] =
R"(
{
    "achievements":
    [{
        "id":"3",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"NotStarted",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789111",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"0001-01-01T00:00:00Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"5",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"NotStarted",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789333",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            }],
            "timeUnlocked": "0001-01-01T00:00:00Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":null,
        "totalRecords" : 1
    }
}
)";

const char firstUserResyncFinishedResponse[] =
R"(
{
    "achievements":
    [{
        "id":"3",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"Achieved",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789111",
                "current" : "100",
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : "100",
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"5",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"NotStarted",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789333",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            }],
            "timeUnlocked": "0001-01-01T00:00:00Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":null,
        "totalRecords" : 1
    }
}
)";

const char firstUserUpdateAchievementsResponse[] =
R"(
{
    "achievements":
    [{
        "id":"2",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"InProgress",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789777",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"3",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"Achieved",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789111",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"4",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"NotStarted",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789444",
                "current" : null,
                "target" : "48",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            }],
            "timeUnlocked": "0001-01-01T00:00:00Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"5",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"NotStarted",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789333",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            }],
            "timeUnlocked": "0001-01-01T00:00:00Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"6",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"InProgress",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789555",
                "current" : "20",
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            }],
            "timeUnlocked": "0001-01-01T00:00:00Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":null,
        "totalRecords" : 1
    }
}
)";

// rta response

const char getSortedAchievementsResponse[] =
R"(
{
    "achievements":
    [{
        "id":"2",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"InProgress",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789777",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"3",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"Achieved",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789111",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            },
            {
                "id":"12345678-1234-1234-1234-123456789222",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Group"
            }],
            "timeUnlocked":"2013-01-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"5",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"NotStarted",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789333",
                "current" : null,
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            }],
            "timeUnlocked": "0001-01-01T00:00:00Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    },
    {
        "id":"7",
        "serviceConfigId" : "b5dd9daf-0000-0000-0000-000000000000",
        "name" : "Default NameString for Microsoft Achievements Sample Achievement 3",
        "titleAssociations" :
        [{
            "name":"Microsoft Achievements Sample",
            "id" : 3051199919,
            "version" : "abc"
        }],
        "progressState":"Achieved",
        "progression" :
        {
            "requirements":
            [{
                "id":"12345678-1234-1234-1234-123456789555",
                "current" : "100",
                "target" : "100",
                "operationType" : "sum",
                "ruleParticipationType" : "Individual"
            }],
            "timeUnlocked":"2017-10-17T03:19:00.3087016Z"
        },
        "mediaAssets":
        [{
            "name":"Icon Name",
            "type" : "Icon",
            "url" : "http://www.xbox.com/"
        }],
        "platforms" : ["Durango", "Xbox360"],
        "isSecret" : true,
        "description" : "Default DescriptionString for Microsoft Achievements Sample Achievement 3",
        "lockedDescription" : "Default UnachievedString for Microsoft Achievements Sample Achievement 3",
        "productId" : "12345678-1234-1234-1234-123456789012",
        "achievementType" : "Challenge",
        "participationType" : "Individual",
        "timeWindow" :
        {
            "startDate":"2013-02-01T00:00:00Z",
            "endDate" : "2100-07-01T00:00:00Z"
        },
        "rewards" :
        [{
            "name":null,
            "description" : null,
            "value" : "10",
            "type" : "Gamerscore",
            "valueType" : "Int",
            "mediaAsset" : null
        },
        {
            "name":"Default Name for InAppReward for Microsoft Achievements Sample Achievement 3",
            "description" : "Default Description for InAppReward for Microsoft Achievements Sample Achievement 3",
            "value" : "1",
            "type" : "InApp",
            "valueType" : "String",
            "mediaAsset" : {"name":"Icon Name", "type" : "Icon", "url" : "http://www.xbox.com"}
        }],
        "estimatedTime":"06:12:14",
        "deeplink" : "aWFtYWRlZXBsaW5r",
        "isRevoked" : false
    }],
    "pagingInfo":
    {
        "continuationToken":null,
        "totalRecords" : 1
    }
}
)";

const char rtaAchievementProgressChangedPayload[] =
R"(
{
    "progression": [
        {
            "id" : "5",
            "progressState" : "InProgress",
            "timeUnlocked" : "2013-01-17T03:19:00.3087016Z",
            "requirements" : [
                {
                    "id":"12345678-1234-1234-1234-123456789333", 
                    "current":"20", 
                    "operationType" : "sum",
                    "valueType" : "Integer",
                    "ruleParticipationType" : "Individual"
                }
            ]
        }
    ],
    "serviceConfigId" : "87654321-4321-4321-4321-210987654321"
}
)";

// check to see if updating progress to 100 will cause the resulting rta
// notification to have progress state be updated to achieved, otherwise
// this needs to be updated to be "InProgress" again
const char rtaAchievementProgressChangedCompletePayload[] =
R"(
{
    "progression": [
        {
            "id" : "6",
            "progressState" : "Achieved",
            "timeUnlocked" : "2013-01-17T03:19:00.3087016Z",
            "requirements" : [
                {
                    "id":"12345678-1234-1234-1234-123456789333", 
                    "current":"100", 
                    "target":"100",
                    "operationType" : "sum",
                    "valueType" : "Integer",
                    "ruleParticipationType" : "Individual"
                }
            ]
        }
    ],
    "serviceConfigId" : "87654321-4321-4321-4321-210987654321"
}
)";

DEFINE_TEST_CLASS(AchievementsManagerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(AchievementsManagerTests);

    static const JsonDocument testJson;
    static const JsonValue& defaultAchievementJson;
private:
    class AMTestEnvironment : public TestEnvironment
    {
    public:
        AMTestEnvironment() noexcept
        {
            auto& mockRtaService{ MockRealTimeActivityService::Instance() };
            mockRtaService.SetSubscribeHandler(
                [&](uint32_t n, xsapi_internal_string)
                {
                    mockRtaService.CompleteSubscribeHandshake(n);
                }
            );
        }

        ~AMTestEnvironment() noexcept
        {
            // remove all local users
            for (auto context : m_testContexts)
            {
                XblAchievementsManagerRemoveLocalUser(context->User().Handle());
            }
        }

        Vector<std::shared_ptr<XblContext>> m_testContexts;
    };

    void AddLocalUserWithoutSyncHelper(const User& user)
    {
        XTaskQueueHandle queue{};

        HttpMock mock("GET", defaultAchievementsUri);
        mock.SetResponseBody(defaultAchievementResponse); // mock the response of Get Achievements

        auto& mockRtaService{ MockRealTimeActivityService::Instance() };
        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
            {
                if (uri.find(defaultAchievementsUri) != xsapi_internal_string::npos
                    || uri.find(defaultNotificationUri) != xsapi_internal_string::npos)
                {
                    mockRtaService.CompleteSubscribeHandshake(n);
                }
            });

        VERIFY_SUCCEEDED(XblAchievementsManagerAddLocalUser(user.Handle(), queue));
    }
       
    void AddLocalUserSyncHelper(const User& user, const char* response = defaultAchievementResponse)
    {
        XTaskQueueHandle queue{};
        
        HttpMock mock("GET", defaultAchievementsUri);
        mock.SetResponseBody(response); // mock the response of Get Achievements

        auto& mockRtaService{ MockRealTimeActivityService::Instance() };
        mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
            {
                if (uri.find(defaultAchievementsUri) != xsapi_internal_string::npos
                    || uri.find(defaultNotificationUri) != xsapi_internal_string::npos)
                {
                    mockRtaService.CompleteSubscribeHandshake(n);
                }
            });
        
        VERIFY_SUCCEEDED(XblAchievementsManagerAddLocalUser(user.Handle(), queue));

        // verify UserAdded event is received
        int count{ 0 };
        bool userAdded{ false };
        while (!userAdded)
        {
            if (++count > 500)
            {
                break;
            }

            size_t eventsCount{};
            const XblAchievementsManagerEvent* events{};
            VERIFY_SUCCEEDED(XblAchievementsManagerDoWork(&events, &eventsCount));

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                switch (events[i].eventType)
                {
                case XblAchievementsManagerEventType::LocalUserInitialStateSynced:
                    userAdded = true;
                    break;
                default:
                    break;
                }
            }
            Sleep(10);
        }

        VERIFY_IS_TRUE(userAdded);
        // verify the user is in the list of users
        auto state{ GlobalState::Get() };
        VERIFY_IS_TRUE(state->AchievementsManager()->HasUser(user.Xuid()));
        VERIFY_IS_TRUE(state->AchievementsManager()->IsUserInitialized(user.Xuid()));
        
        // check for contents of achievements array is the expected size
        JsonDocument doc;
        doc.Parse(response);
        VERIFY_IS_TRUE(doc.FindMember("achievements")->value.IsArray());
        VERIFY_ARE_EQUAL_INT(
            state->AchievementsManager()->GetUserAchievementCount(user.Xuid()), 
            doc["achievements"].GetArray().Size()
        );
    }

    void AddLocalUserMultiplePageHelper(const User& user)
    {
        XTaskQueueHandle queue{};
        Vector<HttpMock> achievementMocks;

        HttpMock initialPageMock{ "Get", defaultAchievementsUri };
        initialPageMock.SetResponseBody(multipageAchievementPage1Response);
        achievementMocks.emplace_back(std::move(initialPageMock));

        HttpMock secondPageMock{ 
            "Get", 
            "https://achievements.xboxlive.com/users/xuid(1234)/achievements?titleId=1234&maxItems=100&continuationToken="
        };
        secondPageMock.SetResponseBody(multipageAchievementPage2Response);
        achievementMocks.emplace_back(std::move(secondPageMock));

        VERIFY_SUCCEEDED(XblAchievementsManagerAddLocalUser(user.Handle(), queue));

        // verify UserAdded event is received
        int count{ 0 };
        bool userAdded{ false };
        while (!userAdded)
        {
            if (++count > 500)
            {
                break;
            }

            size_t eventsCount{};
            const XblAchievementsManagerEvent* events{};
            VERIFY_SUCCEEDED(XblAchievementsManagerDoWork(&events, &eventsCount));

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                switch (events[i].eventType)
                {
                case XblAchievementsManagerEventType::LocalUserInitialStateSynced:
                    userAdded = true;
                    break;
                default:
                    break;
                }
            }
            Sleep(10);
        }

        VERIFY_IS_TRUE(userAdded);
        // verify the user is in the list of users
        auto state{ GlobalState::Get() };
        VERIFY_IS_TRUE(state->AchievementsManager()->HasUser(user.Xuid()));
        // check for initialized?
        VERIFY_IS_TRUE(state->AchievementsManager()->IsUserInitialized(user.Xuid()));

        uint64_t expectedAchievementsInResponses = 0;
        JsonDocument doc;
        doc.Parse(multipageAchievementPage1Response);
        VERIFY_IS_TRUE(doc.FindMember("achievements")->value.IsArray());
        expectedAchievementsInResponses += doc["achievements"].GetArray().Size();

        doc = JsonDocument();
        doc.Parse(multipageAchievementPage2Response);
        VERIFY_IS_TRUE(doc.FindMember("achievements")->value.IsArray());
        expectedAchievementsInResponses += doc["achievements"].GetArray().Size();

        VERIFY_ARE_EQUAL_INT(
            state->AchievementsManager()->GetUserAchievementCount(user.Xuid()),
            expectedAchievementsInResponses
        );
    }

    void AddMultipleLocalUsersHelper(const Map<std::shared_ptr<XblContext>, const char*>& userResponseMap)
    {
        XTaskQueueHandle queue{};
        Vector<HttpMock> achievementMocks;

        // create a mock for each of the users
        for (auto& pair : userResponseMap)
        {
            Stringstream uri;
            uri << "https://achievements.xboxlive.com/users/xuid(";
            uri << pair.first->User().Xuid();
            uri << ")/achievements";
            
            HttpMock mock("Get", uri.str());
            achievementMocks.emplace_back(std::move(mock));
            // set the response body for each of the mocks
            achievementMocks.back().SetResponseBody(pair.second);
        }
        // call add user for each user
        for (auto& pair : userResponseMap)
        {
            VERIFY_SUCCEEDED(XblAchievementsManagerAddLocalUser(pair.first->User().Handle(), queue));
        }

        // verify each user is added
        // verify UserAdded event is received
        int count{ 0 };
        uint32_t usersAdded{ 0 };
        while (usersAdded < userResponseMap.size())
        {
            if (++count > 500)
            {
                break;
            }

            size_t eventsCount{};
            const XblAchievementsManagerEvent* events{};
            VERIFY_SUCCEEDED(XblAchievementsManagerDoWork(&events, &eventsCount));

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                switch (events[i].eventType)
                {
                case XblAchievementsManagerEventType::LocalUserInitialStateSynced:
                    ++usersAdded;
                    break;
                default:
                    break;
                }
            }
            Sleep(10);
        }

        VERIFY_ARE_EQUAL_INT(usersAdded, userResponseMap.size());
        // verify the user is in the list of users
        auto state{ GlobalState::Get() };
        for (auto& pair : userResponseMap)
        {
            VERIFY_IS_TRUE(state->AchievementsManager()->HasUser(pair.first->User().Xuid()));
            VERIFY_IS_TRUE(state->AchievementsManager()->IsUserInitialized(pair.first->User().Xuid()));
            
            // check for contents of achievements array is the expected size
            JsonDocument doc;
            doc.Parse(pair.second);
            VERIFY_IS_TRUE(doc.FindMember("achievements")->value.IsArray());
            VERIFY_ARE_EQUAL_INT(
                state->AchievementsManager()->GetUserAchievementCount(pair.first->User().Xuid()),
                doc["achievements"].GetArray().Size()
            );
        }       
    }

    void SetupRtaResponseForUser(uint64_t xuid, String method, String progressPayload, Vector<std::shared_ptr<HttpMock>>& mocks)
    {
        Stringstream uri;
        uri << "https://achievements.xboxlive.com/users/xuid(";
        uri << xuid;
        uri << ")/achievements";

        std::shared_ptr<HttpMock> mock = std::make_shared<HttpMock>(method.c_str(), uri.str());
        mocks.emplace_back(std::move(mock));
        mocks.back()->SetResponseBody(progressPayload.c_str());
    }

    void SetUpUpdateAchievementRtaResponse(uint64_t xuid, String progressPayload, bool willUnlock, Vector<std::shared_ptr<HttpMock>>& mocks)
    {
        SetupRtaResponseForUser(xuid, "POST", progressPayload, mocks);
        mocks.back()->SetMockMatchedCallback(
            [
                progressPayload, 
                xuid,
                willUnlock
            ]
        (HttpMock* mock, xsapi_internal_string requestUrl, xsapi_internal_string requestBody)
            {
                UNREFERENCED_PARAMETER(mock);
                UNREFERENCED_PARAMETER(requestBody);
                UNREFERENCED_PARAMETER(requestUrl);

                xsapi_internal_stringstream uri;
                uri << "https://achievements.xboxlive.com/users/xuid(" << xuid << ")/achievements/" << utils::ToLower(AppConfig::Instance()->Scid());
                
                auto& mockRtaService{ MockRealTimeActivityService::Instance() };
                mockRtaService.RaiseEvent(uri.str(), progressPayload.c_str());
            });
    }

    bool UpdateAchievementHelper(uint64_t xuid, String achievementId, uint8_t progress, bool setUpMocks = true, bool willUnlock = false, String progressPayload = rtaAchievementProgressChangedPayload)
    {
        Vector<std::shared_ptr<HttpMock>> mocks;
        if (setUpMocks)
        {
            SetUpUpdateAchievementRtaResponse(xuid, progressPayload, willUnlock, mocks);
        }

        VERIFY_SUCCEEDED(XblAchievementsManagerUpdateAchievement(xuid, achievementId.c_str(), progress));
        
        int count{ 0 };
        bool progressEventReceived = false;
        bool unlockEventReceived = false;

        // We only want to care about unlock event if we expect to unlock the 
        //  achievement, so the inverse of XOR will tell us if the values are
        //  the same.
        while(!(progressEventReceived && !(willUnlock ^ unlockEventReceived)))
        {
            if (++count > 500)
            {
                break;
            }

            size_t eventsCount{};
            const XblAchievementsManagerEvent* events{};
            VERIFY_SUCCEEDED(XblAchievementsManagerDoWork(&events, &eventsCount));

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                switch (events[i].eventType)
                {
                case XblAchievementsManagerEventType::AchievementProgressUpdated:
                    progressEventReceived = true;
                    break;
                case XblAchievementsManagerEventType::AchievementUnlocked:
                    unlockEventReceived = true;
                    break;
                default:
                    break;
                }
            }
            Sleep(10);
        }

        // compare update call response to expected mock
        // ensure achievement progress event arrives and is processed
        // check progress value of achievement to value intended to set
        return !(count > 500);
    }

    void UpdateAchievementSuccessHelper()
    {
        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);
        bool completedBeforeTimeout = UpdateAchievementHelper(xuid, achievementId2017InProgress, 100, true, true, rtaAchievementProgressChangedCompletePayload);

        VERIFY_IS_TRUE(completedBeforeTimeout);
        
        XblAchievementsManagerResultHandle resultHandle;
        const XblAchievement* achievement;
        uint64_t size;
        HRESULT hr;
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerGetAchievement(
            xuid,
            achievementId2017InProgress,
            &resultHandle
        ));
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerResultGetAchievements(resultHandle, &achievement, &size));
        VERIFY_IS_TRUE(achievement->progressState == XblAchievementProgressState::Achieved);
        VERIFY_ARE_EQUAL_STR(
            achievement->progression.requirements[0].currentProgressValue,
            achievement->progression.requirements[0].targetProgressValue
        );
        VERIFY_ARE_EQUAL_INT(
            utils::internal_string_to_uint32(achievement->progression.requirements[0].currentProgressValue),
            100
        );
        VERIFY_IS_TRUE(
            utils::TimeTFromDatetime(datetime::from_string(achievementUnlockTime, datetime::date_format::ISO_8601))
            == achievement->progression.timeUnlocked
        );
        XblAchievementsManagerResultCloseHandle(resultHandle);
    }

public:

    DEFINE_TEST_CASE(AddLocalUser_InitializesWithLocalAchievementsCached_Success)
    {
        TEST_LOG(L"Test starting: AddLocalUser_InitializesWithLocalAchievementsCached_Success");
        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User());
    }

    DEFINE_TEST_CASE(AddLocalUser_SingleUserMultiplePages_InitializesAfterAllPagesCached)
    {
        TEST_LOG(L"Test starting: AddLocalUser_SingleUserMultiplePages_InitializesAfterAllPagesCached");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;
        
        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserMultiplePageHelper(xblContext->User());
    }

    DEFINE_TEST_CASE(AddLocalUser_MultipleUser_InitializesWithLocalAchievementsCached_Success)
    {
        TEST_LOG(L"Test starting: AddLocalUser_MultipleUser_InitializesWithLocalAchievementsCached_Success");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;
        
        Map<std::shared_ptr<XblContext>, const char*> responseMap;
        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);
        responseMap[xblContext] = firstUserGetAchievementsResponse;
        xblContext = env.CreateMockXboxLiveContext(2345);
        env.m_testContexts.push_back(xblContext);
        responseMap[xblContext] = secondUserGetAchievementsResponse;

        // add local users
        AddMultipleLocalUsersHelper(responseMap);
    }

    DEFINE_TEST_CASE(RemoveLocalUser_SingleUser_ExpectUserRemoved)
    {
        TEST_LOG(L"Test starting: RemoveLocalUser_SingleUser_ExpectUserRemoved");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User());

        VERIFY_SUCCEEDED(XblAchievementsManagerRemoveLocalUser(
            xblContext->User().Handle()
        ));
        env.m_testContexts.pop_back();

        HRESULT hr = S_OK;
        XblAchievementsManagerResultHandle result;
        VERIFY_FAILED(hr = XblAchievementsManagerGetAchievement(xuid, achievementId2013MultipleRequirementsAchieved, &result));
        VERIFY_ARE_EQUAL(hr, E_BOUNDS);
    }

    DEFINE_TEST_CASE(RemoveLocalUser_MultipleUsers_ExpectCorrectUserRemoved)
    {
        TEST_LOG(L"Test starting: RemoveLocalUser_MultipleUsers_ExpectCorrectUserRemoved");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        Map<std::shared_ptr<XblContext>, const char*> responseMap;
        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);
        responseMap[xblContext] = firstUserGetAchievementsResponse;
        xblContext = env.CreateMockXboxLiveContext(2345);
        env.m_testContexts.push_back(xblContext);
        responseMap[xblContext] = secondUserGetAchievementsResponse;

        // add local users
        AddMultipleLocalUsersHelper(responseMap);

        VERIFY_SUCCEEDED(XblAchievementsManagerRemoveLocalUser(
            xblContext->User().Handle()
        ));
        env.m_testContexts.pop_back();

        HRESULT hr = S_OK;
        XblAchievementsManagerResultHandle result;
        VERIFY_FAILED(hr = XblAchievementsManagerGetAchievement(2345, achievementId2013MultipleRequirementsAchieved, &result));
        VERIFY_ARE_EQUAL(hr, E_BOUNDS);

        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(xuid, achievementId2013MultipleRequirementsAchieved, &result));
        XblAchievementsManagerResultCloseHandle(result);
    }

    DEFINE_TEST_CASE(RemoveLocalUser_UserNotInList_ExpectError)
    {
        TEST_LOG(L"Test starting: RemoveLocalUser_UserNotInList_ExpectError");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        HRESULT hr = S_OK;
        VERIFY_FAILED(hr = XblAchievementsManagerRemoveLocalUser(
            xblContext->User().Handle()
        ));
        VERIFY_ARE_EQUAL(hr, E_BOUNDS);

        env.m_testContexts.pop_back();
    }

    DEFINE_TEST_CASE(IsLocalUserInitialized_UserNotInList_ExpectInvalidArg)
    {
        TEST_LOG(L"Test starting: IsLocalUserInitialized_UserNotInList_ExpectInvalidArg");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerIsUserInitialized(xuid));
        VERIFY_ARE_EQUAL(E_INVALIDARG, hr);
    }
    
    DEFINE_TEST_CASE(IsLocalUserInitialized_UserNotInitialized_ExpectEFail)
    {
        TEST_LOG(L"Test starting: IsLocalUserInitialized_UserNotInitialized_ExpectEFail");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);
        AddLocalUserWithoutSyncHelper(xblContext->User());

        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerIsUserInitialized(xuid));
        VERIFY_ARE_EQUAL(E_FAIL, hr);
    }

    DEFINE_TEST_CASE(IsLocalUserInitialized_UserInitialized_ExpectSuccess)
    {
        TEST_LOG(L"Test starting: IsLocalUserInitialized_UserInitialized_ExpectSuccess");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User());
        VERIFY_SUCCEEDED(XblAchievementsManagerIsUserInitialized(xuid));
    }

    DEFINE_TEST_CASE(GetAchievementForUser_SingleUser_Success)
    {
        TEST_LOG(L"Test starting: GetAchievementForUser_SingleUser_Success");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User());
        // check the values of the achievement to ensure they are what expected
        
        const XblAchievement* achievement = nullptr;
        uint64_t size;
        XblAchievementsManagerResultHandle resultHandle;
        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(
            xuid,
            achievementId2013MultipleRequirementsAchieved,
            &resultHandle
        ));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));
        VERIFY_IS_NOT_NULL(achievement);

        // validate individual fields
        XblAchievementsManagerResultCloseHandle(resultHandle);
    }

    DEFINE_TEST_CASE(GetAchievementForUser_DuplicateHandleOneDuplicate_SuccessfulDataDeletedAfterBothClosed)
    {
        TEST_LOG(L"Test starting: GetAchievementForUser_DuplicateHandleOneDuplicate_SuccessfulDataDeletedAfterBothClosed");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User());
        // check the values of the achievement to ensure they are what expected

        const XblAchievement* achievement = nullptr;
        uint64_t size;
        XblAchievementsManagerResultHandle resultHandle;
        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(
            xuid,
            achievementId2013MultipleRequirementsAchieved,
            &resultHandle
        ));

        XblAchievementsManagerResultHandle duplicateHandle;
        VERIFY_SUCCEEDED(XblAchievementsManagerResultDuplicateHandle(resultHandle, &duplicateHandle));

        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));
        VERIFY_IS_NOT_NULL(achievement);
        XblAchievementsManagerResultCloseHandle(resultHandle);
        
        // ptr should still be valid after duplicate
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));
        VERIFY_IS_NOT_NULL(achievement);

        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            duplicateHandle,
            &achievement,
            &size
        ));
        VERIFY_IS_NOT_NULL(achievement);

        XblAchievementsManagerResultCloseHandle(duplicateHandle);
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));

        VERIFY_IS_NULL(achievement);
    }

    DEFINE_TEST_CASE(GetAchievementForUser_SingleUser_ExpectAchievementNonExistent)
    {
        TEST_LOG(L"Test starting: GetAchievementForUser_SingleUser_ExpectAchievementNonExistent");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User());
        // check the values of the achievement to ensure they are what expected

        XblAchievementsManagerResultHandle achievement;
        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerGetAchievement(
            xuid,
            achievementIdDoesNotExist,
            &achievement
        ));
        VERIFY_ARE_EQUAL(hr, E_BOUNDS);
    }

    DEFINE_TEST_CASE(GetAchievementForUser_SingleUser_ExpectFailureInvalidArgs)
    {
        TEST_LOG(L"Test starting: GetAchievementForUser_SingleUser_ExpectFailureInvalidArgs");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User());
        // check the values of the achievement to ensure they are what expected

        // no Id passed in
        XblAchievementsManagerResultHandle resultHandle = nullptr;
        const XblAchievement* achievement = nullptr;
        uint64_t size;
        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerGetAchievement(
            xuid,
            nullptr,
            &resultHandle
        ));
        VERIFY_ARE_EQUAL(hr, E_INVALIDARG);
        
        // no destination passed in
        VERIFY_FAILED(hr = XblAchievementsManagerGetAchievement(
            xuid,
            achievementId2017NotStarted,
            nullptr
        ));

        VERIFY_FAILED(hr = XblAchievementsManagerResultGetAchievements(
            nullptr,
            &achievement,
            &size
        ));
        VERIFY_ARE_EQUAL(hr, E_INVALIDARG);

        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(
            xuid,
            achievementId2013MultipleRequirementsAchieved,
            &resultHandle
        ));

        VERIFY_FAILED(hr = XblAchievementsManagerResultGetAchievements(
            resultHandle,
            nullptr,
            &size
        ));
        VERIFY_ARE_EQUAL(hr, E_INVALIDARG);
        
        VERIFY_FAILED(hr = XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            nullptr
        ));
        VERIFY_ARE_EQUAL(hr, E_INVALIDARG);

        XblAchievementsManagerResultCloseHandle(resultHandle);
    }
    
    DEFINE_TEST_CASE(GetAchievementForUser_UserDoesNotExist_ExpectFailure)
    {
        TEST_LOG(L"Test starting: GetAchievementForUser_UserDoesNotExist_ExpectFailure");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User());

        HRESULT hr = S_OK;
        XblAchievementsManagerResultHandle achievement;
        VERIFY_FAILED(hr = XblAchievementsManagerGetAchievement(
            2345, 
            defaultAchievementId, 
            &achievement
        ));
        VERIFY_ARE_EQUAL(hr, E_BOUNDS);
    }
    
    DEFINE_TEST_CASE(GetAchievementForUser_UserNotInitialized_ExpectFailure)
    {
        TEST_LOG(L"Test starting: GetAchievementForUser_UserNotInitialized_ExpectFailure");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // not using the helper function here since we are interrupting

        XTaskQueueHandle queue{};

        HttpMock mock("GET", defaultAchievementsUri);
        mock.SetResponseBody(defaultAchievementResponse);

        VERIFY_SUCCEEDED(XblAchievementsManagerAddLocalUser(xblContext->User().Handle(), queue));
        
        XblAchievementsManagerResultHandle achievement;
        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerGetAchievement(
            xuid, 
            defaultAchievementId, 
            &achievement
        ));
        VERIFY_ARE_EQUAL(hr, E_UNEXPECTED);
    }

    DEFINE_TEST_CASE(GetAchievementForUser_MultipleUsers_Success)
    {
        TEST_LOG(L"Test starting: GetAchievementForUser_MultipleUsers_Success");

        AMTestEnvironment env{};

        Map<std::shared_ptr<XblContext>, const char*> responseMap;
        auto xblContext = env.CreateMockXboxLiveContext(1234);
        env.m_testContexts.push_back(xblContext);
        responseMap[xblContext] = firstUserGetAchievementsResponse;
        xblContext = env.CreateMockXboxLiveContext(2345);
        env.m_testContexts.push_back(xblContext);
        responseMap[xblContext] = secondUserGetAchievementsResponse;

        // add local users
        AddMultipleLocalUsersHelper(responseMap);
        // check the values of the achievements for each user to ensure they are part
        //      of the correct user

        XblAchievementsManagerResultHandle resultHandle = nullptr;
        uint64_t size;
        const XblAchievement* user1Achievement;
        const XblAchievement* user2Achievement;
        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(
            env.m_testContexts[0]->User().Xuid(),
            "3",
            &resultHandle
        ));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &user1Achievement,
            &size
        ));
        VERIFY_IS_NOT_NULL(user1Achievement);

        // verify individual fields
        XblAchievementsManagerResultCloseHandle(resultHandle);

        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(
            env.m_testContexts[0]->User().Xuid(),
            "5",
            &resultHandle
        ));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &user1Achievement,
            &size
        ));
        VERIFY_IS_NOT_NULL(user1Achievement);
        // verify individual fields
        XblAchievementsManagerResultCloseHandle(resultHandle);

        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(
            env.m_testContexts[1]->User().Xuid(),
            "5",
            &resultHandle
        ));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &user2Achievement,
            &size
        ));
        VERIFY_IS_NOT_NULL(user2Achievement);

        // verify individual fields
        XblAchievementsManagerResultCloseHandle(resultHandle);

        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(
            env.m_testContexts[1]->User().Xuid(),
            "3",
            &resultHandle
        ));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &user2Achievement,
            &size
        ));
        VERIFY_IS_NOT_NULL(user2Achievement);

        // verify individual fields
        XblAchievementsManagerResultCloseHandle(resultHandle);
    }

    DEFINE_TEST_CASE(GetAchievementsForUser_DefaultOptions_AllAchievementsReturned)
    {
        TEST_LOG(L"Test starting: GetAchievementsForUser_DefaultOptions_AllAchievementsReturned");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);

        // compare list of contents for equality
        HRESULT hr;
        XblAchievementsManagerResultHandle resultHandle;
        const XblAchievement* achievements;
        uint64_t achievementsCount;
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerGetAchievements(
            xuid,
            XblAchievementOrderBy::DefaultOrder,
            XblAchievementsManagerSortOrder::Unsorted,
            &resultHandle
        ));
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerResultGetAchievements(
            resultHandle, 
            &achievements, 
            &achievementsCount
        ));

        Vector<XblAchievement> achievementsVector(achievements, achievements + achievementsCount);
        auto foundAchievement = std::find_if(achievements, achievements + achievementsCount, [&](const XblAchievement& achievement)
            {
                return utils::str_icmp(achievementId2017InProgress, achievement.id) == 0;
            });

        VERIFY_ARE_EQUAL_INT(5, achievementsCount);
        VERIFY_IS_NOT_NULL(foundAchievement);
        VERIFY_ARE_EQUAL_STR_IGNORE_CASE(
            utils::uint32_to_internal_string(achievementInProgressStartProgress).c_str(),
            foundAchievement->progression.requirements[0].currentProgressValue
        );
        XblAchievementsManagerResultCloseHandle(resultHandle);
    }

    DEFINE_TEST_CASE(GetAchievementsForUser_SortFieldSpecifiedDefaultOrder_ExpectInvalidArg)
    {
        TEST_LOG(L"Test starting: GetAchievementsForUser_SortFieldSpecifiedDefaultOrder_ExpectInvalidArg");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);

        // compare list of contents for equality
        HRESULT hr;
        XblAchievementsManagerResultHandle resultHandle;
        VERIFY_FAILED(hr = XblAchievementsManagerGetAchievements(
            xuid,
            XblAchievementOrderBy::UnlockTime,
            XblAchievementsManagerSortOrder::Unsorted,
            &resultHandle
        ));
        VERIFY_ARE_EQUAL(E_INVALIDARG, hr);
    }
    
    DEFINE_TEST_CASE(GetAchievementsForUser_SortByUnlockTimeAscending_ExpectSortedAchievements)
    {
        TEST_LOG(L"Test starting: GetAchievementsForUser_SortByUnlockTimeAscending_ExpectSortedAchievements");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);

        // compare list of contents for equality
        HRESULT hr;
        XblAchievementsManagerResultHandle resultHandle;
        VERIFY_FAILED(hr = XblAchievementsManagerGetAchievements(
            xuid,
            XblAchievementOrderBy::DefaultOrder,
            XblAchievementsManagerSortOrder::Ascending,
            &resultHandle
        ));
        VERIFY_ARE_EQUAL(E_INVALIDARG, hr);
    }
    
    // add other test cases for sort and filter 

    DEFINE_TEST_CASE(GetAchievementsForUserByState_UnlockedSortFieldSpecifiedDefaultOrder_ExpectInvalidArg)
    {
        TEST_LOG(L"Test starting: GetAchievementsForUserByState_UnlockedSortFieldSpecifiedDefaultOrder_ExpectInvalidArg");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);

        // compare list of contents for equality
        HRESULT hr;
        XblAchievementsManagerResultHandle resultHandle;
        VERIFY_FAILED(hr = XblAchievementsManagerGetAchievementsByState(
            xuid,
            XblAchievementOrderBy::UnlockTime,
            XblAchievementsManagerSortOrder::Unsorted,
            XblAchievementProgressState::Achieved,
            &resultHandle
        ));
        VERIFY_ARE_EQUAL(E_INVALIDARG, hr);
    }

    DEFINE_TEST_CASE(GetAchievementsForUserByState_UnlockedOnlyDefaultSort_ExpectTwoResults)
    {
        TEST_LOG(L"Test starting: GetAchievementsForUserByState_UnlockedOnlyDefaultSort_ExpectTwoResults");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User(), getSortedAchievementsResponse);

        // compare list of contents for equality
        HRESULT hr;
        XblAchievementsManagerResultHandle resultHandle;
        const XblAchievement* achievements;
        uint64_t achievementsCount;
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerGetAchievementsByState(
            xuid,
            XblAchievementOrderBy::DefaultOrder,
            XblAchievementsManagerSortOrder::Unsorted,
            XblAchievementProgressState::Achieved,
            &resultHandle
        ));
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievements,
            &achievementsCount
        ));

        VERIFY_ARE_EQUAL_INT(2, achievementsCount);
        VERIFY_ARE_EQUAL_STR(achievementId2013MultipleRequirementsAchieved, achievements[0].id);
        VERIFY_ARE_EQUAL_STR(achievementId2017Achieved, achievements[1].id);
        XblAchievementsManagerResultCloseHandle(resultHandle);
    }

    DEFINE_TEST_CASE(GetAchievementsForUserByState_UnlockedOnlySortUnlockTimeAscending_ExpectTwoResults)
    {
        TEST_LOG(L"Test starting: GetAchievementsForUserByState_UnlockedOnlySortUnlockTimeAscending_ExpectTwoResults");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User(), getSortedAchievementsResponse);

        // compare list of contents for equality
        HRESULT hr;
        XblAchievementsManagerResultHandle resultHandle;
        const XblAchievement* achievements;
        uint64_t achievementsCount;
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerGetAchievementsByState(
            xuid,
            XblAchievementOrderBy::UnlockTime,
            XblAchievementsManagerSortOrder::Ascending,
            XblAchievementProgressState::Achieved,
            &resultHandle
        ));
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievements,
            &achievementsCount
        ));

        VERIFY_ARE_EQUAL_INT(2, achievementsCount);
        VERIFY_ARE_EQUAL_STR(achievementId2013MultipleRequirementsAchieved, achievements[0].id);
        VERIFY_ARE_EQUAL_STR(achievementId2017Achieved, achievements[1].id);
        XblAchievementsManagerResultCloseHandle(resultHandle);
    }

    DEFINE_TEST_CASE(GetAchievementsForUserByState_UnlockedOnlySortUnlockTimeDescending_ExpectTwoResults)
    {
        TEST_LOG(L"Test starting: GetAchievementsForUserByState_UnlockedOnlySortUnlockTimeDescending_ExpectTwoResults");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // add local user
        AddLocalUserSyncHelper(xblContext->User(), getSortedAchievementsResponse);

        // compare list of contents for equality
        HRESULT hr;
        XblAchievementsManagerResultHandle resultHandle;
        const XblAchievement* achievements;
        uint64_t achievementsCount;
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerGetAchievementsByState(
            xuid,
            XblAchievementOrderBy::UnlockTime,
            XblAchievementsManagerSortOrder::Descending,
            XblAchievementProgressState::Achieved,
            &resultHandle
        ));
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievements,
            &achievementsCount
        ));

        VERIFY_ARE_EQUAL_INT(2, achievementsCount);
        VERIFY_ARE_EQUAL_STR(achievementId2017Achieved, achievements[0].id);
        VERIFY_ARE_EQUAL_STR(achievementId2013MultipleRequirementsAchieved, achievements[1].id);
        XblAchievementsManagerResultCloseHandle(resultHandle);
    }

    DEFINE_TEST_CASE(UpdateAchievement_HigherProgressValue_ExpectProgressNotification)
    {
        TEST_LOG(L"Test starting: UpdateAchievement_HigherProgressValue_ExpectProgressNotification");

        AMTestEnvironment env{};
        constexpr uint8_t newProgress = 20;
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);
        
        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);
        
        XblAchievementsManagerResultHandle resultHandle;
        const XblAchievement* achievement;
        uint64_t size;
        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(xuid, achievementId2017NotStarted, &resultHandle));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));
        VERIFY_ARE_EQUAL_INT(
            utils::internal_string_to_uint32(achievement->progression.requirements[0].currentProgressValue),
            0
        );
        XblAchievementsManagerResultCloseHandle(resultHandle);
        resultHandle = nullptr;

        bool completedBeforeTimeout = UpdateAchievementHelper(xuid, achievementId2017NotStarted, newProgress);
        VERIFY_IS_TRUE(completedBeforeTimeout);

        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(xuid, achievementId2017NotStarted, &resultHandle));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));
        VERIFY_ARE_EQUAL_INT(
            utils::internal_string_to_uint32(achievement->progression.requirements[0].currentProgressValue),
            newProgress
        );
        XblAchievementsManagerResultCloseHandle(resultHandle);
        
    }

    DEFINE_TEST_CASE(UpdateAchievement_ExpectFailureInvalidArgs)
    {
        TEST_LOG(L"Test starting: UpdateAchievement_ExpectFailureInvalidArgs");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerUpdateAchievement(
            xuid,
            nullptr,
            10
        ));
        VERIFY_ARE_EQUAL(hr, E_INVALIDARG);
    }

    DEFINE_TEST_CASE(UpdateAchievement_UserDoesNotExist_ExpectFailureInvalidArgs)
    {
        TEST_LOG(L"Test starting: UpdateAchievement_UserDoesNotExist_ExpectFailureInvalidArgs");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerUpdateAchievement(
            2345,
            achievementId2017NotStarted,
            10
        ));
        VERIFY_ARE_EQUAL(hr, E_BOUNDS);
    }

    DEFINE_TEST_CASE(UpdateAchievement_UserNotInitialized_ExpectFailure)
    {
        TEST_LOG(L"Test starting: UpdateAchievement_UserNotInitialized_ExpectFailure");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        // not using the helper function here since we are interrupting

        XTaskQueueHandle queue{};

        HttpMock mock("GET", defaultAchievementsUri);
        mock.SetResponseBody(firstUserUpdateAchievementsResponse);

        VERIFY_SUCCEEDED(XblAchievementsManagerAddLocalUser(xblContext->User().Handle(), queue));

        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerUpdateAchievement(
            xuid,
            achievementId2017NotStarted,
            20
        ));
        VERIFY_ARE_EQUAL(hr, E_UNEXPECTED);
    }

    DEFINE_TEST_CASE(UpdateAchievement_AchievementDoesNotExist_ExpectFailureInvalidArgs)
    {
        TEST_LOG(L"Test starting: UpdateAchievement_AchievementDoesNotExist_ExpectFailureInvalidArgs");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);

        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerUpdateAchievement(
            xuid,
            achievementIdDoesNotExist,
            10
        ));
        VERIFY_ARE_EQUAL(hr, E_BOUNDS);
    }

    DEFINE_TEST_CASE(UpdateAchievement_Is2013AchievementMultipleRequirements_ExpectFailure)
    {
        TEST_LOG(L"Test starting: UpdateAchievement_Is2013AchievementMultipleRequirements_ExpectFailure");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);
        
        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerUpdateAchievement(
            xuid, 
            achievementId2013MultipleRequirements, 
            10
        ));
        VERIFY_ARE_EQUAL(hr, E_NOT_SUPPORTED);
    }
    
    DEFINE_TEST_CASE(UpdateAchievement_Is2013AchievementSingleRequirements_ExpectFailure)
    {
        TEST_LOG(L"Test starting: UpdateAchievement_Is2013AchievementSingleRequirements_ExpectFailure");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(1234);
        env.m_testContexts.push_back(xblContext);
        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);

        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerUpdateAchievement(
            xuid,
            achievementId2013SingleRequirement,
            10
        ));
        VERIFY_ARE_EQUAL(hr, E_NOT_SUPPORTED);
    }

    DEFINE_TEST_CASE(UpdateAchievement_LowerProgressValue_ExpectFailure)
    {
        TEST_LOG(L"Test starting: UpdateAchievement_LowerProgressValue_ExpectFailure");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;
        constexpr uint8_t updateProgress = 10;

        VERIFY_IS_TRUE(updateProgress < achievementInProgressStartProgress);

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);

        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerUpdateAchievement(
            xuid,
            achievementId2017InProgress,
            updateProgress
        ));
        VERIFY_ARE_EQUAL(hr, E_INVALIDARG);
    }

    DEFINE_TEST_CASE(UpdateAchievement_SameProgressValue_ExpectFailure)
    {
        TEST_LOG(L"Test starting: UpdateAchievement_SameProgressValue_ExpectFailure");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);
        
        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerUpdateAchievement(
            xuid,
            achievementId2017InProgress,
            achievementInProgressStartProgress
        ));
        VERIFY_ARE_EQUAL(hr, E_INVALIDARG);
    }

    DEFINE_TEST_CASE(UpdateAchievement_AchievementAlreadyUnlocked_ExpectFailure)
    {
        TEST_LOG(L"Test starting: UpdateAchievement_AchievementAlreadyUnlocked_ExpectFailure");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);
        
        HRESULT hr;
        VERIFY_FAILED(hr = XblAchievementsManagerUpdateAchievement(
            xuid,
            achievementId2013MultipleRequirementsAchieved,
            100
        ));
        VERIFY_ARE_EQUAL(hr, E_UNEXPECTED);
    }

    DEFINE_TEST_CASE(UnlockAchievement_CompleteProgressWithDeducedUnlockNotification_ExpectProgressAndUnlockNotifications)
    {
        TEST_LOG(L"Test starting: UnlockAchievement_CompleteProgressWithDeducedUnlockNotification_ExpectProgressAndUnlockNotifications");

        UpdateAchievementSuccessHelper();
    }

    DEFINE_TEST_CASE(UnlockAchievement_MultipleUsersBothInProgressCompleteOne_ExpectBothNotifications)
    {
        TEST_LOG(L"Test starting: UnlockAchievement_MultipleUsersBothInProgressCompleteOne_ExpectBothNotifications");

        DEFINE_TEST_CASE_PROPERTIES_FOCUS();

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        Map<std::shared_ptr<XblContext>, const char*> responseMap;
        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);
        responseMap[xblContext] = firstUserUpdateAchievementsResponse;
        xblContext = env.CreateMockXboxLiveContext(2345);
        env.m_testContexts.push_back(xblContext);
        responseMap[xblContext] = firstUserUpdateAchievementsResponse;

        // add local users
        AddMultipleLocalUsersHelper(responseMap);

        XblAchievementsManagerResultHandle resultHandle;
        const XblAchievement* achievement;
        uint64_t size;
        HRESULT hr;

        VERIFY_SUCCEEDED(hr = XblAchievementsManagerGetAchievement(
            xuid,
            achievementId2017InProgress,
            &resultHandle
        ));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));
        VERIFY_IS_TRUE(achievement->progressState == XblAchievementProgressState::InProgress);
        VERIFY_IS_TRUE(
            utils::internal_string_to_uint32(achievement->progression.requirements[0].currentProgressValue)
            == achievementInProgressStartProgress
        );
        
        Vector<std::shared_ptr<HttpMock>> mocks;
        SetUpUpdateAchievementRtaResponse(2345, rtaAchievementProgressChangedCompletePayload, true, mocks);

        bool completedBeforeTimeout = UpdateAchievementHelper(2345, achievementId2017InProgress, 100, false, true);
        VERIFY_SUCCEEDED(completedBeforeTimeout);
        
        // make sure user 1234 doesn't show any changes from the event.
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerGetAchievement(
            xuid,
            achievementId2017InProgress,
            &resultHandle
        ));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));
        VERIFY_IS_TRUE(achievement->progressState == XblAchievementProgressState::InProgress);
        VERIFY_IS_TRUE(
            utils::internal_string_to_uint32(achievement->progression.requirements[0].currentProgressValue)
            == achievementInProgressStartProgress
        );
        XblAchievementsManagerResultCloseHandle(resultHandle);
        resultHandle = nullptr;

        // make sure user 2345 actually has the correct changes 
        VERIFY_SUCCEEDED(hr = XblAchievementsManagerGetAchievement(
            2345,
            achievementId2017InProgress,
            &resultHandle
        ));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));
        VERIFY_IS_TRUE(achievement->progressState == XblAchievementProgressState::Achieved);
        VERIFY_ARE_EQUAL_STR(
            achievement->progression.requirements[0].currentProgressValue,
            achievement->progression.requirements[0].targetProgressValue
        );
        VERIFY_ARE_EQUAL_INT(
            utils::internal_string_to_uint32(achievement->progression.requirements[0].currentProgressValue),
            100
        );
        VERIFY_IS_TRUE(
            utils::TimeTFromDatetime(datetime::from_string(achievementUnlockTime, datetime::date_format::ISO_8601))
            == achievement->progression.timeUnlocked
        );
        XblAchievementsManagerResultCloseHandle(resultHandle);
    }
    
    DEFINE_TEST_CASE(AchievementsManagerResync_SingleUserNoAchievementsChanged_GeneratesNoEvents)
    {
        TEST_LOG(L"Test starting: AchievementsManagerResync_SingleUserNoAchievementsChanged_GeneratesNoEvents");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User(), firstUserUpdateAchievementsResponse);
        
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };
        mockRtaService.RaiseResync();

        Sleep(300);

        const XblAchievementsManagerEvent* events;
        size_t eventCount;
        VERIFY_SUCCEEDED(XblAchievementsManagerDoWork(&events, &eventCount));
        VERIFY_ARE_EQUAL_INT(eventCount, 0);
    }
    
    DEFINE_TEST_CASE(AchievementsManagerResync_SingleUserOneAchievementChanged_SuccessfullyUpdatedAndGeneratesEvent)
    {
        TEST_LOG(L"Test starting: AchievementsManagerResync_SingleUserOneAchievementChanged_SuccessfullyUpdatedAndGeneratesEvent");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User(), firstUserGetAchievementsResponse);
        
        HttpMock mock("GET", defaultAchievementsUri);
        mock.SetResponseBody(secondUserGetAchievementsResponse);
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };
        mockRtaService.RaiseResync();
        
        Sleep(200);
        
        uint32_t count = 0;
        const XblAchievementsManagerEvent* events = nullptr;
        size_t eventCount = 0;

        while (eventCount == 0)
        {
            if (++count > 500)
            {
                break;
            }
            VERIFY_SUCCEEDED(XblAchievementsManagerDoWork(&events, &eventCount));
            if (eventCount == 0)
            {
                Sleep(20);
            }
        }
        VERIFY_ARE_EQUAL_INT(eventCount, 1);
        VERIFY_IS_TRUE(events[0].eventType == XblAchievementsManagerEventType::AchievementProgressUpdated);
    }

    DEFINE_TEST_CASE(AchievementsManagerResync_MultipleUsersOneAchievementChanged_SuccessfullyUpdatedAndGeneratesEventPerUser)
    {
        TEST_LOG(L"Test starting: AchievementsManagerResync_MultipleUsersOneAchievementChanged_SuccessfullyUpdatedAndGeneratesEventPerUser");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        Map<std::shared_ptr<XblContext>, const char*> responseMap;
        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);
        responseMap[xblContext] = firstUserResyncNotStartedResponse;
        xblContext = env.CreateMockXboxLiveContext(2345);
        env.m_testContexts.push_back(xblContext);
        responseMap[xblContext] = firstUserGetAchievementsResponse;

        // add local users
        AddMultipleLocalUsersHelper(responseMap);

        Vector<std::shared_ptr<HttpMock>> mocks;
        SetupRtaResponseForUser(xuid, "GET", firstUserResyncFinishedResponse, mocks);
        SetupRtaResponseForUser(2345, "GET", secondUserGetAchievementsResponse, mocks);

        auto& mockRtaService{ MockRealTimeActivityService::Instance() };
        mockRtaService.RaiseResync();

        Sleep(200);

        uint32_t count = 0;
        const XblAchievementsManagerEvent* events = nullptr;
        size_t eventCount = 0;
        size_t totalEventCount = 0;
        uint8_t progressEvents = 0, unlockEvents = 0;
        uint8_t firstUserEvents = 0, secondUserEvents = 0;

        while (!(progressEvents == 2 && unlockEvents == 1))
        {
            if (++count > 500)
            {
                break;
            }
            VERIFY_SUCCEEDED(XblAchievementsManagerDoWork(&events, &eventCount));
            totalEventCount += eventCount;
            for (uint8_t i = 0; i < eventCount; ++i)
            {
                switch (events[i].eventType)
                {
                case XblAchievementsManagerEventType::AchievementProgressUpdated:
                    ++progressEvents;
                    break;
                case XblAchievementsManagerEventType::AchievementUnlocked:
                    ++unlockEvents;
                    break;
                default:
                    break;
                }

                if (events[i].xboxUserId == xuid)
                {
                    ++firstUserEvents;
                }
                else
                {
                    ++secondUserEvents;
                }
            }
            
            Sleep(20);
        }
        VERIFY_ARE_EQUAL_INT(3, totalEventCount);
        VERIFY_ARE_EQUAL_INT(2, progressEvents);
        VERIFY_ARE_EQUAL_INT(1, unlockEvents);
        VERIFY_ARE_EQUAL_INT(2, firstUserEvents);
        VERIFY_ARE_EQUAL_INT(1, secondUserEvents);
        
        XblAchievementsManagerResultHandle resultHandle;
        const XblAchievement* achievement;
        uint64_t size;

        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(2345, achievementId2017NotStarted, &resultHandle));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));

        VERIFY_IS_TRUE(achievement->progressState == XblAchievementProgressState::InProgress);
        VERIFY_ARE_EQUAL_INT(
            utils::internal_string_to_uint32(achievement->progression.requirements[0].currentProgressValue),
            achievementInProgressStartProgress
        );

        XblAchievementsManagerResultCloseHandle(resultHandle);
        resultHandle = nullptr;

        VERIFY_SUCCEEDED(XblAchievementsManagerGetAchievement(xuid, achievementId2013MultipleRequirementsAchieved, &resultHandle));
        VERIFY_SUCCEEDED(XblAchievementsManagerResultGetAchievements(
            resultHandle,
            &achievement,
            &size
        ));

        VERIFY_IS_TRUE(achievement->progressState == XblAchievementProgressState::Achieved);
        for (uint8_t i = 0; i < achievement->progression.requirementsCount; ++i)
        {
            VERIFY_ARE_EQUAL_STR(
                achievement->progression.requirements[i].currentProgressValue,
                achievement->progression.requirements[i].targetProgressValue
                );
            VERIFY_ARE_EQUAL_INT(
                utils::internal_string_to_uint32(achievement->progression.requirements[i].currentProgressValue),
                100
            );
        }
        VERIFY_IS_TRUE(
            utils::TimeTFromDatetime(datetime::from_string(achievementUnlockTime, datetime::date_format::ISO_8601))
            == achievement->progression.timeUnlocked
        );
        XblAchievementsManagerResultCloseHandle(resultHandle);
    }

    /*DEFINE_TEST_CASE(AchievementsManagerResync_SingleUserOneUpdateAlreadyReceivedDifferentValues_SuccessfullyUpdatedOnlyGeneratesOneEvent)
    {
        TEST_LOG(L"Test starting: AchievementsManagerResync_SingleUserOneUpdateAlreadyReceivedDifferentValues_SuccessfullyUpdatedOnlyGeneratesOneEvent");

        // want to see what happens when one achievement is larger than another... could
        // generate two events, but without guarantee that value can be cast to an int,
        // cant really assume it will be larager to allow us to be able to discard it.
    }*/
    
    DEFINE_TEST_CASE(AchievementsManagerResync_SingleUserOneUpdateAlreadyReceivedSameValues_SuccessfullyUpdatedOnlyGeneratesOneEvent)
    {
        TEST_LOG(L"Test starting: AchievementsManagerResync_SingleUserOneUpdateAlreadyReceivedSameValues_SuccessfullyUpdatedOnlyGeneratesOneEvent");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User(), firstUserGetAchievementsResponse);

        VERIFY_SUCCEEDED(XblAchievementsManagerUpdateAchievement(xuid, achievementId2017NotStarted, achievementInProgressStartProgress));

        HttpMock mock("GET", defaultAchievementsUri);
        mock.SetResponseBody(secondUserGetAchievementsResponse);
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };
        mockRtaService.RaiseResync();

        Sleep(200);

        uint32_t count = 0;
        const XblAchievementsManagerEvent* events = nullptr;
        size_t eventCount = 0;
        size_t totalEventCount = 0;

        while (++count < 100)
        {
            VERIFY_SUCCEEDED(XblAchievementsManagerDoWork(&events, &eventCount));
            totalEventCount += eventCount;
            if (totalEventCount < 2)
            {
                Sleep(10);
            }
        }
        VERIFY_ARE_EQUAL_INT(totalEventCount, 1);
    }

    DEFINE_TEST_CASE(RtaConnectionDropped_OneAchievementChanged_StateResyncedCorrectly)
    {
        TEST_LOG(L"Test starting: RtaConnectionDropped_OneAchievementChanged_StateResyncedCorrectly");

        AMTestEnvironment env{};
        constexpr uint64_t xuid = 1234;

        auto xblContext = env.CreateMockXboxLiveContext(xuid);
        env.m_testContexts.push_back(xblContext);

        AddLocalUserSyncHelper(xblContext->User(), firstUserGetAchievementsResponse);

        HttpMock mock("GET", defaultAchievementsUri);
        mock.SetResponseBody(secondUserGetAchievementsResponse);
        auto& mockRtaService{ MockRealTimeActivityService::Instance() };
        mockRtaService.DisconnectClient(xuid);

        Sleep(200);

        uint32_t count = 0;
        const XblAchievementsManagerEvent* events = nullptr;
        size_t eventCount = 0;
        size_t totalEventCount = 0;

        while (++count < 100)
        {
            VERIFY_SUCCEEDED(XblAchievementsManagerDoWork(&events, &eventCount));
            totalEventCount += eventCount;
            if (totalEventCount < 2)
            {
                Sleep(10);
            }
        }
        VERIFY_ARE_EQUAL_INT(totalEventCount, 1);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END
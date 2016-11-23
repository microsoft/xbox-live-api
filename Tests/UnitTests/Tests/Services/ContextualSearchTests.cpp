//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"ContextualSearch"
#include "UnitTestIncludes.h"
#include "xsapi/services.h"

using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::System;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation::Collections;
using namespace xbox::services::contextual_search;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring g_jsonConfigTestResponse =
LR"(
{
    "titleId" : 1144039106,
    "sandboxId" : "RETAIL",
    "serviceconfigs" : 
    [
        {
            "scid" : "77290100-225e-4768-9373-98164430a9f8",
            "stats" : 
            [{
                "name" : "testname1",
                "type" : "Integer",
                "visibility" : "Public"
            }, 
            {
                "name" : "testname2",
                "type" : "Integer",
                "visibility" : "Private"
            }, 
            {
                "name" : "testname3",
                "type" : "Integer",
                "visibility" : "Public"
            }, 
            {
                "name" : "testname4",
                "type" : "Integer",
                "visibility" : "Public"
            }],
            "metadata" : 
            {
                "stats" : 
                [   
                    {
                        "name" : "testname1",
                        "displayName" : "GameType",
                        "filter" : true,
                        "sort" : false,
                        "type" : "Set",
                        "setName" : "GameType",
                        "rangeValues" : null
                    }, 
                    {
                        "name" : "testname2",
                        "displayName" : "Map",
                        "filter" : true,
                        "sort" : false,
                        "type" : "Set",
                        "setName" : "MapType",
                        "rangeValues" : null
                    },
                    {
                        "name" : "testname3",
                        "displayName" : "TestName3",
                        "filter" : false,
                        "sort" : true,
                        "type" : "Range",
                        "rangeValues" : 
                        {
                            "min" : 1,
                            "max" : 50
                        }
                    },
                    {
                        "name" : "testname4",
                        "displayName" : "TestName4",
                        "filter" : false,
                        "sort" : true,
                        "type" : "UndefinedRange",
                        "rangeValues" : null
                    }
                ]
            },
            "sets" :
            [
                {
                    "name" : "testname1",
                    "values" :
                    [
                        {
                            "memberStatValue" : "0",
                            "memberDisplayName" : "CTF"
                        },
                        {
                            "memberStatValue" : "1",
                            "memberDisplayName" : "Slayer"
                        }
                    ]
                },
                {
                    "name" : "testname2",
                    "values" :
                    [
                        {
                            "memberStatValue" : "10",
                            "memberDisplayName" : "Test1"
                        },
                        {
                            "memberStatValue" : "11",
                            "memberDisplayName" : "Test2"
                        }
                    ]
                }
            ]
        }
    ]
}
)";

const std::wstring g_jsonBroadcastTestResponse =
LR"(
{
    "count":2,
    "value":
    [
        {
            "xuid":"12345",
            "provider":"provider1",
            "id":"testid1",
            "viewers":66,
            "started":"2015-01-08T21:10:33.798Z",
            "stats":
            {
                "testname1":"0",
                "testname2":"161",
                "testname3":"1420751955370",
                "testname4":"0"
            }
        },
        {
            "xuid":"123456",
            "provider":"provider2",
            "id":"testid2",
            "viewers":15,
            "started":"2015-01-08T18:21:15.818Z",
            "stats":
            {
                "testname1":"1",
                "testname2":"12",
                "testname3":"1420751870350",
                "testname4":"0"
            }
        }
    ]
}
)";


const std::wstring g_jsonGameClipsTestResponse =
LR"(
{
    "count": 1,
    "value": 
    [
        {
            "gameClipId": "66851e40-a72c-4b40-8ab3-24583c9c6244",
            "type": "UserGenerated",
            "durationInSeconds": 30,
            "views": 7453,
            "xuid": 2814654849282382,
            "clipName": "[en-US] Localized Greatest Moment Name",
            "gameClipLocale": "en-US",
            "thumbnails": 
            [
                {
                    "Uri": "https://gameclipstransfer.xboxlive.com/users/xuid(2814654849282382)/scids/1ff15dd1-9165-4b01-81a6-72149927a85c/clips/66851e40-a72c-4b40-8ab3-24583c9c6244/thumbnails/small",
                    "FileSize": 41557,
                    "ThumbnailType": "Small"
                },
                {
                    "Uri": "https://gameclipstransfer.xboxlive.com/users/xuid(2814654849282382)/scids/1ff15dd1-9165-4b01-81a6-72149927a85c/clips/66851e40-a72c-4b40-8ab3-24583c9c6244/thumbnails/large",
                    "FileSize": 157771,
                    "ThumbnailType": "Large"
                }
            ],
            "gameClipUris": 
            [
                {
                    "Uri": "https://gameclipstransfer.xboxlive.com/users/xuid(2814654849282382)/scids/1ff15dd1-9165-4b01-81a6-72149927a85c/clips/66851e40-a72c-4b40-8ab3-24583c9c6244",
                    "FileSize": 2996766,
                    "UriType": "Download",
                    "Expiration": "2016-05-13T00:46:19.532851Z"
                }
            ],
            "stats": 
            [
                {
                    "statName":"gametype",
                    "value" : "0.1"
                },
                {
                    "statName":"startmap",
                    "value" : "161"
                },
                {
                    "statName":"matchstarttime",
                    "value" : "1420751955370"
                },
                {
                    "statName":"rank",
                    "min" : "0",
                    "max" : "1",
                    "delta" : "1"
                }
            ]
        }
    ]
}
)";



DEFINE_TEST_CLASS(ContextualSearchTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(ContextualSearchTests)

    DEFINE_TEST_CASE(TestConfig)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestConfig);
        auto mockXblContext = GetMockXboxLiveContext_Cpp();
        uint32 titleId = 1234;

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(g_jsonConfigTestResponse), 200);

        mockXblContext->contextual_search_service().get_configuration(titleId)
        .then([](xbox_live_result<std::vector<contextual_search_configured_stat>> result)
        {
            VERIFY_IS_TRUE(result.err() == xbox_live_error_code::no_error);
            auto val = result.payload();
            VERIFY_ARE_EQUAL_INT(4, val.size());

            auto setItem = val[0];
            VERIFY_ARE_EQUAL_STR(L"testname1", setItem.name());
            VERIFY_ARE_EQUAL_STR(L"Integer", setItem.data_type());
            VERIFY_ARE_EQUAL_INT(contextual_search_stat_visibility::public_visibility, setItem.visibility());
            VERIFY_ARE_EQUAL_STR(L"GameType", setItem.display_name());
            VERIFY_ARE_EQUAL(true, setItem.can_be_filtered());
            VERIFY_ARE_EQUAL(false, setItem.can_be_sorted());
            VERIFY_ARE_EQUAL_INT(contextual_search_stat_display_type::set, setItem.display_type());
            VERIFY_ARE_EQUAL_INT(2, setItem.value_to_display_name_map().size());
            VERIFY_ARE_EQUAL_STR(L"CTF", setItem.value_to_display_name_map().at(L"0"));
            VERIFY_ARE_EQUAL_STR(L"Slayer", setItem.value_to_display_name_map().at(L"1"));
            VERIFY_ARE_EQUAL_INT(0, setItem.range_max());
            VERIFY_ARE_EQUAL_UINT(0, setItem.range_min());

            setItem = val[1];
            VERIFY_ARE_EQUAL_STR(L"testname2", setItem.name());
            VERIFY_ARE_EQUAL_STR(L"Integer", setItem.data_type());
            VERIFY_ARE_EQUAL_INT(contextual_search_stat_visibility::private_visibility, setItem.visibility());
            VERIFY_ARE_EQUAL_STR(L"Map", setItem.display_name());
            VERIFY_ARE_EQUAL(true, setItem.can_be_filtered());
            VERIFY_ARE_EQUAL(false, setItem.can_be_sorted());
            VERIFY_ARE_EQUAL_INT(contextual_search_stat_display_type::set, setItem.display_type());
            VERIFY_ARE_EQUAL_INT(2, setItem.value_to_display_name_map().size());
            VERIFY_ARE_EQUAL_STR(L"Test1", setItem.value_to_display_name_map().at(L"10"));
            VERIFY_ARE_EQUAL_STR(L"Test2", setItem.value_to_display_name_map().at(L"11"));
            VERIFY_ARE_EQUAL_UINT(0, setItem.range_max());
            VERIFY_ARE_EQUAL_UINT(0, setItem.range_min());

            setItem = val[2];
            VERIFY_ARE_EQUAL_STR(L"testname3", setItem.name());
            VERIFY_ARE_EQUAL_STR(L"Integer", setItem.data_type());
            VERIFY_ARE_EQUAL_INT(contextual_search_stat_visibility::public_visibility, setItem.visibility());
            VERIFY_ARE_EQUAL_STR(L"TestName3", setItem.display_name());
            VERIFY_ARE_EQUAL(false, setItem.can_be_filtered());
            VERIFY_ARE_EQUAL(true, setItem.can_be_sorted());
            VERIFY_ARE_EQUAL_INT(contextual_search_stat_display_type::defined_range, setItem.display_type());
            VERIFY_ARE_EQUAL_INT(0, setItem.value_to_display_name_map().size());
            VERIFY_ARE_EQUAL_UINT(1, setItem.range_min());
            VERIFY_ARE_EQUAL_UINT(50, setItem.range_max());

            setItem = val[3];
            VERIFY_ARE_EQUAL_STR(L"testname4", setItem.name());
            VERIFY_ARE_EQUAL_STR(L"Integer", setItem.data_type());
            VERIFY_ARE_EQUAL_INT(contextual_search_stat_visibility::public_visibility, setItem.visibility());
            VERIFY_ARE_EQUAL_STR(L"TestName4", setItem.display_name());
            VERIFY_ARE_EQUAL(false, setItem.can_be_filtered());
            VERIFY_ARE_EQUAL(true, setItem.can_be_sorted());
            VERIFY_ARE_EQUAL_INT(contextual_search_stat_display_type::undefined_range, setItem.display_type());
            VERIFY_ARE_EQUAL_INT(0, setItem.value_to_display_name_map().size());

        }).wait();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/1234/configuration", httpCall->PathQueryFragment.to_string());
    }

    static void VerifyBroadcastResult(std::vector<contextual_search_broadcast> val)
    {
        VERIFY_ARE_EQUAL_INT(val.size(), 2);

        auto item = val[0];
        VERIFY_ARE_EQUAL_STR(L"12345", item.xbox_user_id());
        VERIFY_ARE_EQUAL_STR(L"provider1", item.provider());
        VERIFY_ARE_EQUAL_STR(L"testid1", item.broadcaster_id_from_provider());
        VERIFY_ARE_EQUAL_UINT(66, item.viewers());
        VERIFY_ARE_EQUAL_STR(L"Thu, 08 Jan 2015 21:10:33 GMT", item.started_date().to_string());
        VERIFY_ARE_EQUAL_INT(4, item.current_stats().size());
        VERIFY_ARE_EQUAL_STR(L"0", item.current_stats().at(L"testname1"));
        VERIFY_ARE_EQUAL_STR(L"161", item.current_stats().at(L"testname2"));
        VERIFY_ARE_EQUAL_STR(L"1420751955370", item.current_stats().at(L"testname3"));
        VERIFY_ARE_EQUAL_STR(L"0", item.current_stats().at(L"testname4"));

        item = val[1];
        VERIFY_ARE_EQUAL_STR(L"123456", item.xbox_user_id());
        VERIFY_ARE_EQUAL_STR(L"provider2", item.provider());
        VERIFY_ARE_EQUAL_STR(L"testid2", item.broadcaster_id_from_provider());
        VERIFY_ARE_EQUAL_UINT(15, item.viewers());
        VERIFY_ARE_EQUAL_STR(L"Thu, 08 Jan 2015 18:21:15 GMT", item.started_date().to_string());
        VERIFY_ARE_EQUAL_INT(4, item.current_stats().size());
        VERIFY_ARE_EQUAL_STR(L"1", item.current_stats().at(L"testname1"));
        VERIFY_ARE_EQUAL_STR(L"12", item.current_stats().at(L"testname2"));
        VERIFY_ARE_EQUAL_STR(L"1420751870350", item.current_stats().at(L"testname3"));
        VERIFY_ARE_EQUAL_STR(L"0", item.current_stats().at(L"testname4"));
    }

    DEFINE_TEST_CASE(TestGetBroadcasts)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetBroadcasts);
        auto mockXblContext = GetMockXboxLiveContext_Cpp();
        uint32 titleId = 1234;

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(g_jsonBroadcastTestResponse), 200);

        mockXblContext->contextual_search_service().get_broadcasts(titleId)
        .then([](xbox_live_result<std::vector<contextual_search_broadcast>> result)
        {
            VERIFY_IS_TRUE(result.err() == xbox_live_error_code::no_error);
            auto val = result.payload();
            VerifyBroadcastResult(val);

        }).wait();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/1234/broadcasts", httpCall->PathQueryFragment.to_string());
    }

    DEFINE_TEST_CASE(TestGetBroadcastsWithFilter)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetBroadcastsWithFilter);
        auto mockXblContext = GetMockXboxLiveContext_Cpp();
        uint32 titleId = 1234;

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(g_jsonBroadcastTestResponse), 200);

        mockXblContext->contextual_search_service().get_broadcasts(
            titleId,
            10,
            20,
            L"orderStat1",
            true,
            L"stats:orderStat1 eq 2"
            )
        .then([](xbox_live_result<std::vector<contextual_search_broadcast>> result)
        {
            VERIFY_IS_TRUE(result.err() == xbox_live_error_code::no_error);
            auto val = result.payload();
            VerifyBroadcastResult(val);

        }).wait();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/1234/broadcasts?$skip=10&$top=20&$orderby=stats:orderStat1%20asc&$filter=stats:orderStat1%20eq%202", httpCall->PathQueryFragment.to_string());
    }

    DEFINE_TEST_CASE(TestGetBroadcastsWithFixedFilter)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetBroadcastsWithFixedFilter);
        auto mockXblContext = GetMockXboxLiveContext_Cpp();
        uint32 titleId = 1234;

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(g_jsonBroadcastTestResponse), 200);

        mockXblContext->contextual_search_service().get_broadcasts(
            titleId,
            10,
            20,
            L"orderStat1",
            true,
            L"orderStat1",
            contextual_search_filter_operator::greater_than_or_equal,
            L"jason's favs"
            )
        .then([](xbox_live_result<std::vector<contextual_search_broadcast>> result)
        {
            VERIFY_IS_TRUE(result.err() == xbox_live_error_code::no_error);
            auto val = result.payload();
            VerifyBroadcastResult(val);

        }).wait();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/1234/broadcasts?$skip=10&$top=20&$orderby=stats:orderStat1%20asc&$filter=stats:orderStat1%20ge%20'jason''s%20favs'", httpCall->PathQueryFragment.to_string());
    }

    DEFINE_TEST_CASE(TestGetGameclips)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetGameclips);
        auto mockXblContext = GetMockXboxLiveContext_Cpp();
        uint32 titleId = 1234;

        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(web::json::value::parse(g_jsonGameClipsTestResponse), 200);

        mockXblContext->contextual_search_service().get_game_clips(
            titleId,
            1,
            10,
            L"orderStat1",
            true,
            L"orderStat1",
            contextual_search_filter_operator::greater_than_or_equal,
            L"jason's favs"
            )
        .then([httpCall](xbox_live_result<contextual_search_game_clips_result> result)
        {
            VERIFY_IS_TRUE(result.err() == xbox_live_error_code::no_error);
            auto val = result.payload();
            VerifyGameClipResult(val);
           
            VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
            VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
            VERIFY_ARE_EQUAL_STR(L"/titles/1234/gameclips?$top=10&$skip=1&$orderby=stats:orderStat1%20asc&$filter=stats:orderStat1%20ge%20'jason''s%20favs'", httpCall->PathQueryFragment.to_string());

            auto resultNext = create_task(val.get_next(10)).get();
            VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
            VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
            VERIFY_ARE_EQUAL_STR(L"/titles/1234/gameclips?$top=10&$skip=2", httpCall->PathQueryFragment.to_string());
            val = result.payload();
            VerifyGameClipResult(val);
        }).wait();
    }

    static void VerifyGameClipResult(const contextual_search_game_clips_result& val)
    {
        VERIFY_ARE_EQUAL_INT(val.items().size(), 1);
        if (val.items().size() == 0) return;

        const contextual_search_game_clip& clip = val.items()[0];

        VERIFY_ARE_EQUAL_STR(L"[en-US] Localized Greatest Moment Name", clip.clip_name());
        VERIFY_ARE_EQUAL_UINT(30, clip.duration_in_seconds());
        VERIFY_ARE_EQUAL_STR(L"66851e40-a72c-4b40-8ab3-24583c9c6244", clip.game_clip_id());
        VERIFY_ARE_EQUAL_STR(L"en-US", clip.game_clip_locale());
        VERIFY_ARE_EQUAL_STR(L"2814654849282382", clip.xbox_user_id());
        VERIFY_ARE_EQUAL_UINT(7453, clip.views());
        VERIFY_ARE_EQUAL_UINT(contextual_search_game_clip_type::user_generated, clip.game_clip_type());

        VERIFY_ARE_EQUAL_INT(clip.thumbnails().size(), 2);
        const contextual_search_game_clip_thumbnail& th0 = clip.thumbnails()[0];
        VERIFY_ARE_EQUAL_STR(L"https://gameclipstransfer.xboxlive.com/users/xuid(2814654849282382)/scids/1ff15dd1-9165-4b01-81a6-72149927a85c/clips/66851e40-a72c-4b40-8ab3-24583c9c6244/thumbnails/small", th0.url().to_string());
        VERIFY_ARE_EQUAL_UINT(41557, th0.file_size());
        VERIFY_ARE_EQUAL_UINT(contextual_search_game_clip_thumbnail_type::small_thumbnail, th0.thumbnail_type());
        const contextual_search_game_clip_thumbnail& th1 = clip.thumbnails()[1];
        VERIFY_ARE_EQUAL_STR(L"https://gameclipstransfer.xboxlive.com/users/xuid(2814654849282382)/scids/1ff15dd1-9165-4b01-81a6-72149927a85c/clips/66851e40-a72c-4b40-8ab3-24583c9c6244/thumbnails/large", th1.url().to_string());
        VERIFY_ARE_EQUAL_UINT(157771, th1.file_size());
        VERIFY_ARE_EQUAL_UINT(contextual_search_game_clip_thumbnail_type::large_thumbnail, th1.thumbnail_type());

        VERIFY_ARE_EQUAL_INT(clip.game_clip_uris().size(), 1);
        const contextual_search_game_clip_uri_info& sc0 = clip.game_clip_uris()[0];
        VERIFY_ARE_EQUAL_STR(L"https://gameclipstransfer.xboxlive.com/users/xuid(2814654849282382)/scids/1ff15dd1-9165-4b01-81a6-72149927a85c/clips/66851e40-a72c-4b40-8ab3-24583c9c6244", sc0.url().to_string());
        VERIFY_ARE_EQUAL_UINT(2996766, sc0.file_size());
        VERIFY_ARE_EQUAL_UINT(contextual_search_game_clip_uri_type::download, sc0.uri_type());
        VERIFY_ARE_EQUAL_STR(L"Fri, 13 May 2016 00:46:19 GMT", sc0.expiration().to_string());

        VERIFY_ARE_EQUAL_INT(4, clip.stats().size());
        const contextual_search_game_clip_stat& stat0 = clip.stats()[0];
        const contextual_search_game_clip_stat& stat3 = clip.stats()[3];
        VERIFY_ARE_EQUAL_STR(L"gametype", stat0.stat_name());
        VERIFY_ARE_EQUAL_STR(L"0.1", stat0.value());
        VERIFY_ARE_EQUAL_STR(L"rank", stat3.stat_name());
        VERIFY_ARE_EQUAL_STR(L"0", stat3.min_value()); 
        VERIFY_ARE_EQUAL_STR(L"1", stat3.max_value());
        VERIFY_ARE_EQUAL_STR(L"1", stat3.delta_value());
    }

    DEFINE_TEST_CASE(TestGetConfigurationAsyncWinRT)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestConfigWinRT);
        uint32 titleId = 1234;

        auto responseJson = web::json::value::parse(g_jsonConfigTestResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();
        auto result = create_task(xboxLiveContext->ContextualSearchService->GetConfigurationAsync(
            titleId
        )).get();

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/1234/configuration", httpCall->PathQueryFragment.to_string());

        VERIFY_ARE_EQUAL_INT(4, result->Size);

        ContextualSearch::ContextualSearchConfiguredStat^ setItem = result->GetAt(0);
        VERIFY_ARE_EQUAL_STR(L"testname1", setItem->Name);
        VERIFY_ARE_EQUAL_STR(L"Integer", setItem->DataType);
        VERIFY_ARE_EQUAL_INT(ContextualSearch::ContextualSearchStatVisibility::PublicVisibility, setItem->Visibility);
        VERIFY_ARE_EQUAL_STR(L"GameType", setItem->DisplayName);
        VERIFY_ARE_EQUAL(true, setItem->CanBeFiltered);
        VERIFY_ARE_EQUAL(false, setItem->CanBeSorted);
        VERIFY_ARE_EQUAL_INT(ContextualSearch::ContextualSearchStatDisplayType::Set, setItem->DisplayType);
        VERIFY_ARE_EQUAL_INT(2, setItem->ValueToDisplayName->Size);
        VERIFY_ARE_EQUAL_STR(L"CTF", setItem->ValueToDisplayName->Lookup(L"0"));
        VERIFY_ARE_EQUAL_STR(L"Slayer", setItem->ValueToDisplayName->Lookup(L"1"));
        VERIFY_ARE_EQUAL_INT(0, setItem->RangeMax);
        VERIFY_ARE_EQUAL_UINT(0, setItem->RangeMin);
    }

    DEFINE_TEST_CASE(TestGetBroadcastsAsyncWinRT)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestBroadcastsWinRT);
        uint32 titleId = 1234;

        auto responseJson = web::json::value::parse(g_jsonBroadcastTestResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(xboxLiveContext->ContextualSearchService->GetBroadcastsAsync(titleId)).get();

        VERIFY_ARE_EQUAL_INT(result->Size, 2);

        ContextualSearch::ContextualSearchBroadcast^ item = result->GetAt(0);
        VERIFY_ARE_EQUAL_STR(L"12345", item->XboxUserId);
        VERIFY_ARE_EQUAL_STR(L"provider1", item->Provider);
        VERIFY_ARE_EQUAL_STR(L"testid1", item->BroadcasterIdFromProvider);
        VERIFY_ARE_EQUAL_UINT(66, item->Viewers);
        VERIFY_ARE_EQUAL_INT(4, item->CurrentStats->Size);
        VERIFY_ARE_EQUAL_STR(L"0", item->CurrentStats->Lookup(L"testname1"));
        VERIFY_ARE_EQUAL_STR(L"161", item->CurrentStats->Lookup(L"testname2"));
        VERIFY_ARE_EQUAL_STR(L"1420751955370", item->CurrentStats->Lookup(L"testname3"));
        VERIFY_ARE_EQUAL_STR(L"0", item->CurrentStats->Lookup(L"testname4"));

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/1234/broadcasts", httpCall->PathQueryFragment.to_string());
    }

    DEFINE_TEST_CASE(TestGetBroadcastsAsyncWinRTFiltered)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetBroadcastsAsyncWinRTFiltered);
        uint32 titleId = 1234;

        auto responseJson = web::json::value::parse(g_jsonBroadcastTestResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(xboxLiveContext->ContextualSearchService->GetBroadcastsAsync(
            titleId,
            10,
            20,
            L"orderStat1",
            true,
            L"stats:orderStat1 eq 2"
        )).get();

        VERIFY_ARE_EQUAL_INT(result->Size, 2);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/1234/broadcasts?$skip=10&$top=20&$orderby=stats:orderStat1%20asc&$filter=stats:orderStat1%20eq%202", httpCall->PathQueryFragment.to_string());
    }

    DEFINE_TEST_CASE(TestGetBroadcastsAsyncWinRTFilteredFixed)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGetBroadcastsAsyncWinRTFiltered);
        uint32 titleId = 1234;

        auto responseJson = web::json::value::parse(g_jsonBroadcastTestResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(xboxLiveContext->ContextualSearchService->GetBroadcastsAsync(
            titleId,
            10,
            20,
            L"orderStat1",
            true,
            L"stats:orderStat1 eq 2",
            ContextualSearch::ContextualSearchFilterOperator::GreaterThanOrEqual,
            L"jason's favs"
        )).get();

        VERIFY_ARE_EQUAL_INT(result->Size, 2);
        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/1234/broadcasts?$skip=10&$top=20&$orderby=stats:orderStat1%20asc&$filter=stats:stats:orderStat1%20eq%202%20ge%20'jason''s%20favs'", httpCall->PathQueryFragment.to_string());
    }

    DEFINE_TEST_CASE(TestGetGameclipsWinRT)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestBroadcastsWinRT);
        uint32 titleId = 1234;

        auto responseJson = web::json::value::parse(g_jsonGameClipsTestResponse);
        auto httpCall = m_mockXboxSystemFactory->GetMockHttpCall();
        httpCall->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson);

        XboxLiveContext^ xboxLiveContext = GetMockXboxLiveContext_WinRT();

        auto result = create_task(xboxLiveContext->ContextualSearchService->GetGameClipsAsync(
            titleId,
            10,
            20,
            L"orderStat1",
            true,
            L"stats:orderStat1 eq 2",
            ContextualSearch::ContextualSearchFilterOperator::GreaterThanOrEqual,
            L"jason's favs"
        )).get();

        VERIFY_ARE_EQUAL_INT(result->Items->Size, 1);

        ContextualSearch::ContextualSearchGameClip^ clip = result->Items->GetAt(0);
        VERIFY_ARE_EQUAL_STR(L"[en-US] Localized Greatest Moment Name", clip->ClipName);
        VERIFY_ARE_EQUAL_UINT(30, clip->DurationInSeconds);
        VERIFY_ARE_EQUAL_STR(L"66851e40-a72c-4b40-8ab3-24583c9c6244", clip->GameClipId);
        VERIFY_ARE_EQUAL_STR(L"en-US", clip->GameClipLocale);
        VERIFY_ARE_EQUAL_STR(L"2814654849282382", clip->XboxUserId);
        VERIFY_ARE_EQUAL_UINT(7453, clip->Views);
        VERIFY_ARE_EQUAL_UINT(ContextualSearch::ContextualSearchGameClipType::UserGenerated, clip->GameClipType);

        VERIFY_ARE_EQUAL_STR(L"GET", httpCall->HttpMethod);
        VERIFY_ARE_EQUAL_STR(L"https://contextualsearch.mockenv.xboxlive.com", httpCall->ServerName);
        VERIFY_ARE_EQUAL_STR(L"/titles/1234/gameclips?$top=20&$skip=10&$orderby=stats:orderStat1%20asc&$filter=stats:stats:orderStat1%20eq%202%20ge%20'jason''s%20favs'", httpCall->PathQueryFragment.to_string());
    }


};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END


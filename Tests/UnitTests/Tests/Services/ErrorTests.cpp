// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#define TEST_CLASS_OWNER L"jasonsa"
#define TEST_CLASS_AREA L"ErrorTests"
#include "UnitTestIncludes.h"
#include <xsapi/xbox_live_context.h>
#include <xsapi/game_server_platform.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

const std::wstring errorResponse =
LR"(
{
    "asdfasdf": 1234
}
)";

const std::wstring defaultMultiplayerResponse =
LR"(
{
    "branch": "b65ae271-a117-4359-b963-27ff55d3ab92",
    "changeNumber": 2,
    "correlationId": "06365b64-3067-8039-ecee-3608af3d02fc",
    "startTime": "2013-02-01T00:00:00Z",
    "nextTimer": "2013-02-01T00:00:00Z",
    "initializing": {
        "stage": "Measuring",
        "stageStartTime": "2013-02-01T00:00:00Z",
        "episode": 0
    },
    "hostCandidates": [
        "ab90a362",
        "99582e67"
    ],
    "membersInfo": {
        "first": 0,
        "next": 1,
        "count": 2,
        "accepted": 1,
        "active": 1
    },
    "constants": {
        "system": {
            "capabilities": {
                "clientMatchmaking": true,
                "connectivity": true,
                "suppressPresenceActivityCheck": true,
                "gameplay": true,
                "large": false,
                "userAuthorizationStyle": true,
                "crossplay": true,
                "connectionRequiredForActiveMembers": true
            },
            "version": 1,
            "maxMembersCount": 100,
            "visibility": "Open",
            "initiators": [
                "3456"
            ],
            "inviteProtocol": "party",
            "reservedRemovalTimeout": 30000,
            "inactiveRemovalTimeout": 7200000,
            "readyRemovalTimeout": 180000,
            "sessionEmptyTimeout": 0,
            "metrics": {
                "latency": true,
                "bandwidthDown": true,
                "bandwidthUp": true,
                "custom": true
            },
            "memberInitialization": {
                "joinTimeout": 4000,
                "measurementTimeout": 5000,
                "evaluationTimeout": 5000,
                "externalEvaluation": false,
                "membersNeededToStart": 2
            },
            "peerToPeerRequirements": {
                "latencyMaximum": 250,
                "bandwidthMinimum": 10000
            },
            "peerToHostRequirements": {
                "latencyMaximum": 250,
                "bandwidthDownMinimum": 100000,
                "bandwidthUpMinimum": 1000,
                "hostSelectionMetric": "bandwidthUp"
            },
            "measurementServerAddresses": {
                "east.azure.com": {
                    "secureDeviceAddress": "r5Y="
                },
                "west.azure.com": {
                    "secureDeviceAddress": "rwY="
                }
            }
        },
        "custom": {}
    },
    "properties": {
        "system": {
            "closed": true,
            "keywords": [
                "hello"
            ],
            "turn": [
                0
            ],
            "host": "99e4c701",
            "initializationSucceeded": true,
            "joinRestriction": "None",
            "readRestriction": "None",
            "serverConnectionStringCandidates": [
                "west.azure.com",
                "east.azure.com"
            ],
            "matchmaking": {
                "clientResult": {
                    "status": "Searching",
                    "statusDetails": "Description",
                    "typicalWait": 30,
                    "targetSessionRef": {
                        "scid": "1ECFDB89-36EB-4E59-8901-11F7393689AE",
                        "templateName": "capture-the-flag",
                        "name": "2D58F65F-0E3C-4F1F-8277-2BC9873FDB23"
                    }
                },
                "targetSessionConstants": {},
                "serverConnectionString": "west.azure.com"
            },
            "matchmakingResubmit": true
        },
        "custom": {}
    },
    "servers": {
        "matchmaking": {
            "properties": {
                "system": {
                    "status": "searching",
                    "statusDetails": "test",
                    "typicalWait": 10000,
                    "targetSessionRef": {
                        "scid": "1ECFDB89-36EB-4E59-8901-11F7393689AE",
                        "templateName": "capture-the-flag",
                        "name": "2D58F65F-0E3C-4F1F-8277-2BC9873FDB23"
                    }
                }
            }
        }
    },
    "members": {
        "0": {
            "constants": {
                "system": {
                    "index": 0,
                    "xuid": "TestXboxUserId",
                    "initialize": true,
                    "matchmakingResult": {
                        "serverMeasurements": {
                            "east.azure.com": {
                                "latency": 233
                            }
                        }
                    }
                },
                "custom": "Hello"
            },
            "properties": {
                "system": {
                    "subscription": {
                        "changeTypes": [
                            "everything",
                            "host"
                        ]
                    },
                    "ready": true,
                    "active": false,
                    "secureDeviceAddress": "ryY=",
                    "initializationGroup": [
                        5
                    ],
                    "measurements": {
                        "e6Kv5zY=": {
                            "latency": 5953,
                            "bandwidthDown": 19342,
                            "bandwidthUp": 944,
                            "custom": {}
                        }
                    },
                    "serverMeasurements": {
                        "east.azure.com": {
                            "latency": 233
                        }
                    }
                },
                "custom": {}
            },
            "gamertag": "stacy",
            "deviceToken": "9f4032ba7",
            "nat": "Strict",
            "reserved": true,
            "activeTitleId": "8397267",
            "joinTime": "2013-02-01T00:00:00Z",
            "turn": true,
            "initializationFailure": "Latency",
            "initializationEpisode": 0,
            "next": 1
        },
        "1": {
            "constants": {
                "system": {
                    "index": 1,
                    "xuid": "1234",
                    "initialize": true,
                    "matchmakingResult": {
                        "serverMeasurements": {
                            "east.azure.com": {
                                "latency": 233
                            }
                        }
                    }
                },
                "custom": {}
            },
            "properties": {
                "system": {
                    "subscription": {
                        "changeTypes": [
                            "everything",
                            "host"
                        ]
                    },
                    "ready": true,
                    "active": false,
                    "secureDeviceAddress": "ryY=",
                    "initializationGroup": [
                        5
                    ],
                    "measurements": {
                        "e6Kv5zY=": {
                            "latency": 5953,
                            "bandwidthDown": 19342,
                            "bandwidthUp": 944,
                            "custom": {}
                        }
                    },
                    "serverMeasurements": {
                        "east.azure.com": {
                            "latency": 233
                        }
                    }
                },
                "custom": {}
            },
            "gamertag": "stacy",
            "deviceToken": "9f4032ba7",
            "nat": "Strict",
            "reserved": true,
            "activeTitleId": "8397267",
            "joinTime": "2013-02-01T00:00:00Z",
            "turn": true,
            "initializationFailure": "Latency",
            "initializationEpisode": 0,
            "next": 4
        }
    },
    "contractVersion": 105,
    "branch": "334afeb9-dad2-4ee7-b786-1d9c6a816b4c",
    "changeNumber": 1
}
)";


DEFINE_TEST_CLASS(ErrorTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(ErrorTests)

    DEFINE_TEST_CASE(TestHttpErrors)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestHttpErrors);
        auto mockXblContext = GetMockXboxLiveContext_Cpp();
        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(L"", 404);

        mockXblContext->achievement_service().get_achievement(
            L"1234",
            L"1234",
            L"1234"
            )
        .then([](xbox_live_result<achievements::achievement> achievementResult)
        {
            VERIFY_IS_TRUE(achievementResult.err() == xbox_live_error_code::http_status_404_not_found);
            VERIFY_IS_TRUE(achievementResult.err() == xbox_live_error_condition::http);
            VERIFY_IS_TRUE(achievementResult.err() == xbox_live_error_condition::http_404_not_found);
        }).wait();
        
        auto jsonResult = web::json::value::parse(errorResponse);
        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValueInternal = StockMocks::CreateMockHttpCallResponseInternal(jsonResult, 200);

        mockXblContext->achievement_service().get_achievement(
            L"1234",
            L"1234",
            L"1234"
            )
        .then([](xbox_live_result<achievements::achievement> achievementResult)
        {
            VERIFY_IS_TRUE(achievementResult.err() == xbox_live_error_code::json_error);
            VERIFY_IS_TRUE(achievementResult.err() == xbox_live_error_condition::generic_error);
        }).wait();
    }

    DEFINE_TEST_CASE(TestGameServerPlatform)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestGameServerPlatform);
        auto mockXblContext = GetMockXboxLiveContext_Cpp();
        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(L"", 404);

        auto result = mockXblContext->game_server_platform_service().allocate_cluster(
            5,
            L"1234",
            L"1234",
            L"1234",
            false
            ).get();

        VERIFY_IS_TRUE(result.err() == xbox_live_error_code::http_status_404_not_found);

        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(L"", 404);

        auto result1 = mockXblContext->game_server_platform_service().allocate_cluster_inline(
            5,
            L"1234",
            L"1234",
            L"1234",
            L"1234",
            L"1234",
            10,
            L"1234",
            false
            ).get();

        VERIFY_IS_TRUE(result1.err() == xbox_live_error_code::http_status_404_not_found);

        auto result2 = mockXblContext->game_server_platform_service().get_ticket_status(
            5,
            L"1234"
            ).get();

        VERIFY_IS_TRUE(result2.err() == xbox_live_error_code::http_status_404_not_found);

        auto result3 = mockXblContext->game_server_platform_service().get_game_server_metadata(
            4,
            5,
            true,
            6,
            std::map<string_t, string_t>()
            ).get();

        VERIFY_IS_TRUE(result3.err() == xbox_live_error_code::http_status_404_not_found);

        auto result4 = mockXblContext->game_server_platform_service().get_quality_of_service_servers().get();

        VERIFY_IS_TRUE(result4.err() == xbox_live_error_code::http_status_404_not_found);

        auto result5 = mockXblContext->game_server_platform_service().allocate_session_host(
            1,
            std::vector<string_t>(),
            L"1234",
            L"1234",
            L"1234",
            L"1234"
            ).get();

        VERIFY_IS_TRUE(result5.err() == xbox_live_error_code::http_status_404_not_found);

        auto result6 = mockXblContext->game_server_platform_service().get_session_host_allocation_status(
            1,
            L"1234"
            ).get();

        VERIFY_IS_TRUE(result6.err() == xbox_live_error_code::http_status_404_not_found);
    }

    DEFINE_TEST_CASE(TestMultiplayer412)
    {
        DEFINE_TEST_CASE_PROPERTIES(TestMultiplayer412);
        auto mockXblContext = GetMockXboxLiveContext_Cpp();
        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(L"", 412);

        auto result = mockXblContext->multiplayer_service().write_session(
            std::make_shared<multiplayer::multiplayer_session>(
                L"1234", multiplayer::multiplayer_session_reference(L"!234", L"1234", L"1234")
                ),
                multiplayer::multiplayer_session_write_mode::update_or_create_new
            ).get();

        VERIFY_IS_TRUE(result.err() == xbox_live_error_code::http_status_412_precondition_failed);
        VERIFY_IS_TRUE(result.payload() == nullptr);

        auto responseJson = web::json::value::parse(defaultMultiplayerResponse);
        m_mockXboxSystemFactory->GetMockHttpCall()->ResultValue = StockMocks::CreateMockHttpCallResponse(responseJson, 412);

        result = mockXblContext->multiplayer_service().write_session(
            std::make_shared<multiplayer::multiplayer_session>(
            L"1234", multiplayer::multiplayer_session_reference(L"!234", L"1234", L"1234")
            ),
            multiplayer::multiplayer_session_write_mode::update_or_create_new
            ).get();

        VERIFY_IS_TRUE(result.err() == xbox_live_error_code::http_status_412_precondition_failed);
        VERIFY_IS_TRUE(result.err() == xbox_live_error_condition::http_412_precondition_failed);
        VERIFY_IS_TRUE(result.err() == xbox_live_error_condition::http);
        VERIFY_IS_TRUE(result.payload() != nullptr);
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END

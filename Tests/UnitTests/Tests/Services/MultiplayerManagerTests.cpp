// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "multiplayer_manager_internal.h"
#include "UnitTestIncludes.h"

#pragma warning(disable:4996)

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_BEGIN

#define GAME_SESSION_NAME    "MockGameSessionName"
#define LOBBY_SESSION_NAME   "MockLobbySessionName"
#define GAME_TEMPLATE_NAME   "MockGameSessionTemplateName"
#define LOBBY_TEMPLATE_NAME  "MockLobbySessionTemplateName"
#define HOPPER_NAME_NO_QOS   "PlayerSkillNoQoS"
#define HOPPER_NAME_WITH_QOS "PlayerSkill"
#define CONNECTION_ADDR      "AQDXfbIj/QDRr2aLF5vWnwEEAiABSJgA2BES8XsFOdf6/FICIAEAAEE3nnYsBQQNfJRgQwEKfMU7"
#define POST                 "POST"
#define GET                  "GET"

#define MPSD_URI "https://sessiondirectory.xboxlive.com"
#define MPSD_RTA_URI MPSD_URI "/connections/"

static concurrency::event g_sessionEvent;
static xbox_live_callback<xbox_live_result<std::shared_ptr<XblMultiplayerSession>>> g_setSessionEvent = [](xbox_live_result<std::shared_ptr<XblMultiplayerSession>>)
{
    g_sessionEvent.set();
};

const char* defaultGameHttpHeaderUri = "/serviceconfigs/MockScid/sessionTemplates/MockGameSessionTemplateName/sessions/MockGameSessionName";
const char* defaultMpsdUri = "https://sessiondirectory.xboxlive.com";
const char* connectionsUri = "https://sessiondirectory.xboxlive.com/connections";
const char* defaultGameUri = "https://sessiondirectory.xboxlive.com/serviceconfigs/MockScid/sessionTemplates/MockGameSessionTemplateName/sessions/MockGameSessionName";
const char* defaultLobbyUri = "https://sessiondirectory.xboxlive.com/serviceconfigs/MockScid/sessionTemplates/MockLobbySessionTemplateName/sessions/MockLobbySessionName";
const char* transferHandleUri = "https://sessiondirectory.xboxlive.com/handles/TestGameSessionTransferHandle/session";
const char* matchTicketUri = "https://smartmatch.xboxlive.com/serviceconfigs/MockScid/hoppers/PlayerSkillNoQoS";
const xsapi_internal_http_headers defaultGameHttpResponseHeaders = { { "ETag", "MockETag" }, { "Retry-After", "1" }, { "Content-Location", "/serviceconfigs/MockScid/sessionTemplates/MockGameSessionTemplateName/sessions/MockGameSessionName" } };
const xsapi_internal_http_headers defaultLobbyHttpResponseHeaders = { { "ETag", "MockETag" },{ "Retry-After", "1" }, { "Content-Location", "/serviceconfigs/MockScid/sessionTemplates/MockLobbySessionTemplateName/sessions/MockLobbySessionName" } };

const char* defaultLobbySessionResponse = R"({  
       "membersInfo":{
            "accepted":0,
            "active":0,  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"default-lobby-session-corrId",
       "changeNumber":1
    })";

const char* rtaConnectionId = R"({
        "ConnectionId": "d01a8c1b-2f83-4e03-9278-3048b480928f"
    })";

DEFINE_TEST_CLASS(MultiplayerManagerTests)
{
public:
    DEFINE_TEST_CLASS_PROPS(MultiplayerManagerTests)

    const char* emptyResponse = R"({})";
    const char* badResponse = R"({
        "badResponse": null
    })";
    const char* classProperties = R"({
        "properties":{  
          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       }
    })";
    const char* syncProperties = R"({
        "properties":{  
          "custom":{  
             "Map":"MyTestMap",
             "GameMode":"MyTestGameMode",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       }
    })";
    const char* propertiesNoTransferHandle = R"({
        "properties":{  
          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle"
          }
       }
    })";

    const char* defaultLobbySessionNoCustomMemberPropsResponse = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{ 
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{}
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"default-lobby-session-corrId",
       "changeNumber":2
    })";
    const char* defaultMultipleLocalUsersLobbyResponse = R"({  
       "membersInfo":{  
          "first":0,
          "next":2,
          "count":2
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"2345",
                   "index":1
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"TestGamertag_2",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"default-multi-user-lobby-session-corrId",
       "changeNumber":1
    })";
    const char* multipleLocalUsersLobbyResponse = R"({  
       "membersInfo":{  
          "first":0,
          "next":2,
          "count":2
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"3456",
                   "index":1
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"TestGamertag_3",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"default-lobby-session-corrId",
       "changeNumber":2
    })";
    const char* lobbyWithNoTransferHandleResponse = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"lobby-no-trans-handle-corrId",
       "changeNumber":1
    })";
    const char* updatedLobbyWithNoTransferHandleResponse = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"updated-lobby-no-trans-handle-corrId",
       "changeNumber":4
    })";
    const char* updatedMultipleLocalUsersLobbyWithNoTransferHandleResponse = R"({  
       "membersInfo":{  
          "first":0,
          "next":2,
          "count":2
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"2345",
                   "index":1
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"TestGamertag_2",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"no-trans-handle",
       "changeNumber":4
    })";
    const char* lobbyWithPendingTransferHandleResponse = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"pending~1234"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"lobby-pending-trans-handle-corrId",
       "changeNumber":2
    })";
    const char* lobbyWithCompletedTransferHandleResponse = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"lobby-completed-trans-handle-corrId",
       "changeNumber":3
    })";
    const char* defaultGameSessionResponse = R"({
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "custom":{},

          "system":{}
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"default-game-session-corrId",
       "changeNumber":1
    })";
    const char* defaultGameSessionWithXuidsResponse = R"({
       "membersInfo":{  
          "first":0,
          "next":3,
          "count":3
       },

       "constants":{
          "system":{
            "initiators": [
                "TestXboxUserId_1",
                "2345",
                "3456"
            ],
            "version": 1
          },

          "custom":{}
       },

       "properties":{  
          "custom":{},

          "system":{}
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "constants":{  
                "system":{  
                   "xuid":"2345",
                   "index":1
                },

                "custom":{}
             },

             "properties":{  
                "system":{},

                "custom":{}
             },

             "reserved":true
          },

          "2":{  
             "next":3,
             "constants":{  
                "system":{  
                   "xuid":"3456",
                   "index":2
                },

                "custom":{}
             },

             "properties":{  
                "system":{},

                "custom":{}
             },

             "reserved":true
          }
       },

       "correlationId":"default-game-session-corrId",
       "changeNumber":2
    })";
    const char* defaultMultipleLocalUsersGameResponse = R"({
       "membersInfo":{  
          "first":0,
          "next":2,
          "count":2
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "custom":{},

          "system":{}
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"2345",
                   "index":1
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"TestGamertag_2",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"default-multi-user-game-session-corrId",
       "changeNumber":1
    })";
    const char* gameSessionResponseDiffXuid = R"({
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "custom":{},

          "system":{}
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"2345",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"game-session-diff-xuid-corrId",
       "changeNumber":2
    })";
    const char* sessionChangeNum2 = R"({  
        "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "next":1,
             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"sessionChangeNum2-session-corrId",
       "changeNumber":2
    })";
    const char* sessionChangeNum3 = R"({  
        "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "next":1,
             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"sessionChangeNum3-session-corrId",
       "changeNumber":3
    })";

    const char* sessionChangeNum4 = R"({  
        "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"TestHostDeviceToken"
          },

          "custom":{  
             "Map":"MyTestMap",
             "GameMode":"MyTestGameMode",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"TestHostDeviceToken",
             "next":1
          }
       },

       "correlationId":"sessionChangeNum4-session-corrId",
       "changeNumber":4
    })";

    const char* sessionChangeNum5 = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "next":1,
             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"sessionChangeNum5-session-corrId",
       "changeNumber":5
    })";

    const char* sessionChangeNum6 = R"({  
        "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "next":1,
             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"sessionChangeNum6-session-corrId",
       "changeNumber":6
    })";

    const char* sessionChangeNum8 = R"({  
        "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "next":1,
             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"sessionChangeNum8-session-corrId",
       "changeNumber":8
    })";
    const char* matchTicketResponse = R"({
        "ticketId":"ad721649-569f-4151-b519-827244df9f91",
        "waitTime":502967
    })";

    const char* transferHandleResponse = R"({
        "type": "transfer",
        "sessionRef": {
            "scid": "MockScid",
            "templateName": "MockSessionTemplateName",
            "name": "XWIN_7ce12e85-594a-4b3b-9dc3-33b9a4ea57ce"
        },

        "originSessionRef": {
            "scid": "MockScid",
            "templateName": "samplelobbytemplate107",
            "name": "bd6c41c3-01c3-468a-a3b5-3e0fe8133862"
        },

        "version": 1
    })";

    const char* lobbyMatchStatusSearching = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle"
          }
       },

       "servers":{  
          "matchmaking":{  
             "constants":{  
                "system":{},

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "status":"searching"
                },

                "custom":{  
                   "ticketScid":"097d0100-e05c-4d37-8420-46f1169056cf",
                   "ticketHopperName":"PlayerSkillNoQoS",
                   "ticketId":"ad721649-569f-4151-b519-827244df9f91"
                }
             }
          }
       },

       "members":{  
          "0":{  
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"lobbyMatchStatusSearching_corrId",
       "changeNumber":4
    })";

    const char* lobbyMatchStatusExpiredByService = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle"
          }
       },

       "servers":{  
          "matchmaking":{  
             "constants":{  
                "system":{},

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "status":"expired"
                },

                "custom":{  
                   "ticketScid":"097d0100-e05c-4d37-8420-46f1169056cf",
                   "ticketHopperName":"PlayerSkillNoQoS",
                   "ticketId":"ad721649-569f-4151-b519-827244df9f91"
                }
             }
          }
       },

       "members":{  
          "0":{  
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"lobbyMatchStatusSearching_corrId",
       "changeNumber":5
    })";

    const char* lobbyMatchStatusFound = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle"
          }
       },

       "servers":{  
          "matchmaking":{  
             "constants":{  
                "system":{},

                "custom":{}
             },

            "properties":{  
                "system":{  
                   "status":"found",
                   "targetSessionRef":{  
                        "scid":"MockScid",
                        "templateName":"MockGameSessionTemplateName",
                        "name":"MockGameSessionName"
                    }
                },

                "custom":{  
                   "ticketScid":"097d0100-e05c-4d37-8420-46f1169056cf",
                   "ticketHopperName":"PlayerSkillNoQoS",
                   "ticketId":"ad721649-569f-4151-b519-827244df9f91"
                }
             }
          }
       },

       "members":{  
          "0":{  
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "next":1,
             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"lobbyMatchStatusFound_corrId",
       "changeNumber":6
    })";

    const char* lobbyMatchStatusFoundWithTransHandle = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{  
          "matchmaking":{  
             "constants":{  
                "system":{},

                "custom":{}
             },

             "properties":{  
                "status":"found",
                "targetSessionRef":{  
                      "scid":"MockScid",
                      "templateName":"MockGameSessionTemplateName",
                      "name":"MockGameSessionName"
                },

                "custom":{  
                   "ticketScid":"097d0100-e05c-4d37-8420-46f1169056cf",
                   "ticketHopperName":"PlayerSkillNoQoS",
                   "ticketId":"ad721649-569f-4151-b519-827244df9f91"
                }
             }
          }
       },

       "members":{  
          "0":{  
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "next":1,
             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"lobbyMatchStatusFoundWithTransHandle",
       "changeNumber":7
    })";

    const char* matchSessionJoin_1 = R"({  
       "membersInfo":{  
          "first":0,
          "next":2,
          "count":2,
          "accepted":1,
          "active":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "initializing":{
          "stage":"joining",
          "stageStartTime":"2016-06-08T17:03:19.5578022Z",
          "episode":1
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "initializationEpisode":1,
             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "reserved":true,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"2345",
                   "index":1
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{},

                "custom":{}
             },

             "initializationEpisode":1
          }
       },

       "correlationId":"matchSessionJoin_1_corrId",
       "changeNumber":2
    })";

    const char* matchSessionJoin_2 = R"({  
       "membersInfo":{  
          "first":0,
          "next":2,
          "count":2,
          "accepted":2,
          "active":2
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"2345",
                   "index":1
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"TestGamertag_2",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"matchSessionJoin_2_corrId",
       "changeNumber":4
    })";

    const char* matchSessionRemoteClientFailedToJoin = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1,
          "accepted":1,
          "active":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "initializing":{  
          "stage":"failed",
          "stageStartTime":"2016-06-09T18:43:46.0518732Z",
          "episode":1
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"matchSessionFailedToJoin_corrId",
       "changeNumber":4
    })";

    const char* matchSessionMeasuring = R"({  
       "membersInfo":{  
          "first":0,
          "next":2,
          "count":2,
          "accepted":2,
          "active":2
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "initializing":{
          "stage":"measuring",
          "stageStartTime":"2016-06-08T17:03:19.5578022Z",
          "episode":1
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"2345",
                   "index":1
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"TestGamertag_2",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"matchSessionMeasuring_corrId",
       "changeNumber":3
    })";

    const char* matchSessionMeasuringWithQoS = R"({  
       "membersInfo":{  
          "first":0,
          "next":2,
          "count":2,
          "accepted":2,
          "active":2
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "initializing":{
          "stage":"measuring",
          "stageStartTime":"2016-06-08T17:03:19.5578022Z",
          "episode":1
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true,
                    "measurements":{  
                      "cd5a3922ca9fcfb22dc5b8c634d4a754":{  
                         "bandwidthDown":27752,
                         "bandwidthUp":7794,
                         "custom":{  

                         },

                         "latency":7
                      }
                   }
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"2345",
                   "index":1
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"TestGamertag_2",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"matchSessionMeasuringWithQoS_corrId",
       "changeNumber":4
    })";

    const char* matchSessionMeasuringWithQoSComplete = R"({  
       "membersInfo":{  
          "first":0,
          "next":2,
          "count":2,
          "accepted":2,
          "active":2
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true,
                    "measurements":{  
                      "cd5a3922ca9fcfb22dc5b8c634d4a754":{  
                         "bandwidthDown":27752,
                         "bandwidthUp":7794,
                         "custom":{  

                         },

                         "latency":7
                      }
                   }
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"2345",
                   "index":1
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true,
                   "measurements":{  
                      "cd5a3922ca9fcfb22dc5b8c634d4a754":{  
                         "bandwidthDown":27752,
                         "bandwidthUp":7794,
                         "custom":{  

                         },

                         "latency":7
                      }
                   }
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"TestGamertag_2",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"matchSessionMeasuringWithQoSComplete",
       "changeNumber":6
    })";

    const char* matchRemoteClientFailedToUploadQoS = R"({  
       "membersInfo":{  
          "first":0,
          "next":2,
          "count":2,
          "accepted":2,
          "active":2
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{  
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle",
             "GameSessionTransferHandle":"completed~TestGameSessionTransferHandle"
          }
       },

       "initializing":{
          "stage":"failed",
          "stageStartTime":"2016-06-08T17:03:19.5578022Z",
          "episode":1
       },

       "servers":{},

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true,
                    "measurements":{  
                      "cd5a3922ca9fcfb22dc5b8c634d4a754":{  
                         "bandwidthDown":27752,
                         "bandwidthUp":7794,
                         "custom":{  

                         },

                         "latency":7
                      }
                   }
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          },

          "1":{  
             "next":2,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"2345",
                   "index":1
                },

                "custom":{
                   "matchmakingResult":{
                      "playerAttrs":{
                         "OverallReputation":"71",
                         "OverallReputationIsBad":"0"
                      },

                      "ticketAttrs":{}
                   }
                }
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"TestGamertag_2",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"matchSessionMeasuringWithQoS_corrId",
       "changeNumber":6
    })";

    const char* lobbyMatchStatusCanceledByService = R"({  
       "membersInfo":{  
          "first":0,
          "next":1,
          "count":1
       },

       "constants":{  
          "system":{},

          "custom":{}
       },

       "properties":{  
          "system":{
             "host":"e7c221cbe5228043c39865281047b178"
          },

          "custom":{  
             "Map":"Helmand Valley",
             "GameMode":"Team Battle"
          }
       },

       "servers":{  
          "matchmaking":{  
             "constants":{  
                "system":{},

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "status":"canceled"
                },

                "custom":{  
                   "ticketScid":"097d0100-e05c-4d37-8420-46f1169056cf",
                   "ticketHopperName":"PlayerSkillNoQoS",
                   "ticketId":"ad721649-569f-4151-b519-827244df9f91"
                }
             }
          }
       },

       "members":{  
          "0":{  
             "next":1,
             "constants":{  
                "system":{  
                   "initialize":true,
                   "xuid":"1234",
                   "index":0
                },

                "custom":{}
             },

             "properties":{  
                "system":{  
                   "secureDeviceAddress":"QVFEWGZiSWovUURScjJhTEY1dldud0VFQWlBQlNKZ0EyQkVTOFhzRk9kZjYvRklDSUFFQUFFRTNubllzQlFRTmZKUmdRd0VLZk1VNw==",
                   "active":true
                },

                "custom":{  
                   "Health":89,
                   "Skill":17
                }
             },

             "gamertag":"2 Dev 246876529",
             "deviceToken":"e7c221cbe5228043c39865281047b178"
          }
       },

       "correlationId":"lobbyMatchStatusCanceled_corrId",
       "changeNumber":8
    })";

    JsonDocument emptyJson()
    {
        JsonDocument json;
        json.Parse(emptyResponse);
        return json;
    }

    JsonDocument badResponseJson()
    {
        JsonDocument json;
        json.Parse(badResponse);
        return json;
    }

    JsonDocument rtaConnectionIdJson()
    {
        JsonDocument json;
        json.Parse(rtaConnectionId);
        return json;
    }

    JsonDocument matchTicketJson()
    {
        JsonDocument json;
        json.Parse(matchTicketResponse);
        return json;
    }

    JsonDocument matchSessionJoin_1Json()
    {
        JsonDocument json;
        json.Parse(matchSessionJoin_1);
        return json;
    }

    JsonDocument matchSessionJoin_2Json()
    {
        JsonDocument json;
        json.Parse(matchSessionJoin_2);
        return json;
    }

    JsonDocument matchSessionMeasuringJson()
    {
        JsonDocument json;
        json.Parse(matchSessionMeasuring);
        return json;
    }

    JsonDocument matchSessionMeasuringWithQoSJson()
    {
        JsonDocument json;
        json.Parse(matchSessionMeasuringWithQoS);
        return json;
    }

    JsonDocument matchSessionMeasuringWithQoSCompleteJson()
    {
        JsonDocument json;
        json.Parse(matchSessionMeasuringWithQoSComplete);
        return json;
    }

    JsonDocument matchRemoteClientFailedToUploadQoSJson()
    {
        JsonDocument json;
        json.Parse(matchRemoteClientFailedToUploadQoS);
        return json;
    }

    JsonDocument matchSessionRemoteClientFailedToJoinJson()
    {
        JsonDocument json;
        json.Parse(matchSessionRemoteClientFailedToJoin);
        return json;
    }

    JsonDocument transferHandleJson()
    {
        JsonDocument json;
        json.Parse(transferHandleResponse);
        return json;
    }

    JsonDocument classPropertiesJson()
    {
        JsonDocument json;
        json.Parse(classProperties);
        return json;
    }

    JsonDocument propertiesNoTransferHandleJson()
    {
        JsonDocument json;
        json.Parse(propertiesNoTransferHandle);
        return json;
    }

    JsonDocument syncPropertiesJson()
    {
        JsonDocument json;
        json.Parse(syncProperties);
        return json;
    }

    JsonDocument defaultLobbySessionResponseJson()
    {
        JsonDocument json;
        json.Parse(defaultLobbySessionResponse);
        return json;
    }

    JsonDocument defaultLobbySessionNoCustomMemberPropsResponseJson()
    {
        JsonDocument json;
        json.Parse(defaultLobbySessionNoCustomMemberPropsResponse);
        return json;
    }

    JsonDocument defaultGameSessionResponseJson()
    {
        JsonDocument json;
        json.Parse(defaultGameSessionResponse);
        return json;
    }

    JsonDocument defaultGameSessionWithXuidsResponseJson()
    {
        JsonDocument json;
        json.Parse(defaultGameSessionWithXuidsResponse);
        return json;
    }

    JsonDocument defaultMultipleLocalUsersGameResponseJson()
    {
        JsonDocument json;
        json.Parse(defaultMultipleLocalUsersGameResponse);
        return json;
    }

    JsonDocument gameSessionResponseDiffXuidJson()
    {
        JsonDocument json;
        json.Parse(gameSessionResponseDiffXuid);
        return json;
    }

    JsonDocument sessionChangeNum2Json()
    {
        JsonDocument json;
        json.Parse(sessionChangeNum2);
        return json;
    }

    JsonDocument sessionChangeNum3Json()
    {
        JsonDocument json;
        json.Parse(sessionChangeNum3);
        return json;
    }

    JsonDocument sessionChangeNum4Json()
    {
        JsonDocument json;
        json.Parse(sessionChangeNum4);
        return json;
    }

    JsonDocument sessionChangeNum5Json()
    {
        JsonDocument json;
        json.Parse(sessionChangeNum5);
        return json;
    }

    JsonDocument sessionChangeNum6Json()
    {
        JsonDocument json;
        json.Parse(sessionChangeNum6);
        return json;
    }

    JsonDocument sessionChangeNum8Json()
    {
        JsonDocument json;
        json.Parse(sessionChangeNum8);
        return json;
    }

    JsonDocument lobbyMatchStatusSearchingJson()
    {
        JsonDocument json;
        json.Parse(lobbyMatchStatusSearching);
        return json;
    }

    JsonDocument lobbyMatchStatusFoundJson()
    {
        JsonDocument json;
        json.Parse(lobbyMatchStatusFound);
        return json;
    }

    JsonDocument lobbyMatchStatusFoundWithTransHandleJson()
    {
        JsonDocument json;
        json.Parse(lobbyMatchStatusFoundWithTransHandle);
        return json;
    }

    JsonDocument lobbyMatchStatusExpiredByServiceJson()
    {
        JsonDocument json;
        json.Parse(lobbyMatchStatusExpiredByService);
        return json;
    }

    JsonDocument lobbyMatchStatusCanceledByServiceJson()
    {
        JsonDocument json;
        json.Parse(lobbyMatchStatusCanceledByService);
        return json;
    }

    JsonDocument lobbyNoHandleResponseJson()
    {
        JsonDocument json;
        json.Parse(lobbyWithNoTransferHandleResponse);
        return json;
    }

    JsonDocument lobbyPendingTransferHandleResponseJson()
    {
        JsonDocument json;
        json.Parse(lobbyWithPendingTransferHandleResponse);
        return json;
    }

    JsonDocument multipleLocalUsersLobbyResponseJson()
    {
        JsonDocument json;
        json.Parse(multipleLocalUsersLobbyResponse);
        return json;
    }

    JsonDocument updatedLobbyNoHandleResponseJson()
    {
        JsonDocument json;
        json.Parse(updatedLobbyWithNoTransferHandleResponse);
        return json;
    }

    JsonDocument updatedMultipleLocalUsersLobbyWithNoTransferHandleResponseJson()
    {
        JsonDocument json;
        json.Parse(updatedMultipleLocalUsersLobbyWithNoTransferHandleResponse);
        return json;
    }

    JsonDocument lobbyCompletedHandleResponseJson()
    {
        JsonDocument json;
        json.Parse(lobbyWithCompletedTransferHandleResponse);
        return json;
    }

    JsonDocument defaultMultipleLocalUsersLobbyResponseJson()
    {
        JsonDocument json;
        json.Parse(defaultMultipleLocalUsersLobbyResponse);
        return json;
    }

    const char* defaultLobbyResponseHeaders = R"({ 
        "ETag":"MockETag",
        "Retry-After":"1",
        "Content-Location":"/serviceconfigs/MockScid/sessionTemplates/MockLobbySessionTemplateName/sessions/MockLobbySessionName"
    })";

    private:
        class MPMTestEnvironment : public TestEnvironment
        {
        public:
            MPMTestEnvironment() noexcept
            {
                auto& mockRtaService{ MockRealTimeActivityService::Instance() };
                mockRtaService.SetSubscribeHandler([&](uint32_t n, xsapi_internal_string uri)
                {
                    if (uri.find("sessiondirectory") != xsapi_internal_string::npos)
                    {
                        mockRtaService.CompleteSubscribeHandshake(n, rtaConnectionId);
                    }
                });
            }

            ~MPMTestEnvironment() noexcept
            {
                HttpMock mock(POST, defaultMpsdUri, 201);
                mock.SetResponseBody(defaultLobbySessionResponse);
                mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

                size_t userRemoved = 0;
                auto remaining = GlobalState::Get()->MultiplayerManager()->LobbySession()->LocalMembers();

                // Possible all users were removed during a test
                if (remaining.size() == 0)
                {
                    VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
                    
                    m_xboxLiveContexts.clear();
                    return;
                }

                for (auto xboxLiveContext : m_xboxLiveContexts)
                {
                    // Possible some users were removed during a test
                    bool isLobbyMember{ false };
                    for (auto member : remaining)
                    {
                        if (member->Xuid() == xboxLiveContext->User().Xuid())
                        {
                            isLobbyMember = true;
                            break;
                        }
                    }

                    if (isLobbyMember)
                    {
                        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext->User().Handle()));
                    }
                    else
                    {
                        ++userRemoved;
                    }
                }

                int count{ 0 };
                bool clientDisconnected{ false };
                while (userRemoved != m_xboxLiveContexts.size() || !clientDisconnected)
                {
                    if (++count > 500) break;

                    size_t eventsCount{};
                    const XblMultiplayerEvent* events{};
                    XblMultiplayerManagerDoWork(&events, &eventsCount);

                    for (uint32_t i = 0; i < eventsCount; ++i)
                    {
                        if (events[i].EventType == XblMultiplayerEventType::UserRemoved)
                        {
                            userRemoved++;
                        }
                        else if (events[i].EventType == XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                        {
                            VERIFY_IS_FALSE(clientDisconnected);
                            clientDisconnected = true;
                        }
                    }

                    Sleep(10);
                }

                VERIFY_IS_TRUE(userRemoved == m_xboxLiveContexts.size() && clientDisconnected);
                VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
                VERIFY_ARE_EQUAL_UINT(0, XblMultiplayerManagerLobbySessionLocalMembersCount());

                m_xboxLiveContexts.clear();
            }

            std::shared_ptr<XblContext> CreateMockXboxLiveContext(uint64_t xuid = MOCK_XUID)
            {
                auto context = TestEnvironment::CreateMockXboxLiveContext(xuid);
                m_xboxLiveContexts.push_back(context);
                
                return context;
            }

        private:
            std::vector<std::shared_ptr<XblContext>> m_xboxLiveContexts;
        };

    public:

    bool IsPlayerInLobby(uint64_t xboxUserId)
    {
        auto count = XblMultiplayerManagerLobbySessionMembersCount();
        std::vector<XblMultiplayerManagerMember> members(count);
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionMembers(count, members.data()));

        for (auto member : members)
        {
            if (xboxUserId == member.Xuid) return true;
        }

        return false;
    }

    void DestructManager(XblContextHandle xboxLiveContext)
    {
        std::vector<XblContextHandle> xboxLiveContexts{ xboxLiveContext };
        DestructManager(xboxLiveContexts);
    }

    void DestructManager(std::vector<XblContextHandle> xboxLiveContexts)
    {
        HttpMock mock(POST, defaultMpsdUri, 201);
        mock.SetResponseBody(defaultLobbySessionResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        size_t userRemoved = 0;
        for (auto xboxLiveContext : xboxLiveContexts)
        {
            VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext->User().Handle()));
        }

        int count{ 0 };
        bool clientDisconnected{ false };
        while (userRemoved != xboxLiveContexts.size() || !clientDisconnected)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);
            
            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::UserRemoved)
                {
                    userRemoved++;
                }
                else if (events[i].EventType == XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    VERIFY_IS_FALSE(clientDisconnected);
                    clientDisconnected = true;
                }
            }

            Sleep(10);
        }
        
        VERIFY_IS_TRUE(userRemoved == xboxLiveContexts.size() && clientDisconnected);
        VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
        VERIFY_ARE_EQUAL_UINT(0, XblMultiplayerManagerLobbySessionLocalMembersCount());
    }

    void VerifyMultiplayerMember(XblMultiplayerManagerMember member, JsonValue resultToVerify)
    {
        auto str = JsonUtils::SerializeJson(resultToVerify);
        UNREFERENCED_PARAMETER(str);
        JsonValue constantsSystemJson = resultToVerify["constants"]["system"].GetObject();
        
        VERIFY_ARE_EQUAL_UINT(constantsSystemJson["index"].GetUint(), member.MemberId);
        VERIFY_ARE_EQUAL_UINT(strtoull(constantsSystemJson["xuid"].GetString(), nullptr, 0), member.Xuid);

        if (member.Status != XblMultiplayerSessionMemberStatus::Reserved)
        {
            VERIFY_ARE_EQUAL_STR(resultToVerify["gamertag"].GetString(), member.DebugGamertag);
            VERIFY_ARE_EQUAL_STR(CONNECTION_ADDR, member.ConnectionAddress);
        }

        VERIFY_ARE_EQUAL(IsPlayerInLobby(member.Xuid), member.IsInLobby);

        JsonValue propertiesJson = resultToVerify["properties"].GetObject();
        JsonValue propertiesSystemJson = propertiesJson["system"].GetObject();
        switch (member.Status)
        {
            case XblMultiplayerSessionMemberStatus::Active:
                VERIFY_IS_TRUE(propertiesSystemJson["active"].GetBool());
                break;
            case XblMultiplayerSessionMemberStatus::Ready:
                VERIFY_IS_TRUE(propertiesSystemJson["ready"].GetBool());
                break;
            case XblMultiplayerSessionMemberStatus::Reserved:
                VERIFY_IS_TRUE(resultToVerify["reserved"].GetBool());
                break;
            case XblMultiplayerSessionMemberStatus::Inactive:
                VERIFY_IS_FALSE(resultToVerify["reserved"].GetBool());
                VERIFY_IS_FALSE(propertiesSystemJson["active"].GetBool());
                VERIFY_IS_FALSE(propertiesSystemJson["ready"].GetBool());
                break;
            default:
                throw std::invalid_argument("Enum value out of range");
        }

        VERIFY_ARE_EQUAL_STR(JsonUtils::SerializeJson(propertiesJson["custom"]), member.PropertiesJson);
    }

    void VerifyLobby(JsonValue resultToVerify)
    {
        JsonValue membersJson = resultToVerify["members"].GetObject();
        JsonValue memberInfoJson = resultToVerify["membersInfo"].GetObject();
        
        uint32_t memberCount = memberInfoJson["count"].GetInt();
        uint32_t memberFirst = memberInfoJson["first"].GetInt();

        auto lobbyMemberCount = XblMultiplayerManagerLobbySessionMembersCount();
        auto lobbyLocalMemberCount = XblMultiplayerManagerLobbySessionLocalMembersCount();
        std::vector<XblMultiplayerManagerMember> lobbyMembers(lobbyMemberCount);
        std::vector<XblMultiplayerManagerMember> lobbyLocalMembers(lobbyLocalMemberCount);
        
        if (lobbyMemberCount > 0)
        {
            VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionMembers(lobbyMemberCount, lobbyMembers.data()));
        }
        if (lobbyLocalMemberCount > 0)
        {
            VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionLocalMembers(lobbyLocalMemberCount, lobbyLocalMembers.data()));
        }

        for (uint32_t i = memberFirst; i < memberCount; ++i)
        {
            xsapi_internal_stringstream stream;
            stream << i;

            if (i < lobbyLocalMemberCount)
            {
                VerifyMultiplayerMember(lobbyLocalMembers[i], membersJson[stream.str().data()].GetObject());
            }
            else
            {
                VerifyMultiplayerMember(lobbyMembers[i], membersJson[stream.str().data()].GetObject());
            }
        }

        // Session name is dynamically created by the Manager and not returned back in the service response.
        XblMultiplayerSessionReference lobbyRef{};
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSessionReference(&lobbyRef));
        VERIFY_ARE_EQUAL_STR(MOCK_SCID, lobbyRef.Scid);
        VERIFY_ARE_EQUAL_STR(LOBBY_TEMPLATE_NAME, lobbyRef.SessionTemplateName);

        XblGuid corrId{};
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionCorrelationId(&corrId));
        auto s = resultToVerify["correlationId"].GetString();
        VERIFY_ARE_EQUAL_STR(s, corrId.value);

        auto lobbyProperties = XblMultiplayerManagerLobbySessionPropertiesJson();
        VERIFY_ARE_EQUAL_STR(JsonUtils::SerializeJson(resultToVerify["properties"]["custom"]), lobbyProperties);
    }

    void VerifyGame(JsonValue resultToVerify)
    {
        JsonValue membersJson = resultToVerify["members"].GetObject();
        JsonValue memberInfoJson = resultToVerify["membersInfo"].GetObject();

        uint32_t memberCount = memberInfoJson["count"].GetInt();
        uint32_t memberFirst = memberInfoJson["first"].GetInt();

        auto gameMemberCount = XblMultiplayerManagerGameSessionMembersCount();
        std::vector<XblMultiplayerManagerMember> gameMembers(gameMemberCount);
        VERIFY_SUCCEEDED(XblMultiplayerManagerGameSessionMembers(gameMemberCount, gameMembers.data()));

        for (uint32_t i = memberFirst; i < memberCount; ++i)
        {
            xsapi_internal_stringstream stream;
            stream << i;

            VerifyMultiplayerMember(gameMembers[i], membersJson[stream.str().c_str()].GetObject());
        }

        auto gameRef = XblMultiplayerManagerGameSessionSessionReference();
        VERIFY_ARE_EQUAL_STR(MOCK_SCID, gameRef->Scid);
        VERIFY_ARE_EQUAL_STR(GAME_TEMPLATE_NAME, gameRef->SessionTemplateName);

        auto corrId = XblMultiplayerManagerGameSessionCorrelationId();
        VERIFY_ARE_EQUAL_STR(resultToVerify["correlationId"].GetString(), corrId);

        auto gameProperties = XblMultiplayerManagerGameSessionPropertiesJson();
        VERIFY_ARE_EQUAL_STR(JsonUtils::SerializeJson(resultToVerify["properties"]["custom"]), gameProperties);
    }

    void VerifyMultiplayerMember(multiplayer::manager::MultiplayerMember* member, JsonValue resultToVerify)
    {
        auto str = JsonUtils::SerializeJson(resultToVerify);
        UNREFERENCED_PARAMETER(str);
        JsonValue constantsSystemJson = resultToVerify["constants"]["system"].GetObject();

        VERIFY_ARE_EQUAL_UINT(constantsSystemJson["index"].GetUint(), member->MemberId());
        VERIFY_ARE_EQUAL_UINT(strtoull(constantsSystemJson["xuid"].GetString(), nullptr, 0), member->Xuid());

        if (member->Status() != XblMultiplayerSessionMemberStatus::Reserved)
        {
            VERIFY_ARE_EQUAL_STR(resultToVerify["gamertag"].GetString(), member->DebugGamertag());
            VERIFY_ARE_EQUAL_STR(CONNECTION_ADDR, member->ConnectionAddress());
        }

        VERIFY_ARE_EQUAL(IsPlayerInLobby(member->Xuid()), member->IsInLobby());

        JsonValue propertiesJson = resultToVerify["properties"].GetObject();
        JsonValue propertiesSystemJson = propertiesJson["system"].GetObject();
        switch (member->Status())
        {
        case XblMultiplayerSessionMemberStatus::Active:
            VERIFY_IS_TRUE(propertiesSystemJson["active"].GetBool());
            break;
        case XblMultiplayerSessionMemberStatus::Ready:
            VERIFY_IS_TRUE(propertiesSystemJson["ready"].GetBool());
            break;
        case XblMultiplayerSessionMemberStatus::Reserved:
            VERIFY_IS_TRUE(resultToVerify["reserved"].GetBool());
            break;
        case XblMultiplayerSessionMemberStatus::Inactive:
            VERIFY_IS_FALSE(resultToVerify["reserved"].GetBool());
            VERIFY_IS_FALSE(propertiesSystemJson["active"].GetBool());
            VERIFY_IS_FALSE(propertiesSystemJson["ready"].GetBool());
            break;
        default:
            throw std::invalid_argument("Enum value out of range");
        }

        VERIFY_ARE_EQUAL_STR(JsonUtils::SerializeJson(propertiesJson["custom"]), member->CustomPropertiesJson());
    }

    void VerifyLobby(JsonValue resultToVerify, multiplayer::manager::MultiplayerLobbySession* lobbySession)
    {
        JsonValue membersJson = resultToVerify["members"].GetObject();
        JsonValue memberInfoJson = resultToVerify["membersInfo"].GetObject();

        uint32_t memberCount = memberInfoJson["count"].GetInt();
        uint32_t memberFirst = memberInfoJson["first"].GetInt();

        for (uint32_t i = memberFirst; i < memberCount; ++i)
        {
            xsapi_internal_stringstream stream;
            stream << i;

            if (i < lobbySession->LocalMembers().size())
            {
                VerifyMultiplayerMember(lobbySession->LocalMembers()[i].get(), membersJson[stream.str().data()].GetObject());
            }
            else
            {
                VerifyMultiplayerMember(lobbySession->LocalMembers()[i].get(), membersJson[stream.str().data()].GetObject());
            }
        }

        // Session name is dynamically created by the Manager and not returned back in the service response.
        VERIFY_ARE_EQUAL_STR(MOCK_SCID, lobbySession->SessionReference().Scid);
        VERIFY_ARE_EQUAL_STR(LOBBY_TEMPLATE_NAME, lobbySession->SessionReference().SessionTemplateName);
        VERIFY_ARE_EQUAL_STR(resultToVerify["correlationId"].GetString(), lobbySession->CorrelationId().c_str());
        VERIFY_ARE_EQUAL_STR(JsonUtils::SerializeJson(resultToVerify["properties"]["custom"]), lobbySession->CustomPropertiesJson().c_str());
    }

    void VerifyGame(JsonValue resultToVerify, multiplayer::manager::MultiplayerGameSession* gameSession)
    {
        JsonValue membersJson = resultToVerify["members"].GetObject();
        JsonValue memberInfoJson = resultToVerify["membersInfo"].GetObject();

        uint32_t memberCount = memberInfoJson["count"].GetInt();
        uint32_t memberFirst = memberInfoJson["first"].GetInt();

        for (uint32_t i = memberFirst; i < memberCount; ++i)
        {
            xsapi_internal_stringstream stream;
            stream << i;

            VerifyMultiplayerMember(gameSession->Members()[i].get(), membersJson[stream.str().c_str()].GetObject());
        }

        VERIFY_ARE_EQUAL_STR(MOCK_SCID, gameSession->SessionReference().Scid);
        VERIFY_ARE_EQUAL_STR(GAME_TEMPLATE_NAME, gameSession->SessionReference().SessionTemplateName);
        VERIFY_ARE_EQUAL_STR(resultToVerify["correlationId"].GetString(), gameSession->CorrelationId().c_str());
        VERIFY_ARE_EQUAL_STR(JsonUtils::SerializeJson(resultToVerify["properties"]["custom"]), gameSession->Properties().c_str());
    }

    void VerifyContext12AndIncrement(XblMultiplayerEvent event, uint32_t* counter)
    {
        auto pContext = *static_cast<uint32_t*>(event.Context);
        VERIFY_IS_TRUE(pContext == 1 || pContext == 2);
        ++*counter;
    }

    void VerifyContext3AndIncrement(XblMultiplayerEvent event, uint32_t* counter)
    {
        auto pContext = *static_cast<uint32_t*>(event.Context);
        VERIFY_ARE_EQUAL_UINT(3, pContext);
        ++*counter;
    }

    std::shared_ptr<XblMultiplayerSession> GetSession(bool isGame)
    {
        auto latestPendingRead = GlobalState::Get()->MultiplayerManager()->GetMultiplayerClientManager()->LatestPendingRead();

        if (isGame)
        {
            return latestPendingRead->GameClient()->Session();
        }
        else
        {
            return latestPendingRead->LobbyClient()->Session();
        }
    }

    const std::shared_ptr<xbox::services::multiplayer::manager::MultiplayerSessionWriter> GetSessionWriter(bool isGame)
    {
        auto latestPendingRead = GlobalState::Get()->MultiplayerManager()->GetMultiplayerClientManager()->LatestPendingRead();

        if (isGame)
        {
            return latestPendingRead->LobbyClient()->SessionWriter();
        }
        else
        {
            return latestPendingRead->LobbyClient()->SessionWriter();
        }
    }

    void AddLocalUserHelper(XblContextHandle xboxLiveContext, XTaskQueueHandle queue = {})
    {
        AddLocalUserHelper(xboxLiveContext, defaultLobbySessionResponse, queue);
    }

    void AddLocalUserHelper(XblContextHandle xboxLiveContext, const char* jsonLobbySessionResponse, XTaskQueueHandle queue = {})
    {
        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext));

        HttpMock mock("PUT", defaultMpsdUri, 201);
        mock.SetResponseBody(jsonLobbySessionResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        uint32_t contextIds[3]{ 1,2,3 };
        XblUserHandle userHandle = xboxLiveContext->User().Handle();
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionAddLocalUser(userHandle));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(userHandle, "Health", "89", &contextIds[0]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(userHandle, "Skill", "17", &contextIds[1]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(userHandle, CONNECTION_ADDR, &contextIds[2]));

        int count{ 0 };
        bool userAdded{ false };
        uint32_t localUserPropWritten{ 0 };
        while (!userAdded || localUserPropWritten != 3)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                switch (events[i].EventType)
                {
                    case XblMultiplayerEventType::UserAdded:
                        userAdded = true;
                        break;
                    case XblMultiplayerEventType::LocalMemberPropertyWriteCompleted:
                        VerifyContext12AndIncrement(events[i], &localUserPropWritten);
                        break;
                    case XblMultiplayerEventType::LocalMemberConnectionAddressWriteCompleted:
                        VerifyContext3AndIncrement(events[i], &localUserPropWritten);
                        break;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!userAdded || localUserPropWritten != 3);
        JsonDocument responseJson;
        responseJson.Parse(jsonLobbySessionResponse);
        VerifyLobby(responseJson.GetObject());
    }

    void AddLocalUserHelperWithSyncUpdate(XblContextHandle xboxLiveContext)
    {
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext, defaultLobbySessionResponse);
    }

    void AddLocalUserHelperWithSyncUpdate(XblContextHandle xboxLiveContext, const char* jsonLobbySessionResponse)
    {
        XTaskQueueHandle queue{};
        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext));
        
        HttpMock mock(POST, defaultMpsdUri, 200);
        mock.SetResponseBody(jsonLobbySessionResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        // Ids to correlate with event number
        uint32_t contextId = 0;
        uint32_t contextIds[11]{ 1,2,3,4,5,6,7,8,9,10,11 };
        XblUserHandle userHandle = xboxLiveContext->User().Handle();
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionAddLocalUser(userHandle));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(userHandle, "Health", "89", &contextIds[contextId++]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(userHandle, "Skill", "17", &contextIds[contextId++]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(userHandle, CONNECTION_ADDR, &contextIds[contextId++]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetProperties("Map", "1", &contextIds[contextId++]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetSynchronizedProperties("Map", "2", &contextIds[contextId++]));

        int count{ 0 };
        bool userAdded{ false };
        uint32_t localUserPropWritten = 0, propWritten = 0, syncPropWritten = 0, eventValue = 0;
        while (!userAdded || localUserPropWritten != 3 || propWritten != 4 || syncPropWritten != 4)
        {
            if (++count > 500) break;

            if (contextId < 10)
            {
                VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetProperties("Map", "3", &contextIds[contextId++]));
                VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetSynchronizedProperties("Map", "4", &contextIds[contextId++]));
            }

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            VERIFY_SUCCEEDED(XblMultiplayerManagerDoWork(&events, &eventsCount));

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                VERIFY_ARE_EQUAL_UINT(XblMultiplayerSessionType::LobbySession, events[i].SessionType);

                if (events[i].Context != nullptr)
                {
                    uint32_t context = *static_cast<uint32_t*>(events[i].Context);
                    
                    TEST_LOG(FormatString(L" [MPM] AddLocalUserHelperWithSyncUpdate - Event type: %d - Context: eventValue = %d:%d", events[i].EventType, context, eventValue).c_str());
                    VERIFY_ARE_EQUAL_UINT(eventValue, context);
                }
                else
                {
                    TEST_LOG(FormatString(L" [MPM] AddLocalUserHelperWithSyncUpdate - Event type: %d - EventValue = %d", events[i].EventType, eventValue).c_str());
                }

                switch (events[i].EventType)
                {
                    case XblMultiplayerEventType::UserAdded:
                        userAdded = true;
                        break;
                    case XblMultiplayerEventType::LocalMemberPropertyWriteCompleted:
                    case XblMultiplayerEventType::LocalMemberConnectionAddressWriteCompleted:
                        localUserPropWritten++;
                        break;
                    case XblMultiplayerEventType::SessionPropertyWriteCompleted:
                        propWritten++;
                        break;
                    case XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted:
                        syncPropWritten++;
                        break;
                }

                eventValue++;
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!userAdded || localUserPropWritten != 3 || propWritten != 4 || syncPropWritten != 4);
        JsonDocument responseJson;
        responseJson.Parse(jsonLobbySessionResponse);
        VerifyLobby(responseJson.GetObject());
    }

    void AddMultipleLocalUserHelper(std::vector<XblContextHandle> xboxLiveContexts)
    {
        AddMultipleLocalUserHelper(xboxLiveContexts, defaultMultipleLocalUsersLobbyResponse);
    }

    void AddMultipleLocalUserHelper(std::vector<XblContextHandle> xboxLiveContexts, const char* jsonLobbySessionResponse)
    {
        XTaskQueueHandle queue{};
        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        
        JsonDocument responseJson;
        responseJson.Parse(jsonLobbySessionResponse);

        HttpMock mock(POST, defaultMpsdUri, 201);
        mock.SetResponseBody(jsonLobbySessionResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        uint32_t contextIds[3]{ 1,2,3 };
        for (auto xboxLiveContext : xboxLiveContexts)
        {
            XblUserHandle userHandle = xboxLiveContext->User().Handle();
            VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext));
            VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionAddLocalUser(userHandle));
            VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(userHandle, "Health", "89", &contextIds[0]));
            VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(userHandle, "Skill", "17", &contextIds[1]));
            VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(userHandle, CONNECTION_ADDR, &contextIds[2]));
        }

        int count{ 0 };
        size_t usersAdded = 0;
        uint32_t localUserPropWritten = 0;
        while (usersAdded != xboxLiveContexts.size() || localUserPropWritten != (xboxLiveContexts.size() * 3))
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                switch (events[i].EventType)
                {
                    case XblMultiplayerEventType::UserAdded:
                        usersAdded++;
                        break;
                    case XblMultiplayerEventType::LocalMemberPropertyWriteCompleted:
                        VerifyContext12AndIncrement(events[i], &localUserPropWritten);
                        break;
                    case XblMultiplayerEventType::LocalMemberConnectionAddressWriteCompleted:
                        VerifyContext3AndIncrement(events[i], &localUserPropWritten);
                        break;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(usersAdded != xboxLiveContexts.size() || localUserPropWritten != (xboxLiveContexts.size() * 3));
        VerifyLobby(responseJson.GetObject());
    }

    void RemoveLocalUserHelper(XblContextHandle xboxLiveContext)
    {
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext);

        HttpMock mock(POST, defaultMpsdUri, 201);
        mock.SetResponseBody(emptyResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext->User().Handle()));

        int count{ 0 };
        bool userRemoved = false, clientDisconnected = false;
        while (!userRemoved || !clientDisconnected)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                TEST_LOG(FormatString(L" [MPM] RemoveLocalUserHelper - Event type: %d", events[i].EventType).c_str());
                if (events[i].EventType == XblMultiplayerEventType::UserRemoved)
                {
                    userRemoved = true;
                }
                else if (events[i].EventType == XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    VERIFY_IS_FALSE(clientDisconnected);
                    clientDisconnected = true;
                }
            }

            Sleep(10);
        }
        
        VERIFY_IS_FALSE(!userRemoved || !clientDisconnected);
        VERIFY_ARE_EQUAL_UINT(0, XblMultiplayerManagerLobbySessionLocalMembersCount());
        VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
    }

    DEFINE_TEST_CASE(TestAddLocalUser)
    {
        TEST_LOG(L"Test starting: TestAddLocalUser");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        AddLocalUserHelperWithSyncUpdate(xboxLiveContext.get());
    }

    DEFINE_TEST_CASE(TestRemoveLocalUser)
    {
        TEST_LOG(L"Test starting: TestRemoveLocalUser");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        RemoveLocalUserHelper(xboxLiveContext.get());
    }

    DEFINE_TEST_CASE(TestReAddAfterRemovingLocalUser)
    {
        TEST_LOG(L"Test starting: TestReAddAfterRemovingLocalUser");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        RemoveLocalUserHelper(xboxLiveContext.get());
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext.get());
    }

    DEFINE_TEST_CASE(TestDeleteLocalMemberProperties)
    {
        TEST_LOG(L"Test starting: TestDeleteLocalMemberProperties");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        AddLocalUserHelperWithSyncUpdate(xboxLiveContext.get());

        HttpMock mock(POST, defaultMpsdUri, 200);
        mock.SetResponseBody(defaultLobbySessionNoCustomMemberPropsResponseJson());
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        XblUserHandle hostHandle{ xboxLiveContext->User().Handle() };
        uint32_t contextIds[2]{ 1,2 };
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties(hostHandle, "Health", &contextIds[0]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties(hostHandle, "Skill", &contextIds[1]));

        int count{ 0 };
        uint32_t localUserPropWritten = 0, eventValue = 1;
        while (localUserPropWritten != 2)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::LocalMemberPropertyWriteCompleted)
                {
                    uint32_t context = *static_cast<uint32_t*>(events[i].Context);
                    VERIFY_ARE_EQUAL_UINT(eventValue++, context);
                    localUserPropWritten++;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(localUserPropWritten != 2);
        auto lobbyMemberCount = XblMultiplayerManagerLobbySessionMembersCount();
        XblMultiplayerManagerMember lobbyMember{};
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionMembers(lobbyMemberCount, &lobbyMember));
        VERIFY_ARE_EQUAL_STR("{}", lobbyMember.PropertiesJson);
    }

    DEFINE_TEST_CASE(TestSetSynchronizedLobbyProperties)
    {
        TEST_LOG(L"Test starting: TestSetSynchronizedLobbyProperties");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext.get());

        // Set up initial http responses
        std::vector<const char*> writeResponses
        {
            sessionChangeNum4, // change #4
            sessionChangeNum6  // change #6
        };

        HttpMock mockWrite(POST, defaultMpsdUri, 201);
        mockWrite.SetResponseBody(writeResponses[0]);
        mockWrite.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        XblMultiplayerManagerMember member;
        uint32_t contextIds[3]{ 1,2,3 };
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetSynchronizedProperties("Map", "\"MyTestMap\"", &contextIds[0]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetSynchronizedProperties("GameMode", "\"MyTestGameMode\"", &contextIds[1]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionMembers(1, &member));
        
        mockWrite.SetResponseBody(writeResponses[1]);
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetSynchronizedHost(member.DeviceToken, &contextIds[2]));

        int count{ 0 };
        uint32_t syncPropWritten{ 0 };
        uint32_t syncHostWritten{ 0 };
        while (syncPropWritten != 2 || syncHostWritten != 1)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                VERIFY_ARE_EQUAL_UINT(XblMultiplayerSessionType::LobbySession, events[i].SessionType);
                
                if (events[i].EventType == XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted)
                {
                    VerifyContext12AndIncrement(events[i], &syncPropWritten);
                }
                else if (events[i].EventType == XblMultiplayerEventType::SynchronizedHostWriteCompleted)
                {
                    VerifyContext3AndIncrement(events[i], &syncHostWritten);
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(syncPropWritten != 2 || syncHostWritten != 1);
        VERIFY_ARE_EQUAL_UINT(6, GlobalState::Get()->MultiplayerManager()->LobbySession()->ChangeNumber());
        VERIFY_ARE_EQUAL_STR(JsonUtils::SerializeJson(classPropertiesJson()["properties"]["custom"]), XblMultiplayerManagerLobbySessionPropertiesJson());
    }

    DEFINE_TEST_CASE(TestSetSynchronizedGameProperties)
    {
        TEST_LOG(L"Test starting: TestSetSynchronizedGameProperties");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        JoinGameHelper(xboxLiveContext.get());

        // Set up initial http responses
        std::vector<const char*> writeResponses
        {
            sessionChangeNum4, // change #4
            sessionChangeNum6  // change #6
        };

        HttpMock writeMock(POST, defaultMpsdUri, 201);
        writeMock.SetResponseBody(writeResponses[0]);
        writeMock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        XblMultiplayerManagerMember member;
        uint32_t contextIds[3]{ 1,2,3 };
        VERIFY_SUCCEEDED(XblMultiplayerManagerGameSessionSetSynchronizedProperties("Map", "\"MyTestMap\"", &contextIds[0]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerGameSessionSetSynchronizedProperties("GameMode", "\"MyTestGameMode\"", &contextIds[1]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerGameSessionMembers(1, &member));

        writeMock.SetResponseBody(writeResponses[1]);
        VERIFY_SUCCEEDED(XblMultiplayerManagerGameSessionSetSynchronizedHost(member.DeviceToken, &contextIds[2]));

        int count{ 0 };
        uint32_t syncPropWritten{ 0 };
        uint32_t syncHostWritten{ 0 };
        while (syncPropWritten != 2 || syncHostWritten != 1)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::SessionSynchronizedPropertyWriteCompleted)
                {
                    VerifyContext12AndIncrement(events[i], &syncPropWritten);
                    VERIFY_ARE_EQUAL_UINT(XblMultiplayerSessionType::GameSession, events[i].SessionType);
                }
                else if (events[i].EventType == XblMultiplayerEventType::SynchronizedHostWriteCompleted)
                {
                    VerifyContext3AndIncrement(events[i], &syncHostWritten);
                    VERIFY_ARE_EQUAL_UINT(XblMultiplayerSessionType::GameSession, events[i].SessionType);
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(syncPropWritten != 2 || syncHostWritten != 1);
        VERIFY_ARE_EQUAL_UINT(6, GlobalState::Get()->MultiplayerManager()->GameSession()->ChangeNumber());
        VERIFY_ARE_EQUAL_STR(JsonUtils::SerializeJson(classPropertiesJson()["properties"]["custom"]).c_str(), XblMultiplayerManagerGameSessionPropertiesJson());
    }

    DEFINE_TEST_CASE(TestLeaveGame)
    {
        TEST_LOG(L"Test starting: TestLeaveGame");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        JoinGameHelper(xboxLiveContext.get());

        HttpMock lobbyMock(GET, defaultMpsdUri, 200);
        lobbyMock.SetResponseBody(updatedLobbyWithNoTransferHandleResponse);
        lobbyMock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        HttpMock gameMock(POST, defaultGameHttpHeaderUri, 201);
        gameMock.SetResponseBody(emptyResponse);
        gameMock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerLeaveGame());

        int count{ 0 };
        bool leaveGameCompleted = false, isStopAdvertisingGameDone = false; 
        auto customProps{ JsonUtils::SerializeJson(propertiesNoTransferHandleJson()["properties"]["custom"]) };
        while (!leaveGameCompleted || !isStopAdvertisingGameDone)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::LeaveGameCompleted)
                {
                    VERIFY_IS_FALSE(leaveGameCompleted);
                    leaveGameCompleted = true;

                    GetSessionWriter(false)->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ GetSession(false)->SessionReference(), "", 4 });
                }
            }

            auto props{ XblMultiplayerManagerLobbySessionPropertiesJson() };
            if (props != nullptr &&
                utils::str_icmp(props, customProps.c_str()) == 0)
            {
                isStopAdvertisingGameDone = true;
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!leaveGameCompleted || !isStopAdvertisingGameDone);
        VerifyLobby(updatedLobbyNoHandleResponseJson().GetObject());
        VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
    }

    DEFINE_TEST_CASE(TestInviteUsers)
    {
        TEST_LOG(L"Test starting: TestInviteUsers");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext.get());

        HttpMock mock{ "POST", MPSD_URI "/handles" };
        std::vector<uint64_t> xuids{ 1234, 5678 };
#pragma warning(suppress: 6387)
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionInviteUsers(xboxLiveContext->User().Handle(), xuids.data(), xuids.size(), nullptr, nullptr));

        int count{ 0 };
        bool inviteSent{ false };
        while (!inviteSent)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount && !inviteSent; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::InviteSent)
                {
                    inviteSent = true;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_TRUE(inviteSent);
    }

    void JoinLobbyWithValidHandleIdAndContext(
        XblContextHandle xboxLiveContext,
        const char* handleId = nullptr,
        XblUserHandle userHandle = nullptr,
        bool checkForInvalidArg = false)
    {
        XTaskQueueHandle queue{};
        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext));

        HttpMock mock(POST, defaultMpsdUri, 201);
        mock.SetResponseBody(lobbyWithNoTransferHandleResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        if (handleId != nullptr)
        {
            xsapi_internal_vector<XblUserHandle> users{};
            
            if (userHandle)
            {
                users.push_back(userHandle);
            }
            
            auto hr = GlobalState::Get()->MultiplayerManager()->GetMultiplayerClientManager()->JoinLobbyByHandle(handleId, users);

            if (checkForInvalidArg)
            {
                VERIFY_ARE_EQUAL_INT(E_INVALIDARG, hr);
                return;
            }
            else
            {
                VERIFY_ARE_EQUAL_INT(S_OK, hr);
            }
        }
        else
        {
            VERIFY_SUCCEEDED(XblMultiplayerManagerJoinLobby("TestHandleId", xboxLiveContext->User().Handle()));
        }

        int count{ 0 };
        bool lobbyJoined{ false };
        while (!lobbyJoined)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::JoinLobbyCompleted)
                {
                    lobbyJoined = true;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_TRUE(lobbyJoined);
        VerifyLobby(lobbyNoHandleResponseJson().GetObject());
        VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
    }

    /*  Join Lobby Tests:
        1. Test with valid handleId with no transfer handle
        2. Test with invalid handleId
        3. Test with valid handleId & valid transfer handle (so that you can join the game).
        4. Test with valid handleId & invalid transfer handle (don't create the game but clear the game session)
        5. Test switching lobbies
    */
    DEFINE_TEST_CASE(TestJoinLobbyWithValidHandleId)
    {
        TEST_LOG(L"Test starting: TestJoinLobbyWithValidHandleId");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        JoinLobbyWithValidHandleIdAndContext(xboxLiveContext.get());
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithValidHandleIdWithEventArgs)
    {
        TEST_LOG(L"Test starting: TestJoinLobbyWithValidHandleIdWithEventArgs");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        JoinLobbyWithValidHandleIdAndContext(xboxLiveContext.get(), "TestHandleId", xboxLiveContext->User().Handle());
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithInvalidArgs_1)
    {
        TEST_LOG(L"Test starting: TestJoinLobbyWithInvalidArgs_1");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        JoinLobbyWithValidHandleIdAndContext(xboxLiveContext.get(), "", xboxLiveContext->User().Handle(), true);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithInvalidArgs_2)
    {
        TEST_LOG(L"Test starting: TestJoinLobbyWithInvalidArgs_2");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        JoinLobbyWithValidHandleIdAndContext(xboxLiveContext.get(), "TestHandleId", nullptr, true);
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithInvalidHandleId)
    {
        TEST_LOG(L"Test starting: TestJoinLobbyWithInvalidHandleId");

        MPMTestEnvironment env{};
        XTaskQueueHandle queue{};

        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext.get()));

        HttpMock lobbyMock(POST, defaultMpsdUri, 404);
        lobbyMock.SetResponseBody(lobbyWithNoTransferHandleResponse);
        lobbyMock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerJoinLobby("TestHandleId", xboxLiveContext->User().Handle()));

        int count{ 0 };
        bool lobbyJoined{ false };
        while (!lobbyJoined)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::JoinLobbyCompleted)
                {
                    lobbyJoined = true;
                    VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_NOT_FOUND, events[i].Result);
                }
            }

            Sleep(10);
        }

        VERIFY_IS_TRUE(lobbyJoined);
        VERIFY_ARE_EQUAL_UINT(0, XblMultiplayerManagerLobbySessionLocalMembersCount());
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithValidTransferHandle)
    {
        TEST_LOG(L"Test starting: TestJoinLobbyWithValidTransferHandle");

        MPMTestEnvironment env{};
        XTaskQueueHandle queue{};

        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext.get()));

        HttpMock lobbyMock(POST, defaultMpsdUri, 201);
        lobbyMock.SetResponseBody(lobbyWithCompletedTransferHandleResponse);
        lobbyMock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        HttpMock gameMock(POST, transferHandleUri, 201);
        gameMock.SetResponseBody(defaultGameSessionResponse);
        gameMock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerJoinLobby("TestHandleId", xboxLiveContext->User().Handle()));

        int count{ 0 };
        bool lobbyJoined = false, gameJoined = false;
        while (!lobbyJoined || !gameJoined)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::JoinLobbyCompleted)
                {
                    lobbyJoined = true;
                    VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGameFromLobby(GAME_TEMPLATE_NAME));
                }
                else if (events[i].EventType == XblMultiplayerEventType::JoinGameCompleted)
                {
                    gameJoined = true;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!lobbyJoined || !gameJoined);
        VerifyLobby(lobbyCompletedHandleResponseJson().GetObject());
        VerifyGame(defaultGameSessionResponseJson().GetObject());
    }

    DEFINE_TEST_CASE(TestJoinLobbyWithInvalidTransferHandle)
    {
        TEST_LOG(L"Test starting: TestJoinLobbyWithInvalidTransferHandle");

        MPMTestEnvironment env{};
        XTaskQueueHandle queue{};

        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext.get()));

        std::vector<const char*> lobbyResponses
        {
            lobbyWithCompletedTransferHandleResponse,
            updatedLobbyWithNoTransferHandleResponse
        };

        std::vector<uint32_t> lobbyStatuses{ 201, 404 };

        HttpMock lobbyMock(POST, defaultMpsdUri, lobbyStatuses[0]);
        lobbyMock.SetResponseBody(lobbyResponses[0]);
        lobbyMock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerJoinLobby("TestHandleId", xboxLiveContext->User().Handle()));

        int count{ 0 };
        bool lobbyJoined = false, gameJoined = false;
        auto customProps = JsonUtils::SerializeJson(propertiesNoTransferHandleJson()["properties"]["custom"]);
        while (!lobbyJoined || !gameJoined)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::JoinLobbyCompleted)
                {
                    lobbyJoined = true;

                    lobbyMock.SetResponseBody(lobbyResponses[1]);
                    lobbyMock.SetResponseHttpStatus(lobbyStatuses[1]);
                    VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGameFromLobby(GAME_TEMPLATE_NAME));
                }
                else if (events[i].EventType == XblMultiplayerEventType::JoinGameCompleted)
                {
                    gameJoined = true;
                    VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_NOT_FOUND, events[i].Result);
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!lobbyJoined || !gameJoined);
        VerifyLobby(lobbyCompletedHandleResponseJson().GetObject());
        VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
    }

    DEFINE_TEST_CASE(TestSwitchingLobbies)
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext.get());

        // Join a new lobby
        JoinLobbyWithValidHandleIdAndContext(xboxLiveContext.get());
    }

    /*
        1. Test With Transfer Handle
        2. Test with transfer handle but failed to join game. So create a new game
        3. Test without transfer handle i.e. create a new game
        4. Test without transfer handle but failed to create new game
        5. Test without transfer handle i.e. create a new game but failed to advertise game session (TODO)
        6. Test without transfer handle - failed to write "pending~" with 412 with no transfer handle written (TODO)
            : retry writing, or both clients will end up creating it's own session.
    */
    void JoinGameFromLobbyMultipleUsersHelper(std::vector<XblContextHandle> xboxLiveContexts)
    {
        AddMultipleLocalUserHelper(xboxLiveContexts);

        HttpMock mock(POST, transferHandleUri, 201);
        mock.SetResponseBody(defaultMultipleLocalUsersGameResponse);
        mock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGameFromLobby(GAME_TEMPLATE_NAME));

        int count{ 0 };
        bool isDone{ false };
        while (!isDone)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount && !isDone; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::JoinGameCompleted)
                {
                    isDone = true;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_TRUE(isDone);
        VerifyLobby(defaultMultipleLocalUsersLobbyResponseJson().GetObject());
        VerifyGame(defaultMultipleLocalUsersGameResponseJson().GetObject());
    }

    DEFINE_TEST_CASE(TestJoinGameFromLobbyWithTransferHandle)
    {
        TEST_LOG(L"Test starting: TestJoinGameFromLobbyWithTransferHandle");

        MPMTestEnvironment env{};
        XTaskQueueHandle queue{};

        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext.get()));

        AddLocalUserHelper(xboxLiveContext.get(), lobbyWithCompletedTransferHandleResponse);

        HttpMock gameMock(POST, transferHandleUri, 201);
        gameMock.SetResponseBody(defaultGameSessionResponse);
        gameMock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGameFromLobby(GAME_TEMPLATE_NAME));

        int count{ 0 };
        bool isDone{ false };
        while (!isDone)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount && !isDone; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::JoinGameCompleted)
                {
                    isDone = true;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_TRUE(isDone);
        VerifyLobby(lobbyCompletedHandleResponseJson().GetObject());
        VerifyGame(defaultGameSessionResponseJson().GetObject());
    }

    DEFINE_TEST_CASE(TestJoinGameFromLobbyWithTransferHandleMultipleUsers)
    {
        TEST_LOG(L"Test starting: TestJoinGameFromLobbyWithTransferHandleMultipleUsers");

        MPMTestEnvironment env{};
        auto xboxLiveContext1 = env.CreateMockXboxLiveContext(1234);
        auto xboxLiveContext2 = env.CreateMockXboxLiveContext(2345);

        std::vector<XblContextHandle> xboxLiveContexts
        {
            xboxLiveContext1.get(),
            xboxLiveContext2.get(),
        };

        JoinGameFromLobbyMultipleUsersHelper(xboxLiveContexts);
    }

    DEFINE_TEST_CASE(TestJoinGameFromLobbyFailedToJoin)
    {
        TEST_LOG(L"Test starting: TestJoinGameFromLobbyFailedToJoin");

        MPMTestEnvironment env{};
        XTaskQueueHandle queue{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext.get()));

        AddLocalUserHelper(xboxLiveContext.get());

        std::vector<const char*> lobbyResponses
        {
            lobbyWithPendingTransferHandleResponse,
            defaultGameSessionResponse,
            lobbyWithCompletedTransferHandleResponse
        };

        HttpMock lobbyMock(POST, defaultMpsdUri, 201);
        lobbyMock.SetResponseBody(lobbyResponses[2]);
        lobbyMock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        HttpMock gameMock(POST, defaultGameHttpHeaderUri, 404);
        gameMock.SetResponseBody(defaultGameSessionResponse);
        gameMock.SetResponseHeaders(defaultGameHttpResponseHeaders);
        
        HttpMock transferMock(POST, transferHandleUri, 404);
        transferMock.SetResponseBody(defaultGameSessionResponse);
        transferMock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGameFromLobby(GAME_TEMPLATE_NAME));

        int count{ 0 };
        bool isGameJoined = false, isAdvertisingGameDone = false;
        auto customProps = JsonUtils::SerializeJson(classPropertiesJson()["properties"]["custom"]);
        while (!isGameJoined || !isAdvertisingGameDone)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount && !isGameJoined; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::JoinGameCompleted)
                {
                    isGameJoined = true;
                }
            }

            auto props{ XblMultiplayerManagerLobbySessionPropertiesJson() };
            if (isGameJoined && 
                props != nullptr &&
                utils::str_icmp(props, customProps.c_str()) == 0)
            {
                isAdvertisingGameDone = true;
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!isGameJoined || !isAdvertisingGameDone);
        VerifyLobby(lobbyCompletedHandleResponseJson().GetObject());
        VerifyGame(lobbyCompletedHandleResponseJson().GetObject());
    }

    DEFINE_TEST_CASE(TestJoinGameFromLobbyNoHandleCreateNewGame)
    {
        TEST_LOG(L"Test starting: TestJoinGameFromLobbyNoHandleCreateNewGame");

        MPMTestEnvironment env{};
        XTaskQueueHandle queue{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext.get()));

        AddLocalUserHelper(xboxLiveContext.get(), lobbyWithNoTransferHandleResponse);

        std::vector<const char*> lobbyResponses
        {
            lobbyWithPendingTransferHandleResponse,
            defaultGameSessionResponse,
            lobbyWithCompletedTransferHandleResponse
        };

        HttpMock lobbyMock(POST, defaultMpsdUri, 200);
        lobbyMock.SetResponseBody(lobbyResponses[2]);
        lobbyMock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGameFromLobby(GAME_TEMPLATE_NAME));

        int count{ 0 };
        bool isDone = false, isAdvertisingGameDone = false;
        auto customProps = JsonUtils::SerializeJson(classPropertiesJson()["properties"]["custom"]);
        while (!isDone || !isAdvertisingGameDone)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount && !isDone; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::JoinGameCompleted)
                {
                    isDone = true;
                }
            }
            
            auto props{ XblMultiplayerManagerLobbySessionPropertiesJson() };
            if (props != nullptr &&
                utils::str_icmp(props, customProps.c_str()) == 0)
            {
                isAdvertisingGameDone = true;
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!isDone || !isAdvertisingGameDone);
        VerifyLobby(lobbyCompletedHandleResponseJson().GetObject());
        VerifyGame(lobbyCompletedHandleResponseJson().GetObject());
    }

    DEFINE_TEST_CASE(TestJoinGameFromLobbyNoHandleFailedToCreateNewGame)
    {
        TEST_LOG(L"Test starting: TestJoinGameFromLobbyNoHandleFailedToCreateNewGame");

        MPMTestEnvironment env{};
        XTaskQueueHandle queue{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext.get()));

        AddLocalUserHelper(xboxLiveContext.get(), lobbyWithNoTransferHandleResponse);

        std::vector<const char*> gameResponses
        {
            lobbyWithPendingTransferHandleResponse,
            defaultGameSessionResponse,
            updatedLobbyWithNoTransferHandleResponse
        };

        HttpMock gameMock(POST, defaultMpsdUri, 404);
        gameMock.SetResponseBody(gameResponses[2]);
        gameMock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGameFromLobby(GAME_TEMPLATE_NAME));

        int count{ 0 };
        bool isDone = false, isStopAdvertisingGameDone = false;
        auto customProps = JsonUtils::SerializeJson(propertiesNoTransferHandleJson()["properties"]["custom"]);
        while (!isDone || !isStopAdvertisingGameDone)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::JoinGameCompleted)
                {
                    isDone = true;
                    VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_NOT_FOUND, events[i].Result);
                }
            }

            auto props{ XblMultiplayerManagerLobbySessionPropertiesJson() };
            if (isDone &&
                props != nullptr &&
                utils::str_icmp(props, customProps.c_str()) == 0)
            {
                isStopAdvertisingGameDone = true;
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!isDone || !isStopAdvertisingGameDone);
        VerifyLobby(lobbyNoHandleResponseJson().GetObject());
        VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
    }

    void JoinGameHelper(
        XblContextHandle xboxLiveContext,
        std::vector<uint64_t> xboxUserIds = {}
        )
    {
        /*
            join_game_helper -> join_game_for_all_local_members
            AdvertiseGameSession -> commit_pending_lobby_changes -> commit_lobby_changes_helper (write_session)
            set_local_member_properties_to_remote_session
            commit_lobby_changes_helper (set_activity)
            AdvertiseGameSession -> set_transfer_handle
            AdvertiseGameSession -> write_session (transfer handle)
        */

        XTaskQueueHandle queue{};
        VERIFY_SUCCEEDED(XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue));
        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext));

        auto gameResponse{ xboxUserIds.size() > 0 ? defaultGameSessionWithXuidsResponse
                                                  : defaultGameSessionResponse };

        std::vector<const char*> lobbyResponses
        {
            lobbyWithNoTransferHandleResponse,
            lobbyWithCompletedTransferHandleResponse
        };

        HttpMock lobbyMock(POST, defaultMpsdUri, 201);
        lobbyMock.SetResponseBody(lobbyResponses[0]);
        lobbyMock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        HttpMock gameMock(POST, defaultGameUri, 201);
        gameMock.SetResponseBody(gameResponse);
        gameMock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        uint32_t contextIds[3]{ 1,2,3 };
        XblUserHandle userHandle = xboxLiveContext->User().Handle();
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionAddLocalUser(userHandle));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(userHandle, "Health", "89", &contextIds[0]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(userHandle, "Skill", "17", &contextIds[1]));

        lobbyMock.SetResponseBody(lobbyResponses[1]);
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(userHandle, CONNECTION_ADDR, &contextIds[2]));
        
        if (xboxUserIds.size() > 0)
        {
            VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGame(GAME_SESSION_NAME, GAME_TEMPLATE_NAME, xboxUserIds.data(), xboxUserIds.size()));
        }
        else
        {
#pragma warning(suppress: 6387)
            VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGame(GAME_SESSION_NAME, GAME_TEMPLATE_NAME, nullptr, 0));
        }

        int count{ 0 };
        bool lobbyJoined{ false };
        bool gameJoined{ false };
        bool isAdvertisingGameDone{ false };
        uint32_t localUserPropWritten{ 0 };
        auto customProps = JsonUtils::SerializeJson(classPropertiesJson()["properties"]["custom"]);
        while (!lobbyJoined || !gameJoined || !isAdvertisingGameDone || localUserPropWritten != 3)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                switch (events[i].EventType)
                {
                    case XblMultiplayerEventType::UserAdded:
                        lobbyJoined = true;
                        break;
                    case XblMultiplayerEventType::LocalMemberPropertyWriteCompleted:
                        VerifyContext12AndIncrement(events[i], &localUserPropWritten);
                        break;
                    case XblMultiplayerEventType::LocalMemberConnectionAddressWriteCompleted:
                        VerifyContext3AndIncrement(events[i], &localUserPropWritten);
                        break;
                    case XblMultiplayerEventType::JoinGameCompleted:
                        gameJoined = true;
                        break;
                }
            }

            auto propertiesStr = XblMultiplayerManagerLobbySessionPropertiesJson();
            if (propertiesStr != nullptr && utils::str_icmp(propertiesStr, customProps.c_str()) == 0)
            {
                isAdvertisingGameDone = true;
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!lobbyJoined || !gameJoined || !isAdvertisingGameDone || localUserPropWritten != 3);
        VerifyLobby(lobbyCompletedHandleResponseJson().GetObject());

        if (xboxUserIds.size() > 0)
        {
            VerifyGame(defaultGameSessionWithXuidsResponseJson().GetObject());
        }
        else
        {
            VerifyGame(defaultGameSessionResponseJson().GetObject());
        }
    }

    DEFINE_TEST_CASE(TestJoinGame)
    {
        TEST_LOG(L"Test starting: TestJoinGame");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        JoinGameHelper(xboxLiveContext.get());
    }

    DEFINE_TEST_CASE(TestJoinGameWithXuids)
    {
        TEST_LOG(L"Test starting: TestJoinGameWithXuids");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        std::vector<uint64_t> initiatorIds{ 1234, 2345, 3456 };
        JoinGameHelper(xboxLiveContext.get(), initiatorIds);
    }

    enum class CallingPatternType
    {
        Sync,
        Async,
        ReverseSync,
        ReverseAsync,
        Combination,
        ReverseCombination
    };

    void AsyncRemoveLocalUser(XblContextHandle xboxLiveContext, XAsyncBlock* async)
    {
        auto uri{ defaultMpsdUri };
        auto rtaId{ rtaConnectionId };
        auto response{ defaultLobbySessionResponse };
        auto headers{ defaultLobbyHttpResponseHeaders };

        RunAsync(async, __FUNCTION__,
            [xboxLiveContext, rtaId, uri, response, headers](XAsyncOp op, const XAsyncProviderData* data)
            {
                UNREFERENCED_PARAMETER(data);
                switch (op)
                {
                    case XAsyncOp::DoWork:
                    {
                        HttpMock lobbyMock(POST, uri, 201);
                        lobbyMock.SetResponseBody(response);
                        lobbyMock.SetResponseHeaders(headers);

                        return XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext->User().Handle());
                    }
                    default:
                        return S_OK;
                }
            });
    }

    void AsyncJoinGame(XblContextHandle xboxLiveContext, XAsyncBlock* async)
    {
        auto uri{ defaultGameUri };
        auto rtaUri{ defaultMpsdUri };
        auto rtaId{ rtaConnectionId };
        auto response{ defaultGameSessionResponse };
        auto headers{ defaultGameHttpResponseHeaders };

        RunAsync(async, __FUNCTION__,
            [xboxLiveContext, rtaUri, rtaId, uri, response, headers](XAsyncOp op, const XAsyncProviderData* data)
            {
                UNREFERENCED_PARAMETER(data);
                switch (op)
                {
                    case XAsyncOp::DoWork:
                    {
                        HttpMock gameMock(POST, uri, 201);
                        gameMock.SetResponseBody(response);
                        gameMock.SetResponseHeaders(headers);

#pragma warning(suppress: 6387)
                        return XblMultiplayerManagerJoinGame(GAME_SESSION_NAME, GAME_TEMPLATE_NAME, nullptr, 0);
                    }
                    default:
                        return S_OK;
                }
            });
    }

    void AsyncLeaveGame(bool& leaveGameCompleted, XAsyncBlock* async)
    {
        RunAsync(async, __FUNCTION__,
            [&leaveGameCompleted](XAsyncOp op, const XAsyncProviderData* data)
            {
                UNREFERENCED_PARAMETER(data);
                switch (op)
                {
                    case XAsyncOp::DoWork:
                    {
                        auto hr = XblMultiplayerManagerLeaveGame();
                        if (FAILED(hr))
                        {
                            leaveGameCompleted = true;
                        }

                        return hr;
                    }
                    default:
                        return S_OK;
                }
            });
    }

    void TestLeaveMultiplayerHelper(CallingPatternType type)
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        JoinGameHelper(xboxLiveContext.get());

        HttpMock gameMock(GET, defaultGameHttpHeaderUri, 200);
        gameMock.SetResponseBody(emptyResponse);
        gameMock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        HttpMock lobbyMock(GET, defaultMpsdUri, 200);
        lobbyMock.SetResponseBody(emptyResponse);
        lobbyMock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        XAsyncBlock asyncLeave{};
        XAsyncBlock asyncRemove{};
        bool leaveGameCompleted{ false };

        switch (type)
        {
            case CallingPatternType::Sync:
            {
                VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext->User().Handle()));
                VERIFY_SUCCEEDED(XblMultiplayerManagerLeaveGame());
                break;
            }
            case CallingPatternType::ReverseSync:
            {
                VERIFY_SUCCEEDED(XblMultiplayerManagerLeaveGame());
                VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext->User().Handle()));
                break;
            }
            case CallingPatternType::Async:
            {
                AsyncRemoveLocalUser(xboxLiveContext.get(), &asyncRemove);
                AsyncLeaveGame(leaveGameCompleted, &asyncLeave);
                break;
            }
            case CallingPatternType::ReverseAsync:
            {
                AsyncLeaveGame(leaveGameCompleted, &asyncLeave);
                AsyncRemoveLocalUser(xboxLiveContext.get(), &asyncRemove);
                break;
            }
            case CallingPatternType::Combination:
            {
                VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext->User().Handle()));
                AsyncLeaveGame(leaveGameCompleted, &asyncLeave);
                break;
            }
            case CallingPatternType::ReverseCombination:
            {
                VERIFY_SUCCEEDED(XblMultiplayerManagerLeaveGame());
                AsyncRemoveLocalUser(xboxLiveContext.get(), &asyncRemove);
                break;
            }
        }

        int count{ 0 };
        bool userRemoved{ false }, clientDisconnected{ false };
        while (!userRemoved || !leaveGameCompleted || !clientDisconnected)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                switch (events[i].EventType)
                {
                    case XblMultiplayerEventType::UserRemoved:
                        userRemoved = true;
                        TEST_LOG(L"TestLeaveMultiplayerHelper - user removed.");
                        break;
                    case XblMultiplayerEventType::LeaveGameCompleted:
                        leaveGameCompleted = true;
                        TEST_LOG(L"TestLeaveMultiplayerHelper - leave game completed.");
                        break;
                    case XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService:
                        VERIFY_IS_FALSE(clientDisconnected);
                        clientDisconnected = true;
                        TEST_LOG(L"TestLeaveMultiplayerHelper - client disconnected from multiplayer service.");
                        break;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!userRemoved || !leaveGameCompleted || !clientDisconnected);
        XAsyncGetStatus(&asyncLeave, true);
        XAsyncGetStatus(&asyncRemove, true);

        VERIFY_ARE_EQUAL_UINT(0, XblMultiplayerManagerLobbySessionLocalMembersCount());
        VERIFY_ARE_EQUAL_UINT(0, XblMultiplayerManagerLobbySessionMembersCount());
        VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
    }

    DEFINE_TEST_CASE(TestLeaveMultiplayer_1)
    {
        TEST_LOG(L"Test starting: TestLeaveMultiplayer_1");

        TestLeaveMultiplayerHelper(CallingPatternType::Sync);
    }

    DEFINE_TEST_CASE(TestLeaveMultiplayer_2)
    {
        TEST_LOG(L"Test starting: TestLeaveMultiplayer_2");

        TestLeaveMultiplayerHelper(CallingPatternType::Async);
    }

    DEFINE_TEST_CASE(TestLeaveMultiplayer_3)
    {
        TEST_LOG(L"Test starting: TestLeaveMultiplayer_3");

        TestLeaveMultiplayerHelper(CallingPatternType::Combination);
    }

    DEFINE_TEST_CASE(TestLeaveMultiplayer_4)
    {
        TEST_LOG(L"Test starting: TestLeaveMultiplayer_4");

        TestLeaveMultiplayerHelper(CallingPatternType::ReverseSync);
    }

    DEFINE_TEST_CASE(TestLeaveMultiplayer_5)
    {
        TEST_LOG(L"Test starting: TestLeaveMultiplayer_5");

        TestLeaveMultiplayerHelper(CallingPatternType::ReverseAsync);
    }

    DEFINE_TEST_CASE(TestLeaveMultiplayer_6)
    {
        TEST_LOG(L"Test starting: TestLeaveMultiplayer_6");

        TestLeaveMultiplayerHelper(CallingPatternType::ReverseCombination);
    }

    DEFINE_TEST_CASE(TestMultipleLocalUsers_1)
    {
        TEST_LOG(L"Test starting: TestMultipleLocalUsers_1");

        MPMTestEnvironment env{};
        auto xboxLiveContext1 = env.CreateMockXboxLiveContext(1234);
        auto xboxLiveContext2 = env.CreateMockXboxLiveContext(2345);

        std::vector<XblContextHandle> xboxLiveContexts{ xboxLiveContext1.get(), xboxLiveContext2.get() };
        AddMultipleLocalUserHelper(xboxLiveContexts);

        HttpMock mock(POST, defaultMpsdUri, 201);
        mock.SetResponseBody(defaultLobbySessionResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext2.get()->User().Handle()));

        int count{ 0 };
        bool userRemoved{ false };
        while (!userRemoved)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::UserRemoved)
                {
                    userRemoved = true;
                }
                else if (events[i].EventType == XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    // Should not have fired as we still have one more user.
                    VERIFY_IS_TRUE(false);
                }
            }

            Sleep(10);
        }

        VERIFY_IS_TRUE(userRemoved);
        VerifyLobby(defaultMultipleLocalUsersLobbyResponseJson().GetObject());
    }

    // Add multiple users while removing a user (on diff threads)
    DEFINE_TEST_CASE(TestMultipleLocalUsers_2)
    {
        TEST_LOG(L"Test starting: TestMultipleLocalUsers_2");

        MPMTestEnvironment env{};
        auto xboxLiveContext1 = env.CreateMockXboxLiveContext(1234);
        auto xboxLiveContext2 = env.CreateMockXboxLiveContext(2345);
        auto xboxLiveContext3 = env.CreateMockXboxLiveContext(3456);

        std::vector<XblContextHandle> xboxLiveContexts{ xboxLiveContext1.get(), xboxLiveContext2.get() };
        AddMultipleLocalUserHelper(xboxLiveContexts);

        HttpMock mock(GET, defaultMpsdUri, 200);
        mock.SetResponseBody(multipleLocalUsersLobbyResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        uint32_t contextIds[2]{ 1,2 };
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionAddLocalUser(xboxLiveContext3->User().Handle()));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(xboxLiveContext3->User().Handle(), "Health", "89", &contextIds[0]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionSetLocalMemberProperties(xboxLiveContext3->User().Handle(), "Skill", "17", &contextIds[1]));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext2.get()->User().Handle()));

        int count{ 0 };
        bool userRemoved = false, userAdded = false;
        uint32_t localUserPropWritten = 0;
        while (!userRemoved || !userAdded || localUserPropWritten != 2)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                switch (events[i].EventType)
                {
                    case XblMultiplayerEventType::UserAdded:
                        userAdded = true;
                        break;
                    case XblMultiplayerEventType::LocalMemberPropertyWriteCompleted:
                        VerifyContext12AndIncrement(events[i], &localUserPropWritten);
                        break;
                    case XblMultiplayerEventType::UserRemoved:
                        userRemoved = true;
                        break;
                    case XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService:
                        // Should not have fired as we still have one more user.
                        VERIFY_IS_TRUE(false);
                        break;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!userRemoved || !userAdded || localUserPropWritten != 2);
        VerifyLobby(multipleLocalUsersLobbyResponseJson().GetObject());
    }

    // Constantly add/remove users
    DEFINE_TEST_CASE(TestMultipleLocalUsers_3)
    {
        TEST_LOG(L"Test starting: TestMultipleLocalUsers_3");

        MPMTestEnvironment env{};
        auto xboxLiveContext1 = env.CreateMockXboxLiveContext(1234);
        auto xboxLiveContext2 = env.CreateMockXboxLiveContext(2345);
        auto xboxLiveContext3 = env.CreateMockXboxLiveContext(3456);
        auto xboxLiveContext4 = env.CreateMockXboxLiveContext(4567);
        auto xboxLiveContext5 = env.CreateMockXboxLiveContext(5678);

        std::vector<XblContextHandle> xboxLiveContexts{ xboxLiveContext1.get(), xboxLiveContext2.get() };

        AddMultipleLocalUserHelper(xboxLiveContexts);

        HttpMock mock(GET, defaultMpsdUri, 200);
        mock.SetResponseBody(multipleLocalUsersLobbyResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionAddLocalUser(xboxLiveContext3->User().Handle()));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionAddLocalUser(xboxLiveContext4->User().Handle()));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext2.get()->User().Handle()));
        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionAddLocalUser(xboxLiveContext5->User().Handle()));

        int count{ 0 };
        uint32_t userRemoved = 0, userAdded = 0, eventCount = 0;
        // Since the mocked session only has 2 members, you will get into a scenario where the user was removed before he could have been added,
        // giving you 2 extra added events. However, due to a race between adding & removing (being called on diff threads) this happens only sometimes.
        while (userRemoved != 3 || userAdded < 3)   
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (eventCount == 0)
                {
                    eventCount++;
                    VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext4.get()->User().Handle()));
                }
                else if (eventCount == 1)
                {
                    eventCount++;
                    VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext5.get()->User().Handle()));
                }

                switch (events[i].EventType)
                {
                    case XblMultiplayerEventType::UserAdded:
                        userAdded++;
                        break;
                    case XblMultiplayerEventType::UserRemoved:
                        userRemoved++;
                        break;
                    case XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService:
                        // Should not have fired as we still have one more user.
                        VERIFY_IS_TRUE(false);
                        break;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(userRemoved != 3 || userAdded < 3);
        VerifyLobby(multipleLocalUsersLobbyResponseJson().GetObject());
    }

    void TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType type)
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelper(xboxLiveContext.get());

        HttpMock gameMock(POST, defaultGameUri, 201);
        gameMock.SetResponseBody(defaultGameSessionResponse);
        gameMock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        HttpMock lobbyMock(POST, defaultMpsdUri, 201);
        lobbyMock.SetResponseBody(defaultLobbySessionResponse);
        lobbyMock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext.get()));

        XAsyncBlock asyncJoin{};
        XAsyncBlock asyncRemove{};

        switch (type)
        {
            case CallingPatternType::Sync:
            {
                VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext->User().Handle()));
                VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGame(GAME_SESSION_NAME, GAME_TEMPLATE_NAME, nullptr, 0));
                break;
            }
            case CallingPatternType::ReverseSync:
            {
                VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGame(GAME_SESSION_NAME, GAME_TEMPLATE_NAME, nullptr, 0));
                VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext->User().Handle()));
                break;
            }
            case CallingPatternType::Async:
            {
                AsyncRemoveLocalUser(xboxLiveContext.get(), &asyncRemove);
                AsyncJoinGame(xboxLiveContext.get(), &asyncJoin);
                break;
            }
            case CallingPatternType::ReverseAsync:
            {
                AsyncJoinGame(xboxLiveContext.get(), &asyncJoin);
                AsyncRemoveLocalUser(xboxLiveContext.get(), &asyncRemove);
                break;
            }
            case CallingPatternType::Combination:
            {
                VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionRemoveLocalUser(xboxLiveContext->User().Handle()));
                AsyncJoinGame(xboxLiveContext.get(), &asyncJoin);
                break;
            }
            case CallingPatternType::ReverseCombination:
            {
#pragma warning(suppress: 6387)
                VERIFY_SUCCEEDED(XblMultiplayerManagerJoinGame(GAME_SESSION_NAME, GAME_TEMPLATE_NAME, nullptr, 0));
                AsyncRemoveLocalUser(xboxLiveContext.get(), &asyncRemove);
                break;
            }
        }

        int count{ 0 };
        bool userRemoved = false, clientDisconnected = false;
        while (!userRemoved || !clientDisconnected)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::UserRemoved)
                {
                    userRemoved = true;
                }
                else if (events[i].EventType == XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    VERIFY_IS_FALSE(clientDisconnected);
                    clientDisconnected = true;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!userRemoved || !clientDisconnected);
        VERIFY_ARE_EQUAL_UINT(0, XblMultiplayerManagerLobbySessionLocalMembersCount());
        VERIFY_ARE_EQUAL_UINT(0, XblMultiplayerManagerLobbySessionMembersCount());
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_1)
    {
        TEST_LOG(L"Test starting: TestJoinGameWhileRemovingLocalUser_1");

        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::Sync);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_2)
    {
        TEST_LOG(L"Test starting: TestJoinGameWhileRemovingLocalUser_2");

        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::Async);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_3)
    {
        TEST_LOG(L"Test starting: TestJoinGameWhileRemovingLocalUser_3");

        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::Combination);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_4)
    {
        TEST_LOG(L"Test starting: TestJoinGameWhileRemovingLocalUser_4");

        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::ReverseSync);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_5)
    {
        TEST_LOG(L"Test starting: TestJoinGameWhileRemovingLocalUser_5");

        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::ReverseAsync);
    }

    DEFINE_TEST_CASE(TestJoinGameWhileRemovingLocalUser_6)
    {
        TEST_LOG(L"Test starting: TestJoinGameWhileRemovingLocalUser_6");

        TestJoinGameWhileRemovingLocalUserHelper(CallingPatternType::ReverseCombination);
    }

    DEFINE_TEST_CASE(TestSubscriptionsLostEvent)
    {
        TEST_LOG(L"Test starting: TestSubscriptionsLostEvent");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelperWithSyncUpdate(xboxLiveContext.get());

        HttpMock mock(POST, defaultMpsdUri, 201);
        mock.SetResponseBody(emptyResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        GlobalState::Get()->MultiplayerManager()->GetMultiplayerClientManager()->OnMultiplayerSubscriptionsLost();

        int count{ 0 };
        bool userRemoved = false, clientDisconnected = false;
        while (!userRemoved || !clientDisconnected)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            VERIFY_SUCCEEDED(XblMultiplayerManagerDoWork(&events, &eventsCount));

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                TEST_LOG(FormatString(L" [MPM] RemoveLocalUserHelper - Event type: %d", events[i].EventType).c_str());
                
                if (events[i].EventType == XblMultiplayerEventType::UserRemoved)
                {
                    userRemoved = true;
                }
                else if (events[i].EventType == XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService)
                {
                    VERIFY_IS_FALSE(clientDisconnected);
                    clientDisconnected = true;
                }
            }

            Sleep(10);
        }
        
        VERIFY_IS_FALSE(!userRemoved || !clientDisconnected);
        VERIFY_ARE_EQUAL_UINT(0, XblMultiplayerManagerLobbySessionLocalMembersCount());
        VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
    }

    /*
        multiplayer_session_writer: Back-to-back session writes
        1. Write change #1 followed with change # 2 == final result #2
        2. Write change #2 followed with change # 1 == final result #2
    */

    void TestBackToBackSessionWriteLogicHelper(std::vector<const char*> responses, uint64_t maxChangeNumber)
    {
        MPMTestEnvironment env{};

        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelper(xboxLiveContext.get());
        
        HttpMock mock(POST, defaultMpsdUri, 201);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        auto session = GetSession(false);
        auto sessionWriter = GetSessionWriter(false);
        auto mpmInstance = GlobalState::Get()->MultiplayerManager();

        Event sessionEvent;
        auto callback = [&sessionEvent](Result<std::shared_ptr<XblMultiplayerSession>> result)
        {
            sessionEvent.Set();
        };
        
        mock.SetResponseBody(responses[0]);
        VERIFY_SUCCEEDED(sessionWriter->WriteSession(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, true, callback));
        sessionEvent.Wait();
        mock.SetResponseBody(responses[1]);
        VERIFY_SUCCEEDED(sessionWriter->WriteSession(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, true, callback));
        sessionEvent.Wait();
        mock.SetResponseBody(responses[2]);
        VERIFY_SUCCEEDED(sessionWriter->WriteSession(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, true, callback));
        
        auto isDone = 0;
        while (isDone < 10)
        {
            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            Sleep(10);
            isDone++;
        }

        VERIFY_ARE_EQUAL_UINT(maxChangeNumber, mpmInstance->LobbySession()->ChangeNumber());

        VerifyLobby(lobbyCompletedHandleResponseJson().GetObject());
    }

    DEFINE_TEST_CASE(TestBackToBackSessionWriteLogic_1)
    {
        TEST_LOG(L"Test starting: TestBackToBackSessionWriteLogic_1");

        std::vector<const char*> responses
        {
            defaultLobbySessionResponse,             // change #1
            lobbyWithPendingTransferHandleResponse,  // change #2
            lobbyWithCompletedTransferHandleResponse // change #3
        };

        TestBackToBackSessionWriteLogicHelper(responses, 3);
    }

    DEFINE_TEST_CASE(TestBackToBackSessionWriteLogic_2)
    {
        TEST_LOG(L"Test starting: TestBackToBackSessionWriteLogic_2");

        std::vector<const char*> responses
        {
            lobbyWithCompletedTransferHandleResponse, // change #3
            lobbyWithPendingTransferHandleResponse,   // change #2
            defaultLobbySessionResponse               // change #1
        };

        TestBackToBackSessionWriteLogicHelper(responses, 3);
    }

    void MultipleTapsHelper(std::vector<const char*> getResponses, std::vector<uint32_t> getStatuses, std::vector<uint64_t> changeNumbers, uint64_t maxChangeNumber)
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelper(xboxLiveContext.get());

        uint32_t mockCount{ 0 };
        auto mock = std::make_shared<HttpMock>(GET, defaultMpsdUri);
        mock->SetResponseBody(getResponses[0]);
        mock->SetResponseHttpStatus(getStatuses[0]);
        mock->SetResponseHeaders(defaultLobbyHttpResponseHeaders);
        mock->SetMockMatchedCallback([getResponses, getStatuses, &mockCount](class HttpMock* matchedMock, xsapi_internal_string actualRequestUrl, xsapi_internal_string requestBody)
        {
            if (mockCount < getResponses.size())
            {
                matchedMock->SetResponseBody(getResponses[mockCount]);
                matchedMock->SetResponseHttpStatus(getStatuses[mockCount]);
                ++mockCount;
            }
        });

        auto session = GetSession(false);
        auto sessionWriter = GetSessionWriter(false);
        auto mpmInstance = GlobalState::Get()->MultiplayerManager();
        
        for (uint32_t i = 0; i < getResponses.size(); ++i)
        {
            sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", changeNumbers[i] });
        }

        int count{ 0 };
        while (mpmInstance->LobbySession()->ChangeNumber() != maxChangeNumber)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            Sleep(10);
        }

        VERIFY_ARE_EQUAL_UINT(maxChangeNumber, mpmInstance->LobbySession()->ChangeNumber());

        VerifyLobby(lobbyCompletedHandleResponseJson().GetObject());
    }

    DEFINE_TEST_CASE(TestMultipleTaps_1)
    {
        TEST_LOG(L"Test starting: TestMultipleTaps_1");

        // Set up initial http responses
        std::vector<const char*> getResponses
        {
            lobbyWithCompletedTransferHandleResponse, // change #3
            lobbyWithPendingTransferHandleResponse,   // change #2
            defaultLobbySessionResponse               // change #1
        };

        std::vector<uint32_t> getStatuses{ 200,200,404 };
        std::vector<uint64_t> changeNumbers{ 3,2,1 };

        MultipleTapsHelper(getResponses, getStatuses, changeNumbers, 3);
    }

    DEFINE_TEST_CASE(TestMultipleTaps_2)
    {
        TEST_LOG(L"Test starting: TestMultipleTaps_2");

        std::vector<const char*> getResponses
        {
            lobbyWithPendingTransferHandleResponse,   // change #2
            lobbyWithCompletedTransferHandleResponse, // change #3
        };

        std::vector<uint32_t> getStatuses{ 200,200 };
        std::vector<uint64_t> changeNumbers{ 2,3 };

        MultipleTapsHelper(getResponses, getStatuses, changeNumbers, 3);
    }

    /*
        multiplayer_session_writer:
        Write Session + Taps (write_session & on_session_changed)
        1. call multiple different writes with shoulder taps in between; ensure that the fianl session # is correct.
        Note: MPM currently does not optimize to ensure that only 1 GET is in flight for multiple shoulder taps.
        2. call write (ch. #6) followed with multiple different writes with shoulder taps; ensure that GET is never called.
        3. call write (ch. #2, #4) followed with shoulder tap (ch. #6); ensure that GET is called.
    */

    void TestMultipleWriteSessionWithTapsHelper(
        std::vector<const char*> writeResponses,
        std::vector<const char*> getResponses,
        uint64_t maxChangeNumber,
        xsapi_internal_string handleId = xsapi_internal_string()
        )
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelper(xboxLiveContext.get());

        HttpMock mockGet(GET, defaultMpsdUri, 200);
        mockGet.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        HttpMock mockWrite(POST, defaultMpsdUri, 201);
        mockWrite.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        auto session = GetSession(false);
        auto sessionWriter = GetSessionWriter(false);
        auto mpmInstance = GlobalState::Get()->MultiplayerManager();

        Event sessionEvent;
        auto callback = [&sessionEvent](Result<std::shared_ptr<XblMultiplayerSession>> result)
        {
            sessionEvent.Set();
        };

        for (auto writeResponse : writeResponses)
        {
            mockWrite.SetResponseBody(writeResponse);

            if (handleId.empty())
            {
                VERIFY_SUCCEEDED(sessionWriter->WriteSession(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, true, callback));
            }
            else
            {
                VERIFY_SUCCEEDED(sessionWriter->WriteSessionByHandle(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, handleId, true, callback));
            }

            sessionEvent.Wait();
        }

        auto isDone = 0;
        while (isDone < 10)
        {
            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            Sleep(10);
            isDone++;
        }

        VERIFY_ARE_EQUAL_UINT(maxChangeNumber, mpmInstance->LobbySession()->ChangeNumber());
    }

    DEFINE_TEST_CASE(TestWriteSessionWithTaps_1)
    {
        TEST_LOG(L"Test starting: TestWriteSessionWithTaps_1");

        // Set up initial http responses
        std::vector<const char*> writeResponses
        {
            lobbyWithPendingTransferHandleResponse, // change #2
            sessionChangeNum4,                      // change #4
            sessionChangeNum6                       // change #6
        };

        std::vector<const char*> getResponses
        {
            sessionChangeNum3, // change #3
            sessionChangeNum5  // change #5
        };

        TestMultipleWriteSessionWithTapsHelper(writeResponses, getResponses, 6);
        TestMultipleWriteSessionWithTapsHelper(writeResponses, getResponses, 6, "TestHandleId");
    }

    // The getResponseStruct is used for shoulderTaps.
    void TestWriteSessionWithTapsHelper(
        std::vector<const char*> writeResponses, 
        const char* getResponse,
        uint64_t maxChangeNumberForTap,
        uint64_t maxChangeNumberToTest,
        bool waitForFirstWriteToFinish,
        xsapi_internal_string handleId = xsapi_internal_string()
        )
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelper(xboxLiveContext.get());

        uint32_t mockCount{ 0 };
        auto mock = std::make_shared<HttpMock>(POST, defaultMpsdUri, 201);
        mock->SetResponseBody(writeResponses[0]);
        mock->SetResponseHeaders(defaultLobbyHttpResponseHeaders);
        mock->SetMockMatchedCallback([writeResponses, &mockCount](class HttpMock* matchedMock, xsapi_internal_string actualRequestUrl, xsapi_internal_string requestBody)
        {
            if (mockCount < writeResponses.size())
            {
                matchedMock->SetResponseBody(writeResponses[mockCount]);
                ++mockCount;
            }
        });

        Event sessionEvent;
        auto callback = [&sessionEvent](Result<std::shared_ptr<XblMultiplayerSession>> result)
        {
            sessionEvent.Set();
        };

        auto session = GetSession(false);
        auto sessionWriter = GetSessionWriter(false);
        auto mpmInstance = GlobalState::Get()->MultiplayerManager();

        if (handleId.empty())
        {
            VERIFY_SUCCEEDED(sessionWriter->WriteSession(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, true, callback));
        }
        else
        {
            VERIFY_SUCCEEDED(sessionWriter->WriteSessionByHandle(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, handleId, true, callback));
        }

        // Wait before OnSessionChanged or not
        if (waitForFirstWriteToFinish)
        {
            sessionEvent.Wait();
        }

        auto eventArgs = XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 3 };
        sessionWriter->OnSessionChanged(eventArgs);

        if (!waitForFirstWriteToFinish)
        {
            sessionEvent.Wait();
        }

        if (handleId.empty())
        {
            VERIFY_SUCCEEDED(sessionWriter->WriteSession(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, true, callback));
        }
        else
        {
            VERIFY_SUCCEEDED(sessionWriter->WriteSessionByHandle(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, handleId, true, callback));
        }

        sessionEvent.Wait();
        auto eventArgs2 = XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 3 };
        sessionWriter->OnSessionChanged(eventArgs2);

        if (handleId.empty())
        {
            VERIFY_SUCCEEDED(sessionWriter->WriteSession(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, true, callback));
        }
        else
        {
            VERIFY_SUCCEEDED(sessionWriter->WriteSessionByHandle(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, handleId, true, callback));
        }

        sessionEvent.Wait();
        XAsyncBlock async{};
        mock->SetResponseBody(getResponse);
        RunAsync(&async, __FUNCTION__,
            [maxChangeNumberForTap, &sessionEvent, session, sessionWriter](XAsyncOp op, const XAsyncProviderData* data)
            {
                UNREFERENCED_PARAMETER(data);
                switch (op)
                {
                case XAsyncOp::DoWork:
                {
                    sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", maxChangeNumberForTap });

                    sessionEvent.Set();
                }
                default:
                    return S_OK;
                }
            });

        auto isDone = 0;
        while (isDone < 10)
        {
            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            Sleep(10);
            isDone++;

            if (isDone == 5)
            {
                sessionEvent.Wait();
            }
        }
        
        VERIFY_ARE_EQUAL_UINT(maxChangeNumberToTest, mpmInstance->LobbySession()->ChangeNumber());
    }

    DEFINE_TEST_CASE(TestWriteSessionWithTaps_2)
    {
        TEST_LOG(L"Test starting: TestWriteSessionWithTaps_2");

        std::vector<const char*> writeResponses
        {
            sessionChangeNum6,                     // change #6
            sessionChangeNum4,                     // change #4
            lobbyWithPendingTransferHandleResponse // change #2
        };

        const char* getResponse{ sessionChangeNum8 };

        TestWriteSessionWithTapsHelper(writeResponses, getResponse, 5, 6, true);
        TestWriteSessionWithTapsHelper(writeResponses, getResponse, 5, 6, true, "TestHandleId");
    }

    DEFINE_TEST_CASE(TestWriteSessionWithTaps_3)
    {
        TEST_LOG(L"Test starting: TestWriteSessionWithTaps_3");

        std::vector<const char*> writeResponses
        {
            sessionChangeNum6,                     // change #6
            sessionChangeNum4,                     // change #4
            lobbyWithPendingTransferHandleResponse // change #2
        };

        const char* getResponse{ sessionChangeNum8 };

        TestWriteSessionWithTapsHelper(writeResponses, getResponse, 8, 8, false);
        TestWriteSessionWithTapsHelper(writeResponses, getResponse, 8, 8, false, "TestHandleId");
    }

    // multiplayer_session_writer:
    DEFINE_TEST_CASE(TestSessionWriterLeaveRemoteSession)
    {
        TEST_LOG(L"Test starting: TestSessionWriterLeaveRemoteSession");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        JoinGameHelper(xboxLiveContext.get());

        auto mpmInstance = GlobalState::Get()->MultiplayerManager();
        auto num = mpmInstance->LobbySession()->ChangeNumber();
        VERIFY_ARE_EQUAL_UINT(3, num);

        HttpMock writeSessionMock{ POST, MPSD_URI, 200 };
        writeSessionMock.SetResponseBody(sessionChangeNum5);

        auto session = GetSession(false);
        auto sessionWriter = GetSessionWriter(false);

        Event sessionEvent;
        auto callback = [&sessionEvent](Result<std::shared_ptr<XblMultiplayerSession>> joinResult)
        {
            sessionEvent.Set();
        };

        VERIFY_SUCCEEDED(sessionWriter->LeaveRemoteSession(session, callback));
        sessionEvent.Wait();

        uint32_t isDone = 0;
        while (isDone < 10)
        {
            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            Sleep(10);
            isDone++;
        }

        // Since updateLatest is false, this should not update the latest session.
        VERIFY_ARE_EQUAL_UINT(3, mpmInstance->LobbySession()->ChangeNumber());
    }

    DEFINE_TEST_CASE(TestTransferHandleState)
    {
        TEST_LOG(L"Test starting: TestTransferHandleState");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        JoinGameHelper(xboxLiveContext.get());

        // Set up initial http responses
        std::vector<const char*> writeResponses
        {
            lobbyWithPendingTransferHandleResponse,                    // change #2
            lobbyWithCompletedTransferHandleResponse,                  // change #3
            updatedMultipleLocalUsersLobbyWithNoTransferHandleResponse // clear_game_session_from_lobby
        };

        std::vector<const char*> transferHandles
        {
            "1234",
            "TestGameSessionTransferHandle",
            ""
        };

        HttpMock mock(GET, defaultMpsdUri, 200);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        auto session = GetSession(false);
        auto sessionWriter = GetSessionWriter(false);
        auto clientManager = GlobalState::Get()->MultiplayerManager()->GetMultiplayerClientManager();
        auto lobbyClient = clientManager->LatestPendingRead()->LobbyClient();
        VERIFY_IS_TRUE(lobbyClient->IsTransferHandleState("completed"));
        VERIFY_ARE_EQUAL_STR(transferHandles[1], lobbyClient->GetTransferHandle());

        Event sessionEvent;
        Result<std::shared_ptr<XblMultiplayerSession>> writeResult;
        auto callback = [&writeResult, &sessionEvent](Result<std::shared_ptr<XblMultiplayerSession>> result)
        {
            writeResult = result;
            sessionEvent.Set();
        };

        mock.SetResponseBody(writeResponses[0]);
        VERIFY_SUCCEEDED(sessionWriter->WriteSession(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, true, callback));
        sessionEvent.Wait();
        clientManager->LatestPendingRead()->LobbyClient()->UpdateSession(writeResult.Payload());
        VERIFY_IS_TRUE(lobbyClient->IsTransferHandleState("completed"));
        VERIFY_ARE_EQUAL_STR(transferHandles[1], lobbyClient->GetTransferHandle());

        mock.SetResponseBody(writeResponses[1]);
        VERIFY_SUCCEEDED(sessionWriter->WriteSession(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, true, callback));
        sessionEvent.Wait();
        clientManager->LatestPendingRead()->LobbyClient()->UpdateSession(writeResult.Payload());
        VERIFY_IS_TRUE(lobbyClient->IsTransferHandleState("completed"));
        VERIFY_ARE_EQUAL_STR(transferHandles[1], lobbyClient->GetTransferHandle());

        mock.SetResponseBody(writeResponses[2]);
        VERIFY_SUCCEEDED(sessionWriter->WriteSession(xboxLiveContext, session, XblMultiplayerSessionWriteMode::UpdateExisting, true, callback));
        sessionEvent.Wait();
        clientManager->LatestPendingRead()->LobbyClient()->UpdateSession(writeResult.Payload());
        VERIFY_IS_FALSE(lobbyClient->IsTransferHandleState("pending"));
        VERIFY_IS_FALSE(lobbyClient->IsTransferHandleState("completed"));
        VERIFY_ARE_EQUAL_STR(transferHandles[2], lobbyClient->GetTransferHandle());
    }

    void LeaveRemoteSessionWithEmptyGameSession(const char* gameSessionResponse, bool removeStaleUsers)
    {
        UNREFERENCED_PARAMETER(gameSessionResponse);
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        JoinGameHelper(xboxLiveContext.get());

        HttpMock mockWrite(POST, defaultMpsdUri, 201);
        mockWrite.SetResponseBody(updatedLobbyWithNoTransferHandleResponse);
        mockWrite.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        auto lobbySession = GetSession(false);
        auto gameSession = GetSession(true);
        auto mpmInstance = GlobalState::Get()->MultiplayerManager();
        auto clientManager = mpmInstance->GetMultiplayerClientManager();
        auto lobbyClient = clientManager->LatestPendingRead()->LobbyClient();
        auto gameClient = clientManager->LatestPendingRead()->GameClient();
        
        VERIFY_ARE_EQUAL_UINT(1, mpmInstance->GameSession()->ChangeNumber());

        if (removeStaleUsers)
        {
            for (auto context : lobbyClient->GetLocalUserMap())
            {
                auto user = context.second;
                if (user != nullptr)
                {
                    user->SetLobbyState(multiplayer::manager::MultiplayerLocalUserLobbyState::Remove);
                }
            }

            gameClient->RemoveStaleUsersFromRemoteSession();
        }
        else
        {
            gameClient->LeaveRemoteSession(gameSession, true, true);
        }
        
        int count{ 0 };
        bool isStopAdvertisingGameDone{ false };
        auto customProps = JsonUtils::SerializeJson(propertiesNoTransferHandleJson()["properties"]["custom"]);
        while (!isStopAdvertisingGameDone)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            XblMultiplayerSessionReadLockGuard lobbySessionSafe(lobbySession);
            auto props{ lobbySessionSafe.SessionProperties().SessionCustomPropertiesJson };
            if (utils::str_icmp(props, customProps.c_str()) == 0)
            {
                isStopAdvertisingGameDone = true;
            }

            Sleep(10);
        }

        VERIFY_IS_TRUE(isStopAdvertisingGameDone);
        VerifyLobby(updatedLobbyNoHandleResponseJson().GetObject());
        VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
    }

    void RemoveStaleUsersFromRemoteSession(
        const char* gameSessionResponse,
        JsonValue gameSessionResultToVerify
        )
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext1 = env.CreateMockXboxLiveContext(1234);
        auto xboxLiveContext2 = env.CreateMockXboxLiveContext(2345);

        std::vector<XblContextHandle> xboxLiveContexts{ xboxLiveContext1.get(), xboxLiveContext2.get() };

        JoinGameFromLobbyMultipleUsersHelper(xboxLiveContexts);

        auto mpmInstance = GlobalState::Get()->MultiplayerManager();
        VERIFY_ARE_EQUAL_UINT(1, mpmInstance->GameSession()->ChangeNumber());
        
        HttpMock mock(GET, defaultMpsdUri, 200);
        mock.SetResponseBody(updatedLobbyWithNoTransferHandleResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        auto clientManager = mpmInstance->GetMultiplayerClientManager();
        auto lobbyClient = clientManager->LatestPendingRead()->LobbyClient();
        auto gameClient = clientManager->LatestPendingRead()->GameClient();

        auto members = mpmInstance->GameSession()->Members();

        for (auto context : lobbyClient->GetLocalUserMap())
        {
            auto user = context.second;
            if (user != nullptr)
            {
                user->SetLobbyState(multiplayer::manager::MultiplayerLocalUserLobbyState::Remove);
            }
        }

        mock.SetResponseBody(gameSessionResponse);
        gameClient->RemoveStaleUsersFromRemoteSession();

        uint32_t isDone = 0;
        while (isDone < 10)
        {
            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            Sleep(10);
            isDone++;
        }

        members = mpmInstance->GameSession()->Members();
        VERIFY_ARE_EQUAL_UINT(2, mpmInstance->LobbySession()->LocalMembers().size());
        VERIFY_ARE_EQUAL_UINT(1, mpmInstance->GameSession()->Members().size());

        VerifyLobby(defaultMultipleLocalUsersLobbyResponseJson().GetObject());
        VerifyGame(gameSessionResultToVerify.GetObject());
    }

    void LeaveRemoteSession()
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext1 = env.CreateMockXboxLiveContext(1234);
        auto xboxLiveContext2 = env.CreateMockXboxLiveContext(2345);

        std::vector<XblContextHandle> xboxLiveContexts{ xboxLiveContext1.get(), xboxLiveContext2.get() };

        JoinGameFromLobbyMultipleUsersHelper(xboxLiveContexts);

        auto lobbySession = GetSession(false);
        auto mpmInstance = GlobalState::Get()->MultiplayerManager();
        auto gameClient = mpmInstance->GetMultiplayerClientManager()->LatestPendingRead()->GameClient();

        VERIFY_ARE_EQUAL_UINT(1, mpmInstance->GameSession()->ChangeNumber());

        HttpMock mock(POST, defaultMpsdUri, 201);
        mock.SetResponseBody(updatedMultipleLocalUsersLobbyWithNoTransferHandleResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        HttpMock gamemock(POST, defaultGameUri, 201);
        gamemock.SetResponseBody(emptyResponse);
        gamemock.SetResponseHeaders(defaultGameHttpResponseHeaders);

        gameClient->LeaveRemoteSession(GetSession(true), true, true);

        int count{ 0 };
        auto customProps = JsonUtils::SerializeJson(propertiesNoTransferHandleJson()["properties"]["custom"]);
        bool leaveGameCompleted = false, isStopAdvertisingGameDone = false;
        while (!leaveGameCompleted || !isStopAdvertisingGameDone)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);
            
            if (!leaveGameCompleted)
            {
                for (uint32_t i = 0; i < eventsCount; ++i)
                {
                    if (events[i].EventType == XblMultiplayerEventType::LeaveGameCompleted)
                    {
                        leaveGameCompleted = true;
                        break;
                    }
                }
            }

            JsonDocument props{};
            props.Parse(XblMultiplayerManagerLobbySessionPropertiesJson());
            if (utils::str_icmp(customProps.c_str(), JsonUtils::SerializeJson(props).c_str()) == 0)
            {
                isStopAdvertisingGameDone = true;
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!leaveGameCompleted || !isStopAdvertisingGameDone);
        VERIFY_ARE_EQUAL_UINT(2, XblMultiplayerManagerLobbySessionLocalMembersCount());

        VerifyLobby(updatedMultipleLocalUsersLobbyWithNoTransferHandleResponseJson().GetObject());
        VERIFY_IS_FALSE(XblMultiplayerManagerGameSessionActive());
    }

    DEFINE_TEST_CASE(TestLeaveRemoteSession)
    {
        TEST_LOG(L"Test starting: TestLeaveRemoteSession");

        LeaveRemoteSession();
    }

    DEFINE_TEST_CASE(TestRemoveStaleUsersFromRemoteSession)
    {
        TEST_LOG(L"Test starting: TestRemoveStaleUsersFromRemoteSession");

        RemoveStaleUsersFromRemoteSession(gameSessionResponseDiffXuid, gameSessionResponseDiffXuidJson().GetObject());
    }

    DEFINE_TEST_CASE(TestErrorHandling)
    {
        TEST_LOG(L"Test starting: TestErrorHandling");

        MPMTestEnvironment env{};
        XTaskQueueHandle queue{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        XblMultiplayerManagerInitialize(LOBBY_TEMPLATE_NAME, queue);

        size_t eventsCount{};
        const XblMultiplayerEvent* events{};
        VERIFY_SUCCEEDED(XblMultiplayerManagerDoWork(&events, &eventsCount));

#pragma warning(suppress: 6387)
        VERIFY_ARE_EQUAL_INT(
            E_INVALIDARG,
            XblMultiplayerManagerLobbySessionAddLocalUser(nullptr)
            );

#pragma warning(suppress: 6387)
        VERIFY_ARE_EQUAL_INT(
            E_INVALIDARG,
            XblMultiplayerManagerLobbySessionRemoveLocalUser(nullptr)
            );

        XblUserHandle userHandle = xboxLiveContext->User().Handle();
        VERIFY_ARE_EQUAL_INT(
            E_UNEXPECTED,
            XblMultiplayerManagerLobbySessionRemoveLocalUser(userHandle)
            );

        uint32_t contextIds[2]{ 1,2 };
#pragma warning(suppress: 6387)
        VERIFY_ARE_EQUAL_INT(
            E_INVALIDARG,
            XblMultiplayerManagerLobbySessionSetLocalMemberProperties(nullptr, "Health", "89", &contextIds[0])
            );

        VERIFY_ARE_EQUAL_INT(
            E_UNEXPECTED,
            XblMultiplayerManagerLobbySessionSetLocalMemberProperties(userHandle, "", "89", &contextIds[1])
            );

#pragma warning(suppress: 6387)
        VERIFY_ARE_EQUAL_INT(
            E_INVALIDARG,
            XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties(nullptr, "Health", &contextIds[0])
            );

        VERIFY_ARE_EQUAL_INT(
            E_UNEXPECTED,
            XblMultiplayerManagerLobbySessionDeleteLocalMemberProperties(userHandle, "", &contextIds[0])
            );

#pragma warning(suppress: 6387)
        VERIFY_ARE_EQUAL_INT(
            E_INVALIDARG,
            XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(nullptr, "AQAI1Fy6", &contextIds[0])
            );

        VERIFY_ARE_EQUAL_INT(
            E_UNEXPECTED,
            XblMultiplayerManagerLobbySessionSetLocalMemberConnectionAddress(userHandle, "AQAI1Fy6", &contextIds[0])
            );

        VERIFY_ARE_EQUAL_INT(
            E_INVALIDARG,
            XblMultiplayerManagerLobbySessionSetProperties("", "89", &contextIds[0])
            );

        VERIFY_ARE_EQUAL_INT(
            E_UNEXPECTED,
            XblMultiplayerManagerLobbySessionSetSynchronizedProperties("name", "89", &contextIds[0])
            );

#pragma warning(suppress: 6387)
        VERIFY_ARE_EQUAL_INT(
            E_INVALIDARG,
            XblMultiplayerManagerLobbySessionSetSynchronizedHost(nullptr, &contextIds[0])
            );

#pragma warning(suppress: 4973)
        VERIFY_ARE_EQUAL_INT(
            E_INVALIDARG,
            XblMultiplayerManagerJoinGame("", GAME_TEMPLATE_NAME, nullptr, 0)
            );

#pragma warning(suppress: 4973)
        VERIFY_ARE_EQUAL_INT(
            E_UNEXPECTED,
            XblMultiplayerManagerJoinGame("TestSessionName", GAME_TEMPLATE_NAME, nullptr, 0)
            );

        VERIFY_SUCCEEDED(XblRealTimeActivityActivate(xboxLiveContext.get()));

        HttpMock mock(POST, defaultMpsdUri, 400);
        mock.SetResponseBody(badResponse);
        mock.SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        VERIFY_SUCCEEDED(XblMultiplayerManagerLobbySessionAddLocalUser(userHandle));

        VERIFY_ARE_EQUAL_INT(
            E_UNEXPECTED,
            XblMultiplayerManagerLobbySessionAddLocalUser(userHandle)
            );  // User already added

        int count{ 0 };
        bool isDone{ false };
        while (!isDone)
        {
            if (++count > 500) break;

            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::UserAdded)
                {
                    VERIFY_ARE_EQUAL_INT(HTTP_E_STATUS_BAD_REQUEST, events[i].Result);
                    isDone = true;
                }
            }

            Sleep(10);
        }

        VERIFY_IS_TRUE(isDone);
    }

    /*
        Matchmaking Tests:
    */

    enum class MatchCallingPatternType
    {
        Completed,
        Canceled,
        CanceledByService,
        RemoteClientFailedToJoin,       // initialization stage = failed as the remote client failed to join the target session
        ExpiredByNextTimer,             // Didn't get any shoulder taps after searching.
        ExpiredByService,               // Service could not find anything to match with.
        RemoteClientFailedToUploadQoS   // initialization stage = failed as the remote client failed to upload qos data
    };

    void FindMatchNoQoSHelper(MatchCallingPatternType matchCallingPattern)
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelper(xboxLiveContext.get(), lobbyWithNoTransferHandleResponse);

        // Set up initial http responses
        std::vector<const char*> lobbyResponses{ lobbyMatchStatusSearching };
        std::vector<const char*> gameResponses{ matchSessionJoin_1 };

        if (matchCallingPattern == MatchCallingPatternType::ExpiredByNextTimer || 
            matchCallingPattern == MatchCallingPatternType::ExpiredByService)
        {
            lobbyResponses.push_back(lobbyMatchStatusExpiredByService);
        }
        else
        {
            lobbyResponses.push_back(lobbyMatchStatusFound);
            lobbyResponses.push_back(lobbyMatchStatusFoundWithTransHandle);
        }

        if (matchCallingPattern == MatchCallingPatternType::Completed)
        {
            gameResponses.push_back(matchSessionJoin_2);
        }
        else if (matchCallingPattern == MatchCallingPatternType::RemoteClientFailedToJoin)
        {
            gameResponses.push_back(matchSessionRemoteClientFailedToJoin);
        }

        uint32_t lobbyMockCount{ 0 };
        auto lobbyMock = std::make_shared<HttpMock>(GET, defaultMpsdUri, 200);
        lobbyMock->SetResponseBody(lobbyResponses[0]);
        lobbyMock->SetResponseHeaders(defaultLobbyHttpResponseHeaders);
        lobbyMock->SetMockMatchedCallback([lobbyResponses, &lobbyMockCount](class HttpMock* matchedMock, xsapi_internal_string actualRequestUrl, xsapi_internal_string requestBody)
        {
            if (lobbyMockCount < lobbyResponses.size())
            {
                matchedMock->SetResponseBody(lobbyResponses[lobbyMockCount]);
                ++lobbyMockCount;
            }
        });

        uint32_t gameMockCount{ 0 };
        auto gameMock = std::make_shared<HttpMock>(GET, defaultGameUri, 200);
        gameMock->SetResponseBody(gameResponses[0]);
        gameMock->SetResponseHeaders(defaultGameHttpResponseHeaders);
        gameMock->SetMockMatchedCallback([gameResponses, &gameMockCount](class HttpMock* matchedMock, xsapi_internal_string actualRequestUrl, xsapi_internal_string requestBody)
        {
            if (gameMockCount < gameResponses.size())
            {
                matchedMock->SetResponseBody(gameResponses[gameMockCount]);
                ++gameMockCount;
            }
        });

        HttpMock matchMock("PUT", matchTicketUri, 201);
        matchMock.SetResponseBody(matchTicketResponse);

        HttpMock transferMock("PUT", transferHandleUri, 201);
        transferMock.SetResponseBody(transferHandleResponse);

        auto mpmInstance = GlobalState::Get()->MultiplayerManager();
        auto clientManager = mpmInstance->GetMultiplayerClientManager();

        JsonDocument doc{};
        mpmInstance->FindMatch(HOPPER_NAME_NO_QOS, doc, std::chrono::seconds{ TICKS_PER_SECOND });
        clientManager->MatchClient()->DisableNextTimer(true);

        // Match ticket response
        // LB: Shoulder tap with a Get: status to searching
        // LB: Shoulder tap with a Get: status to found
        // GS: PUT to join
        // GS: Shoulder tap with a GET with both players joined
        // Create Transfer handle
        // LB: PUT Transfer handle

        auto session = GetSession(false);
        auto sessionWriter = GetSessionWriter(false);

        int count{ 0 };
        auto customProps = JsonUtils::SerializeJson(classPropertiesJson()["properties"]["custom"]);
        bool matchFound = false, isAdvertisingGameDone = false, searchingTapTriggered = false, foundTapTriggered = false, waitingForClientsTapTriggered = false;
        while (!matchFound || !isAdvertisingGameDone)
        {
            if (++count > 500) break;

            if (!searchingTapTriggered && XblMultiplayerManagerMatchStatus() == XblMultiplayerMatchStatus::Searching)
            {
                searchingTapTriggered = true;
                VERIFY_ARE_EQUAL_INT(mpmInstance->EstimatedMatchWaitTime().count(), matchTicketJson()["waitTime"].GetUint64() );

                sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 4 });
            }

            if (!foundTapTriggered && GlobalState::Get()->MultiplayerManager()->LobbySession()->ChangeNumber() == 4)
            {
                foundTapTriggered = true;
                if (matchCallingPattern == MatchCallingPatternType::ExpiredByNextTimer)
                {
                    clientManager->MatchClient()->DisableNextTimer(false);
                }

                // Session upgraded to searching. Force a tap to change match status == found or expired (ExpiredByService)
                sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 6 });
            }

            if (!waitingForClientsTapTriggered && XblMultiplayerManagerMatchStatus() == XblMultiplayerMatchStatus::WaitingForRemoteClientsToJoin)
            {
                waitingForClientsTapTriggered = true;
                if (matchCallingPattern == MatchCallingPatternType::RemoteClientFailedToJoin)
                {
                    clientManager->MatchClient()->DisableNextTimer(false);
                }
                else if (matchCallingPattern == MatchCallingPatternType::Completed)
                {
                    // Force a tap to simulate 2nd user joining.
                    clientManager->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ clientManager->MatchClient()->Session()->SessionReference(), "", 3 });
                }
            }

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::FindMatchCompleted)
                {
                    matchFound = true; 
                    XblMultiplayerMatchStatus matchStatus{};
                    VERIFY_SUCCEEDED(XblMultiplayerEventArgsFindMatchCompleted(events[i].EventArgsHandle, &matchStatus, nullptr));

                    switch (matchCallingPattern)
                    {
                        case MatchCallingPatternType::Completed:
                        {
                            VERIFY_ARE_EQUAL_UINT(XblMultiplayerMatchStatus::Completed, matchStatus);
                            sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 7 });
                            break;
                        }
                        case MatchCallingPatternType::RemoteClientFailedToJoin:
                        {
                            VERIFY_ARE_EQUAL_UINT(XblMultiplayerMatchStatus::Failed, matchStatus);
                            break;
                        }
                        case MatchCallingPatternType::ExpiredByNextTimer:
                        case MatchCallingPatternType::ExpiredByService:
                        {
                            VERIFY_ARE_EQUAL_UINT(XblMultiplayerMatchStatus::Expired, matchStatus);
                            break;
                        }
                    }

                }
            }

            if (matchFound)
            {
                if (matchCallingPattern != MatchCallingPatternType::Completed)
                {
                    isAdvertisingGameDone = true;
                }
                else
                {
                    auto props{ XblMultiplayerManagerLobbySessionPropertiesJson() };
                    if (props != nullptr &&
                        utils::str_icmp(props, customProps.c_str()) == 0)
                    {
                        isAdvertisingGameDone = true;
                    }
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!matchFound || !isAdvertisingGameDone);
    }

    void FindMatchWithQoSHelper(MatchCallingPatternType callingPattern)
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelper(xboxLiveContext.get(), lobbyWithNoTransferHandleResponse);

        // Set up initial http responses
        std::vector<const char*> lobbyResponses
        {
            lobbyMatchStatusSearching,
            lobbyMatchStatusFound,
            lobbyMatchStatusFoundWithTransHandle
        };

        std::vector<const char*> gameResponses
        { 
            matchSessionJoin_1,
            matchSessionMeasuring,
            matchSessionMeasuringWithQoS
        };

        if (callingPattern == MatchCallingPatternType::RemoteClientFailedToUploadQoS)
        {
            gameResponses.push_back(matchRemoteClientFailedToUploadQoS);
        }
        else if(callingPattern == MatchCallingPatternType::Completed)
        {
            gameResponses.push_back(matchSessionMeasuringWithQoSComplete);
        }

        uint32_t lobbyMockCount{ 0 };
        auto lobbyMock = std::make_shared<HttpMock>(GET, defaultMpsdUri, 200);
        lobbyMock->SetResponseBody(lobbyResponses[0]);
        lobbyMock->SetResponseHeaders(defaultLobbyHttpResponseHeaders);
        lobbyMock->SetMockMatchedCallback([lobbyResponses, &lobbyMockCount](class HttpMock* matchedMock, xsapi_internal_string actualRequestUrl, xsapi_internal_string requestBody)
        {
            if (lobbyMockCount < lobbyResponses.size())
            {
                matchedMock->SetResponseBody(lobbyResponses[lobbyMockCount]);
                ++lobbyMockCount;
            }
        });

        uint32_t gameMockCount{ 0 };
        auto gameMock = std::make_shared<HttpMock>(GET, defaultGameUri, 200);
        gameMock->SetResponseBody(gameResponses[0]);
        gameMock->SetResponseHeaders(defaultGameHttpResponseHeaders);
        gameMock->SetMockMatchedCallback([gameResponses, &gameMockCount](class HttpMock* matchedMock, xsapi_internal_string actualRequestUrl, xsapi_internal_string requestBody)
        {
            if (gameMockCount < gameResponses.size())
            {
                matchedMock->SetResponseBody(gameResponses[gameMockCount]);
                ++gameMockCount;
            }
        });

        HttpMock matchMock("PUT", matchTicketUri, 201);
        matchMock.SetResponseBody(matchTicketResponse);

        HttpMock transferMock("PUT", transferHandleUri, 201);
        transferMock.SetResponseBody(transferHandleResponse);
        
        auto mpmInstance = GlobalState::Get()->MultiplayerManager();
        auto clientManager = mpmInstance->GetMultiplayerClientManager();

        JsonDocument doc{};
        mpmInstance->FindMatch(HOPPER_NAME_NO_QOS, doc, std::chrono::seconds{ TICKS_PER_SECOND });
        clientManager->MatchClient()->DisableNextTimer(true);

        // Match ticket response
        // LB: Shoulder tap with a Get: status to searching
        // LB: Shoulder tap with a Get: status to found
        // GS: PUT to join
        // GS: Shoulder tap with a GET with both players joined
        // GS: Trigger set qos
        // title: set qos metrics
        // match completed
        // Create Transfer handle
        // LB: PUT Transfer handle

        auto lobbySession = GetSession(false);
        auto lobbySessionWriter = GetSessionWriter(false);
        auto gameSession = GetSession(true);
        auto gameSessionWriter = GetSessionWriter(true);

        int count{ 0 };
        auto customProps = JsonUtils::SerializeJson(classPropertiesJson()["properties"]["custom"]);
        bool matchFound{ false };
        bool isAdvertisingGameDone{ false };
        bool searchingTapTriggered{ false };
        bool foundTapTriggered{ false };
        bool waitingForClientsToJoinTapTriggered{ false };
        bool waitingForClientsToUploadQoSTapTriggered{ false };
        while (!matchFound || !isAdvertisingGameDone)
        {
            if (++count > 500) break;

            if (!searchingTapTriggered && XblMultiplayerManagerMatchStatus() == XblMultiplayerMatchStatus::Searching)
            {
                searchingTapTriggered = true;
                VERIFY_ARE_EQUAL_UINT(mpmInstance->EstimatedMatchWaitTime().count(), matchTicketJson()["waitTime"].GetUint64());

                lobbySessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ lobbySession->SessionReference(), "", 4 });
            }

            if (!foundTapTriggered && GlobalState::Get()->MultiplayerManager()->LobbySession()->ChangeNumber() == 4)
            {
                foundTapTriggered = true;

                // Session upgraded to searching. Force a tap to change match status == found or expired (ExpiredByService)
                lobbySessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ lobbySession->SessionReference(), "", 6 });
            }

            if (!waitingForClientsToJoinTapTriggered && XblMultiplayerManagerMatchStatus() == XblMultiplayerMatchStatus::WaitingForRemoteClientsToJoin)
            {
                waitingForClientsToJoinTapTriggered = true;

                // Force a tap to simulate 2nd user joining.
                clientManager->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ clientManager->MatchClient()->Session()->SessionReference(), "", 3 });
            }

            if (!waitingForClientsToUploadQoSTapTriggered && XblMultiplayerManagerMatchStatus() == XblMultiplayerMatchStatus::WaitingForRemoteClientsToUploadQos)
            {
                waitingForClientsToUploadQoSTapTriggered = true;
                if (callingPattern == MatchCallingPatternType::RemoteClientFailedToUploadQoS)
                {
                    clientManager->MatchClient()->DisableNextTimer(false);
                }
                else
                {
                    // Force a tap to simulate 2nd user uploading QoS.
                    clientManager->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ clientManager->MatchClient()->Session()->SessionReference(), "", 6 });
                }
            }

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::PerformQosMeasurements)
                {
                    VERIFY_SUCCEEDED(XblMultiplayerManagerSetQosMeasurements("{}"));
                }
                else if (events[i].EventType == XblMultiplayerEventType::FindMatchCompleted)
                {
                    matchFound = true;
                    XblMultiplayerMatchStatus matchStatuss{};
                    VERIFY_SUCCEEDED(XblMultiplayerEventArgsFindMatchCompleted(events[i].EventArgsHandle, &matchStatuss, nullptr));

                    if (callingPattern == MatchCallingPatternType::Completed)
                    {
                        VERIFY_ARE_EQUAL_UINT(XblMultiplayerMatchStatus::Completed, matchStatuss);
                    }
                    else if (callingPattern == MatchCallingPatternType::RemoteClientFailedToUploadQoS)
                    {
                        VERIFY_ARE_EQUAL_UINT(XblMultiplayerMatchStatus::Failed, matchStatuss);
                    }

                    lobbySessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ lobbySession->SessionReference(), "", 7 });
                }
            }

            if (matchFound)
            {
                if (callingPattern != MatchCallingPatternType::Completed)
                {
                    isAdvertisingGameDone = true;
                }
                else
                {
                    auto props{ XblMultiplayerManagerLobbySessionPropertiesJson() };
                    if (props != nullptr &&
                        utils::str_icmp(props, customProps.c_str()) == 0)
                    {
                        isAdvertisingGameDone = true;
                    }
                }
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!matchFound || !isAdvertisingGameDone);
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSCompleted)
    {
        TEST_LOG(L"Test starting: TestFindMatchNoQoSCompleted");

        FindMatchNoQoSHelper(MatchCallingPatternType::Completed);
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSRemoteClientFailedToJoin)
    {
        TEST_LOG(L"Test starting: TestFindMatchNoQoSRemoteClientFailedToJoin");

        FindMatchNoQoSHelper(MatchCallingPatternType::RemoteClientFailedToJoin);
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSExpiredByNextTimer)
    {
        TEST_LOG(L"Test starting: TestFindMatchNoQoSExpiredByNextTimer");

        FindMatchNoQoSHelper(MatchCallingPatternType::ExpiredByNextTimer);
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSExpiredByService)
    {
        TEST_LOG(L"Test starting: TestFindMatchNoQoSExpiredByService");

        FindMatchNoQoSHelper(MatchCallingPatternType::ExpiredByService);
    }

    DEFINE_TEST_CASE(TestFindMatchWithQoSCompleted)
    {
        TEST_LOG(L"Test starting: TestFindMatchWithQoSCompleted");

        FindMatchWithQoSHelper(MatchCallingPatternType::Completed);
    }

    DEFINE_TEST_CASE(TestFindMatchWithQoSRemoteClientFailedToUploadQoS)
    {
        TEST_LOG(L"Test starting: TestFindMatchWithQoSRemoteClientFailedToUploadQoS");

        FindMatchWithQoSHelper(MatchCallingPatternType::RemoteClientFailedToUploadQoS);
    }

    void FindMatchNoQoSRemoteClientJoiningMatchSessionHelper()
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelper(xboxLiveContext.get(), lobbyWithNoTransferHandleResponse);

        // Set up initial http responses
        std::vector<const char*> lobbyResponses
        {
            lobbyMatchStatusSearching,
            lobbyMatchStatusFound,
            lobbyMatchStatusFoundWithTransHandle
        };

        std::vector<const char*> gameResponses
        {
            matchSessionJoin_1,
            matchSessionJoin_2
        };

        uint32_t lobbyMockCount{ 0 };
        auto lobbyMock = std::make_shared<HttpMock>(GET, defaultMpsdUri, 200);
        lobbyMock->SetResponseHeaders(defaultLobbyHttpResponseHeaders);
        lobbyMock->SetMockMatchedCallback([lobbyResponses, &lobbyMockCount](class HttpMock* matchedMock, xsapi_internal_string actualRequestUrl, xsapi_internal_string requestBody)
        {
            if (lobbyMockCount < lobbyResponses.size())
            {
                matchedMock->SetResponseBody(lobbyResponses[lobbyMockCount]);
                ++lobbyMockCount;
            }
        });

        uint32_t gameMockCount{ 0 };
        auto gameMock = std::make_shared<HttpMock>(GET, defaultGameUri, 200);
        gameMock->SetResponseHeaders(defaultGameHttpResponseHeaders);
        gameMock->SetMockMatchedCallback([gameResponses, &gameMockCount](class HttpMock* matchedMock, xsapi_internal_string actualRequestUrl, xsapi_internal_string requestBody)
        {
            if (gameMockCount < gameResponses.size())
            {
                matchedMock->SetResponseBody(gameResponses[gameMockCount]);
                ++gameMockCount;
            }
        });

        HttpMock matchMock("PUT", matchTicketUri, 201);
        matchMock.SetResponseBody(matchTicketResponse);

        HttpMock transferMock("PUT", transferHandleUri, 201);
        transferMock.SetResponseBody(transferHandleResponse);

        // LB: Shoulder tap with a Get: status to searching
        // LB: Shoulder tap with a Get: status to found
        // GS: PUT to join
        // GS: Shoulder tap with a GET with both players joined
        // Create Transfer handle
        // LB: PUT Transfer handle

        auto session = GetSession(false);
        auto sessionWriter = GetSessionWriter(false);
        auto mpmInstance = GlobalState::Get()->MultiplayerManager();
        auto clientManager = mpmInstance->GetMultiplayerClientManager();

        clientManager->MatchClient()->DisableNextTimer(true);

        int count{ 0 };
        auto customProps = JsonUtils::SerializeJson(classPropertiesJson()["properties"]["custom"]);
        bool matchFound{ false };
        bool isAdvertisingGameDone{ false };
        bool searchingTapTriggered{ false };
        bool foundTapTriggered{ false };
        bool waitingForClientsTapTriggered{ false };
        while (!matchFound || !isAdvertisingGameDone)
        {
            if (++count > 500) break;

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            if (!searchingTapTriggered)
            {
                searchingTapTriggered = true;
                sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 4 });
            }

            if (!foundTapTriggered && mpmInstance->LobbySession()->ChangeNumber() == 4)
            {
                foundTapTriggered = true;
                
                // Session upgraded to searching. Force a tap to change match status == found or expired (ExpiredByService)
                sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 6 });
            }

            if (!waitingForClientsTapTriggered && XblMultiplayerManagerMatchStatus() == XblMultiplayerMatchStatus::WaitingForRemoteClientsToJoin)
            {
                waitingForClientsTapTriggered = true;

                // Force a tap to simulate 2nd user joining.
                clientManager->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ clientManager->MatchClient()->Session()->SessionReference(), "", 3 });
            }

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::FindMatchCompleted)
                {
                    matchFound = true;
                    XblMultiplayerMatchStatus matchStatus{};
                    VERIFY_SUCCEEDED(XblMultiplayerEventArgsFindMatchCompleted(events[i].EventArgsHandle, &matchStatus, nullptr));
                    VERIFY_ARE_EQUAL_UINT(XblMultiplayerMatchStatus::Completed, matchStatus);

                    sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 7 });
                }
            }

            auto props{ XblMultiplayerManagerLobbySessionPropertiesJson() };
            if (matchFound &&
                props != nullptr &&
                utils::str_icmp(props, customProps.c_str()) == 0)
            {
                isAdvertisingGameDone = true;
            }

            Sleep(10);
        }

        VERIFY_IS_FALSE(!matchFound || !isAdvertisingGameDone);
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSRemoteClientJoiningMatchSession)
    {
        TEST_LOG(L"Test starting: TestFindMatchNoQoSRemoteClientJoiningMatchSession");

        FindMatchNoQoSRemoteClientJoiningMatchSessionHelper();
    }

    DEFINE_TEST_CASE(TestFindMatchNoQoSInvalidArg)
    {
        TEST_LOG(L"Test starting: TestFindMatchNoQoSInvalidArg");

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelper(xboxLiveContext.get());

        uint32_t timeout = 100 * TICKS_PER_SECOND;
        VERIFY_ARE_EQUAL_INT(E_INVALIDARG, XblMultiplayerManagerFindMatch(nullptr, "", timeout));
        VERIFY_ARE_EQUAL_INT(E_INVALIDARG, XblMultiplayerManagerFindMatch(HOPPER_NAME_NO_QOS, "json", timeout));
    }

    void CancelMatchHelper(MatchCallingPatternType callingPattern)
    {
        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);
        AddLocalUserHelper(xboxLiveContext.get(), lobbyWithNoTransferHandleResponse);

        // Set up initial http responses
        std::vector<const char*> lobbyResponses{ lobbyMatchStatusSearching };

        std::vector<const char*> gameResponses
        {
            matchSessionJoin_1,
            matchSessionJoin_2
        };

        if (callingPattern == MatchCallingPatternType::CanceledByService)
        {
            lobbyResponses.push_back(lobbyMatchStatusCanceledByService);
        }
        else
        {
            lobbyResponses.push_back(lobbyMatchStatusFound);
            lobbyResponses.push_back(lobbyMatchStatusCanceledByService);
        }

        uint32_t lobbyMockCount{ 0 };
        auto lobbyMock = std::make_shared<HttpMock>(GET, defaultMpsdUri, 200);
        lobbyMock->SetResponseBody(lobbyResponses[0]);
        lobbyMock->SetResponseHeaders(defaultLobbyHttpResponseHeaders);
        lobbyMock->SetMockMatchedCallback([lobbyResponses, &lobbyMockCount](class HttpMock* matchedMock, xsapi_internal_string actualRequestUrl, xsapi_internal_string requestBody)
        {
            if (lobbyMockCount < lobbyResponses.size())
            {
                matchedMock->SetResponseBody(lobbyResponses[lobbyMockCount]);
                ++lobbyMockCount;
            }
        });

        uint32_t gameMockCount{ 0 };
        auto gameMock = std::make_shared<HttpMock>(GET, defaultGameUri, 200);
        gameMock->SetResponseBody(gameResponses[0]);
        gameMock->SetResponseHeaders(defaultGameHttpResponseHeaders);
        gameMock->SetMockMatchedCallback([gameResponses, &gameMockCount](class HttpMock* matchedMock, xsapi_internal_string actualRequestUrl, xsapi_internal_string requestBody)
        {
            if (gameMockCount < gameResponses.size())
            {
                matchedMock->SetResponseBody(gameResponses[gameMockCount]);
                ++gameMockCount;
            }
        });

        HttpMock matchMock("PUT", matchTicketUri, 201);
        matchMock.SetResponseBody(matchTicketResponse);

        HttpMock transferMock("PUT", transferHandleUri, 201);
        transferMock.SetResponseBody(transferHandleResponse);

        auto session = GetSession(false);
        auto sessionWriter = GetSessionWriter(false);
        auto mpmInstance = GlobalState::Get()->MultiplayerManager();
        auto clientManager = mpmInstance->GetMultiplayerClientManager();

        JsonDocument doc{};
        mpmInstance->FindMatch(HOPPER_NAME_NO_QOS, doc, std::chrono::seconds{ TICKS_PER_SECOND });
        clientManager->MatchClient()->DisableNextTimer(true);

        int count{ 0 };
        auto customProps = JsonUtils::SerializeJson(classPropertiesJson()["properties"]["custom"]);
        bool matchFound{ false };
        bool searchingTapTriggered{ false };
        bool foundTapTriggered{ false };
        bool waitingForClientsTapTriggered{ false };
        while (!matchFound)
        {
            if (++count > 500) break;

            if (!searchingTapTriggered && XblMultiplayerManagerMatchStatus() == XblMultiplayerMatchStatus::Searching)
            {
                searchingTapTriggered = true;

                auto matchTicketeJson = matchTicketJson();
                VERIFY_ARE_EQUAL_UINT(mpmInstance->EstimatedMatchWaitTime().count(), matchTicketeJson["waitTime"].GetUint64());

                sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 4 });
            }

            if (!foundTapTriggered && mpmInstance->LobbySession()->ChangeNumber() == 4)
            {
                foundTapTriggered = true;

                // For CanceledByService, this will cause a get with status = canceled.
                sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 6 });
            }

            if (!waitingForClientsTapTriggered && XblMultiplayerManagerMatchStatus() == XblMultiplayerMatchStatus::WaitingForRemoteClientsToJoin)
            {
                waitingForClientsTapTriggered = true;
                if (callingPattern == MatchCallingPatternType::Canceled)
                {
                    mpmInstance->CancelMatch();

                    // This will trigger a shoulder tap with canceled as the status from the service after deleting the match ticket.
                    sessionWriter->OnSessionChanged(XblMultiplayerSessionChangeEventArgs{ session->SessionReference(), "", 8 });
                }
            }

            size_t eventsCount{};
            const XblMultiplayerEvent* events{};
            XblMultiplayerManagerDoWork(&events, &eventsCount);

            for (uint32_t i = 0; i < eventsCount; ++i)
            {
                if (events[i].EventType == XblMultiplayerEventType::FindMatchCompleted)
                {
                    matchFound = true;
                    XblMultiplayerMatchStatus matchStatus{};
                    VERIFY_SUCCEEDED(XblMultiplayerEventArgsFindMatchCompleted(events[i].EventArgsHandle, &matchStatus, nullptr));
                    VERIFY_ARE_EQUAL_UINT(XblMultiplayerMatchStatus::Canceled, matchStatus);
                }
            }

            Sleep(10);
        }

        VERIFY_IS_TRUE(matchFound);
    }

    DEFINE_TEST_CASE(TestCancelMatch)
    {
        TEST_LOG(L"Test starting: TestCancelMatch");

        CancelMatchHelper(MatchCallingPatternType::Canceled);
    }

    DEFINE_TEST_CASE(TestCancelMatchByService)
    {
        TEST_LOG(L"Test starting: TestCancelMatchByService");

        CancelMatchHelper(MatchCallingPatternType::CanceledByService);
    }

    DEFINE_TEST_CASE(TestRtaResync)
    {
        TEST_LOG(L"Test starting: TestRtaResync");

        DEFINE_TEST_CASE_PROPERTIES_FOCUS();

        MPMTestEnvironment env{};
        auto xboxLiveContext = env.CreateMockXboxLiveContext(1234);

        AddLocalUserHelper(xboxLiveContext.get());

        auto mock = std::make_shared<HttpMock>("GET", defaultMpsdUri, 201);
        mock->SetResponseBody(defaultLobbySessionResponse);
        mock->SetResponseHeaders(defaultLobbyHttpResponseHeaders);

        // Wait for MPM to refresh the lobby
        Event e;
        mock->SetMockMatchedCallback([&](HttpMock*, xsapi_internal_string, xsapi_internal_string)
        {
            e.Set();
        });

        env.MockRtaService().RaiseResync();
        e.Wait();
    }
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SYSTEM_CPP_END